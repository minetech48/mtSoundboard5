#pragma once

#include <string>

#include "../engine/engineSystem.h"
#include "../engine/engineEvent.h"
#include "../engine/util/bimap.h"

#include <functional>
#include <map>
#include <SDL.h>
#include <unordered_set>

#include "miniaudio.h"

class Soundboard : public EngineSystem {
		
	static bimap<std::string, int> boardBindings;
	static bimap<std::string, int> soundBindings;
	
	static std::string currentBoard;
	
	static bool boardBinding, soundBinding;
	
	public:
		Soundboard() {title = "Soundboard";}
		
		void initialize();
		void handleEvent(EngineEvent event);
		void update();

		static void SBSelectBoard(EngineEvent event);
		static void SBPlaySound(EngineEvent event);
		static void SBStopSounds(EngineEvent event);
		
		static inline void SBBoardBinding(EngineEvent event) {boardBinding = true;}
		static inline void SBSoundBinding(EngineEvent event) {soundBinding = true;}
		static void SBBoardBindingR(EngineEvent event);
		static void SBSoundBindingR(EngineEvent event);
		
		static void GUIReset(EngineEvent event);
		static void Shutdown(EngineEvent event);

	static void SDLEventHandler(SDL_Event event);
	
	static std::string boardStringHandler(std::string str);
	static std::string soundStringHandler(std::string str);
	
	static std::string keyToString(int keyCode);
	
	
	static void loadBoards();
	static void loadSounds(std::string boardName);
	
	static void setBinding(bimap<std::string, int>* bindingMap, std::string action, int trigger);
	
	static void loadBindings(std::string filePath, bimap<std::string, int>* bindingMap);
	static void saveBindings(std::string filePath, bimap<std::string, int>* bindingMap);
};


static std::map<std::string, std::function<void(EngineEvent)>> eventMap;

class SBAudio {
	public:
		static std::vector<ma_engine*> playbackEngines;
		static ma_engine_config engineConfig;
		static ma_resource_manager resourceManager;
		
		static int engineIndex, engineIndex2;
		static std::unordered_set<ma_sound*> activeSounds;
		
		static void initialize();
		
		static void update();
		
		static void playSound(std::string path);
		
		static void stopAll();
};