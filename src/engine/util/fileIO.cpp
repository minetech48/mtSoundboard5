#include "fileIO.h"
#include "../math.h"

//finding file on system
#define checkFile(name) if (std::filesystem::exists({name})) return name;
std::string FileIO::findFile(std::string fileName, std::string extension) {
	namespace fs = std::filesystem;
	fileName+= extension;
	
	switch (hash(extension)) {
		case hash(".ttf"):
			checkFile("resources/fonts/" + fileName);
			break;
			
		case hash(".yml"):
			checkFile("resources/gui/" + fileName);
			checkFile("resources/gui/menus/" + fileName);
			checkFile("resources/gui/lang/" + fileName)
			break;
			
		case hash(".txt"):
			checkFile("resources/config/" + fileName);
			checkFile("resources/gui/" + fileName);
			checkFile("resources/gui/lists/" + fileName);
			break;
	}
	
	checkFile("resources/" + fileName);
	checkFile(fileName);
	
	return "FILE_NOT_FOUND: " + fileName;
}
#undef checkFile

std::string FileIO::getFileName(std::string filePath) {
	int start = filePath.find_last_of('/')+1;
	int end = filePath.find_last_of('.');
	
	return filePath.substr(start, end-start);
}
std::string FileIO::removeFilePath(std::string filePath) {
	int start = filePath.find_last_of('/')+1;
	
	return filePath.substr(start);
}
std::string FileIO::getFileExtention(std::string fileName){
	int dotPos = fileName.find_last_of('.');
	
	if (dotPos == std::string::npos) return "";
	
	return fileName.substr(dotPos);
}