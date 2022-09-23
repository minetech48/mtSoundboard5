#ifdef _WIN32
#include "keyboardHook.h"
#include "winkeymap.h"
#include "soundboard.h"

#include <iostream>
#include <queue>
#include <SDL2/SDL.h>

#include <Windows.h>

static int modifierMask;

static std::queue<SDL_Event> eventQueue;

static HHOOK hook;

#define modCase(key, SDL_Mask) \
	case (key):\
		modifierMask^= SDL_Mask;\
		break;

LRESULT CALLBACK KeyboardCallback(int nCode, WPARAM wParam, LPARAM lParam) {
	int keycode;

	KBDLLHOOKSTRUCT *event = (KBDLLHOOKSTRUCT*) lParam;
	if (wParam == WM_KEYDOWN) {
		keycode = (int) event->vkCode;//get key code
	}else{
		return CallNextHookEx( NULL, nCode, wParam, lParam );
	}
	
	//handling keypress
	keycode = WinKeyToSDLKey(keycode);
	
	switch (keycode) {//binding modifiers to keypresses
		modCase(SDLK_LSHIFT, KMOD_LSHIFT)
		modCase(SDLK_RSHIFT, KMOD_RSHIFT)
		modCase(SDLK_LCTRL, KMOD_LCTRL)
		modCase(SDLK_RCTRL, KMOD_RCTRL)
		modCase(SDLK_LALT, KMOD_LALT)
		modCase(SDLK_RALT, KMOD_RALT)
		modCase(SDLK_LGUI, KMOD_LGUI)
		modCase(SDLK_RGUI, KMOD_RGUI)
	}
	
	int modifier = modifierMask;
	if (GetKeyState(VK_CAPITAL) & 0x0001)
		modifier|= KMOD_CAPS;
	if (GetKeyState(VK_NUMLOCK) & 0x0001)
		modifier|= KMOD_NUM;
	if (GetKeyState(VK_SCROLL) & 0x0001)
		modifier|= KMOD_SCROLL;
	
	SDL_Event testEvent;
	testEvent.type = SDL_KEYDOWN;
	testEvent.key.keysym.sym = keycode;
	testEvent.key.keysym.mod = modifier;
	testEvent.key.padding2 = 1;//hijacking variable to determine where keypress originates
	
	eventQueue.push(testEvent);
	
	//passing to next hook, todo: block this call if sound was played
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}
#undef modCase

void KeyboardHook::initialize() {
	printf("\tInitializing for Windows system.\n");
	
	modifierMask = 0;
	
	//Set Windows global keyboard hook
	hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardCallback, NULL, 0);
	
	printf("\tSuccessfully initialized Windows keyboard hook!\n");
}
void KeyboardHook::pollEvents() {
	while (!eventQueue.empty()) {
		//printf("key: %d\n", eventQueue.front().key.keysym.sym);
		Soundboard::SDLEventHandler(eventQueue.front());
		eventQueue.pop();
	}
}

void KeyboardHook::uninitialize() {
	UnhookWindowsHookEx(hook);
}

#endif