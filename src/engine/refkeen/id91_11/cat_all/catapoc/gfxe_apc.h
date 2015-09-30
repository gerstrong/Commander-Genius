/* Catacomb Apocalypse Source Code
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
// Graphics .H file for .APC
// IGRAB-ed on Thu Dec 02 16:04:05 1993
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
		FIRSTSCALEPIC,               // 20
		// Lump Start
		EYESTALKUNDER1PIC,           // 21
		EYESTALKUNDER2PIC,           // 22
		EYESTALKUNDER3PIC,           // 23
		EYESTALKUNDER4PIC,           // 24
		EYESTALKUNDER5PIC,           // 25
		EYESTALKRISE1PIC,            // 26
		EYESTALKRISE2PIC,            // 27
		EYESTALKWALK1PIC,            // 28
		EYESTALKWALK2PIC,            // 29
		EYESTALKATTACKPIC,           // 30
		EYESTALKDEATH1PIC,           // 31
		EYESTALKDEATH2PIC,           // 32
		EYESTALKDEATH3PIC,           // 33
		EYESTALKDEATH4PIC,           // 34
		EYESTALKDEATH5PIC,           // 35
		// Lump Start
		BLOB_SHOT1PIC,               // 36
		BLOBGND1PIC,                 // 37
		BLOBGND2PIC,                 // 38
		BLOBRISE1PIC,                // 39
		BLOBRISE2PIC,                // 40
		BLOBWALK1PIC,                // 41
		BLOB_SHOT2PIC,               // 42
		BLOBWALK2PIC,                // 43
		BLOBWALK3PIC,                // 44
		BLOBDEATH1PIC,               // 45
		BLOBDEATH2PIC,               // 46
		BLOBDEATH3PIC,               // 47
		// Lump Start
		POTIONOBJPIC,                // 48
		// Lump Start
		RKEYOBJPIC,                  // 49
		// Lump Start
		YKEYOBJPIC,                  // 50
		// Lump Start
		GKEYOBJPIC,                  // 51
		// Lump Start
		BKEYOBJPIC,                  // 52
		// Lump Start
		RGEM1PIC,                    // 53
		// Lump Start
		GGEM1PIC,                    // 54
		// Lump Start
		BGEM1PIC,                    // 55
		// Lump Start
		YGEM1PIC,                    // 56
		// Lump Start
		PGEM1PIC,                    // 57
		// Lump Start
		CHESTOBJPIC,                 // 58
		// Lump Start
		PSHOT1PIC,                   // 59
		PSHOT2PIC,                   // 60
		PSHOT_EXP1PIC,               // 61
		PSHOT_EXP2PIC,               // 62
		PSHOT_EXP3PIC,               // 63
		// Lump Start
		HEADPIC,                     // 64
		// Lump Start
		BOLTOBJPIC,                  // 65
		BOLT2OBJPIC,                 // 66
		BOLT3OBJPIC,                 // 67
		// Lump Start
		NUKEOBJPIC,                  // 68
		NUKE2OBJPIC,                 // 69
		NUKE3OBJPIC,                 // 70
		// Lump Start
		TIMEOBJ1PIC,                 // 71
		TIMEOBJ2PIC,                 // 72
		// Lump Start
		COLUMN5PIC,                  // 73
		// Lump Start
		COLUMN1PIC,                  // 74
		// Lump Start
		FFIRE_POTPIC,                // 75
		// Lump Start
		COLUMN2PIC,                  // 76
		// Lump Start
		COLUMN3PIC,                  // 77
		// Lump Start
		COLUMN4PIC,                  // 78
		// Lump Start
		OFIRE_POT1PIC,               // 79
		OFIRE_POT2PIC,               // 80
		// Lump Start
		TOMB1PIC,                    // 81
		// Lump Start
		TOMB2PIC,                    // 82
		// Lump Start
		O_WATER_CHEST1PIC,           // 83
		O_WATER_CHEST2PIC,           // 84
		O_WATER_CHEST3PIC,           // 85
		// Lump Start
		OLD_CHESTPIC,                // 86
		// Lump Start
		EYE_WALK1PIC,                // 87
		EYE_WALK2PIC,                // 88
		EYE_WALK3PIC,                // 89
		EYE_OUCH1PIC,                // 90
		EYE_OUCH2PIC,                // 91
		EYE_DEATH1PIC,               // 92
		EYE_DEATH2PIC,               // 93
		EYE_DEATH3PIC,               // 94
		EYE_DEATH4PIC,               // 95
		EYE_SHOT1PIC,                // 96
		EYE_SHOT2PIC,                // 97
		// Lump Start
		FMAGEWALK1PIC,               // 98
		FMAGEWALK2PIC,               // 99
		FMAGEWALK3PIC,               // 100
		FMAGEATTACK1PIC,             // 101
		FMAGEATTACK2PIC,             // 102
		FMAGEDEATH1PIC,              // 103
		FMAGEDEATH2PIC,              // 104
		FMAGEDEATH3PIC,              // 105
		FMAGESHOT1PIC,               // 106
		FMAGESHOT2PIC,               // 107
		// Lump Start
		FORCE_FIELD_1PIC,            // 108
		FORCE_FIELD_2PIC,            // 109
		FORCE_FIELD_3PIC,            // 110
		FORCE_FIELD_4PIC,            // 111
		// Lump Start
		ROBOTANKWALK1PIC,            // 112
		ROBOTANKWALK2PIC,            // 113
		ROBOTANKWALK3PIC,            // 114
		ROBOTANKWALK4PIC,            // 115
		ROBOTANKOUCHPIC,             // 116
		ROBOTANKATTACK1PIC,          // 117
		ROBOTANKATTACK2PIC,          // 118
		ROBOTANKDEATH1PIC,           // 119
		ROBOTANKDEATH2PIC,           // 120
		ROBOTANKDEATH3PIC,           // 121
		ROBOTANKDEATH4PIC,           // 122
		// Lump Start
		RAMBONEWALK1PIC,             // 123
		RAMBONEWALK2PIC,             // 124
		RAMBONEWALK3PIC,             // 125
		RAMBONEWALK4PIC,             // 126
		RAMBONEATTACK1PIC,           // 127
		RAMBONEATTACK2PIC,           // 128
		RAMBONEATTACK3PIC,           // 129
		RAMBONEDEATH1PIC,            // 130
		RAMBONEDEATH2PIC,            // 131
		RAMBONEDEATH3PIC,            // 132
		RAMBONESHOT1PIC,             // 133
		RAMBONESHOT2PIC,             // 134
		// Lump Start
		STOMPYWALK1PIC,              // 135
		STOMPYWALK2PIC,              // 136
		STOMPYWALK3PIC,              // 137
		STOMPYWALK4PIC,              // 138
		STOMPYATTACK1PIC,            // 139
		STOMPYATTACK2PIC,            // 140
		STOMPYDEATH1PIC,             // 141
		STOMPYDEATH2PIC,             // 142
		STOMPYDEATH3PIC,             // 143
		STOMPYDEATH4PIC,             // 144
		STOMPYSHOT1PIC,              // 145
		STOMPYSHOT2PIC,              // 146
		STOMPYSHOT3PIC,              // 147
		STOMPYSHOT4PIC,              // 148
		// Lump Start
		TROLL1PIC,                   // 149
		TROLL2PIC,                   // 150
		TROLL3PIC,                   // 151
		TROLL4PIC,                   // 152
		TROLLOUCHPIC,                // 153
		TROLLATTACK1PIC,             // 154
		TROLLATTACK2PIC,             // 155
		TROLLDIE1PIC,                // 156
		TROLLDIE2PIC,                // 157
		TROLLDIE3PIC,                // 158
		// Lump Start
		WIZARDWALK1PIC,              // 159
		WIZARDWALK2PIC,              // 160
		WIZARDWALK3PIC,              // 161
		WIZARDWALK4PIC,              // 162
		WIZARDDEATH4PIC,             // 163
		WIZARD_SHOT1PIC,             // 164
		WIZARD_SHOT2PIC,             // 165
		WIZARDATTACK1PIC,            // 166
		WIZARDATTACK2PIC,            // 167
		WIZARDOUCHPIC,               // 168
		WIZARDDEATH1PIC,             // 169
		WIZARDDEATH2PIC,             // 170
		WIZARDDEATH3PIC,             // 171
		// Lump Start
		INVIS_FIZZ1PIC,              // 172
		INVIS_FIZZ2PIC,              // 173
		INVIS_DEATH1PIC,             // 174
		INVIS_DEATH2PIC,             // 175
		INVIS_DEATH3PIC,             // 176
		INVIS_FIZZ3PIC,              // 177
		// Lump Start
		BUG_WALK1PIC,                // 178
		BUG_WALK2PIC,                // 179
		BUG_WALK3PIC,                // 180
		BUG_ATTACK1PIC,              // 181
		BUG_ATTACK2PIC,              // 182
		BUG_DEATH1PIC,               // 183
		BUG_DEATH2PIC,               // 184
		BUG_SHOT1PIC,                // 185
		BUG_SHOT2PIC,                // 186
		// Lump Start
		CYBORG1PIC,                  // 187
		CYBORG2PIC,                  // 188
		CYBORG3PIC,                  // 189
		CYBORG4PIC,                  // 190
		CYBORGATTACK1PIC,            // 191
		CYBORGATTACK2PIC,            // 192
		CYBORGATTACK3PIC,            // 193
		CYBORGOUCHPIC,               // 194
		CYBORGDIE1PIC,               // 195
		CYBORGDIE2PIC,               // 196
		// Lump Start
		GREL1PIC,                    // 197
		GREL2PIC,                    // 198
		GRELATTACKPIC,               // 199
		GRELHITPIC,                  // 200
		GRELDIE1PIC,                 // 201
		GRELDIE2PIC,                 // 202
		GRELDIE3PIC,                 // 203
		GRELDIE4PIC,                 // 204
		GRELDIE5PIC,                 // 205
		GRELDIE6PIC,                 // 206
		SKULL_SHOTPIC,               // 207
		// Lump Start
		RAYFLY1PIC,                  // 208
		RAYFLY2PIC,                  // 209
		RAYFLY3PIC,                  // 210
		RAYSHOT1PIC,                 // 211
		RAYSHOT2PIC,                 // 212
		RAYSHOOT1PIC,                // 213
		RAYSHOOT2PIC,                // 214
		RAYDEATH1PIC,                // 215
		RAYDEATH2PIC,                // 216
		RAYRISEPIC,                  // 217
		// Lump Start
		PORTAL1PIC,                  // 218
		PORTAL2PIC,                  // 219
		PORTAL3PIC,                  // 220
		PORTAL4PIC,                  // 221
		PORTAL5PIC,                  // 222
		PORTAL6PIC,                  // 223
		// Lump Start
		DEMON1PIC,                   // 224
		DEMON2PIC,                   // 225
		DEMON3PIC,                   // 226
		DEMON4PIC,                   // 227
		DEMONATTACK1PIC,             // 228
		DEMONATTACK2PIC,             // 229
		DEMONATTACK3PIC,             // 230
		DEMONOUCHPIC,                // 231
		DEMONDIE1PIC,                // 232
		DEMONDIE2PIC,                // 233
		DEMONDIE3PIC,                // 234
		FIRSTWALLPIC,                // 235
		NEMESISPIC,                  // 236
		ALTARLEFTPIC,                // 237
		ALTARRIGHTPIC,               // 238
		TEMPLEWALLLIGHTPIC,          // 239
		TEMPLEWALLDARKPIC,           // 240
		TORCHWALL1PIC,               // 241
		TORCHWALL2PIC,               // 242
		BRNBRKLIGHTPIC,              // 243
		BRNBRKDARKPIC,               // 244
		BRNBRKEMLIGHTPIC,            // 245
		BRNBRKEMDARKPIC,             // 246
		FIREWALL1PIC,                // 247
		FIREWALL2PIC,                // 248
		FIREWALL3PIC,                // 249
		FIREWALL4PIC,                // 250
		IRONGATEPIC,                 // 251
		BRNFLGLIGHTPIC,              // 252
		BRNFLGDARKPIC,               // 253
		BRNFLGVINELIGHTPIC,          // 254
		BRNFLGVINEDARKPIC,           // 255
		BRNFLGDMGLIGHTPIC,           // 256
		BRNFLGDMGDARKPIC,            // 257
		SPACEDMG1LIGHTPIC,           // 258
		SPACEDMG1DARKPIC,            // 259
		SPACEDMG2LIGHTPIC,           // 260
		SPACEDMG2DARKPIC,            // 261
		SPACE1LIGHTPIC,              // 262
		SPACE1DARKPIC,               // 263
		SPACE2LIGHTPIC,              // 264
		SPACE2DARKPIC,               // 265
		SPACE3LIGHTPIC,              // 266
		SPACE3DARKPIC,               // 267
		SPACE4LIGHTPIC,              // 268
		SPACE4DARKPIC,               // 269
		SPACE5LIGHTPIC,              // 270
		SPACE5DARKPIC,               // 271
		SPACE6LIGHTPIC,              // 272
		SPACE6DARKPIC,               // 273
		SPACE7LIGHTPIC,              // 274
		SPACE7DARKPIC,               // 275
		SPACE8LIGHTPIC,              // 276
		SPACE8DARKPIC,               // 277
		SPACE10LIGHTPIC,             // 278
		SPACE10DARKPIC,              // 279
		RUSTDOORLIGHTPIC,            // 280
		RUSTDOORDARKPIC,             // 281
		SPACE11LIGHTPIC,             // 282
		SPACE11DARKPIC,              // 283
		SPACE12LIGHTPIC,             // 284
		SPACE12DARKPIC,              // 285
		SPACE13LIGHTPIC,             // 286
		SPACE13DARKPIC,              // 287
		SPACE14LIGHTPIC,             // 288
		SPACE14DARKPIC,              // 289
		SPACEDMG5LIGHTPIC,           // 290
		SPACEDMG5DARKPIC,            // 291
		SPACEDMG6LIGHTPIC,           // 292
		SPACEDMG6DARKPIC,            // 293
		SPCDOOR1LIGHTPIC,            // 294
		SPCDOOR1DARKPIC,             // 295
		SPCDOOR2LIGHTPIC,            // 296
		SPCDOOR2DARKPIC,             // 297
		SPCDOOR3LIGHTPIC,            // 298
		SPCDOOR3DARKPIC,             // 299
		SPCDOOR4LIGHTPIC,            // 300
		SPCDOOR4DARKPIC,             // 301
		COLUMNSLIGHTPIC,             // 302
		COLUMNSDARKPIC,              // 303
		DEMONSTATUELIGHTPIC,         // 304
		DEMONSTATUEDARKPIC,          // 305
		TROLLSTATUELIGHTPIC,         // 306
		TROLLSTATUEDARKPIC,          // 307
		BRNDMGVINELIGHTPIC,          // 308
		BRNDMGVINEDARKPIC,           // 309
		HORNDOORPIC,                 // 310
		RUNEDOORPIC,                 // 311
		IRONDMGLIGHTPIC,             // 312
		IRONDMGDARKPIC,              // 313
		IRONLIGHTPIC,                // 314
		IRONDARKPIC,                 // 315
		TROLLLIGHTPIC,               // 316
		TROLLDARKPIC,                // 317
		TROLLBLOODYLIGHTPIC,         // 318
		TROLLBLOODYDARKPIC,          // 319
		STONEDOORLIGHTPIC,           // 320
		STONEDOORDARKPIC,            // 321
		IRONWTR1LIGHTPIC,            // 322
		IRONWTR2LIGHTPIC,            // 323
		IRONWTR3LIGHTPIC,            // 324
		IRONWTR1DARKPIC,             // 325
		IRONWTR2DARKPIC,             // 326
		IRONWTR3DARKPIC,             // 327
		RUSTWTR1LIGHTPIC,            // 328
		RUSTWTR2LIGHTPIC,            // 329
		RUSTWTR3LIGHTPIC,            // 330
		RUSTWTR1DARKPIC,             // 331
		RUSTWTR2DARKPIC,             // 332
		RUSTWTR3DARKPIC,             // 333
		CEMETARYLIGHTPIC,            // 334
		CEMETARYDARKPIC,             // 335
		WGRATE1LIGHTPIC,             // 336
		WGRATE2LIGHTPIC,             // 337
		WGRATE3LIGHTPIC,             // 338
		WGRATE1DARKPIC,              // 339
		WGRATE2DARKPIC,              // 340
		WGRATE3DARKPIC,              // 341
		BWGRATE1PIC,                 // 342
		BWGRATE2PIC,                 // 343
		GWGRATE3PIC,                 // 344
		EGYPT2LIGHTPIC,              // 345
		EGYPT3LIGHTPIC,              // 346
		EGYPT2DARKPIC,               // 347
		EGYPT3DARKPIC,               // 348
		BRNFLGWINDOWLIGHTPIC,        // 349
		BRNFLGWINDOWDARKPIC,         // 350
		ENDPIC,                      // 351
		SGRATEPIC,                   // 352
		EXP_WALL_1PIC,               // 353
		EXP_WALL_2PIC,               // 354
		EXP_WALL_3PIC,               // 355
		WATER_EXP_WALL_1PIC,         // 356
		WATER_EXP_WALL_2PIC,         // 357
		WATER_EXP_WALL_3PIC,         // 358
		MAS_WIN_LIGHTPIC,            // 359
		MAS_WIN_DARKPIC,             // 360
		MAS_VINE1_LIGHTPIC,          // 361
		MAS_VINE1_DARKPIC,           // 362
		MAS_DOOR_LIGHTPIC,           // 363
		MAS_DOOR_DARKPIC,            // 364
		MAS_VINE2_LIGHTPIC,          // 365
		MAS_VINE2_DARKPIC,           // 366
		TAP1PIC,                     // 367
		TAP2PIC,                     // 368
		TAP3PIC,                     // 369
		LASTWALLPIC,                 // 370

		HAND1PICM=371,

		NORTHICONSPR=372,

		LEVEL1TEXT=663,
		LEVEL2TEXT,                  // 664
		LEVEL3TEXT,                  // 665
		LEVEL4TEXT,                  // 666
		LEVEL5TEXT,                  // 667
		LEVEL6TEXT,                  // 668
		LEVEL7TEXT,                  // 669
		LEVEL8TEXT,                  // 670
		LEVEL9TEXT,                  // 671
		LEVEL10TEXT,                 // 672
		LEVEL11TEXT,                 // 673
		LEVEL12TEXT,                 // 674
		LEVEL13TEXT,                 // 675
		LEVEL14TEXT,                 // 676
		LEVEL15TEXT,                 // 677
		LEVEL16TEXT,                 // 678
		LEVEL17TEXT,                 // 679
		LEVEL18TEXT,                 // 680
		PIRACY,                      // 681
		ENUMEND
	     } graphicnums;

//
// Data LUMPs
//
#define EYESTALK_LUMP_START		21
#define EYESTALK_LUMP_END		35

#define BLOB_LUMP_START			36
#define BLOB_LUMP_END			47

#define POTION_LUMP_START		48
#define POTION_LUMP_END			48

#define RKEY_LUMP_START			49
#define RKEY_LUMP_END			49

#define YKEY_LUMP_START			50
#define YKEY_LUMP_END			50

#define GKEY_LUMP_START			51
#define GKEY_LUMP_END			51

#define BKEY_LUMP_START			52
#define BKEY_LUMP_END			52

#define RGEM_LUMP_START			53
#define RGEM_LUMP_END			53

#define GGEM_LUMP_START			54
#define GGEM_LUMP_END			54

#define BGEM_LUMP_START			55
#define BGEM_LUMP_END			55

#define YGEM_LUMP_START			56
#define YGEM_LUMP_END			56

#define PGEM_LUMP_START			57
#define PGEM_LUMP_END			57

#define CHEST_LUMP_START		58
#define CHEST_LUMP_END			58

#define PLAYER_LUMP_START		59
#define PLAYER_LUMP_END			63

#define HEAD_LUMP_START			64
#define HEAD_LUMP_END			64

#define BOLT_LUMP_START			65
#define BOLT_LUMP_END			67

#define NUKE_LUMP_START			68
#define NUKE_LUMP_END			70

#define TIME_LUMP_START			71
#define TIME_LUMP_END			72

#define COLUMN5_LUMP_START		73
#define COLUMN5_LUMP_END		73

#define COLUMN1_LUMP_START		74
#define COLUMN1_LUMP_END		74

#define FFIREPOT_LUMP_START		75
#define FFIREPOT_LUMP_END		75

#define COLUMN2_LUMP_START		76
#define COLUMN2_LUMP_END		76

#define COLUMN3_LUMP_START		77
#define COLUMN3_LUMP_END		77

#define COLUMN4_LUMP_START		78
#define COLUMN4_LUMP_END		78

#define OFIREPOT_LUMP_START		79
#define OFIREPOT_LUMP_END		80

#define TOMB1_LUMP_START		81
#define TOMB1_LUMP_END			81

#define TOMB2_LUMP_START		82
#define TOMB2_LUMP_END			82

#define O_WATER_CHEST_LUMP_START	83
#define O_WATER_CHEST_LUMP_END		85

#define OLD_CHEST_LUMP_START		86
#define OLD_CHEST_LUMP_END		86

#define EYE_LUMP_START			87
#define EYE_LUMP_END			97

#define FUTUREMAGE_LUMP_START		98
#define FUTUREMAGE_LUMP_END		107

#define FORCEFIELD_LUMP_START		108
#define FORCEFIELD_LUMP_END		111

#define ROBOTANK_LUMP_START		112
#define ROBOTANK_LUMP_END		122

#define RAMBONE_LUMP_START		123
#define RAMBONE_LUMP_END		134

#define STOMPY_LUMP_START		135
#define STOMPY_LUMP_END			148

#define TROLL_LUMP_START		149
#define TROLL_LUMP_END			158

#define WIZARD_LUMP_START		159
#define WIZARD_LUMP_END			171

#define INVISDUDE_LUMP_START		172
#define INVISDUDE_LUMP_END		177

#define BUG_LUMP_START			178
#define BUG_LUMP_END			186

#define CYBORG_LUMP_START		187
#define CYBORG_LUMP_END			196

#define GREL_LUMP_START			197
#define GREL_LUMP_END			207

#define RAY_LUMP_START			208
#define RAY_LUMP_END			217

#define PORTAL_LUMP_START		218
#define PORTAL_LUMP_END			223

#define DEMON_LUMP_START		224
#define DEMON_LUMP_END			234


//
// Amount of each data item
//
#define NUMCHUNKS    682
#define NUMFONT      1
#define NUMFONTM     0
#define NUMPICS      367
#define NUMPICM      1
#define NUMSPRITES   1
#define NUMTILE8     108
#define NUMTILE8M    36
#define NUMTILE16    216
#define NUMTILE16M   72
#define NUMTILE32    0
#define NUMTILE32M   0
#define NUMEXTERNS   19
//
// File offsets for data items
//
#define STRUCTPIC    0
#define STRUCTPICM   1
#define STRUCTSPRITE 2

#define STARTFONT    3
#define STARTFONTM   4
#define STARTPICS    4
#define STARTPICM    371
#define STARTSPRITES 372
#define STARTTILE8   373
#define STARTTILE8M  374
#define STARTTILE16  375
#define STARTTILE16M 591
#define STARTTILE32  663
#define STARTTILE32M 663
#define STARTEXTERNS 663

//
// Thank you for using IGRAB!
//
