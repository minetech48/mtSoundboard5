#pragma once

#include <filesystem>
#include <string>

class FileIO {
	public:
		static std::string findFile(std::string fileName, std::string extension);
		
		static std::string getFileName(std::string filePath);
		static std::string removeFilePath(std::string filePath);
		static std::string getFileExtention(std::string fileName);
};