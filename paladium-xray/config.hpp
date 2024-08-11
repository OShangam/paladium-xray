#pragma once

#include <Windows.h>
#include <vector>
#include <string>

namespace config
{

	void download_file();

	void export_config(const std::string& filename);
	void import_config(const std::string& filename);

	std::string getPath();
	std::vector<std::string>  getJsonFileNames();
}