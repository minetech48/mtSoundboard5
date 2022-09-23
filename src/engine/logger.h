#pragma once

#include <string>

class Logger {
	public:
		static void init();
};

void logf(const char* str...);