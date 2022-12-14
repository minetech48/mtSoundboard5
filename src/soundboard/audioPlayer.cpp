#include "soundboard.h"
#include "engine/gui/guiData.h"
#include "engine/logger.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

int SBAudio::deviceIndex = 0, SBAudio::deviceIndex2 = 0;

ma_engine_config SBAudio::engineConfig;
ma_resource_manager SBAudio::resourceManager;

ma_device_info *SBAudio::playbackInfos;
ma_uint32 SBAudio::playbackInfoSize;

std::vector<ma_engine*> SBAudio::playbackEngines;

std::unordered_set<ma_sound*> SBAudio::activeSounds;

void SBAudio::initialize() {
	//initializing context
	ma_context context;
	if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS) {
		logf("\tFailed to initialize miniaudio context!\n");
		return;
	}
	
	//loading playback devices
	if (ma_context_get_devices(&context, &playbackInfos,
						&playbackInfoSize, NULL, NULL) != MA_SUCCESS) {
		logf("\tFailed to initialize miniaudio playback!\n");
		return;
	}
	
	//initializing resource manager

	ma_resource_manager_config config = ma_resource_manager_config_init();
	ma_result result = ma_resource_manager_init(&config, &resourceManager);
	if (result != MA_SUCCESS) {
		logf("Failed to initialize resource manager, %d\n", result);
		return;
	}
	
	//initializing engines
	ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
	engineConfig = ma_engine_config_init();
	engineConfig.pResourceManager = &resourceManager;
	//default audio engine
	ma_engine *engine;
	// engine = new ma_engine();
	// if (ma_engine_init(NULL, engine) != MA_SUCCESS) {
	// 	logf("\tFailed to initialize miniaudio engine!\n");
	// 	return;
	// }
	// playbackEngines.push_back(engine);
	//specific device engines
	std::vector<std::string> *deviceNames = GUIData::addList("audioDeviceNames", {});
	for (int i = 0; i < playbackInfoSize; i++) {//todo, init devices, 2 engines
		logf("\tInitializing engine [%d] - %s\n", i, playbackInfos[i].name);
		
		engineConfig.pPlaybackDeviceID = &(playbackInfos[i].id);
		
		engine = new ma_engine();
		if (ma_engine_init(&engineConfig, engine) != MA_SUCCESS) {
			logf("\tFailed to initialize miniaudio engine!\n");
			return;
		}
		playbackEngines.push_back(engine);
		
		deviceNames->push_back(playbackInfos[i].name);
		
		if (Soundboard::config->stringMap["AudioDevice1"] == playbackInfos[i].name)
			deviceIndex = i;
		if (Soundboard::config->stringMap["AudioDevice2"] == playbackInfos[i].name)
			deviceIndex2 = i;
	}
	
	GUIData::setString("AudioDevice1", SBAudio::playbackInfos[SBAudio::deviceIndex].name);
	GUIData::setString("AudioDevice2", SBAudio::playbackInfos[SBAudio::deviceIndex2].name);
	
	logf("\tSuccess!\n");
}

void SBAudio::uninit() {
	for (ma_engine *engine : playbackEngines) {
		ma_engine_uninit(engine);
	}
	playbackEngines.clear();
	
	playbackInfos = NULL;
}

void SBAudio::update() {
	for (auto i = activeSounds.begin(); i != activeSounds.end(); i++) {
		if (ma_sound_at_end(*i)) {
			ma_sound_uninit(*i);
			activeSounds.erase(i);
			break;//me not wanting to deal with itterators
		}
	}
}

void SBAudio::stopAll() {
	while (!activeSounds.empty()) {
		auto it = activeSounds.begin();
		
		ma_sound_uninit(*it);
		activeSounds.erase(it);
	}
}

void startAudioFile(ma_engine* engine, std::string path) {
	ma_result result;
	ma_sound *sound = new ma_sound();
	
	result = ma_sound_init_from_file(engine, path.c_str(),//todo: stream only if sound is large
		 MA_SOUND_FLAG_STREAM | MA_SOUND_FLAG_ASYNC, NULL, NULL, sound);
	if (result != MA_SUCCESS) {
		logf("Failed to load sound file, %d: %s\n", result, path.c_str());
		return;
	}
	
	SBAudio::activeSounds.insert(sound);
	ma_sound_start(sound);
}
void SBAudio::playSound(std::string path) {
	logf("playing sound: %s\n", path.c_str());
	
	if (deviceIndex > 0 && deviceIndex < playbackEngines.size())
		startAudioFile(playbackEngines[deviceIndex], path);
	
	if (deviceIndex2 > 0 && deviceIndex2 < playbackEngines.size() && deviceIndex != deviceIndex2)
		startAudioFile(playbackEngines[deviceIndex2], path);
}