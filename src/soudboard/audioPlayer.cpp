#include "soundboard.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

int SBAudio::engineIndex;

ma_engine_config SBAudio::engineConfig;
ma_resource_manager SBAudio::resourceManager;

std::vector<ma_engine*> SBAudio::playbackEngines;

std::unordered_set<ma_sound*> SBAudio::activeSounds;

void SBAudio::initialize() {
	
	//initializing context
	ma_context context;
	if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS) {
		printf("\tFailed to initialize miniaudio context!\n");
		return;
	}
	
	//loading playback devices
	ma_device_info *playbackInfos;
	ma_uint32 playbackInfoSize;
	if (ma_context_get_devices(&context, &playbackInfos,
						&playbackInfoSize, NULL, NULL) != MA_SUCCESS) {
		printf("\tFailed to initialize miniaudio playback!\n");
		return;
	}
	
	//initializing resource manager

	ma_resource_manager_config config = ma_resource_manager_config_init();
	ma_result result = ma_resource_manager_init(&config, &resourceManager);
	if (result != MA_SUCCESS) {
		printf("Failed to initialize resource manager, %d\n", result);
		return;
	}
	
	//initializing engines
	ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
	engineConfig = ma_engine_config_init();
	engineConfig.pResourceManager = &resourceManager;
	//default audio engine
	ma_engine *engine = new ma_engine();
	if (ma_engine_init(&engineConfig, engine) != MA_SUCCESS) {
		printf("\tFailed to initialize miniaudio engine!\n");
		return;
	}
	playbackEngines.push_back(engine);
	//specific device engines
	for (int i = 0; i < playbackInfoSize; i++) {
		printf("\tInitializing engine [%d] - %s\n", i, playbackInfos[i].name);
		
		engineConfig.pPlaybackDeviceID = &(playbackInfos[i].id);
		
		engine = new ma_engine();
		if (ma_engine_init(&engineConfig, engine) != MA_SUCCESS) {
			printf("\tFailed to initialize miniaudio engine!\n");
			return;
		}
		playbackEngines.push_back(engine);
	}
	
	engineIndex = 0;
	
	printf("\tSuccess!\n");
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
	
	result = ma_sound_init_from_file(engine, path.c_str(),
		 MA_SOUND_FLAG_STREAM | MA_SOUND_FLAG_ASYNC, NULL, NULL, sound);
	if (result != MA_SUCCESS) {
		printf("Failed to load sound file, %d: %s\n", result, path.c_str());
		return;
	}
	
	SBAudio::activeSounds.insert(sound);
	ma_sound_start(sound);
}
void SBAudio::playSound(std::string path) {
	startAudioFile(playbackEngines[0], path);
	
	if (engineIndex > 0)
		startAudioFile(playbackEngines[engineIndex], path);
}