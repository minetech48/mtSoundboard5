#pragma once

#define pairCase(sdlkey, xkey) case xkey: return sdlkey;

int XKeyToSDLKey(int xkey) {//todo, investigate auto-generation to file from setting keybinds
	switch (xkey) {
		pairCase(27, 9)
		pairCase(1073741882, 67)
		pairCase(1073741883, 68)
		pairCase(1073741884, 69)
		pairCase(1073741885, 70)
		pairCase(1073741886, 71)
		pairCase(1073741887, 72)
		pairCase(1073741888, 73)
		pairCase(1073741889, 74)
		pairCase(1073741890, 75)
		pairCase(1073741891, 76)
		pairCase(1073741892, 95)
		pairCase(1073741893, 96)
		pairCase(1073741894, 107)
		pairCase(1073741895, 78)
		pairCase(1073741896, 127)
		pairCase(96, 49)
		pairCase(49, 10)
		pairCase(50, 11)
		pairCase(51, 12)
		pairCase(52, 13)
		pairCase(53, 14)
		pairCase(54, 15)
		pairCase(55, 16)
		pairCase(56, 17)
		pairCase(57, 18)
		pairCase(48, 19)
		pairCase(45, 20)
		pairCase(61, 21)
		pairCase(8, 22)
		pairCase(1073741897, 118)
		pairCase(1073741898, 110)
		pairCase(1073741899, 112)
		pairCase(1073741907, 77)
		pairCase(1073741908, 106)
		pairCase(1073741909, 63)
		pairCase(1073741910, 82)
		pairCase(9, 23)
		pairCase(113, 24)
		pairCase(119, 25)
		pairCase(101, 26)
		pairCase(114, 27)
		pairCase(116, 28)
		pairCase(121, 29)
		pairCase(117, 30)
		pairCase(105, 31)
		pairCase(111, 32)
		pairCase(112, 33)
		pairCase(91, 34)
		pairCase(93, 35)
		pairCase(92, 51)
		pairCase(127, 119)
		pairCase(1073741901, 115)
		pairCase(1073741902, 117)
		pairCase(1073741919, 79)
		pairCase(1073741920, 80)
		pairCase(1073741921, 81)
		pairCase(1073741911, 86)
		pairCase(1073741881, 66)
		pairCase(97, 38)
		pairCase(115, 39)
		pairCase(100, 40)
		pairCase(102, 41)
		pairCase(103, 42)
		pairCase(104, 43)
		pairCase(106, 44)
		pairCase(107, 45)
		pairCase(108, 46)
		pairCase(59, 47)
		pairCase(39, 48)
		pairCase(13, 36)
		pairCase(1073741916, 83)
		pairCase(1073741917, 84)
		pairCase(1073741918, 85)
		pairCase(1073742049, 50)
		pairCase(122, 52)
		pairCase(120, 53)
		pairCase(99, 54)
		pairCase(118, 55)
		pairCase(98, 56)
		pairCase(110, 57)
		pairCase(109, 58)
		pairCase(44, 59)
		pairCase(46, 60)
		pairCase(47, 61)
		pairCase(1073742053, 62)
		pairCase(1073741906, 111)
		pairCase(1073741913, 87)
		pairCase(1073741914, 88)
		pairCase(1073741915, 89)
		pairCase(1073741912, 104)
		pairCase(1073742048, 37)
		pairCase(1073742051, 133)
		pairCase(1073742050, 64)
		pairCase(32, 65)
		pairCase(1073742054, 108)
		pairCase(1073741942, 135)
		pairCase(1073742052, 105)
		pairCase(1073741904, 113)
		pairCase(1073741905, 116)
		pairCase(1073741903, 114)
		pairCase(1073741922, 90)
		pairCase(1073741923, 91)
	}
	
	return -1;
}

#undef pairCase