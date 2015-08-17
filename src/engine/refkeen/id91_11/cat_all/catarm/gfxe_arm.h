/* Catacomb Armageddon Source Code
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
// Graphics .H file for .ARM
// IGRAB-ed on Thu Dec 02 13:58:59 1993
//
//////////////////////////////////////

typedef enum {
		FINALEPIC=4,
		STATUSPIC,                   // 5
		FACE5PIC,                    // 6
		FIRSTLATCHPIC,               // 7
		FACE1PIC,                    // 8
		FACE2PIC,                    // 9
		FACE3PIC,                    // 10
		FACE4PIC,                    // 11
		RADAR_TOPPIC,                // 12
		RADAR_BOTTOMPIC,             // 13
		RADAR_RGEMPIC,               // 14
		RADAR_GGEMPIC,               // 15
		RADAR_BGEMPIC,               // 16
		RADAR_YGEMPIC,               // 17
		RADAR_PGEMPIC,               // 18
		FIRSTGROUNDPIC,              // 19
		FIRSTSTRIPPIC,               // 20
		FIRSTSCALEPIC,               // 21
		// Lump Start
		SKELETON_1PIC,               // 22
		SKELETON_2PIC,               // 23
		SKELETON_3PIC,               // 24
		SKELETON_4PIC,               // 25
		SKELETON_ATTACK_1PIC,        // 26
		SKELETON_ATTACK_2PIC,        // 27
		SKELETON_ATTACK_3PIC,        // 28
		SKELETON_OUCHPIC,            // 29
		SKELETON_DEATH_1PIC,         // 30
		SKELETON_DEATH_2PIC,         // 31
		// Lump Start
		TOMB1PIC,                    // 32
		TOMB2PIC,                    // 33
		TOMB3PIC,                    // 34
		// Lump Start
		OBJ_WARP1PIC,                // 35
		OBJ_WARP2PIC,                // 36
		OBJ_WARP3PIC,                // 37
		OBJ_WARP4PIC,                // 38
		// Lump Start
		EYE_WALK1PIC,                // 39
		EYE_WALK2PIC,                // 40
		EYE_WALK3PIC,                // 41
		EYE_OUCH1PIC,                // 42
		EYE_OUCH2PIC,                // 43
		EYE_DEATH1PIC,               // 44
		EYE_DEATH2PIC,               // 45
		EYE_DEATH3PIC,               // 46
		EYE_SCOWLPIC,                // 47
		EYE_SHOT1PIC,                // 48
		EYE_SHOT2PIC,                // 49
		// Lump Start
		ZOMB_APPEAR1PIC,             // 50
		ZOMB_APPEAR2PIC,             // 51
		ZOMB_APPEAR3PIC,             // 52
		ZOMB_APPEAR4PIC,             // 53
		ZOMB_WALK1PIC,               // 54
		ZOMB_WALK2PIC,               // 55
		ZOMB_WALK3PIC,               // 56
		ZOMB_OUCHPIC,                // 57
		ZOMB_ATTACKPIC,              // 58
		ZOMB_DIE1PIC,                // 59
		ZOMB_DIE2PIC,                // 60
		ZOMB_DIE3PIC,                // 61
		// Lump Start
		BOLTOBJPIC,                  // 62
		BOLT2OBJPIC,                 // 63
		BOLT3OBJPIC,                 // 64
		// Lump Start
		NUKEOBJPIC,                  // 65
		NUKE2OBJPIC,                 // 66
		NUKE3OBJPIC,                 // 67
		// Lump Start
		TIMEOBJ1PIC,                 // 68
		TIMEOBJ2PIC,                 // 69
		// Lump Start
		O_WATER_CHEST1PIC,           // 70
		O_WATER_CHEST2PIC,           // 71
		// Lump Start
		POTIONOBJPIC,                // 72
		// Lump Start
		RKEYOBJPIC,                  // 73
		// Lump Start
		YKEYOBJPIC,                  // 74
		// Lump Start
		GKEYOBJPIC,                  // 75
		// Lump Start
		BKEYOBJPIC,                  // 76
		// Lump Start
		RGEM1PIC,                    // 77
		RGEM2PIC,                    // 78
		// Lump Start
		GGEM1PIC,                    // 79
		GGEM2PIC,                    // 80
		// Lump Start
		BGEM1PIC,                    // 81
		BGEM2PIC,                    // 82
		// Lump Start
		YGEM1PIC,                    // 83
		YGEM2PIC,                    // 84
		// Lump Start
		PGEM1PIC,                    // 85
		PGEM2PIC,                    // 86
		// Lump Start
		CHESTOBJPIC,                 // 87
		// Lump Start
		PSHOT1PIC,                   // 88
		PSHOT2PIC,                   // 89
		PSHOT_EXP1PIC,               // 90
		PSHOT_EXP2PIC,               // 91
		PSHOT_EXP3PIC,               // 92
		// Lump Start
		RED_DEMON1PIC,               // 93
		RED_DEMON2PIC,               // 94
		RED_DEMON3PIC,               // 95
		RED_DEMON4PIC,               // 96
		RED_DEMONATTACK1PIC,         // 97
		RED_DEMONATTACK2PIC,         // 98
		RED_DEMONATTACK3PIC,         // 99
		RED_DEMONOUCHPIC,            // 100
		RED_DEMONDIE1PIC,            // 101
		RED_DEMONDIE2PIC,            // 102
		RED_DEMONDIE3PIC,            // 103
		// Lump Start
		MAGE1PIC,                    // 104
		MAGE2PIC,                    // 105
		MAGEOUCHPIC,                 // 106
		MAGEATTACKPIC,               // 107
		MAGEDIE1PIC,                 // 108
		MAGEDIE2PIC,                 // 109
		// Lump Start
		BAT1PIC,                     // 110
		BAT2PIC,                     // 111
		BAT3PIC,                     // 112
		BAT4PIC,                     // 113
		BATDIE1PIC,                  // 114
		BATDIE2PIC,                  // 115
		// Lump Start
		GREL1PIC,                    // 116
		GREL2PIC,                    // 117
		GRELATTACKPIC,               // 118
		GRELHITPIC,                  // 119
		GRELDIE1PIC,                 // 120
		GRELDIE2PIC,                 // 121
		GRELDIE3PIC,                 // 122
		GRELDIE4PIC,                 // 123
		GRELDIE5PIC,                 // 124
		GRELDIE6PIC,                 // 125
		SKULL_SHOTPIC,               // 126
		// Lump Start
		GODESS_WALK1PIC,             // 127
		GODESS_WALK2PIC,             // 128
		GODESS_WALK3PIC,             // 129
		GODESS_ATTACK1PIC,           // 130
		GODESS_ATTACK2PIC,           // 131
		GODESS_ATTACK3PIC,           // 132
		GODESS_STATUEPIC,            // 133
		GODESS_OUCHPIC,              // 134
		GODESS_DEATH1PIC,            // 135
		GODESS_DEATH2PIC,            // 136
		// Lump Start
		ANT_EGG1PIC,                 // 137
		ANT_EGG2PIC,                 // 138
		ANT_WALK1PIC,                // 139
		ANT_WALK2PIC,                // 140
		ANT_WALK3PIC,                // 141
		ANT_ATTACKPIC,               // 142
		ANT_DEATH1PIC,               // 143
		ANT_DEATH2PIC,               // 144
		ANT_DEATH3PIC,               // 145
		// Lump Start
		FATDEMON_WALK1PIC,           // 146
		FATDEMON_WALK2PIC,           // 147
		FATDEMON_WALK3PIC,           // 148
		FATDEMON_WALK4PIC,           // 149
		FATDEMON_ATTACK1PIC,         // 150
		FATDEMON_ATTACK2PIC,         // 151
		FATDEMON_OUCHPIC,            // 152
		FATDEMON_BLOWUP1PIC,         // 153
		FATDEMON_BLOWUP2PIC,         // 154
		FATDEMON_BLOWUP3PIC,         // 155
		FATDEMON_EXPLODEPIC,         // 156
		FATDEMON_FEETPIC,            // 157
		// Lump Start
		SUCCUBUS_WALK1PIC,           // 158
		SUCCUBUS_WALK2PIC,           // 159
		SUCCUBUS_WALK3PIC,           // 160
		SUCCUBUS_WALK4PIC,           // 161
		SUCCUBUS_ATTACK1PIC,         // 162
		SUCCUBUS_ATTACK2PIC,         // 163
		SUCCUBUS_OUCHPIC,            // 164
		SUCCUBUS_DEATH1PIC,          // 165
		SUCCUBUS_DEATH2PIC,          // 166
		SUCCUBUS_SHOT1PIC,           // 167
		// Lump Start
		TREE_IDLEPIC,                // 168
		TREE_AWAKENINGPIC,           // 169
		TREE_WALK1PIC,               // 170
		TREE_WALK2PIC,               // 171
		TREE_ATTACK1PIC,             // 172
		TREE_ATTACK2PIC,             // 173
		TREE_ATTACK3PIC,             // 174
		TREE_DEATH1PIC,              // 175
		TREE_DEATH2PIC,              // 176
		TREE_DEATH3PIC,              // 177
		// Lump Start
		DRAGON_BUBBLES1PIC,          // 178
		DRAGON_BUBBLES2PIC,          // 179
		DRAGON_EYESPIC,              // 180
		DRAGON_RISE1PIC,             // 181
		DRAGON_RISE2PIC,             // 182
		DRAGON_WALK1PIC,             // 183
		DRAGON_WALK2PIC,             // 184
		DRAGON_WALK3PIC,             // 185
		DRAGON_WALK4PIC,             // 186
		DRAGON_ATTACK1PIC,           // 187
		DRAGON_ATTACK2PIC,           // 188
		DRAGON_ATTACK3PIC,           // 189
		DRAGON_OUCHPIC,              // 190
		DRAGON_DEATH1PIC,            // 191
		DRAGON_DEATH2PIC,            // 192
		DRAGON_DEATH3PIC,            // 193
		// Lump Start
		BUNNY_LEFT1PIC,              // 194
		BUNNY_LEFT2PIC,              // 195
		BUNNY_RIGHT1PIC,             // 196
		BUNNY_RIGHT2PIC,             // 197
		BUNNY_META1PIC,              // 198
		BUNNY_META2PIC,              // 199
		BUNNY_WALK1PIC,              // 200
		BUNNY_WALK2PIC,              // 201
		BUNNY_OUCHPIC,               // 202
		BUNNY_DEATH1PIC,             // 203
		BUNNY_DEATH2PIC,             // 204
		// Lump Start
		ARCH1PIC,                    // 205
		// Lump Start
		ARCH2PIC,                    // 206
		// Lump Start
		ARCH3PIC,                    // 207
		// Lump Start
		ARCH4PIC,                    // 208
		// Lump Start
		ARCH5PIC,                    // 209
		// Lump Start
		ARCH6PIC,                    // 210
		// Lump Start
		ARCH7PIC,                    // 211
		// Lump Start
		ARCH8PIC,                    // 212
		// Lump Start
		ARCH9PIC,                    // 213
		// Lump Start
		ARCH10PIC,                   // 214
		// Lump Start
		ARCH11PIC,                   // 215
		// Lump Start
		ARCH12PIC,                   // 216
		// Lump Start
		ARCH13PIC,                   // 217
		// Lump Start
		ANT_HILLPIC,                 // 218
		// Lump Start
		COLUMNPIC,                   // 219
		// Lump Start
		SULPHUR_GAS_1PIC,            // 220
		SULPHUR_GAS_2PIC,            // 221
		SULPHUR_GAS_3PIC,            // 222
		// Lump Start
		FIRE_POT_1PIC,               // 223
		FIRE_POT_2PIC,               // 224
		// Lump Start
		SKEL_HANGPIC,                // 225
		// Lump Start
		FORCE_FIELD_1PIC,            // 226
		FORCE_FIELD_2PIC,            // 227
		FORCE_FIELD_3PIC,            // 228
		FORCE_FIELD_4PIC,            // 229
		// Lump Start
		WFOUNTAINPIC,                // 230
		FIRSTWALLPIC,                // 231
		CRYSTAL_LIGHT_1PIC,          // 232
		CRYSTAL_LIGHT_2PIC,          // 233
		CRYSTAL_LIGHT_3PIC,          // 234
		CRYSTAL_LIGHT_4PIC,          // 235
		CRYSTAL_DARK_1PIC,           // 236
		CRYSTAL_DARK_2PIC,           // 237
		CRYSTAL_DARK_3PIC,           // 238
		CRYSTAL_DARK_4PIC,           // 239
		FIRE_WALL_1PIC,              // 240
		FIRE_WALL_2PIC,              // 241
		FIRE_WALL_3PIC,              // 242
		FIRE_WALL_4PIC,              // 243
		BRN_STONE_GATEPIC,           // 244
		BRN_STONE_WALL_1PIC,         // 245
		BRN_STONE_WALL_2PIC,         // 246
		KUDZU_LIGHT_WALLPIC,         // 247
		KUDZU_DARK_WALLPIC,          // 248
		HEDGE_WALLPIC,               // 249
		HEDGE_EYESPIC,               // 250
		BRN_WINDOW_LIGHTPIC,         // 251
		ALTAR_LEFTPIC,               // 252
		ALTAR_RIGHTPIC,              // 253
		GRAY_LIGHT_WALLPIC,          // 254
		GRAY_DARK_WALLPIC,           // 255
		GRAY_LIGHT_SIGNPIC,          // 256
		GRAY_DARK_SIGNPIC,           // 257
		MANICLE_LIGHT_BLOODYPIC,     // 258
		MANICLE_DARK_BLOODYPIC,      // 259
		LIGHT_CURTAIN_WINDOWPIC,     // 260
		LIGHT_CURTAIN_WALLPIC,       // 261
		DARK_CURTAIN_WINDOWPIC,      // 262
		DARK_CURTAIN_WALLPIC,        // 263
		BRN_LIGHT_SIGNPIC,           // 264
		BRN_DARK_SIGNPIC,            // 265
		LIGHT_STONE_WALLPIC,         // 266
		DARK_STONE_WALLPIC,          // 267
		BRN_FLAGSTONE_LIGHT_2PIC,    // 268
		BRN_FLAGSTONE_DARK_2PIC,     // 269
		RUST_METAL_LIGHTPIC,         // 270
		RUST_METAL_DARKPIC,          // 271
		GRAY_METAL_LIGHTPIC,         // 272
		GRAY_METAL_DARKPIC,          // 273
		WEAK_STONE_LIGHTPIC,         // 274
		WEAK_STONE_DARKPIC,          // 275
		WEAK_GRAY_RFGSTN_LIGHTPIC,   // 276
		WEAK_GRAY_RFGSTN_DARKPIC,    // 277
		WEAK_CRYSTAL_LIGHTPIC,       // 278
		WEAK_CRYSTAL_DARKPIC,        // 279
		RED_MUD_LIGHTPIC,            // 280
		BRN_MUD_DARKPIC,             // 281
		RED_MUD_WEAK_LIGHTPIC,       // 282
		BRN_MUD_WEAK_DARKPIC,        // 283
		HORN_DOORPIC,                // 284
		CLOSED_DOOR_1PIC,            // 285
		DOOR_2PIC,                   // 286
		WATER_LIGHT_WEAK_1PIC,       // 287
		WATER_LIGHT_WEAK_2PIC,       // 288
		WATER_LIGHT_WEAK_3PIC,       // 289
		WATER_DARK_WEAK_1PIC,        // 290
		WATER_DARK_WEAK_2PIC,        // 291
		WATER_DARK_WEAK_3PIC,        // 292
		WATER_LIGHT_1PIC,            // 293
		WATER_LIGHT_2PIC,            // 294
		WATER_LIGHT_3PIC,            // 295
		WATER_DARK_1PIC,             // 296
		WATER_DARK_2PIC,             // 297
		WATER_DARK_3PIC,             // 298
		TROLL_LIGHT_STONEPIC,        // 299
		TROLL_DARK_STONEPIC,         // 300
		TROLL_BLOODY_LT_STONEPIC,    // 301
		TROLL_BLOODY_DK_STONEPIC,    // 302
		LIGHT_BREATH_1PIC,           // 303
		LIGHT_BREATH_2PIC,           // 304
		LIGHT_BREATH_3PIC,           // 305
		DARK_BREATH_1PIC,            // 306
		DARK_BREATH_2PIC,            // 307
		DARK_BREATH_3PIC,            // 308
		EXP_WALL_1PIC,               // 309
		EXP_WALL_2PIC,               // 310
		EXP_WALL_3PIC,               // 311
		WATER_EXP_WALL_1PIC,         // 312
		WATER_EXP_WALL_2PIC,         // 313
		WATER_EXP_WALL_3PIC,         // 314
		W_GEN_DOOR1PIC,              // 315
		W_GEN_DOOR2PIC,              // 316
		W_CRYSTAL_DOORPIC,           // 317
		DMG_BRN_FSTN_LTPIC,          // 318
		DMG_BRN_FSTN_DKPIC,          // 319
		DMG_FIN_FSTN_LTPIC,          // 320
		DMG_FIN_FSTN_DKPIC,          // 321
		STEEL_DOOR1PIC,              // 322
		STEEL_DOOR2PIC,              // 323
		BRN_WINDOW_DARKPIC,          // 324
		GRY_DOOR_LTPIC,              // 325
		GRY_DOOR_DKPIC,              // 326
		BRN_DOOR_LTPIC,              // 327
		BRN_DOOR_DKPIC,              // 328
		GRY_FGSTN_LTPIC,             // 329
		GRY_FGSTN_DKPIC,             // 330
		KUDZU_WEAK_LIGHTPIC,         // 331
		KUDZU_WEAK_DARKPIC,          // 332
		LT_SKEL1PIC,                 // 333
		DK_SKEL1PIC,                 // 334
		LT_SKEL2PIC,                 // 335
		DK_SKEL2PIC,                 // 336
		MANICLE_LIGHT_WALLPIC,       // 337
		MANICLE_DARK_WALLPIC,        // 338
		TAP_1PIC,                    // 339
		TAP_2PIC,                    // 340
		TAP_3PIC,                    // 341
		TAP_4PIC,                    // 342
		TAP_5PIC,                    // 343
		FINALWALLPIC,                // 344
		WATER_DOOR1_PIC,             // 345
		WATER_DOOR2_PIC,             // 346
		LASTWALLPIC,                 // 347

		HAND1PICM=348,

		NORTHICONSPR=349,

		LEVEL1TEXT=640,
		LEVEL2TEXT,                  // 641
		LEVEL3TEXT,                  // 642
		LEVEL4TEXT,                  // 643
		LEVEL5TEXT,                  // 644
		LEVEL6TEXT,                  // 645
		LEVEL7TEXT,                  // 646
		LEVEL8TEXT,                  // 647
		LEVEL9TEXT,                  // 648
		LEVEL10TEXT,                 // 649
		LEVEL11TEXT,                 // 650
		LEVEL12TEXT,                 // 651
		LEVEL13TEXT,                 // 652
		LEVEL14TEXT,                 // 653
		LEVEL15TEXT,                 // 654
		LEVEL16TEXT,                 // 655
		LEVEL17TEXT,                 // 656
		PIRACY,                      // 657
		ENUMEND
	     } graphicnums;

//
// Data LUMPs
//
#define SKELDUDE_LUMP_START		22
#define SKELDUDE_LUMP_END		31

#define TOMBSTONES_LUMP_START		32
#define TOMBSTONES_LUMP_END		34

#define OBJ_WARP_LUMP_START		35
#define OBJ_WARP_LUMP_END		38

#define EYE_LUMP_START			39
#define EYE_LUMP_END			49

#define ZOMBIE_LUMP_START		50
#define ZOMBIE_LUMP_END			61

#define BOLT_LUMP_START			62
#define BOLT_LUMP_END			64

#define NUKE_LUMP_START			65
#define NUKE_LUMP_END			67

#define TIME_LUMP_START			68
#define TIME_LUMP_END			69

#define O_WATER_CHEST_LUMP_START	70
#define O_WATER_CHEST_LUMP_END		71

#define POTION_LUMP_START		72
#define POTION_LUMP_END			72

#define RKEY_LUMP_START			73
#define RKEY_LUMP_END			73

#define YKEY_LUMP_START			74
#define YKEY_LUMP_END			74

#define GKEY_LUMP_START			75
#define GKEY_LUMP_END			75

#define BKEY_LUMP_START			76
#define BKEY_LUMP_END			76

#define RGEM_LUMP_START			77
#define RGEM_LUMP_END			78

#define GGEM_LUMP_START			79
#define GGEM_LUMP_END			80

#define BGEM_LUMP_START			81
#define BGEM_LUMP_END			82

#define YGEM_LUMP_START			83
#define YGEM_LUMP_END			84

#define PGEM_LUMP_START			85
#define PGEM_LUMP_END			86

#define CHEST_LUMP_START		87
#define CHEST_LUMP_END			87

#define PLAYER_LUMP_START		88
#define PLAYER_LUMP_END			92

#define REDDEMON_LUMP_START		93
#define REDDEMON_LUMP_END		103

#define MAGE_LUMP_START			104
#define MAGE_LUMP_END			109

#define BAT_LUMP_START			110
#define BAT_LUMP_END			115

#define GREL_LUMP_START			116
#define GREL_LUMP_END			126

#define GODESS_LUMP_START		127
#define GODESS_LUMP_END			136

#define ANT_LUMP_START			137
#define ANT_LUMP_END			145

#define FATDEMON_LUMP_START		146
#define FATDEMON_LUMP_END		157

#define SUCCUBUS_LUMP_START		158
#define SUCCUBUS_LUMP_END		167

#define TREE_LUMP_START			168
#define TREE_LUMP_END			177

#define DRAGON_LUMP_START		178
#define DRAGON_LUMP_END			193

#define BUNNY_LUMP_START		194
#define BUNNY_LUMP_END			204

#define ARCH1_LUMP_START		205
#define ARCH1_LUMP_END			205

#define ARCH2_LUMP_START		206
#define ARCH2_LUMP_END			206

#define ARCH3_LUMP_START		207
#define ARCH3_LUMP_END			207

#define ARCH4_LUMP_START		208
#define ARCH4_LUMP_END			208

#define ARCH5_LUMP_START		209
#define ARCH5_LUMP_END			209

#define ARCH6_LUMP_START		210
#define ARCH6_LUMP_END			210

#define ARCH7_LUMP_START		211
#define ARCH7_LUMP_END			211

#define ARCH8_LUMP_START		212
#define ARCH8_LUMP_END			212

#define ARCH9_LUMP_START		213
#define ARCH9_LUMP_END			213

#define ARCH10_LUMP_START		214
#define ARCH10_LUMP_END			214

#define ARCH11_LUMP_START		215
#define ARCH11_LUMP_END			215

#define ARCH12_LUMP_START		216
#define ARCH12_LUMP_END			216

#define ARCH13_LUMP_START		217
#define ARCH13_LUMP_END			217

#define ANTHILL_LUMP_START		218
#define ANTHILL_LUMP_END		218

#define COLUMN_LUMP_START		219
#define COLUMN_LUMP_END			219

#define SULPHURGAS_LUMP_START		220
#define SULPHURGAS_LUMP_END		222

#define FIREPOT_LUMP_START		223
#define FIREPOT_LUMP_END		224

#define SKELHANG_LUMP_START		225
#define SKELHANG_LUMP_END		225

#define FORCEFIELD_LUMP_START		226
#define FORCEFIELD_LUMP_END		229

#define FOUNTAIN_LUMP_START		230
#define FOUNTAIN_LUMP_END		230


//
// Amount of each data item
//
#define NUMCHUNKS    658
#define NUMFONT      1
#define NUMFONTM     0
#define NUMPICS      344
#define NUMPICM      1
#define NUMSPRITES   1
#define NUMTILE8     108
#define NUMTILE8M    36
#define NUMTILE16    216
#define NUMTILE16M   72
#define NUMTILE32    0
#define NUMTILE32M   0
#define NUMEXTERNS   18
//
// File offsets for data items
//
#define STRUCTPIC    0
#define STRUCTPICM   1
#define STRUCTSPRITE 2

#define STARTFONT    3
#define STARTFONTM   4
#define STARTPICS    4
#define STARTPICM    348
#define STARTSPRITES 349
#define STARTTILE8   350
#define STARTTILE8M  351
#define STARTTILE16  352
#define STARTTILE16M 568
#define STARTTILE32  640
#define STARTTILE32M 640
#define STARTEXTERNS 640

//
// Thank you for using IGRAB!
//
