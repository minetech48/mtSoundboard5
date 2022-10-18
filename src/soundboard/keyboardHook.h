#include <string>

class KeyboardHook {
	public:
		static void initialize();
		static void uninitialize();
		static void pollEvents();
		
		static std::string toString(int keyCode);
		static void pressPTTKey();
		static void releasePTTKey();
	
	static bool naitiveKeySet(int keyCode);
};