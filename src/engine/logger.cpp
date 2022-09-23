#include "logger.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <stdarg.h>

FILE* outFile;

void Logger::init() {
	std::time_t time = std::time(0);
	
	std::string fileName = "resources/logs/";
	std::string tStr = std::to_string(time);
	
	outFile = fopen((fileName + tStr + ".txt").c_str(), "w+");
}

void logf(const char* str...) {
	va_list args;
	va_start(args, str);
	
	vfprintf(stdout, str, args);
	
	va_start(args, str);
	if (outFile != NULL && outFile) {
		vfprintf(outFile, str, args);
		
		fflush(outFile);
	}
	va_end(args);
}