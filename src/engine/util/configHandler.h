#pragma once

#include <map>
#include <unordered_map>
#include <string>

struct ConfigFile {
	std::string path;
	
	std::map<std::string, int> intMap;
	std::map<std::string, bool> boolMap;
	std::map<std::string, std::string> stringMap;
	
	inline void setValue(std::string key, int value) {intMap.insert({key, value});}
	inline void setValue(std::string key, bool value) {boolMap.insert({key, value});}
	inline void setValue(std::string key, std::string value) {stringMap.insert({key, value});}
};

class ConfigHandler {
	static std::unordered_map<std::string, ConfigFile> configs;
	
	public:
		static ConfigFile* loadConfig(std::string configFile);
		static void loadConfig(std::string configFile, ConfigFile *config);
		
		static ConfigFile* getConfig(std::string configName);
		
		static void saveConfig(ConfigFile *config);
};