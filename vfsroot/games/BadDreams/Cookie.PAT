
# SpaceBar Status Menu
%patch 0x00014e66 "LIFE"
%patch 0x00014e77 "LIVES"
%patch 0x00014e9B "N.STUN"
%patch 0x00014eac "KEYGEMS:"
%patch 0x00014ec8 "SHOTS:"

# This is the color palette: 16 entries of r, g, b (each 0-63) 
%patch $13054 
       00 00 00 # black 
       00 00 52 # blue 
       00 52 00 # green 
       00 52 52 # cyan 
       52 00 00 # red 
       45 00 45 # magenta 
       52 31 00 # brown 
       45 45 45 # bright gray 
       30 30 30 # gray 
       17 17 63 # bright blue 
       17 63 17 # bright green 
       17 63 63 # bright cyan 
       63 27 27 # bright red 
       63 17 63 # pink 
       63 63 17 # yellow 
       63 63 63 # white 

# Set the palette when we go into graphics mode 
%patch $6E46  $E8 $D3 $D5 $90 $90 

# Code to set graphics mode and set the palette 
%patch $441C                  $B8 $0D $00 $CD 
              $10 $B8 $12 $10 $B9 $08 $00 $BB 
              $00 $00 $8C $DA $8E $C2 $BA $04 
              $00 $CD $10 $BB $18 $00 $BA $1C 
              $00 $CD $10 $C3 

# Include this part if the "Change background tiles" patch is not used 
# It gives us 36 spare bytes for the palette-setting code 
%patch $4409      $B8 $14 $01 $26 $81 $3F $31 
              $01 $73 $03 $B8 $8F $00 $26 $89 
              $07 $E9 $5C $01 

%patch $97B2   $00 $00     # Y location, in pixels 
%patch $97B6   $04 $00     # (X location in pixels) / 8 + 4

# Disable the "no-clipping" on the map with GOD mode 
%patch $A626  $EB 

%patch $15076 400
%patch $1507C 800
%patch $1507E 1600

#change border colour values are: $ 01 00 02 to 07 38 to 3F
# All these should have the same value -- this is the usual border color 
%patch $4A62   $13 
%patch $6F9D   $03 
%patch $15568  $01 
%patch $15579  $11 
%patch $1558A  $03 
%patch $1559B  $13 

# The two colors that the border flashes when a Vorticon dies 
%patch $4A7D $02 
%patch $4A85 $12

#God mode
%patch 0x01500A "Sweeeeeet!      "
%patch 0x01501B "Suxx0r!          "

#CTSpace
%patch 0x014DEE "Keen thinks: It's MY "
%patch 0x014E05 "dream, I think I'll have  "
%patch 0x014E21 "99 flower powers, Oh, "
%patch 0x014E39 "and a pony too!         "

%patch $180D  $0B $00      # Strength of regular vorticon (1 less than # shots) 
%patch $1813  $10          # Level on which vorticon commander appears 
%patch $1819  $68 $00      # Strength of vorticon commander (1 less than # shots) 

#God Cheat Code
%patch 0x0120D 0x504A
%patch 0x01214 0x502F
%patch 0x0121B 0x5048

#CTSpace Cheat Code
%patch 0x011F5 0x5040
%patch 0x011FC 0x504A
%patch 0x01203 0x505C


# high score tiles:
%patch $992A  $B1 $00   # Joystick 
%patch $995D  $B2 $00   # Battery 
%patch $9990  $B1 $00   # Vacuum cleaner 
%patch $99C3  $B2 $00   # Whisky bottle 

%patch 0x015EF6 "Restore  Game"
%patch 0x015F12 "About Me..."
%patch 0x015F22 "The Keenest"
%patch 0x015F44 "Pictures!"

%patch 0x015F80 "Hi I am Adriael, a long-time Keen "
%patch 0x015FA8 "fan and a new mod-maker. Big     "
%patch 0x015FD0 "thanks to all those whose     "
%patch 0x015FF8 "graphics and stuff  "
%patch 0x016020 "contributed to this mod. To     "
%patch 0x016048 "everyone else, enjoy the game and  "
%patch 0x016070 "watch out for the next    "
%patch 0x016098 "episodes comming soon! "
%patch 0x0160C0 "Hey Apogee! release Keen 7   "
%patch 0x0160E8 "now before I come over there and "
%patch 0x016110 "recolour your palette!   "

# Patch in the long-integer drawing function 
%patch $11B4                   $55 $89 $E5 $B8 $0A $00 $50 $B8 $90 $82 $50 $FF 
               $76 $06 $FF $76 $04 $E8 $03 $C2 $83 $C4 $08 $B8 $90 $82 $50 $E8 
               $A5 $C5 $44 $44 $8B $56 $08 $29 $C2 $89 $16 $82 $82 $8B $46 $0A 
               $A3 $F8 $82 $B8 $90 $82 $50 $E8 $E8 $50 $44 $44 $89 $EC $5D $C3 

# Patch in the name of the status dialog text file 
%patch $13084  "STATUS.CK1" $00 

# Patch in the code to draw the status text (coordinates are measure in characters--8 pixel blocks) 
%patch $0E6A                                           $B8 $00 $3D $BA $34 $00   # Show status window (0E6A) 
               $CD $21 $89 $C3 $BF $27 $00 $C6 $06 $9A $82 $00 $B9 $0A $00 $BA 
               $90 $82 $B4 $3F $CD $21 $53 $B8 $90 $82 $50 $E8 $2E $97 $44 $44 
               $5B $4F $75 $E8 $B4 $3E $CD $21 
                                               $8B $46 $FE $05 $01 $00 $50 $89   # Show score (0E98) 
               $F0 $05 $0A $00 $50 $FF $36 $CC $AA $FF $36 $CA $AA $E8 $04 $03 
               $83 $C4 $08 
                           $8B $46 $FE $05 $01 $00 $50 $89 $F0 $05 $1A $00 $50   # Show extra Keen score (0EB3) 
               $A1 $C4 $6E $8B $16 $C2 $6E $81 $C2 $20 $4E $15 $00 $00 $50 $52 
               $E8 $E1 $02 $83 $C4 $08 
                                       $31 $FF $3B $3E $C6 $AA $7D $29 $83 $FF   # Show Keens (0ED6) 
               $06 $7D $24 $B8 $7B $00 $50 $8B $46 $FE $05 $03 $00 $B1 $03 $D3 
               $E0 $90 $90 $50 $89 $F8 $D1 $E0 $05 $01 $00 $01 $F0 $50 $E8 $2C 
               $AC $83 $C4 $06 $47 $EB $D1 
%patch $0F07                               $B8 $44 $01 $83 $3E $94 $AA $00 $74   # Draw the joystick 
               $03 $B8 $43 $01 $50 $8B $46 $FE $05 $03 $00 $B1 $03 $D3 $E0 $05 
               $04 $00 $50 $89 $F0 $05 $10 $00 $50 $E8 $0D $AA $83 $C4 $06 
                                                                           $B8   # Draw the battery 
               $44 $01 $83 $3E $9C $AA $00 $74 $03 $B8 $43 $01 $50 $8B $46 $FE 
               $05 $03 $00 $B1 $03 $D3 $E0 $05 $04 $00 $50 $89 $F0 $05 $13 $00 
               $50 $E8 $E5 $A9 $83 $C4 $06 
                                           $B8 $44 $01 $83 $3E $96 $AA $00 $74   # Draw the vacuum 
               $03 $B8 $43 $01 $50 $8B $46 $FE $05 $03 $00 $B1 $03 $D3 $E0 $05 
               $04 $00 $50 $89 $F0 $05 $16 $00 $50 $E8 $BD $A9 $83 $C4 $06 
                                                                           $B8   # Draw the whiskey 
               $44 $01 $83 $3E $98 $AA $00 $74 $03 $B8 $43 $01 $50 $8B $46 $FE 
               $05 $03 $00 $B1 $03 $D3 $E0 $05 $04 $00 $50 $89 $F0 $05 $19 $00 
               $50 $E8 $95 $A9 $83 $C4 $06 
%patch $0FA7                               $B8 $44 $01 $83 $3E $9E $AA $00 $74   # Keycard A 
               $03 $B8 $42 $01 $50 $8B $46 $FE $05 $07 $00 $B1 $03 $D3 $E0 $05 
               $04 $00 $50 $89 $F0 $05 $13 $00 $50 $E8 $6D $A9 $83 $C4 $06 
                                                                           $B8   # Keycard B 
               $44 $01 $83 $3E $A0 $AA $00 $74 $03 $B8 $44 $01 $50 $8B $46 $FE 
               $05 $07 $00 $B1 $03 $D3 $E0 $05 $04 $00 $50 $89 $F0 $05 $17 $00 
               $50 $E8 $45 $A9 $83 $C4 $06 
                                           $B8 $44 $01 $83 $3E $A2 $AA $00 $74   # Keycard C 
               $03 $B8 $44 $01 $50 $8B $46 $FE $05 $09 $00 $B1 $03 $D3 $E0 $05 
               $04 $00 $50 $89 $F0 $05 $13 $00 $50 $E8 $1D $A9 $83 $C4 $06 
                                                                           $B8   # Keycard D 
               $44 $01 $83 $3E $A4 $AA $00 $74 $03 $B8 $44 $01 $50 $8B $46 $FE 
               $05 $09 $00 $B1 $03 $D3 $E0 $05 $04 $00 $50 $89 $F0 $05 $17 $00 
               $50 $E8 $F5 $A8 $83 $C4 $06 
%patch $1047                               $B8 $44 $01 $83 $3E $9A $AA $00 $74   # Pogo stick 
               $03 $B8 $44 $01 $50 $8B $46 $FE $05 $08 $00 $B1 $03 $D3 $E0 $05 
               $04 $00 $50 $89 $F0 $05 $2E $02 $50 $E8 $CD $A8 $83 $C4 $06 
                                                                           $8B   # Draw ammo
               $46 $FE $05 $0B $00 $50 $89 $F0 $05 $06 $00 $50 $31 $C0 $50 $FF
               $36 $C8 $AA $E8 $2E $01 $83 $C4 $08
                                                   $B8 $9E $01 $50 $8B $46 $FE   # Draw raygun (replace first two bytes with $EB $1C if you don't
               $05 $07 $00 $B1 $03 $D3 $E0 $05 $04 $00 $50 $89 $F0 $05 $03 $00   # want to draw the raygun on your status screen)
               $50 $E8 $95 $A8 $83 $C4 $06
                                           $E8 $90 $4B $31 $C0 $50 $E8 $1E $AD   # Wait for a key, then return 
               $44 $44 $E8 $81 $A6 $E8 $D1 $F1 $E8 $CE $F1 $E8 $7C $4B $8B $46 
               $FC $8B $56 $FA $89 $16 $35 $51 $A3 $37 $51 $5F $5E $89 $EC $5D 
               $C3 
 
#Give Keen the pogo at the start of every game
%patch $900E  $01 $00

#make keen3 points background
%patch $4409      $26 $8B $07 $B6 $0D $F6 $FE 
%patch $4410  $F6 $EE $26 $89 $07 $E9 $60 $01 

%patch $4482  $E9 $74 $FF 
%patch $44BA  $E9 $3C $FF 
%patch $4513  $E9 $E3 $FE 

%patch $28EC                  $26 $8B $07 $B6 
%patch $28F0  $0D $F6 $FE $F6 $EE $26 $89 $07 
%patch $28F8  $03 $1E $08 $6C $03 $1E $08 $6C 
%patch $2900  $26 $89 $07 $90 $90 $90 $90 $90 

#Teddy bear 20'000 points.
%patch $1507E $20 $4E 

%patchfile 0x130F8 set.tli

#God mode
%patch 0x014EF6 "Sweeeeeet!      "
%patch 0x014F07 "Suxx0r!          "

# screen on Mars, at coordinates 9, 37 on World Map.
%patch 0x00015c24 "And so the land of Tuberia was     "
%patch 0x00015c47 0x0A 0x00
%patch 0x00015c49 "freed from its candy enforced      "
%patch 0x00015c6a 0x0A 0x00
%patch 0x00015c6c "slavery. And peace reigned once    "
%patch 0x00015c8F 0x0A 0x00
%patch 0x00015c91 "more.                           "

# First Dialogue box at home, and so on
%patch 0x00015cbd "Like his brother Jabba"
%patch 0x00015cd3 0x0A 0x00
%patch 0x00015cd5 "Pizza the Hut was "
%patch 0x00015ce7 0x0A 0x00
%patch 0x00015ce9 "defeated.        "
%patch 0x00015cf9 0x00 0x00

%patch 0x00015cfb "In celebration the   "
%patch 0x00015d0e 0x0A 0x00
%patch 0x00015d10 "Tuberians threw a  "
%patch 0x00015d23 0x0A 0x00
%patch 0x00015d25 "Yesss!    "
%patch 0x00015d25 "party.    "
%patch 0x00015d2F 0x00

%patch 0x00015d30 "Three cheers for Keen"
%patch 0x00015d45 0x0A 0x00
# WHAT IS THAT ONE-EYED GREEN THING IN YOUR ROOM?
%patch 0x00015d47 "HUZZAH HUZZAH HUZZAH!"
%patch 0x00015d5c 0x0A 0x00
%patch 0x00015d5e "Considering that   "
%patch 0x00015d71 0x0a 0x00
%patch 0x00015d73 "the food wassss"
%patch 0x00015d7E 0x0a 0x00
%patch 0x00015d80 "mainly dirt,    "
%patch 0x00015d90 0x0A 0x00

%patch 0x00015d92 "Keen      "
%patch 0x00015d9B 0x00
%patch 0x00015d9c "stayed quite a while. "
%patch 0x00015db2 0x0A 0x00
%patch 0x00015db4 "(He dreamed up some "
%patch 0x00015dc8 0x0A 0x00
%patch 0x00015dcA "cheetos and pepsi)    "
%patch 0x00015de0 0x00 0x00

# OK, Mom. Goodnight.
%patch 0x00015de2 "And some Vodka when"
%patch 0x00015df5 0x00
# Goodnight, Dear
%patch 0x00015df6 "noone was looking"
%patch 0x00015e06 0x00

%patch 0x00015e07 "Now why did Keen get "
%patch 0x00015e1C 0x0A 0x00
%patch 0x00015e1E "Boobus bombs? Well,    "
%patch 0x00015e35 0x0A 0x00
%patch 0x00015e37 "                   "
%patch 0x00015e4a 0x0A 0x00
%patch 0x00015e4c "Dreams are funny     "
%patch 0x00015e61 0x0A 0x00
%patch 0x00015e63 "like that.    "
%patch 0x00015e71 0x00

%end
