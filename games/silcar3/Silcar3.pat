#Tile info patching script for Keen 3
%ext ck3

%version 1.0
%patchfile 0x199F8 tiles.tli

%version 1.31
%patchfile 0x198C8 tiles.tli

%patchfile $18BD0 "STORYTXT.CK3" #Story
%patchfile $179D0 "HELPTEXT.CK3" #Help
%patchfile $181A4 "ENDTXT.CK3"  #End text

# Menu Items
%patch 0x01C801 "This Mod   "
%patch 0x01BB3D "ENERGY   "

#About ID...
%patch 0x01C870 "I'm Tulip and I bring you this    "
%patch 0x01C898 "new episode of Onward: Silcar    "
%patch 0x01C8C0 "And again I thank you for     "
%patch 0x01C8E8 "downloading and     "
%patch 0x01C910 "playing it. Have fun!           "
%patch 0x01C938 "Special thanks go to Levellass     "
%patch 0x01C960 "for her incredible patches"
%patch 0x01C988 "and Karz for           "
%patch 0x01C9B0 "beta-testing. Go visit       "
%patch 0x01C9D8 "keenmodding.org for more mods!   "
%patch 0x01CA00 "Now go play it!          "

#Foob
#Running away right 
%patch $4B6C $06 $FF 
#Running away left 
%patch $4B74 $FA $00
%patch $3E1B $02 $00 #Foob kills

#Jack
#Behaviour to touch
%patch $3EFF $03 $00

%patch $3F4C $96 $00 #Jack initial if spawned above/left of Keen [right]
%patch $3F45 $6A $FF #Jack initial if spawned below right of Keen [left]
%patch $3F6C $96 $00 #Jack initial if spawned above Keen [down]
%patch $3F65 $6A $FF #Jack initial if spawned below Keen [up]

%patch $4BE2 $6A $FF #Jack left speed 2
%patch $4BEE $96 $00 #Jack right speed 2
%patch $4BFA $6A $FF #Jack upwards speed 2
%patch $4C06 $96 $00 #Jack downwards speed 2

#Changing pointitem scores:
%patch $1C085 $64 $00 #100
%patch $1C087 $C8 $00 #200
%patch $1C083 $F4 $01 #500
%patch $1C089 $20 $03 #800
%patch $1C08B $18 $43 #17176

#Purple Spark is killing
%patch $4071 $02 $00

################
# Meep         #
################
%patch $3D84 $04 $00 #Strength
#The new behaviors:
%patch $153A $C3     #Create space for behaviors
%patchfile 5435 DATA.CK3
#Make sprites use this data:
%patch $4877 $BB $15 #Make Meep bullet use new behavior
%patch $47B6 $3B $16 #Falling Meep uses new behavior
#Meep shot:
%patch $15D2 $80 $00 #left sprite
%patch $15CA $7E $00 #Right sprite
%patch $15DC $01 $00 #Frames used -1
%patch $15F0 $03 $00 #Vanish frequency
#Meep falling:
%patch $1652 $76 $00 #Left sprite
%patch $164A $78 $00 #Right sprite 
#Meep shot position (Default $000010 - $000004,if zero then no shot appears)
%patch $4848     $01 #Horizontal
%patch $484A $00 $00
%patch $486A     $18 #Vertical
%patch $486C $00 $00 
#Meep, blank, use yorpdeathsnd
%patch $13D20 $36 $20 $C8


#Vortininja
%patch $3DE6 $01 $00 #Strength

# Vortimom
%patch $3D1C $01 $00 #Strength
#Vortimom shooting height (Default = $FFFFF0, if zero then no shot appears)
%patch $4514     $F9
%patch $4516 $FF $FF 

# Make vortikid kill Keen. 
%patch $3C8D $02 $00 
#Initial [right] speed IF vortikid is spawned above/left of Keen
%patch $3CDA $FE $00 
#Initial [left] speed IF vortikid is spawned below right of Keen
%patch $3CD3 $01 $FF 
#Kid hits left wall while running\sliding [Goes right]
%patch $43D4 $FC $00 
#Kid hits left wall while jumping [goes right]
%patch $438C $FC $00 
#Kid hits right wall while jumping [goes left]
%patch $4380 $04 $FF 
#Kid hits right wall when running/sliding [goes left]
%patch $43C8 $04 $FF



#Make pogo only fly if Keen has Ankh-time. 
%patch $75F0 $46 $99 
#The "invincibility sprite" is not shown until the "time running out" time is reached (even for God mode). 
%patch $8502 $EB $74 
#The ankh does not stop vortikids from knocking over Keen 
%patch $7C8A $EB
#The ankh does not make Keen invincible 
%patch $7918 $EB 
%patch $75FA $75 

#Mangling Machine
#Wallpaper tiles that appear when legs rise/fall
%patch $532E $9C $00 #Heel of both legs, rising
%patch $534D $9C $00 #Left foot right middle rising/falling
%patch $5368 $9C $00 #Left foot left middle rising/falling
%patch $5383 $9C $00 #Left foot toe rising/falling
%patch $539E $9C $00 #Right foot left middle rising/falling
%patch $53B8 $9C $00 #Right foot right middle rising/falling
%patch $53D3 $9C $00 #Right foot toe rising/falling
#Wallpaper tiles that appear when arm moves:
%patch $5149 $9C $00 #Falling left finger
%patch $517C $9C $00 #Rising left finger
%patch $5131 $9C $00 #Lifting arm
%patch $5194 $9C $00 #Falling right finger
%patch $51C7 $9C $00 #Rising right finger

#Mortimer Conversation
%patch 0x01C08D "No..."

%patch 0x01C093 "You're not.."

%patch 0x01C0A0 "MORTIMER McMIRE?  "

%patch 0x01C0B3 "No, I'm not! But I wanted you to      "
%patch 0x01C0DB "believe it was him. It was I who      "
%patch 0x01C103 "brought you here. Mortimer only gave "
%patch 0x01C12A "me the power of this GENE MACHINE  "
%patch 0x01C14F "With this machine I rule the Universe"

%patch 0x01C176 "I thought my genetically improved  "
%patch 0x01C19B "Killer Salcs would have killed you"
%patch 0x01C1BF "by now, but it seems they have failed "
%patch 0x01C1E7 "me. I'll have to create stronger    "
%patch 0x01C20D "Killers!   "

%patch 0x01C21A "Ya won't get through with this!"
%patch 0x01C23B "I'm gonna foil your evil plan"

%patch 0x01C25A "You don't seem to realize this "
%patch 0x01C27B "is in fact a one way force field."
%patch 0x01C29E "To get here you'll have to get  "
%patch 0x01C2C0 "through these Caves of Doom!!    "

%patch 0x01C2E2 "The Stones in there will stomp you"

%patch 0x01C305 "But I can see you'll try anyway...   "
%patch 0x01C32C "I can't take that risk, so I'll just "
%patch 0x01C353 "have to shoot you.       "

#Finale

%patch $1C36D "Thank you, Commander Keen! You've     "
%patch $1C395 "saved us from that mad Salc. He once"
%patch $1C3BB "was a good minister of me but then the"
%patch $1C3E3 "Gannalech came and changed everything."

%patch $1C40B "He turned against me and enslaved     "
%patch $1C433 "my poor Salcs. Now you freed them     "
%patch $1C45B "and I can rule them righteously.     "

%patch $1C482 "Here is a picture of our Fax-o-Racle. "
%patch $1C4AA "It will tell you your future. But we  "
%patch $1C4D2 "Don't understand it yet.  "

%patch $1C4EE "Neither do I"

%patch $1C507 "The End..?"
%patch $1C512 "No way.  "

#Stop flash tile appearing
%patch $5B9B $05 $01
#Stop head being replaced
%patch $5BB9 $05 $01
#Stop leftmost vorticon head being replaced
%patch $5BF4 $05 $01
#Stop middle right facing vorticons head being replaced
%patch $5C18 $05 $01
%patch $5C37 $05 $01

%patch $5CC8 $86 $01 #Vort 3

#How many sparks need to be shot to destroy the arms
%patch $4E53 $96 $00

#Pogo tiles dissapear when touched, without ANY othe effects
%patch $7AD6  $7E $06 $48 $9D $01 $00 


%end
