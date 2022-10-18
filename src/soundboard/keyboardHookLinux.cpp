#ifdef unix
#include "keyboardHook.h"
#include "engine/gui/gui.h"
#include "soundboard.h"
#include "xkeymap.h"
#include "engine/logger.h"
#include <iostream>

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XTest.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <chrono>
#include <linux/input.h>

SDL_SysWMinfo info;
Display* xdisplay;
XEvent event;

static int modifierMask;

void KeyboardHook::initialize() {
	logf("\tInitializing for Unix system.\n");
	
	xdisplay = XOpenDisplay(std::getenv("DISPLAY"));//getting root of current main display
	if (xdisplay == NULL) {
		logf("Failed to find X display, key hook stopping!");
	}
	
	
	XIEventMask mask;
	mask.deviceid = XIAllMasterDevices;
	mask.mask_len = XIMaskLen(XI_LASTEVENT);
	mask.mask = (unsigned char*) calloc(mask.mask_len, sizeof(char));
	
	modifierMask = 0;
	
	XISetMask(mask.mask, XI_RawKeyPress);
	XISetMask(mask.mask, XI_RawKeyRelease);
	XISelectEvents(xdisplay, DefaultRootWindow(xdisplay), &mask, 1);
	
	XSync(xdisplay, 0);
	free(mask.mask);
	
	logf("\tSuccessfully initialized XInput2 keyboard hook!\n");
}

void KeyboardHook::uninitialize() {}

#define modCase(xkey, SDL_Mask) \
	case (xkey):\
		modifierMask^= SDL_Mask;\
		break;
#define modCaseToggle(xkey, SDL_Mask) \
	case (xkey):\
		if (cookie->evtype == XI_RawKeyPress)\
			modifierMask^= SDL_Mask;\
		break;
		
void KeyboardHook::pollEvents() {
	XGenericEventCookie* cookie;
	
	for (int i = XPending(xdisplay); i > 0; i--) {
		XNextEvent(xdisplay, &event);
		
		cookie = &event.xcookie;
		XGetEventData(xdisplay, cookie);
		
		XIRawEvent* eventData = (XIRawEvent*) cookie->data;
        int xkey = eventData->detail;
		int keycode = XKeyToSDLKey(xkey);
		
		//logf("xkey: %d\n", keycode);
		switch (keycode) {//binding modifiers to keypresses
			modCase(SDLK_LSHIFT, KMOD_LSHIFT)
			modCase(SDLK_RSHIFT, KMOD_RSHIFT)
			modCase(SDLK_LCTRL, KMOD_LCTRL)
			modCase(SDLK_RCTRL, KMOD_RCTRL)
			modCase(SDLK_LALT, KMOD_LALT)
			modCase(SDLK_RALT, KMOD_RALT)
			modCase(SDLK_LGUI, KMOD_LGUI)
			modCase(SDLK_RGUI, KMOD_RGUI)
			
			modCaseToggle(SDLK_CAPSLOCK, KMOD_CAPS)
			modCaseToggle(SDLK_NUMLOCKCLEAR, KMOD_NUM)
			modCaseToggle(SDLK_SCROLLLOCK, KMOD_SCROLL)
		}
		
		SDL_Event testEvent;
		
		int kbid = Soundboard::config->intMap["SecondaryKeyboardID"];
		if (kbid != 0 && eventData->sourceid != kbid && Soundboard::globalBinding == false)
			testEvent.key.padding2 = 0;//hijacking variable to determine where keypress originates
		else
			testEvent.key.padding2 = 1;
			
		testEvent.type = cookie->evtype == 13 ? SDL_KEYDOWN : SDL_KEYUP;
		testEvent.key.keysym.sym = keycode;
		testEvent.key.keysym.mod = modifierMask;
		//testEvent.key.padding2 = 1;//hijacking variable to determine where keypress originates
		testEvent.key.padding3 = xkey;//hijacking for naitive keycode
		
		Soundboard::SDLEventHandler(testEvent);
		
		XFreeEventData(xdisplay, cookie);
	}
}
#undef modCase
#undef modCaseToggle

std::string KeyboardHook::toString(int keyCode) {
	if (keyCode == 0) return "";
	return Soundboard::keyToString(XKeyToSDLKey(keyCode & 0xFF0000FF) | (keyCode & 0xFFFF00));
}

void KeyboardHook::pressPTTKey() {
	XTestFakeKeyEvent(xdisplay, Soundboard::globalBindings.getValue("PushToTalk") & 0xFF0000FF, true, 0);
}
void KeyboardHook::releasePTTKey() {
	XTestFakeKeyEvent(xdisplay, Soundboard::globalBindings.getValue("PushToTalk") & 0xFF0000FF, false, 0);
}

#endif