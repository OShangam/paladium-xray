#define _CRT_SECURE_NO_WARNINGS

#include "blocks.hpp"
#include "config.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <curl/curl.h>
#include <shlobj.h>
#include <thread>
#include <chrono> 

using json = nlohmann::json;
namespace fs = std::filesystem;

std::string config::getPath() {
	const char* appdata = std::getenv("APPDATA");
	if (!appdata) {
		std::cerr << "Erreur: Impossible de trouver la variable d'environnement APPDATA" << std::endl;
	}

	std::string path = std::string(appdata) + "\\.paladium\\7CpljoaZEr\\";
	fs::create_directory(path);

	return path;

}

void config::export_config(const std::string& filename) {
	std::vector<whitelisted_block>* whitelisted_blocks = blocks::get_whitelisted_blocks();

	json j;
	for (whitelisted_block& block : *whitelisted_blocks) {
		j["blocks"].push_back({
			{"name", block.name},
			{"id", block.block_id},
			{"color", {block.color.r, block.color.g, block.color.b}}
			});
	}

	std::ofstream file((getPath() + filename));
	if (file.is_open()) {
		file << j.dump(4);
		file.close();
	}
}

void config::import_config(const std::string& filename) {
	std::vector<whitelisted_block>* whitelisted_blocks = blocks::get_whitelisted_blocks();

	std::ifstream file(getPath() + filename);

	if (!file.is_open()) {
		return;
	}

	json j;
	file >> j;
	file.close();

	whitelisted_blocks->clear();

	for (const auto& item : j["blocks"]) {
		std::string name = item["name"];
		int block_id = item["id"];
		vec3f color = vec3f(
			item["color"][0],
			item["color"][1],
			item["color"][2]
		);

		whitelisted_blocks->push_back(whitelisted_block(name, block_id, color));
	}
}

std::vector<std::string> config::getJsonFileNames() {
	std::vector<std::string> fileNames;

	try {
		for (const auto& entry : fs::directory_iterator(getPath())) {
			if (fs::is_regular_file(entry.path()) && entry.path().extension() == ".json") {
				fileNames.push_back(entry.path().filename().string());
			}
		}
	}
	catch (const fs::filesystem_error& e) {
		return fileNames;
	}

	return fileNames;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	std::ofstream* ofs = static_cast<std::ofstream*>(userp);
	ofs->write(static_cast<char*>(contents), size * nmemb);
	return size * nmemb;
}

void config::download_file()
{
	CURL* curl;
	CURLcode res;
	curl = curl_easy_init();

	std::string output_path = config::getPath();

	std::vector<std::string> urls = {
		"https://pastebin.com/raw/8HKut2zZ",
		"https://pastebin.com/raw/MVMCaWmU"
	};

	std::vector<std::string> filenames = {
		"default_ores.json",
		"default_chests.json"
	};

	if (curl)
	{
		for (size_t i = 0; i < urls.size(); ++i) {
			std::string url = urls[i];
			std::string filename = filenames[i];
			std::string full_output_path = output_path + filename;

			std::ofstream ofs(full_output_path, std::ios::binary);
			if (!ofs.is_open()) {
				continue;
			}

			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ofs);

			res = curl_easy_perform(curl);

			ofs.close();
		}

		curl_easy_cleanup(curl);
	}
}

std::string config::getVersion() {
	CURL* curl;
	CURLcode res;
	std::string version;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://pastebin.com/raw/3QDg74qj");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &version);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}

	return version;
}