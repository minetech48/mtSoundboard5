#include "soundboard.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

ma_engine SBAudio::engine, SBAudio::engine2;
ma_engine_config SBAudio::engineConfig;

ma_device_info *SBAudio::playbackInfos;
ma_uint32 SBAudio::playbackInfoSize;

void SBAudio::initialize() {
	
	ma_context context;
	if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS) {
		printf("Failed to initialize miniaudio context!\n");
		return;
	}
	
	if (ma_context_get_devices(&context, &playbackInfos,
						&playbackInfoSize, NULL, NULL) != MA_SUCCESS) {
		printf("Failed to initialize miniaudio playback!\n");
		return;
	}
	
	engineConfig = ma_engine_config_init();
	engineConfig.pPlaybackDeviceID = &(playbackInfos[0].id);
	
	if (ma_engine_init(&engineConfig, &engine) != MA_SUCCESS) {
		printf("Failed to initialize miniaudio engine!\n");
		return;
	}
	
	engineConfig.pPlaybackDeviceID = &(playbackInfos[4].id);
	if (ma_engine_init(&engineConfig, &engine2) != MA_SUCCESS) {
		printf("Failed to initialize miniaudio engine!\n");
		return;
	}
	
	
	
	for (ma_uint32 iDevice = 0; iDevice < playbackInfoSize; iDevice += 1) {
		printf("%d - %s\n", iDevice, playbackInfos[iDevice].name);
	}
	
	printf("\tSuccess!\n");
}

void SBAudio::playSound(std::string path) {
	ma_engine_play_sound(&engine, path.c_str(), NULL);
	ma_engine_play_sound(&engine2, path.c_str(), NULL);
}