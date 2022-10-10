#include "configHandler.h"

#include "fileIO.h"
#include "engine/logger.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

bool isNumber(const std::string str) {
    for (char c : str) {
        if (!std::isdigit(c)) return false;
    }
    return true;
}

std::unordered_map<std::string, ConfigFile> ConfigHandler::configs;

ConfigFile* ConfigHandler::loadConfig(std::string configFile) {
	std::string configPath = FileIO::findFile(configFile, ".txt");
	logf("Loading config file: %s.txt\n", configFile.c_str());
	
	configs.erase(configFile);
	configs.insert({configFile, {}});
	
	ConfigFile *config = &configs[configFile];
	
	if (!fs::exists(configPath)) {
		logf("Config file \"%s\" not found!\n", configPath.c_str());
		
		config->path = "resources/config/" + configFile + ".txt";
	}else{
		logf("\tFrom: %s\n", configPath.c_str());
		config->path = configPath;
		
		std::ifstream file(configPath);
		
		std::string line, value;
		int delimiterIndex;
		
		while (std::getline(file, line)) {
			delimiterIndex = line.find_last_of(':');
			
			value = line.substr(delimiterIndex+1);
			
			if (isNumber(value)) {
				config->setValue(
					line.substr(0, delimiterIndex),
					std::stoi(value));
			}else if (value == "true" || value == "false") {
				config->setValue(
					line.substr(0, delimiterIndex),
					value == "true");
			}else{
				config->setValue(
					line.substr(0, delimiterIndex),
					value);
			}
			
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
void ConfigHandler::saveConfig(ConfigFile *config) {
	std::ofstream file;
	file.open(config->path);
	
	itterate(intMap, file << entry.first + ":" + std::to_string(entry.second) + '\n';)
	itterate(boolMap, file << entry.first + ":" + (entry.second ? "true" : "false") + '\n';)
	itterate(stringMap, file << entry.first + ":" + entry.second + '\n';)
	
	file.close();
}
#undef itterate
