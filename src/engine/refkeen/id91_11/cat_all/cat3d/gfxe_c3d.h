/* Catacomb 3-D Source Code
 * Copyright (C) 1993-2014 Flat Rock Software
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

//////////////////////////////////////
//
// Graphics .H file for .C3D
// IGRAB-ed on Tue Dec 21 15:06:10 1993
//
//////////////////////////////////////

typedef enum {
		// Lump Start
		CP_MAINMENUPIC=5,
		CP_NEWGAMEMENUPIC,           // 6
		CP_LOADMENUPIC,              // 7
		CP_SAVEMENUPIC,              // 8
		CP_CONFIGMENUPIC,            // 9
		CP_SOUNDMENUPIC,             // 10
		CP_MUSICMENUPIC,             // 11
		CP_KEYBOARDMENUPIC,          // 12
		CP_KEYMOVEMENTPIC,           // 13
		CP_KEYBUTTONPIC,             // 14
		CP_JOYSTICKMENUPIC,          // 15
		CP_OPTIONSMENUPIC,           // 16
		CP_PADDLEWARPIC,             // 17
		CP_QUITPIC,                  // 18
		CP_JOYSTICKPIC,              // 19
		CP_MENUSCREENPIC,            // 20
		TITLEPIC,                    // 21
		CREDITSPIC,                  // 22
		HIGHSCORESPIC,               // 23
		FINALEPIC,                   // 24
		STATUSPIC,                   // 25
		SIDEBARSPIC,                 // 26
		SCROLLTOPPIC,                // 27
		SCROLL1PIC,                  // 28
		SCROLL2PIC,                  // 29
		SCROLL3PIC,                  // 30
		SCROLL4PIC,                  // 31
		SCROLL5PIC,                  // 32
		SCROLL6PIC,                  // 33
		SCROLL7PIC,                  // 34
		SCROLL8PIC,                  // 35
		FIRSTLATCHPIC,               // 36
		NOSHOTPOWERPIC,              // 37
		SHOTPOWERPIC,                // 38
		NOBODYPIC,                   // 39
		BODYPIC,                     // 40
		COMPAS1PIC,                  // 41
		COMPAS2PIC,                  // 42
		COMPAS3PIC,                  // 43
		COMPAS4PIC,                  // 44
		COMPAS5PIC,                  // 45
		COMPAS6PIC,                  // 46
		COMPAS7PIC,                  // 47
		COMPAS8PIC,                  // 48
		COMPAS9PIC,                  // 49
		COMPAS10PIC,                 // 50
		COMPAS11PIC,                 // 51
		COMPAS12PIC,                 // 52
		COMPAS13PIC,                 // 53
		COMPAS14PIC,                 // 54
		COMPAS15PIC,                 // 55
		COMPAS16PIC,                 // 56
		DEADPIC,                     // 57
		FIRSTSCALEPIC,               // 58
		ORC1PIC,                     // 59
		ORC2PIC,                     // 60
		ORC3PIC,                     // 61
		ORC4PIC,                     // 62
		ORCATTACK1PIC,               // 63
		ORCATTACK2PIC,               // 64
		ORCOUCHPIC,                  // 65
		ORCDIE1PIC,                  // 66
		ORCDIE2PIC,                  // 67
		ORCDIE3PIC,                  // 68
		TROLL1PIC,                   // 69
		TROLL2PIC,                   // 70
		TROLL3PIC,                   // 71
		TROLL4PIC,                   // 72
		TROLLOUCHPIC,                // 73
		TROLLATTACK1PIC,             // 74
		TROLLATTACK2PIC,             // 75
		TROLLATTACK3PIC,             // 76
		TROLLDIE1PIC,                // 77
		TROLLDIE2PIC,                // 78
		TROLLDIE3PIC,                // 79
		WARP1PIC,                    // 80
		WARP2PIC,                    // 81
		WARP3PIC,                    // 82
		WARP4PIC,                    // 83
		BOLTOBJPIC,                  // 84
		BOLTOBJ2PIC,                 // 85
		NUKEOBJPIC,                  // 86
		NUKEOBJ2PIC,                 // 87
		POTIONOBJPIC,                // 88
		RKEYOBJPIC,                  // 89
		YKEYOBJPIC,                  // 90
		GKEYOBJPIC,                  // 91
		BKEYOBJPIC,                  // 92
		SCROLLOBJPIC,                // 93
		CHESTOBJPIC,                 // 94
		PSHOT1PIC,                   // 95
		PSHOT2PIC,                   // 96
		BIGPSHOT1PIC,                // 97
		BIGPSHOT2PIC,                // 98
		DEMON1PIC,                   // 99
		DEMON2PIC,                   // 100
		DEMON3PIC,                   // 101
		DEMON4PIC,                   // 102
		DEMONATTACK1PIC,             // 103
		DEMONATTACK2PIC,             // 104
		DEMONATTACK3PIC,             // 105
		DEMONOUCHPIC,                // 106
		DEMONDIE1PIC,                // 107
		DEMONDIE2PIC,                // 108
		DEMONDIE3PIC,                // 109
		MAGE1PIC,                    // 110
		MAGE2PIC,                    // 111
		MAGEOUCHPIC,                 // 112
		MAGEATTACKPIC,               // 113
		MAGEDIE1PIC,                 // 114
		MAGEDIE2PIC,                 // 115
		BAT1PIC,                     // 116
		BAT2PIC,                     // 117
		BAT3PIC,                     // 118
		BAT4PIC,                     // 119
		BATDIE1PIC,                  // 120
		BATDIE2PIC,                  // 121
		GREL1PIC,                    // 122
		GREL2PIC,                    // 123
		GRELATTACKPIC,               // 124
		GRELHITPIC,                  // 125
		GRELDIE1PIC,                 // 126
		GRELDIE2PIC,                 // 127
		GRELDIE3PIC,                 // 128
		GRELDIE4PIC,                 // 129
		GRELDIE5PIC,                 // 130
		GRELDIE6PIC,                 // 131
		NEMESISPIC,                  // 132
		FIRSTWALLPIC,                // 133
		EXPWALL1PIC,                 // 134
		EXPWALL2PIC,                 // 135
		EXPWALL3PIC,                 // 136
		WALL1LPIC,                   // 137
		WALL1DPIC,                   // 138
		WALL2DPIC,                   // 139
		WALL2LPIC,                   // 140
		WALL3DPIC,                   // 141
		WALL3LPIC,                   // 142
		WALL4DPIC,                   // 143
		WALL4LPIC,                   // 144
		WALL5DPIC,                   // 145
		WALL5LPIC,                   // 146
		WALL6DPIC,                   // 147
		WALL6LPIC,                   // 148
		WALL7DPIC,                   // 149
		WALL7LPIC,                   // 150
		RDOOR1PIC,                   // 151
		RDOOR2PIC,                   // 152
		YDOOR1PIC,                   // 153
		YDOOR2PIC,                   // 154
		GDOOR1PIC,                   // 155
		GDOOR2PIC,                   // 156
		BDOOR1PIC,                   // 157
		BDOOR2PIC,                   // 158
		ENTERPLAQUEPIC,              // 159

		CP_MENUMASKPICM=160,
		HAND1PICM,                   // 161
		HAND2PICM,                   // 162

		// Lump Start
		PADDLESPR=163,
		BALLSPR,                     // 164
		BALL1PIXELTOTHERIGHTSPR,     // 165

		LEVEL1TEXT=456,
		LEVEL2TEXT,                  // 457
		LEVEL3TEXT,                  // 458
		LEVEL4TEXT,                  // 459
		LEVEL5TEXT,                  // 460
		LEVEL6TEXT,                  // 461
		LEVEL7TEXT,                  // 462
		LEVEL8TEXT,                  // 463
		LEVEL9TEXT,                  // 464
		LEVEL10TEXT,                 // 465
		LEVEL11TEXT,                 // 466
		LEVEL12TEXT,                 // 467
		LEVEL13TEXT,                 // 468
		LEVEL14TEXT,                 // 469
		LEVEL15TEXT,                 // 470
		LEVEL16TEXT,                 // 471
		LEVEL17TEXT,                 // 472
		LEVEL18TEXT,                 // 473
		LEVEL19TEXT,                 // 474
		LEVEL20TEXT,                 // 475
		OUTOFMEM,                    // 476
		PIRACY,                      // 477
		ENUMEND
	     } graphicnums;

//
// Data LUMPs
//
#define CONTROLS_LUMP_START		5
#define CONTROLS_LUMP_END		20

#define PADDLE_LUMP_START		163
#define PADDLE_LUMP_END			165

#define ORC_LUMP_START			59
#define ORC_LUMP_END			68

#define TROLL_LUMP_START		69
#define TROLL_LUMP_END			79

#define WARP_LUMP_START			80
#define WARP_LUMP_END			83

#define BOLT_LUMP_START			84
#define BOLT_LUMP_END			85

#define NUKE_LUMP_START			86
#define NUKE_LUMP_END			87

#define POTION_LUMP_START		88
#define POTION_LUMP_END			88

#define RKEY_LUMP_START			89
#define RKEY_LUMP_END			89

#define YKEY_LUMP_START			90
#define YKEY_LUMP_END			90

#define GKEY_LUMP_START			91
#define GKEY_LUMP_END			91

#define BKEY_LUMP_START			92
#define BKEY_LUMP_END			92

#define SCROLL_LUMP_START		93
#define SCROLL_LUMP_END			93

#define CHEST_LUMP_START		94
#define CHEST_LUMP_END			94

#define PLAYER_LUMP_START		95
#define PLAYER_LUMP_END			98

#define DEMON_LUMP_START		99
#define DEMON_LUMP_END			109

#define MAGE_LUMP_START			110
#define MAGE_LUMP_END			115

#define BAT_LUMP_START			116
#define BAT_LUMP_END			121

#define GREL_LUMP_START			122
#define GREL_LUMP_END			132

#define EXPWALL_LUMP_START		134
#define EXPWALL_LUMP_END		136

#define WALL1_LUMP_START		137
#define WALL1_LUMP_END			138

#define WALL2_LUMP_START		139
#define WALL2_LUMP_END			140

#define WALL3_LUMP_START		141
#define WALL3_LUMP_END			142

#define WALL4_LUMP_START		143
#define WALL4_LUMP_END			144

#define WALL5_LUMP_START		145
#define WALL5_LUMP_END			146

#define WALL6_LUMP_START		147
#define WALL6_LUMP_END			148

#define WALL7_LUMP_START		149
#define WALL7_LUMP_END			150

#define RDOOR_LUMP_START		151
#define RDOOR_LUMP_END			152

#define YDOOR_LUMP_START		153
#define YDOOR_LUMP_END			154

#define GDOOR_LUMP_START		155
#define GDOOR_LUMP_END			156

#define BDOOR_LUMP_START		157
#define BDOOR_LUMP_END			158


//
// Amount of each data item
//
#define NUMCHUNKS    478
#define NUMFONT      2
#define NUMFONTM     0
#define NUMPICS      155
#define NUMPICM      3
#define NUMSPRITES   3
#define NUMTILE8     108
#define NUMTILE8M    36
#define NUMTILE16    216
#define NUMTILE16M   72
#define NUMTILE32    0
#define NUMTILE32M   0
#define NUMEXTERNS   22
//
// File offsets for data items
//
#define STRUCTPIC    0
#define STRUCTPICM   1
#define STRUCTSPRITE 2

#define STARTFONT    3
#define STARTFONTM   5
#define STARTPICS    5
#define STARTPICM    160
#define STARTSPRITES 163
#define STARTTILE8   166
#define STARTTILE8M  167
#define STARTTILE16  168
#define STARTTILE16M 384
#define STARTTILE32  456
#define STARTTILE32M 456
#define STARTEXTERNS 456

//
// Thank you for using IGRAB!
//
