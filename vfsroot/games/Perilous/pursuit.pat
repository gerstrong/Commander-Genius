#Tile info patching script for Keen 1
%ext ck1

%version 1.31
%patchfile 0x130F8 tiles.tli

# Your ship is missing...
%patch $0001659B "Keep searching... ;)"
%patch $000165B0 "Clues found:"
%patch $000165C5 "GO FIND MORT!"

#High Score Entries
%patch 0x01593F "Keen "
%patch 0x01594E "Ceilick"
%patch 0x01595D "Spleen"


#Highscore tiles
%patch $992A  $C0 $01   # Joystick
%patch $995D  $C1 $01   # Battery
%patch $9990  $C2 $01   # Vacuum cleaner
%patch $99C3  $C3 $01   # Whisky bottle

#Highscores animation offset disable
%patch $9929 $B8
%patch $995C $B8
%patch $998F $B8
%patch $99C2 $B8

# Change the controls to: Space, shoot; Tab, status.
%patch $1251  $48
%patch $7FA3  $48
%patch $3B23  $90 $90 $90 $90 $90 $90 $90 $80 $3E $65 $50 $00
%patch $3C4E  $90 $90 $90 $90 $90 $90 $90 $80 $3E $65 $50 $00
%patch $3DB9  $90 $90 $90 $90 $90 $90 $90 $80 $3E $65 $50 $00
%patch $3DCC  $90 $90
%patch $3E6D  $90 $90 $90 $90 $90 $90 $90 $80 $3E $65 $50 $00
%patch $3FFC  $90 $90 $90 $90 $90 $90 $90 $80 $3E $65 $50 $00
%patch $40AA  $90 $90 $90 $90 $90 $90 $90 $80 $3E $65 $50 $00

# leaves 227 spare bytes at CS:10D1
# leaves 164 spare bytes at DS:1E02 ($14E52)

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
               $06 $7D $24 $B8 $78 $00 $50 $8B $46 $FE $05 $03 $00 $B1 $03 $D3
               $E0 $90 $90 $50 $89 $F8 $D1 $E0 $05 $01 $00 $01 $F0 $50 $E8 $2C
               $AC $83 $C4 $06 $47 $EB $D1
%patch $0F07                               $B8 $BF $01 $83 $3E $94 $AA $00 $74   # Draw the joystick
               $03 $B8 $C0 $01 $50 $8B $46 $FE $05 $08 $00 $B1 $03 $D3 $E0 $05
               $04 $00 $50 $89 $F0 $05 $10 $00 $50 $E8 $0D $AA $83 $C4 $06
                                                                           $B8   # Draw the battery
               $BF $01 $83 $3E $9C $AA $00 $74 $03 $B8 $C1 $01 $50 $8B $46 $FE
               $05 $08 $00 $B1 $03 $D3 $E0 $05 $04 $00 $50 $89 $F0 $05 $13 $00
               $50 $E8 $E5 $A9 $83 $C4 $06
                                           $B8 $BF $01 $83 $3E $96 $AA $00 $74   # Draw the vacuum
               $03 $B8 $C2 $01 $50 $8B $46 $FE $05 $08 $00 $B1 $03 $D3 $E0 $05
               $04 $00 $50 $89 $F0 $05 $16 $00 $50 $E8 $BD $A9 $83 $C4 $06
                                                                           $B8   # Draw the whiskey
               $BF $01 $83 $3E $98 $AA $00 $74 $03 $B8 $C3 $01 $50 $8B $46 $FE
               $05 $08 $00 $B1 $03 $D3 $E0 $05 $04 $00 $50 $89 $F0 $05 $19 $00
               $50 $E8 $95 $A9 $83 $C4 $06
%patch $0FA7                               $B8 $BF $01 $83 $3E $9E $AA $00 $74   # Keycard A
               $03 $B8 $A8 $01 $50 $8B $46 $FE $05 $03 $00 $B1 $03 $D3 $E0 $05
               $04 $00 $50 $89 $F0 $05 $10 $00 $50 $E8 $6D $A9 $83 $C4 $06
                                                                           $B8   # Keycard B
               $BF $01 $83 $3E $A0 $AA $00 $74 $03 $B8 $A9 $01 $50 $8B $46 $FE
               $05 $03 $00 $B1 $03 $D3 $E0 $05 $04 $00 $50 $89 $F0 $05 $13 $00
               $50 $E8 $45 $A9 $83 $C4 $06
                                           $B8 $BF $01 $83 $3E $A2 $AA $00 $74   # Keycard C
               $03 $B8 $AA $01 $50 $8B $46 $FE $05 $03 $00 $B1 $03 $D3 $E0 $05
               $04 $00 $50 $89 $F0 $05 $16 $00 $50 $E8 $1D $A9 $83 $C4 $06
                                                                           $B8   # Keycard D
               $BF $01 $83 $3E $A4 $AA $00 $74 $03 $B8 $AB $01 $50 $8B $46 $FE
               $05 $03 $00 $B1 $03 $D3 $E0 $05 $04 $00 $50 $89 $F0 $05 $19 $00
               $50 $E8 $F5 $A8 $83 $C4 $06
%patch $1047                               $B8 $0B $00 $83 $3E $9A $AA $00 $74   # Pogo stick
               $03 $B8 $9F $01 $50 $8B $46 $FE $05 $08 $00 $B1 $03 $D3 $E0 $05
               $04 $00 $50 $89 $F0 $05 $0B $00 $50 $E8 $CD $A8 $83 $C4 $06
                                                                           $8B   # Draw ammo
               $46 $FE $05 $09 $00 $50 $89 $F0 $05 $0A $00 $50 $31 $C0 $50 $FF
               $36 $C8 $AA $E8 $2E $01 $83 $C4 $08
                                                   $B8 $9E $01 $50 $8B $46 $FE   # Draw raygun (replace first two bytes
               $05 $08 $00 $B1 $03 $D3 $E0 $05 $04 $00 $50 $89 $F0 $05 $02 $00   # with $EB $1C if you don't want to
               $50 $E8 $95 $A8 $83 $C4 $06                                       # draw the raygun on the status screen)

                                           $E8 $90 $4B $31 $C0 $50 $E8 $1E $AD   # Wait for a key, then return
               $44 $44 $E8 $81 $A6 $E8 $D1 $F1 $E8 $CE $F1 $E8 $7C $4B $8B $46
               $FC $8B $56 $FA $89 $16 $35 $51 $A3 $37 $51 $5F $5E $89 $EC $5D
               $C3

#Raygun Charge Y-Offset
%patch $46DC $04

#Raygun Charge X-Offset
  #Call our subroutine from the correct place
  %patch 0x46CD $E8 $4C $FD

  #Our subroutine, Add ?? pixels to the sprite's x co-ordinate and
  #store it in the sprite structure.
  %patch 0x441C $83 $3E $4C $60 $00
  %patch 0x4421 $7C $0B
  %patch 0x4423 $81 $C2 $00 $12
  %patch 0x4427 $15 $00 $00
  %patch 0x442A $89 $54 $04
  %patch 0x442D $C3
  %patch 0x442E $89 $54 $04
  %patch 0x4431 $C3

  # Include this part if the "multiple background tiles" patch is not used.
  # It gives us 36 spare bytes. (Code stolen from Palette Patch)
  %patch $4409      $B8 $14 $01 $26 $81 $3F $31
                $01 $73 $03 $B8 $8F $00 $26 $89
                $07 $E9 $5C $01

#FlagsPatch
%patch $7C07  $83 $EC $20

%patch $7D13              $31 $F6 $C4 $3E $4C $6C $8B $16 $08 $6C $A1 $58 $6C
%patch $7D20  $F7 $E2 $89 $C1 $09 $C9 $75 $03 $E9 $DA $00 $26 $8B $05 $C7 $46
%patch $7D30  $E0 $00 $00 $A9 $00 $80 $74 $05 $C7 $46 $E0 $01 $00 $25 $FF $7F
%patch $7D40  $75 $03 $E9 $B9 $00 $89 $46 $FC $3D $10 $00 $7C $03 $E9 $85 $00
%patch $7D50  $89 $C3 $D1 $E3 $83 $BF $A4 $AA $00 $75 $03 $E9 $A0 $00 $26 $C7
%patch $7D60  $05 $00 $00 $83 $7E $E0 $00 $74 $03 $E9 $92 $00 $89 $FB $43 $43
%patch $7D70  $26 $8B $07 $25 $FF $7F $3B $46 $FC $75 $47 $26 $C7 $07 $00 $00
%patch $7D80  $03 $1E $08 $6C $03 $1E $08 $6C $26 $C7 $07 $00 $00 $4B $4B $26
%patch $7D90  $C7 $07 $00 $00 $06 $C4 $1E $48 $6C $89 $F0 $D1 $E0 $01 $C3 $26
%patch $7DA0  $C7 $07 $4E $00 $43 $43 $26 $C7 $07 $4F $00 $03 $1E $08 $6C $03
%patch $7DB0  $1E $08 $6C $26 $C7 $07 $51 $00 $4B $4B $26 $C7 $07 $50 $00 $07
%patch $7DC0  $EB $3C $06 $C4 $1E $48 $6C $89 $F0 $D1 $E0 $01 $C3 $26 $C7 $07
%patch $7DD0  $4D $00 $07 $EB $29 $3D $65 $00 $7C $24 $3D $74 $00 $7F $1F $2D
%patch $7DE0  $64 $00 $89 $C3 $D1 $E3 $83 $BF $A4 $AA $00 $74 $11 $06 $C4 $1E
%patch $7DF0  $48 $6C $89 $F0 $D1 $E0 $01 $C3 $26 $C7 $07 $7B $01 $07 $49 $47
%patch $7E00  $47 $46 $E9 $1F $FF $EB $4A

#PogoPatch
%patch $900E  $01 $00 

#Teleporters
#Destination1 (36, 5)
%patch $158DF  $40 $02
%patch $158E3  $50 $00
#Destination3 (59,37)
%patch $158F3  $B0 $03
%patch $158F7  $50 $02

#Background
%patch $4409      $26 $8B $07 $B6 $0D $F6 $FE 
%patch $4410  $F6 $EE $26 $89 $07 $E9 $60 $01 

%patch $4482  $E9 $74 $FF 
%patch $44BA  $E9 $3C $FF 
%patch $4513  $E9 $E3 $FE 

%patch $28EC                  $26 $8B $07 $B6 
%patch $28F0  $0D $F6 $FE $F6 $EE $26 $89 $07 
%patch $28F8  $03 $1E $08 $6C $03 $1E $08 $6C 
%patch $2900  $26 $89 $07 $90 $90 $90 $90 $90 

#TitleScreen
# The title bitmap will be shown at offset 0,0 
%patch $97B2   $00 $00     # Y location, in pixels 
%patch $97B6   $04 $00     # (X location in pixels) / 8 + 4 

# GO ANDY'S TEAM!!
# Make the door act like a wall when moving right
%patch $2F31  $E8 $9D $E1 $90 $90
# Make the door act like a wall when moving left
%patch $2FE6  $E8 $0B $E1 $90 $90

# Here's where the real work is done
%patch $10D1  $81 $3E $54 $82 $C4 $45 $75 $15
              $8B $87 $6E $05 $48 $48 $3D $04
              $00 $73 $0A $89 $C3 $D1 $E3 $83
              $BF $9E $AA $01 $C3 $83 $BF $86
              $18 $00 $C3 $81 $3E $54 $82 $C4
              $45 $75 $15 $8B $87 $6E $05 $48
              $48 $3D $04 $00 $73 $0A $89 $C3
              $D1 $E3 $83 $BF $9E $AA $01 $C3
              $83 $BF $FA $0E $00 $C3

# Stop the door bouncing
%patch $43A4  $E9 $D1 $01

#Menu
%patch 0x015EE9 "New Game"
%patch 0x015EF6 "Load Game    "
%patch 0x015F06 "  Story"
%patch 0x015F12 "Creators   "
%patch 0x015F22 "The Keenest"
%patch 0x015F32 "Community    "
%patch 0x015F44 "Appetiser"
%patch 0x015F52 "Show Title  "

#About ID...
%patch 0x015F80 "Ceilick and Commander Spleen are  "
%patch 0x015FA8 "Commander Keen fans who joined   "
%patch 0x015FD0 "forces to bring you this mod. "
%patch 0x015FF8 "Hope you enjoy it!  "
%patch 0x016020 "Be sure to check out our other  "
%patch 0x016048 "series; Spleen's Monky Business and"
%patch 0x016070 "Ceilick's Decree of the   "
%patch 0x016098 "Skree! Don't miss them. "
%patch 0x0160C0 "                             "
%patch 0x0160E8 "                                 "
%patch 0x016110 "         I like pancakes."
#Finale
%patch 0x015C24 "Commander Keen, only pausing for a "
%patch 0x015C49 "moment to tie his shoe, runs back"
%patch 0x015C6C "to the Bean-with-Bacon Megarocket. "
%patch 0x015C91 "He must catch Mortimer McMire! "

# First dialogue box at home
%patch 0x015CBD "Hey, has another ship "
%patch 0x015CD5 "flown by near this"
%patch 0x015CE9 "planet recently? "

%patch 0x015CFB "'No', said the big "
%patch 0x015D10 "Alien, looking at  "
%patch 0x015D25 "keen oddly"

%patch 0x015D30 "But before you go... "
#Text jump
%patch 0x015D47 "CAN I HAVE YOUR AUTO-"
%patch 0x015D5E "GRAPH? I'M YOUR    "
%patch 0x015D73 "BIGGEST FAN"

%patch 0x015D80 "Uhh, I'm kinda  "
%patch 0x015D92 "busy...  "

%patch 0x015D9C "Just sign right here, "
%patch 0x015DB4 "on this piece of    "
%patch 0x015DCA "paper...               "

%patch 0x015DE2 "Ok, is that good?  "

%patch 0x015DF6 "Thats perfect.  "

%patch 0x015E07 "The Alien waves good-"
%patch 0x015E1E "bye as Keen flies into "
%patch 0x015E37 "the star in pursuit"
%patch 0x015E4C "of his nemesis, Mort-"
%patch 0x015E63 "imer McMire..."

%level.hint 15
You've found a note
from Mortimer. No
time to read it now-
his ship is taking
off! Hurry!

%level.hint 2
It's Mortimer's gun!
You're gaining on
him!

%level.hint 12
Mort's yo-yo!
He must be close!

%level.hint 9
This is Mortimer's
Report Card. He has
all As--get him!!

#CTSpace Cheat Code
%patch 0x011F5 0x505C
%patch 0x011FC 0x5040
%patch 0x01203 0x504A

#Ordering Info
%patch 0x01632C "     Long Live Keen!    "
%patch 0x016346 " ----------------------- "
%patch 0x016364 " Public Commander  "
%patch 0x01637B "    Keen Forum       "
%patch 0x016392 "    p072.ezboard.com/    "
%patch 0x0163AD "bpubliccommanderkeenforum"
%patch 0x0163C8 "      Keen:Modding       "
%patch 0x0163E4 "     andy.durdin.net/keen/modding    "
%patch 0x01640D "                                   "
%patch 0x016434 "With so many new patches and utils "
%patch 0x01645B "of late, now's the perfect time  "
%patch 0x016480 "to start a new Keen mod! ;,"
%patch 0x0164B5 "Tom 0wnz Keen! "
%patch 0x0164DD "The Universe   "
%patch 0x016505 "WILL be Toast! "
%patch 0x01652D "         -Spleen "
%patch 0x0164C6 "Coming soon...  "
%patch 0x0164EE "Spleen's 1-Lev "
%patch 0x016516 "mod: Retrosis. "
%patch 0x016542 "All hail the patch gods: Andy Durdin,   "
%patch 0x016573 "MultiMania, and Xtraverse!        "

#Vorticon strength patch
%patch $180D  $03 $00      # Strength of regular vorticon (1 less than # shots)
%patch $1813  $10          # Level on which vorticon commander appears
%patch $1819  $03 $00      # Strength of vorticon commander (1 less than # shots)

%end
