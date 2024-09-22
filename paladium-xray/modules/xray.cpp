#define _CRT_SECURE_NO_WARNINGS

#include "xray.hpp"
#include "../blocks.hpp"
#include "../config.hpp"
#include "../hook.hpp"
#include "../utils.hpp"
#include "../gl_utils.hpp"
#include <filesystem>

#include <algorithm>
#include <mutex>
#include "../imgui/imgui.h"
#include "../imtheme.h"
#include "../imgui/imgui_impl_opengl2.h"
#include "../imgui/imgui_impl_win32.h"
#include <iostream>


JNIEnv* env;

// world methods
jmethodID m_get_block, m_is_air_block, m_get_chunk_provider;

// matrix shit
jobject modelview_buffer, projection_buffer;
jmethodID buffer_get_method;
GLfloat projection[16];
GLfloat modelview[16];

// block methods
jclass block_class;
jmethodID m_get_id_from_block;

// render manager
jclass render_manager_class;
jfieldID render_pos_x_field, render_pos_y_field, render_pos_z_field;

// mc
jobject mc_instance;
jfieldID the_player_field, the_world_field;

// player fields
jfieldID player_x_field, player_y_field, player_z_field;

vec3d* render_pos_vec = new vec3d(0, 0, 0);

std::vector<chunk> chunks;
std::mutex mutex;
bool enabled, stop;

// gui
ImFont* font;
int min_y_search = 0;
int max_y_search = 255;

int max_xz_search = 50.0;

bool xray::gui_open;

void xray::render()
{

	if (enabled) {
		for (chunk chunk : chunks)
		{
			for (block block : *chunk.blocks)
			{
				glPushMatrix();
				gl_utils::start();

				vec3f* color = blocks::get_block_color(block.block_id);
				glColor4f(color->r, color->g, color->b, 0.05F);

				double x = block.position.x - render_pos_vec->x;
				double y = block.position.y - render_pos_vec->y;
				double z = block.position.z - render_pos_vec->z;

				double start_offset = 0.5;
				double offset = 0.5;

				gl_utils::quads(x + start_offset - offset, y + start_offset - offset, z + start_offset - offset, x + start_offset + offset, y + start_offset + offset, z + start_offset + offset);
				glColor4f(color->r, color->g, color->b, 1);
				gl_utils::outlines(x + start_offset - offset, y + start_offset - offset, z + start_offset - offset, x + start_offset + offset, y + start_offset + offset, z + start_offset + offset, 1);

				gl_utils::end();
				glPopMatrix();

			}
		}
	}

}

void update_matrix()
{
	for (int i = 0; i < 16; ++i)
	{
		modelview[i] = env->CallFloatMethod(modelview_buffer, buffer_get_method, i);
	}

	for (int i = 0; i < 16; ++i)
	{
		projection[i] = env->CallFloatMethod(projection_buffer, buffer_get_method, i);
	}
}

void update_render_pos()
{
	render_pos_vec->x = env->GetStaticDoubleField(render_manager_class, render_pos_x_field);
	render_pos_vec->y = env->GetStaticDoubleField(render_manager_class, render_pos_y_field);
	render_pos_vec->z = env->GetStaticDoubleField(render_manager_class, render_pos_z_field);
}

bool is_air_block(jobject world, vec3i vec)
{
	return env->CallBooleanMethod(world, m_is_air_block, vec.x, vec.y, vec.z);
}

jobject get_block(jobject world, vec3i vec)
{
	return env->CallObjectMethod(world, m_get_block, vec.x, vec.y, vec.z);
}

int get_id_from_block(jobject world, vec3i vec)
{
	if (is_air_block(world, vec)) {
		return 0;
	}

	jobject block = get_block(world, vec);
	int block_id = env->CallStaticIntMethod(block_class, m_get_id_from_block, block);
	env->DeleteLocalRef(block);

	return block_id;
}

chunk get_chunk_data(int chunk_x, int chunk_z)
{
	chunk chunk;
	std::vector<block>* blocks = new std::vector<block>();

	jobject world = env->GetObjectField(mc_instance, the_world_field);

	for (int y = min_y_search; y < max_y_search; ++y) 
	{
		update_render_pos();
		update_matrix();

		for (int x = chunk_x; x < (chunk_x + 16); ++x)
		{
			for (int z = chunk_z; z < (chunk_z + 16); ++z)
			{
				vec3i block_pos = vec3i(x, y, z);
				int block_id = get_id_from_block(world, block_pos);

				if (!blocks::is_block_whitelisted(block_id))
				{
					continue;
				}

				mutex.lock();
				blocks->push_back(block(block_pos, block_id));
				mutex.unlock();
			}
		}
	}

	chunk.blocks = blocks;
	env->DeleteLocalRef(world);
	return chunk;
}

void find_chunks()
{
	jobject player = env->GetObjectField(mc_instance, the_player_field);
	int x = (int) (env->GetDoubleField(player, player_x_field) / 16);
	int z = (int) (env->GetDoubleField(player, player_z_field) / 16);

	if (x < 0) 
	{
		x--;
	}

	if (z < 0) 
	{
		z--;
	}


	// chunk rad
	int chunk_radius = 3;

	for (int chunk_x = -chunk_radius; chunk_x < chunk_radius; ++chunk_x)
	{
		for (int chunk_z = -chunk_radius; chunk_z < chunk_radius; ++chunk_z)
		{
			bool already_checked = false;

			for (chunk c : chunks)
			{
				if (c.x - (chunk_x + x) == 0 && c.z - (chunk_z + z) == 0) {
					already_checked = true;
					break;
				}
			}

			if (already_checked)
			{
				continue;
			}

			chunk chunk = get_chunk_data((chunk_x + x) * 16, (chunk_z + z) * 16);
			chunk.x = chunk_x + x;
			chunk.z = chunk_z + z;

			if (chunk.blocks->empty()) 
			{
				continue;
			}

			mutex.lock();
			chunks.push_back(chunk);
			mutex.unlock();
		}
	}

	env->DeleteLocalRef(player);
}

double distance_between_points(vec3d point1, vec3d point2) {
	return sqrt(pow(point2.x - point1.x, 2) + pow(point2.y - point1.y, 2) + pow(point2.z - point1.z, 2));
}

void validate_chunks()
{
	jobject world = env->GetObjectField(mc_instance, the_world_field);
	jobject player = env->GetObjectField(mc_instance, the_player_field);
	double player_x = env->GetDoubleField(player, player_x_field);
	double player_y = env->GetDoubleField(player, player_y_field);
	double player_z = env->GetDoubleField(player, player_z_field);
	vec3d player_pos(player_x, player_y, player_z);

	std::vector<chunk>::iterator it = chunks.begin();
	while (it != chunks.end()) {

		double chunk_center_x = it->x * 16 + 8;
		double chunk_center_z = it->z * 16 + 8;
		double distance = distance_between_points(player_pos, vec3d(chunk_center_x, player_y, chunk_center_z));

		if (distance > max_xz_search) {
			it = chunks.erase(it);
		}
		else {
			std::vector<block>* blocks = it->blocks;

			for (int i = 0; i < blocks->size(); i++) {
				block block = blocks->data()[i];
				int block_id = get_id_from_block(world, block.position);

				if (!blocks::is_block_whitelisted(block_id)) {
					blocks->erase(blocks->begin() + i);
					--i;
				}
			}
			++it;
		}
	}

	env->DeleteLocalRef(player);
	env->DeleteLocalRef(world);
}

void initialize_fields(JClassLoader* cl)
{
	jclass world_class = cl->find_class("net/minecraft/world/World");
	m_get_block = env->GetMethodID(world_class, "func_147439_a", "(III)Lnet/minecraft/block/Block;");
	m_is_air_block = env->GetMethodID(world_class, "func_147437_c", "(III)Z");
	m_get_chunk_provider = env->GetMethodID(world_class, "func_72863_F", "()Lnet/minecraft/world/chunk/IChunkProvider;");

	block_class = cl->find_class("net/minecraft/block/Block");
	m_get_id_from_block = env->GetStaticMethodID(block_class, "func_149682_b", "(Lnet/minecraft/block/Block;)I");

	render_manager_class = cl->find_class("net/minecraft/client/renderer/entity/RenderManager");
	render_pos_x_field = env->GetStaticFieldID(render_manager_class, "field_78725_b", "D");
	render_pos_y_field = env->GetStaticFieldID(render_manager_class, "field_78726_c", "D");
	render_pos_z_field = env->GetStaticFieldID(render_manager_class, "field_78723_d", "D");

	jclass active_render_info_class = cl->find_class("net/minecraft/client/renderer/ActiveRenderInfo");
	jclass int_buffer_class = env->FindClass("java/nio/IntBuffer");
	jclass float_buffer_class = env->FindClass("java/nio/FloatBuffer");

	jfieldID modelview_field = env->GetStaticFieldID(active_render_info_class, "field_74594_j", "Ljava/nio/FloatBuffer;");
	jfieldID projection_field = env->GetStaticFieldID(active_render_info_class, "field_74595_k", "Ljava/nio/FloatBuffer;");

	buffer_get_method = env->GetMethodID(int_buffer_class, "get", "(I)I");
	modelview_buffer = env->GetStaticObjectField(active_render_info_class, modelview_field);
	projection_buffer = env->GetStaticObjectField(active_render_info_class, projection_field);

	jclass minecraft_class = cl->find_class("net/minecraft/client/Minecraft");
	jmethodID get_minecraft_method = env->GetStaticMethodID(minecraft_class, "func_71410_x", "()Lnet/minecraft/client/Minecraft;");
	mc_instance = env->CallStaticObjectMethod(minecraft_class, get_minecraft_method);
	the_player_field = env->GetFieldID(minecraft_class, "field_71439_g", "Lnet/minecraft/client/entity/EntityClientPlayerMP;");
	the_world_field = env->GetFieldID(minecraft_class, "field_71441_e", "Lnet/minecraft/client/multiplayer/WorldClient;");;

	jclass entity_class = cl->find_class("net/minecraft/entity/Entity");
	player_x_field = env->GetFieldID(entity_class, "field_70165_t", "D");
	player_y_field = env->GetFieldID(entity_class, "field_70163_u", "D");
	player_z_field = env->GetFieldID(entity_class, "field_70161_v", "D");

	env->DeleteLocalRef(entity_class);
	env->DeleteLocalRef(minecraft_class);
	env->DeleteLocalRef(active_render_info_class);
	env->DeleteLocalRef(int_buffer_class);
	env->DeleteLocalRef(float_buffer_class);
	env->DeleteLocalRef(world_class);
}

void clear_chunks()
{
	for (chunk c : chunks)
	{
		c.blocks->clear();
		delete c.blocks;
	}

	chunks.clear();
}

void xray::render_gui() {
	auto io = ImGui::GetIO();

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f)); // Couleur de fond
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.30f, 0.30f, 0.30f, 1.0f));   // Couleur des boutons
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.40f, 0.40f, 0.40f, 1.0f)); // Couleur des boutons survolés
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));  // Couleur des boutons actifs
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));  // Padding de la fenêtre

	ImGui::Begin("X-Ray", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::PushFont(font);

	std::vector<whitelisted_block>* vec = blocks::get_whitelisted_blocks();

	if (ImGui::BeginTabBar("Categories")) {
		if (ImGui::BeginTabItem("Settings")) {
			ImGui::Checkbox("Enabled", &enabled);
			ImGui::SliderInt("Min Y Search", &min_y_search, 0, 255);
			ImGui::SliderInt("Max Y Search", &max_y_search, 25, 255);
			ImGui::SliderInt("Range", &max_xz_search, 1, 100);

			ImGui::Spacing();

			if (ImGui::Button("Reload Chunks")) {
				clear_chunks();
			}

			ImGui::SameLine();

			if (ImGui::Button("Self Destruct")) {
				stop = true;
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Add Block")) {
			static char name[128] = "";
			static int id;
			static ImVec4 color = ImVec4(0, 0, 0, 0);

			ImGui::InputText("Block Name", name, IM_ARRAYSIZE(name));
			ImGui::InputInt("Block ID", &id);

			ImGui::Spacing();

			ImGui::ColorEdit3("Color", (float*)&color);

			ImGui::Spacing();

			if (ImGui::Button("Add Block")) {
				vec3f color_vec = vec3f(color.x, color.y, color.z);

				vec->push_back(whitelisted_block(std::string(name), id, color_vec));

				clear_chunks();
			}



			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Whitelisted Blocks")) 
		{
			ImGui::Columns(2, NULL, true);

			for (int i = 0; i < vec->size(); ++i) {
				whitelisted_block& b = vec->at(i);

				char buffer[128];
				std::strncpy(buffer, b.name.c_str(), sizeof(buffer));
				if (ImGui::InputText(("##Name" + std::to_string(i)).c_str(), buffer, sizeof(buffer))) {
					b.name = buffer;
				}

				ImGui::NextColumn();

				ImGui::InputInt(("ID##" + std::to_string(i)).c_str(), &b.block_id);
				
				ImGui::NextColumn();

				ImGui::ColorEdit3(("Color##" + std::to_string(i)).c_str(), (float*)&b.color);

				ImGui::NextColumn();

				std::string button_id = "Remove##" + std::to_string(i);
				if (ImGui::Button(button_id.c_str())) {
					vec->erase(vec->begin() + i);
					break;
				}

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::NextColumn();
			}

			ImGui::Columns(1);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Config")) {
			static char file_name[128] = "config.json";

			std::string path = config::getPath();
			const char* cstr_path = path.c_str();

			ImGui::Columns(2, NULL, false);

			ImGui::Text("Config Name");
			ImGui::InputText("##ConfigName", file_name, IM_ARRAYSIZE(file_name));


			if (ImGui::Button("Export Config")) {
				config::export_config(file_name);
			}

			ImGui::SameLine();

			if (ImGui::Button("Config Folder")) {
				ShellExecute(NULL, "open", cstr_path, NULL, NULL, SW_SHOWDEFAULT);
			}

			ImGui::NextColumn();

			ImGui::Text("Existing configs:\n");
			std::vector<std::string> jsonnames = config::getJsonFileNames();

			namespace fs = std::filesystem;

			for (const auto& name : jsonnames) {
				ImGui::Text("%s", name.c_str());

				if (ImGui::Button(("Import##" + name).c_str())) {
					config::import_config(name);
					clear_chunks();
				}

				ImGui::SameLine();

				if (ImGui::Button(("Remove##" + name).c_str())) {
					fs::remove(path + name);
				}
			}

			ImGui::Columns(1);

			ImGui::EndTabItem();
		}



		if (ImGui::BeginTabItem("Updater (!)") && blocks::dev)
		{
			ImGui::Text("Paladium Ore ID: 548");
			ImGui::Text("Oak Drawer ID: 2991");


			ImGui::InputInt("Ore Offset", &blocks::ore_offset);
			ImGui::InputInt("Chest Offset", &blocks::chest_offset);

			ImGui::Text
			("COMMENT UTILISER: Si tu ne sais pas compiler et update les ids des blocks utilise cette option. \nPar exemple tu as comme ID pour le paladium dans le cheat qui est egale a 548, tu vas chercher dans le client l'ID actuelle du paladium et effectuer le calcul suivant: \noffset = pala_id - 548\nEt tu vas effectuer la meme chose pour les coffres en oubliant pas d'appuyer sur 'Reload'!!\n");


			if (ImGui::Button("Reload"))
			{
				vec->clear();
				blocks::initialize();
				config::download_file();

				clear_chunks();
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::PopFont();
	ImGui::End();

	ImGui::PopStyleVar();
	ImGui::PopStyleColor(4);
}



#include <dwmapi.h>

void xray::initialize(HMODULE handle)
{
	ImGui::CreateContext();

	HWND hwnd = FindWindowA("LWJGL", NULL);
	if (hwnd == NULL) {
		MessageBoxA(NULL, "Could not find the target window.", "Error", MB_ICONERROR);
		exit(1);
	}

	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplOpenGL2_Init();

	imtheme::init_theme();
	font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 20);

	hook::initialize_hooks();

	JavaVM* jvm = utils::get_jvm_instance();
	if (!jvm) {
		MessageBoxA(NULL, "Could not find JVM handle", NULL, MB_ICONERROR);
		exit(1);
	}

	jvmtiEnv* jvmti;
	jvm->AttachCurrentThread((void**)&env, 0);
	jvm->GetEnv((void**)&env, JNI_VERSION_1_8);
	jvm->GetEnv((void**)&jvmti, JVMTI_VERSION_1_2);

	JClassLoader* class_loader = utils::get_classloader_by_thread_name(env, jvmti, "Client thread");
	if (!class_loader) {
		MessageBoxA(NULL, "Could not find Minecraft's class loader", NULL, MB_ICONERROR);
		exit(1);
	}

	initialize_fields(class_loader);

	config::download_file();
	blocks::initialize();

	MessageBox(NULL, "Injected Successfully! Press \"INSERT\" to open.", "Success", MB_OK | MB_ICONINFORMATION);

	int i = 2000;

	while (!GetAsyncKeyState(VK_END) && !stop) {
		jobject world = env->GetObjectField(mc_instance, the_world_field);
		bool is_world_null = world == nullptr;
		env->DeleteLocalRef(world);

		if (is_world_null) {
			clear_chunks();
			continue;
		}

		if (i > 2000) {
			find_chunks();
			i = 0;
		}

		if (i % 10 == 0) {
			validate_chunks();
		}

		update_matrix();
		update_render_pos();

		i++;
		Sleep(1);
	}

	clear_chunks();
	hook::uninitialize_hooks();
	ImGui::DestroyContext();
	FreeLibraryAndExitThread(handle, 0);
}
