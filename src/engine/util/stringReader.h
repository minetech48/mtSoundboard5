#pragma once

#include <string>

struct StringReader {
	StringReader(std::string strIn) {str = strIn.c_str();}
	StringReader(const char* strIn) : str(strIn) {}
	
	const char* str;
	int index = -1;
	
	bool hasNext() {return str[index+1] != '\0';}
	
	char current() {
		return str[index];
	}
	char next() 	{++index; return current();}
	char previous()	{--index; return current();}
	
	//Todo: use substring instead of constant appending for performance?
	std::string advanceTo(char c) {
		std::string toReturn = "";
		
		while (hasNext() && next() != c) {
			toReturn+= current();
		}
		
		return toReturn;
	}
	void setPosition(int i) {
		index = i;
	}
};
