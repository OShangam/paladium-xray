#include "xray.hpp"
#include "blocks.hpp"
#include "hook.hpp"
#include "utils.hpp"
#include "gl_utils.hpp"

#include <algorithm>
#include <mutex>
#include "imgui/imgui.h"
#include "imtheme.h"
#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_win32.h"


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
int max_y_search = 50;

int max_xz_search = 50.0;

bool xray::gui_open;

void xray::render()
{


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

void xray::render_gui()
{
	auto io = ImGui::GetIO();

	ImGui::Begin("X-Ray", 0, ImGuiWindowFlags_NoCollapse);
	ImGui::PushFont(font);

	std::vector<whitelisted_block>* vec = blocks::get_whitelisted_blocks();

	if (ImGui::BeginTabBar("Categories"))
	{
		if (ImGui::BeginTabItem("Settings"))
		{
			ImGui::Checkbox("Enabled", &enabled);
			ImGui::SliderInt("Min Y Search", &min_y_search, 0, 255);
			ImGui::SliderInt("Max Y Search", &max_y_search, 25, 255);
			ImGui::SliderInt("Range", &max_xz_search, 1, 100);

			if (ImGui::Button("Reload Chunks"))
			{
				clear_chunks();
			}

			ImGui::SameLine();

			if (ImGui::Button("Self Destruct"))
			{
				stop = true;
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Whitelisted Blocks"))
		{
			ImGui::Columns(2, NULL, true);

			for (int i = 0; i < vec->size(); ++i)
			{
				whitelisted_block b = vec->data()[i];
				std::string block_name = b.name;

				ImGui::Text("%s", block_name.c_str());
				ImGui::NextColumn();

				std::string button_id = "Remove##" + std::to_string(i);
				if (ImGui::Button(button_id.c_str()))
				{
					vec->erase(vec->begin() + i);
					break;
				}

				ImGui::NextColumn();
			}

			ImGui::Columns(1);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Add Block"))
		{
			static std::string name;
			static int id;
			static ImVec4 color = ImVec4(0, 0, 0, 0);

			ImGui::InputInt("Block ID", &id);
			ImGui::ColorEdit3("Color", (float*)&color);

			if (ImGui::Button("Add Block"))
			{
				vec3f color_vec = vec3f(color.x, color.y, color.z);

				vec->push_back(whitelisted_block(name, id, color_vec));
				clear_chunks();
			}
			
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Config"))
		{
			ImGui::Text("Your Configs is: %s", blocks::detect_minerals ? "Minerals" : "Chests");

			if (ImGui::Checkbox("Detect Minerals", &blocks::detect_minerals)) {
				vec->clear();
				blocks::initialize();
				clear_chunks();
			}
		}


		ImGui::EndTabBar();
	}

	ImGui::PopFont();
	ImGui::End();
}

void xray::initialize(HMODULE handle)
{
	// we don't want paladium to check for imgui.ini file
	//auto io = ImGui::GetIO();
	//io.IniFilename = NULL;
	//io.LogFilename = NULL;

	ImGui::CreateContext();
	ImGui_ImplWin32_Init(FindWindowA("LWJGL", NULL));
	ImGui_ImplOpenGL2_Init();
	imtheme::init_theme();

	font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 20);

	hook::initialize_hooks();

	JavaVM* jvm = utils::get_jvm_instance();

	if (!jvm) 
	{
		MessageBoxA(NULL, "Could not find jvm handle", NULL, MB_ICONERROR);
		exit(1);
	}

	jvmtiEnv* jvmti;

	jvm->AttachCurrentThread((void**)&env, 0);
	jvm->GetEnv((void**)&env, JNI_VERSION_1_8);
	jvm->GetEnv((void**)&jvmti, JVMTI_VERSION_1_2);

	JClassLoader* class_loader = utils::get_classloader_by_thread_name(env, jvmti, "Client thread");

	if (!class_loader)
	{
		MessageBoxA(NULL, "Could not find minecraft's class loader", NULL, MB_ICONERROR);
		exit(1);
	}

	initialize_fields(class_loader);
	blocks::initialize();

	int i = 2000;
	MessageBox(NULL, "Injected Successfully! Press \"INSERT\" to open.", "Success", MB_OK | MB_ICONINFORMATION);
	while (!GetAsyncKeyState(VK_END) && !stop) 
	{
		jobject world = env->GetObjectField(mc_instance, the_world_field);
		bool is_world_null = world == nullptr;
		env->DeleteLocalRef(world);

		if (is_world_null)
		{
			clear_chunks();
			continue;
		}

		if (i > 2000) {
			find_chunks();
			i = 0;
		}

		if (i % 10 == 0)
		{
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