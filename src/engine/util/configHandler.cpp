#include "configHandler.h"

#include "fileIO.h"
#include "engine/logger.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

std::unordered_map<std::string, ConfigFile> ConfigHandler::configs;

ConfigFile* ConfigHandler::loadConfig(std::string configFile) {
	std::string configPath = FileIO::findFile(configFile, ".txt");
	logf("Loading config file: %s\n", configFile.c_str());
	
	configs.erase(configFile);
	configs.insert({configFile, {}});
	
	ConfigFile *config = &configs[configFile];
	config->path = configPath;
	
	if (!fs::exists(configPath)) {
		logf("Config file \"%s\" not found!", configPath.c_str());
	}else{
		std::ifstream file(configPath);
		
		std::string line;
		int delimiterIndex;
		
		while (std::getline(file, line)) {
			delimiterIndex = line.find_last_of(':');
			
			config->setValue(
				line.substr(0, delimiterIndex),
				std::stoi(line.substr(delimiterIndex+1)));
			
		}
	}
	
	return config;
}

ConfigFile* ConfigHandler::getConfig(std::string ConfigFile) {
	if (configs.contains(ConfigFile))
		return &configs[ConfigFile];
	
	return loadConfig(ConfigFile);
}

#define itterate(map, expression) for (auto& entry : config->map) {expression}
static void saveConfig(ConfigFile *config) {
	std::ofstream file;
	file.open(config->path);
	
	itterate(intMap, file << entry.first + ":" + std::to_string(entry.second) + '\n';)
	itterate(boolMap, file << entry.first + ":" + std::to_string(entry.second) + '\n';)
	itterate(stringMap, file << entry.first + ":" + entry.second + '\n';)
	
	file.close();
}
#undef itterate