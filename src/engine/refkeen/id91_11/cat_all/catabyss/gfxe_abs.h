/* Catacomb Abyss Source Code
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
// Graphics .H file for .ABS
// IGRAB-ed on Thu Dec 02 11:58:32 1993
//
//////////////////////////////////////

typedef enum {
		FINALEPIC=4,
		STATUSPIC,                   // 5
		SCROLLTOPPIC,                // 6
		SCROLLBOTTOMPIC,             // 7
		SCROLL1PIC,                  // 8
		SCROLL2PIC,                  // 9
		SCROLL3PIC,                  // 10
		SCROLL4PIC,                  // 11
		SCROLL5PIC,                  // 12
		SCROLL6PIC,                  // 13
		SCROLL7PIC,                  // 14
		SCROLL8PIC,                  // 15
		FACE5PIC,                    // 16
		FIRSTLATCHPIC,               // 17
		FACE1PIC,                    // 18
		FACE2PIC,                    // 19
		FACE3PIC,                    // 20
		FACE4PIC,                    // 21
		RADAR_TOPPIC,                // 22
		RADAR_BOTTOMPIC,             // 23
		RADAR_RGEMPIC,               // 24
		RADAR_GGEMPIC,               // 25
		RADAR_BGEMPIC,               // 26
		RADAR_YGEMPIC,               // 27
		RADAR_PGEMPIC,               // 28
		FIRSTSCALEPIC,               // 29
		// Lump Start
		SKELETON_1PIC,               // 30
		SKELETON_2PIC,               // 31
		SKELETON_3PIC,               // 32
		SKELETON_4PIC,               // 33
		SKELETON_ATTACK_1PIC,        // 34
		SKELETON_ATTACK_2PIC,        // 35
		SKELETON_ATTACK_3PIC,        // 36
		SKELETON_OUCHPIC,            // 37
		SKELETON_DEATH_1PIC,         // 38
		SKELETON_DEATH_2PIC,         // 39
		// Lump Start
		SPOOK1PIC,                   // 40
		SPOOK2PIC,                   // 41
		SPOOK3PIC,                   // 42
		SPOOK4PIC,                   // 43
		SPOOK_ATTACKPIC,             // 44
		SPOOKHITPIC,                 // 45
		SPOOK_INOUTPIC,              // 46
		// Lump Start
		TOMB1PIC,                    // 47
		TOMB2PIC,                    // 48
		TOMB3PIC,                    // 49
		// Lump Start
		WET_BUBBLE1PIC,              // 50
		WET_BUBBLE2PIC,              // 51
		WET_EYESPIC,                 // 52
		WET_RISE1PIC,                // 53
		WET_RISE2PIC,                // 54
		WET_WALK1PIC,                // 55
		WET_WALK2PIC,                // 56
		WET_WALK3PIC,                // 57
		WET_WALK4PIC,                // 58
		WET_ATTACK1PIC,              // 59
		WET_ATTACK2PIC,              // 60
		WET_ATTACK3PIC,              // 61
		WET_OUCHPIC,                 // 62
		WET_DIE1PIC,                 // 63
		WET_DIE2PIC,                 // 64
		WET_DIE3PIC,                 // 65
		// Lump Start
		OBJ_WARP1PIC,                // 66
		OBJ_WARP2PIC,                // 67
		OBJ_WARP3PIC,                // 68
		OBJ_WARP4PIC,                // 69
		// Lump Start
		EYE_WALK1PIC,                // 70
		EYE_WALK2PIC,                // 71
		EYE_WALK3PIC,                // 72
		EYE_OUCH1PIC,                // 73
		EYE_OUCH2PIC,                // 74
		EYE_DEATH1PIC,               // 75
		EYE_DEATH2PIC,               // 76
		EYE_DEATH3PIC,               // 77
		EYE_SHOT1PIC,                // 78
		EYE_SHOT2PIC,                // 79
		// Lump Start
		ZOMB_APPEAR1PIC,             // 80
		ZOMB_APPEAR2PIC,             // 81
		ZOMB_APPEAR3PIC,             // 82
		ZOMB_APPEAR4PIC,             // 83
		ZOMB_WALK1PIC,               // 84
		ZOMB_WALK2PIC,               // 85
		ZOMB_WALK3PIC,               // 86
		ZOMB_OUCHPIC,                // 87
		ZOMB_ATTACKPIC,              // 88
		ZOMB_DIE1PIC,                // 89
		ZOMB_DIE2PIC,                // 90
		ZOMB_DIE3PIC,                // 91
		// Lump Start
		ORC1PIC,                     // 92
		ORC2PIC,                     // 93
		ORC3PIC,                     // 94
		ORC4PIC,                     // 95
		ORCATTACK1PIC,               // 96
		ORCATTACK2PIC,               // 97
		ORCOUCHPIC,                  // 98
		ORCDIE1PIC,                  // 99
		ORCDIE2PIC,                  // 100
		ORCDIE3PIC,                  // 101
		// Lump Start
		TROLL1PIC,                   // 102
		TROLL2PIC,                   // 103
		TROLL3PIC,                   // 104
		TROLL4PIC,                   // 105
		TROLLOUCHPIC,                // 106
		TROLLATTACK1PIC,             // 107
		TROLLATTACK2PIC,             // 108
		TROLLATTACK3PIC,             // 109
		TROLLDIE1PIC,                // 110
		TROLLDIE2PIC,                // 111
		TROLLDIE3PIC,                // 112
		// Lump Start
		BOLTOBJPIC,                  // 113
		BOLT2OBJPIC,                 // 114
		BOLT3OBJPIC,                 // 115
		// Lump Start
		NUKEOBJPIC,                  // 116
		NUKE2OBJPIC,                 // 117
		NUKE3OBJPIC,                 // 118
		// Lump Start
		PITOBJPIC,                   // 119
		// Lump Start
		TIMEOBJ1PIC,                 // 120
		TIMEOBJ2PIC,                 // 121
		// Lump Start
		O_WATER_CHEST1PIC,           // 122
		O_WATER_CHEST2PIC,           // 123
		// Lump Start
		POTIONOBJPIC,                // 124
		// Lump Start
		RKEYOBJPIC,                  // 125
		// Lump Start
		YKEYOBJPIC,                  // 126
		// Lump Start
		GKEYOBJPIC,                  // 127
		// Lump Start
		BKEYOBJPIC,                  // 128
		// Lump Start
		RGEM1PIC,                    // 129
		RGEM2PIC,                    // 130
		// Lump Start
		GGEM1PIC,                    // 131
		GGEM2PIC,                    // 132
		// Lump Start
		BGEM1PIC,                    // 133
		BGEM2PIC,                    // 134
		// Lump Start
		YGEM1PIC,                    // 135
		YGEM2PIC,                    // 136
		// Lump Start
		PGEM1PIC,                    // 137
		PGEM2PIC,                    // 138
		// Lump Start
		SCROLLOBJPIC,                // 139
		// Lump Start
		CHESTOBJPIC,                 // 140
		// Lump Start
		RKEY2PIC,                    // 141
		// Lump Start
		PSHOT1PIC,                   // 142
		PSHOT2PIC,                   // 143
		PSHOT_EXP1PIC,               // 144
		PSHOT_EXP2PIC,               // 145
		PSHOT_EXP3PIC,               // 146
		// Lump Start
		DEMON1PIC,                   // 147
		DEMON2PIC,                   // 148
		DEMON3PIC,                   // 149
		DEMON4PIC,                   // 150
		DEMONATTACK1PIC,             // 151
		DEMONATTACK2PIC,             // 152
		DEMONATTACK3PIC,             // 153
		DEMONOUCHPIC,                // 154
		DEMONDIE1PIC,                // 155
		DEMONDIE2PIC,                // 156
		DEMONDIE3PIC,                // 157
		// Lump Start
		RED_DEMON1PIC,               // 158
		RED_DEMON2PIC,               // 159
		RED_DEMON3PIC,               // 160
		RED_DEMON4PIC,               // 161
		RED_DEMONATTACK1PIC,         // 162
		RED_DEMONATTACK2PIC,         // 163
		RED_DEMONATTACK3PIC,         // 164
		RED_DEMONOUCHPIC,            // 165
		RED_DEMONDIE1PIC,            // 166
		RED_DEMONDIE2PIC,            // 167
		RED_DEMONDIE3PIC,            // 168
		// Lump Start
		MAGE1PIC,                    // 169
		MAGE2PIC,                    // 170
		MAGEOUCHPIC,                 // 171
		MAGEATTACKPIC,               // 172
		MAGEDIE1PIC,                 // 173
		MAGEDIE2PIC,                 // 174
		// Lump Start
		BAT1PIC,                     // 175
		BAT2PIC,                     // 176
		BAT3PIC,                     // 177
		BAT4PIC,                     // 178
		BATDIE1PIC,                  // 179
		BATDIE2PIC,                  // 180
		// Lump Start
		GREL1PIC,                    // 181
		GREL2PIC,                    // 182
		GRELATTACKPIC,               // 183
		GRELHITPIC,                  // 184
		GRELDIE1PIC,                 // 185
		GRELDIE2PIC,                 // 186
		GRELDIE3PIC,                 // 187
		GRELDIE4PIC,                 // 188
		GRELDIE5PIC,                 // 189
		GRELDIE6PIC,                 // 190
		SKULL_SHOTPIC,               // 191
		FIRSTWALLPIC,                // 192
		// Lump Start
		EXPWALL1PIC,                 // 193
		EXPWALL2PIC,                 // 194
		EXPWALL3PIC,                 // 195
		// Lump Start
		W_WATER_EXP1PIC,             // 196
		W_WATER_EXP2PIC,             // 197
		W_WATER_EXP3PIC,             // 198
		// Lump Start
		W_NEMPICEWPIC,               // 199
		// Lump Start
		W_PENTAEWPIC,                // 200
		W_PENTANSPIC,                // 201
		// Lump Start
		W_ALTER_LFPIC,               // 202
		// Lump Start
		W_ALTER_RTPIC,               // 203
		// Lump Start
		W_SUB1EWPIC,                 // 204
		W_SUB1NSPIC,                 // 205
		// Lump Start
		W_SUB2EWPIC,                 // 206
		W_SUB2NSPIC,                 // 207
		// Lump Start
		W_SUB3EWPIC,                 // 208
		W_SUB3NSPIC,                 // 209
		// Lump Start
		WALL8LPIC,                   // 210
		WALL8DPIC,                   // 211
		// Lump Start
		WALL9LPIC,                   // 212
		WALL9DPIC,                   // 213
		// Lump Start
		WALL10LPIC,                  // 214
		WALL10DPIC,                  // 215
		// Lump Start
		WALL11LPIC,                  // 216
		WALL11DPIC,                  // 217
		// Lump Start
		WALL12LPIC,                  // 218
		WALL12DPIC,                  // 219
		// Lump Start
		WALL13LPIC,                  // 220
		WALL13DPIC,                  // 221
		// Lump Start
		WALL14LPIC,                  // 222
		WALL14DPIC,                  // 223
		// Lump Start
		WALL15LPIC,                  // 224
		WALL15DPIC,                  // 225
		// Lump Start
		WALL16LPIC,                  // 226
		WALL16DPIC,                  // 227
		// Lump Start
		WALL18LPIC,                  // 228
		WALL18DPIC,                  // 229
		// Lump Start
		WALL17LPIC,                  // 230
		WALL17DPIC,                  // 231
		// Lump Start
		WALL19LPIC,                  // 232
		WALL19DPIC,                  // 233
		// Lump Start
		WALL20LPIC,                  // 234
		WALL20DPIC,                  // 235
		// Lump Start
		WALL21LPIC,                  // 236
		WALL21DPIC,                  // 237
		// Lump Start
		WALL22LPIC,                  // 238
		WALL22DPIC,                  // 239
		// Lump Start
		WALL23LPIC,                  // 240
		WALL23DPIC,                  // 241
		// Lump Start
		WALL24LPIC,                  // 242
		WALL24DPIC,                  // 243
		// Lump Start
		WALL25LPIC,                  // 244
		WALL25DPIC,                  // 245
		// Lump Start
		WALL26LPIC,                  // 246
		WALL26DPIC,                  // 247
		// Lump Start
		WALL27LPIC,                  // 248
		WALL27DPIC,                  // 249
		// Lump Start
		WALL28LPIC,                  // 250
		WALL28DPIC,                  // 251
		// Lump Start
		WALL29LPIC,                  // 252
		WALL29DPIC,                  // 253
		// Lump Start
		WALL30LPIC,                  // 254
		WALL30DPIC,                  // 255
		// Lump Start
		WALL31LPIC,                  // 256
		WALL31DPIC,                  // 257
		// Lump Start
		W_WATER1EWPIC,               // 258
		W_WATER1NSPIC,               // 259
		// Lump Start
		W_WATER2EWPIC,               // 260
		W_WATER2NSPIC,               // 261
		// Lump Start
		W_FINALEXITPIC,              // 262
		// Lump Start
		W_DRAIN1EWPIC,               // 263
		W_DRAIN1NSPIC,               // 264
		W_DRAIN2EWPIC,               // 265
		W_DRAIN2NSPIC,               // 266
		// Lump Start
		W_WARP1EWPIC,                // 267
		W_WARP2EWPIC,                // 268
		W_WARP1NSPIC,                // 269
		W_WARP2NSPIC,                // 270
		W_WINDOWEWPIC,               // 271
		W_WINDOWNSPIC,               // 272
		W_WOOD_DOOREWPIC,            // 273
		W_WOOD_DOORNSPIC,            // 274
		W_WOODEN_DOORPIC,            // 275
		W_METAL_DOORPIC,             // 276
		W_GLOW_DOORPIC,              // 277
		// Lump Start
		W_TORCH1PIC,                 // 278
		W_TORCH2PIC,                 // 279
		// Lump Start
		W_LSUB_STONEPIC,             // 280
		W_DSUB_STONEPIC,             // 281
		// Lump Start
		W_BLOODY_LSUB_STONEPIC,      // 282
		W_BLOODY_DSUB_STONEPIC,      // 283
		// Lump Start
		W_BREATH_LWALL1PIC,          // 284
		W_BREATH_DWALL1PIC,          // 285
		W_BREATH_LWALL2PIC,          // 286
		W_BREATH_DWALL2PIC,          // 287
		// Lump Start
		W_WOOD_DOORWAYPIC,           // 288
		// Lump Start
		W_WOOD_DOORWAY_GLOWPIC,      // 289
		// Lump Start
		W_WATER_GATEEW1PIC,          // 290
		W_WATER_GATEEW2PIC,          // 291
		W_WATER_GATENS1PIC,          // 292
		W_WATER_GATENS2PIC,          // 293
		// Lump Start
		W_PRE_CHEATSPIC,             // 294
		// Lump Start
		W_CHEAT_GODPIC,              // 295
		// Lump Start
		W_CHEAT_WARPPIC,             // 296
		// Lump Start
		W_CHEAT_ITEMSPIC,            // 297
		// Lump Start
		W_CHEAT_FREEZEPIC,           // 298
		// Lump Start
		W_POST_CHEATPIC,             // 299
		// Lump Start
		W_SURFACE_PLAQPIC,           // 300
		// Lump Start
		W_BREATH_LWALL3PIC,          // 301
		W_BREATH_DWALL3PIC,          // 302
		W_BREATH_LWALL4PIC,          // 303
		W_BREATH_DWALL4PIC,          // 304
		// Lump Start
		MAGE_STATUEPIC,              // 305
		// Lump Start
		ZOMBIE_STATUEPIC,            // 306
		// Lump Start
		EYE_STATUEPIC,               // 307
		// Lump Start
		NEM_STATUEPIC,               // 308
		// Lump Start
		SKELETON_STATUEPIC,          // 309
		// Lump Start
		SPOOK_STATUEPIC,             // 310
		// Lump Start
		ORCH_STATUEPIC,              // 311
		LASTWALLPIC,                 // 312

		HAND1PICM=313,

		NORTHICONSPR=314,

		LEVEL1TEXT=605,
		LEVEL2TEXT,                  // 606
		LEVEL3TEXT,                  // 607
		LEVEL4TEXT,                  // 608
		LEVEL5TEXT,                  // 609
		LEVEL6TEXT,                  // 610
		LEVEL7TEXT,                  // 611
		LEVEL8TEXT,                  // 612
		LEVEL9TEXT,                  // 613
		LEVEL10TEXT,                 // 614
		LEVEL11TEXT,                 // 615
		LEVEL12TEXT,                 // 616
		LEVEL13TEXT,                 // 617
		LEVEL14TEXT,                 // 618
		LEVEL15TEXT,                 // 619
		LEVEL16TEXT,                 // 620
		LEVEL17TEXT,                 // 621
		LEVEL18TEXT,                 // 622
		LEVEL19TEXT,                 // 623
		LEVEL20TEXT,                 // 624
		PIRACY,                      // 625
		ENUMEND
	     } graphicnums;

//
// Data LUMPs
//
#define SKELDUDE_LUMP_START		30
#define SKELDUDE_LUMP_END		39

#define SPOOK_LUMP_START		40
#define SPOOK_LUMP_END			46

#define TOMBSTONES_LUMP_START		47
#define TOMBSTONES_LUMP_END		49

#define WETMAN_LUMP_START		50
#define WETMAN_LUMP_END			65

#define OBJ_WARP_LUMP_START		66
#define OBJ_WARP_LUMP_END		69

#define EYE_LUMP_START			70
#define EYE_LUMP_END			79

#define ZOMBIE_LUMP_START		80
#define ZOMBIE_LUMP_END			91

#define ORC_LUMP_START			92
#define ORC_LUMP_END			101

#define TROLL_LUMP_START		102
#define TROLL_LUMP_END			112

#define BOLT_LUMP_START			113
#define BOLT_LUMP_END			115

#define NUKE_LUMP_START			116
#define NUKE_LUMP_END			118

#define PIT_LUMP_START			119
#define PIT_LUMP_END			119

#define TIME_LUMP_START			120
#define TIME_LUMP_END			121

#define O_WATER_CHEST_LUMP_START	122
#define O_WATER_CHEST_LUMP_END		123

#define POTION_LUMP_START		124
#define POTION_LUMP_END			124

#define RKEY_LUMP_START			125
#define RKEY_LUMP_END			125

#define YKEY_LUMP_START			126
#define YKEY_LUMP_END			126

#define GKEY_LUMP_START			127
#define GKEY_LUMP_END			127

#define BKEY_LUMP_START			128
#define BKEY_LUMP_END			128

#define RGEM_LUMP_START			129
#define RGEM_LUMP_END			130

#define GGEM_LUMP_START			131
#define GGEM_LUMP_END			132

#define BGEM_LUMP_START			133
#define BGEM_LUMP_END			134

#define YGEM_LUMP_START			135
#define YGEM_LUMP_END			136

#define PGEM_LUMP_START			137
#define PGEM_LUMP_END			138

#define SCROLL_LUMP_START		139
#define SCROLL_LUMP_END			139

#define CHEST_LUMP_START		140
#define CHEST_LUMP_END			140

#define RKEY2_LUMP_START		141
#define RKEY2_LUMP_END			141

#define PLAYER_LUMP_START		142
#define PLAYER_LUMP_END			146

#define DEMON_LUMP_START		147
#define DEMON_LUMP_END			157

#define REDDEMON_LUMP_START		158
#define REDDEMON_LUMP_END		168

#define MAGE_LUMP_START			169
#define MAGE_LUMP_END			174

#define BAT_LUMP_START			175
#define BAT_LUMP_END			180

#define GREL_LUMP_START			181
#define GREL_LUMP_END			191

#define EXPWALL_LUMP_START		193
#define EXPWALL_LUMP_END		195

#define W_WATER_EXP_LUMP_START		196
#define W_WATER_EXP_LUMP_END		198

#define WALL2_LUMP_START		199
#define WALL2_LUMP_END			199

#define WALL3_LUMP_START		200
#define WALL3_LUMP_END			201

#define WALL4_LUMP_START		202
#define WALL4_LUMP_END			202

#define WALL5_LUMP_START		203
#define WALL5_LUMP_END			203

#define WALL6_LUMP_START		204
#define WALL6_LUMP_END			205

#define WALL7_LUMP_START		206
#define WALL7_LUMP_END			207

#define WALL6_2_LUMP_START		208
#define WALL6_2_LUMP_END		209

#define WALL8_LUMP_START		210
#define WALL8_LUMP_END			211

#define WALL9_LUMP_START		212
#define WALL9_LUMP_END			213

#define WALL10_LUMP_START		214
#define WALL10_LUMP_END			215

#define WALL11_LUMP_START		216
#define WALL11_LUMP_END			217

#define WALL12_LUMP_START		218
#define WALL12_LUMP_END			219

#define WALL13_LUMP_START		220
#define WALL13_LUMP_END			221

#define WALL14_LUMP_START		222
#define WALL14_LUMP_END			223

#define WALL15_LUMP_START		224
#define WALL15_LUMP_END			225

#define WALL16_LUMP_START		226
#define WALL16_LUMP_END			227

#define WALL18_LUMP_START		228
#define WALL18_LUMP_END			229

#define WALL17_LUMP_START		230
#define WALL17_LUMP_END			231

#define WALL19_LUMP_START		232
#define WALL19_LUMP_END			233

#define WALL20_LUMP_START		234
#define WALL20_LUMP_END			235

#define WALL21_LUMP_START		236
#define WALL21_LUMP_END			237

#define WALL22_LUMP_START		238
#define WALL22_LUMP_END			239

#define WALL23_LUMP_START		240
#define WALL23_LUMP_END			241

#define WALL24_LUMP_START		242
#define WALL24_LUMP_END			243

#define WALL25_LUMP_START		244
#define WALL25_LUMP_END			245

#define WALL26_LUMP_START		246
#define WALL26_LUMP_END			247

#define WALL27_LUMP_START		248
#define WALL27_LUMP_END			249

#define WALL28_LUMP_START		250
#define WALL28_LUMP_END			251

#define WALL29_LUMP_START		252
#define WALL29_LUMP_END			253

#define WALL30_LUMP_START		254
#define WALL30_LUMP_END			255

#define WALL31_LUMP_START		256
#define WALL31_LUMP_END			257

#define WALL32_LUMP_START		258
#define WALL32_LUMP_END			259

#define WALL33_LUMP_START		260
#define WALL33_LUMP_END			261

#define WALL34_LUMP_START		262
#define WALL34_LUMP_END			262

#define WALL36_LUMP_START		263
#define WALL36_LUMP_END			266

#define W_WARP_LUMP_START		267
#define W_WARP_LUMP_END			270

#define W_TORCH_LUMP_START		278
#define W_TORCH_LUMP_END		279

#define SUB_STONE_LUMP_START		280
#define SUB_STONE_LUMP_END		281

#define BLOODY_SUB_STONE_LUMP_START	282
#define BLOODY_SUB_STONE_LUMP_END	283

#define W_BREATH_LUMP_START		284
#define W_BREATH_LUMP_END		287

#define W_WOOD_DOORWAY_LUMP_START	288
#define W_WOOD_DOORWAY_LUMP_END		288

#define W_WOOD_DOORWAY_GLOW_LUMP_START	289
#define W_WOOD_DOORWAY_GLOW_LUMP_END	289

#define W_WATER_GATES_LUMP_START	290
#define W_WATER_GATES_LUMP_END		293

#define W_PRE_CHEATS_LUMP_START		294
#define W_PRE_CHEATS_LUMP_END		294

#define W_CHEAT_GOD_LUMP_START		295
#define W_CHEAT_GOD_LUMP_END		295

#define W_CHEAT_WARP_LUMP_START		296
#define W_CHEAT_WARP_LUMP_END		296

#define W_CHEAT_ITEMS_LUMP_START	297
#define W_CHEAT_ITEMS_LUMP_END		297

#define W_CHEAT_FREEZE_LUMP_START	298
#define W_CHEAT_FREEZE_LUMP_END		298

#define W_POST_CHEAT_LUMP_START		299
#define W_POST_CHEAT_LUMP_END		299

#define W_SURFACE_PLAQ_LUMP_START	300
#define W_SURFACE_PLAQ_LUMP_END		300

#define W_BREATH2_LUMP_START		301
#define W_BREATH2_LUMP_END		304

#define W_MAGE_STATUE_LUMP_START	305
#define W_MAGE_STATUE_LUMP_END		305

#define W_ZOMBIE_STATUE_LUMP_START	306
#define W_ZOMBIE_STATUE_LUMP_END	306

#define W_EYE_STATUE_LUMP_START		307
#define W_EYE_STATUE_LUMP_END		307

#define W_NEM_STATUE_LUMP_START		308
#define W_NEM_STATUE_LUMP_END		308

#define W_SKELETION_STATUE_LUMP_START	309
#define W_SKELETION_STATUE_LUMP_END	309

#define W_SPOOK_STATUE_LUMP_START	310
#define W_SPOOK_STATUE_LUMP_END		310

#define W_ORCH_STATUE_LUMP_START	311
#define W_ORCH_STATUE_LUMP_END		311


//
// Amount of each data item
//
#define NUMCHUNKS    626
#define NUMFONT      1
#define NUMFONTM     0
#define NUMPICS      309
#define NUMPICM      1
#define NUMSPRITES   1
#define NUMTILE8     108
#define NUMTILE8M    36
#define NUMTILE16    216
#define NUMTILE16M   72
#define NUMTILE32    0
#define NUMTILE32M   0
#define NUMEXTERNS   21
//
// File offsets for data items
//
#define STRUCTPIC    0
#define STRUCTPICM   1
#define STRUCTSPRITE 2

#define STARTFONT    3
#define STARTFONTM   4
#define STARTPICS    4
#define STARTPICM    313
#define STARTSPRITES 314
#define STARTTILE8   315
#define STARTTILE8M  316
#define STARTTILE16  317
#define STARTTILE16M 533
#define STARTTILE32  605
#define STARTTILE32M 605
#define STARTEXTERNS 605

//
// Thank you for using IGRAB!
//
