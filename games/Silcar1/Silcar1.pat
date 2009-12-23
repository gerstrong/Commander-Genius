#Tile info patching script for Keen 1
%ext ck1

%version 1.1
%patchfile 0x131F8 Tiles.tli

%version 1.31
%patchfile 0x130F8 Tiles.tli

#makes the butler bot deadly
%patch $1782 $04 $00 

#Makes all Vorticon jumps the maximum height:
%patch $C05C $C3

#Shots for Vorticon Commander
%patch $1819  $02 $00

#Shipparts
%patch 0x01659B "The Set is missing" 

%level.hint 2
The Salc says:
"Listen to what 
my friends have 
to say!"

%level.hint 6
"There are two
ways to reach
the exit of 
this level."

%level.hint 11
"An essential 
item is here.
Don't leave
without it."

%level.hint 15
"You will have
to climb the 
highest tree
on this planet
to get the
last item."

#vorticon shots
%patch $180d $01 $00

#About ID...
%patch 0x015F80 "I'm a long time commander keen fan"
%patch 0x015FA8 "Back in 1990 I played Keen 1. Now"
%patch 0x015FD0 "Keen turns 18, and I've come  "
%patch 0x015FF8 "up with a new Mod.  "
%patch 0x016020 "This mod is made with iKeen,    "
%patch 0x016048 "TILEINFO, Patchotron, Corel Photo- "
%patch 0x016070 "paint, and above all,     "
%patch 0x016098 "Modkeen, and Ck1patch. "
%patch 0x0160C0 "Thanks to all the creators of"
%patch 0x0160E8 "these programs who made this     "
%patch 0x016110 "mod possible.            "

#Status Screen
%patch 0x014E83 "TEA SET   "
%patch 0x014EAC "KEYS    "
%patch 0x014E9B "PISTOL"

#Menu
%patch 0x015F12 "About Me   "
%patch 0x015F32 "Episodes     "

#After text, Keen doesn't leave Earth
%patch $81B2 $90 

#Window x location
%patch $8596 $00 $00

#Dark Window x location
%patch $85B9 $00 $00 

#Finale
%patch 0x015C24 "Commander Keen rushes back to the  "
%patch 0x015C49 "B-W-B Megarocket, to bring the   "
%patch 0x015C6C "Tea Set home before his grandma    "
%patch 0x015C91 "wakes up!                      "

# First dialogue box at home
%patch 0x015CBD "On his way Keen       "
%patch 0x015CD5 "stumbled upon     "
%patch 0x015CE9 "giant spaceships "

%patch 0x015CFB "They looked like   "
%patch 0x015D10 "dangerous warships "
%patch 0x015D25 "          "

%patch 0x015D30 "And the design looked"
#Text jump
%patch 0x015D47 "strangely familiar..."
%patch 0x015D5E "                   "
%patch 0x015D73 "just like.."

%patch 0x015D80 "Mortimer!!!     "
%patch 0x015D92 "         "

%patch 0x015D9C "One of the ships had  "
%patch 0x015DB4 "the label he most   "
%patch 0x015DCA "feared to find here.   "

%patch 0x015DE2 "MM                 "

%patch 0x015DF6 "!!!             "

%patch 0x015E07 "There is no cake     "
%patch 0x015E1E "for Commander Keen now!"
%patch 0x015E37 "Keen must          "
%patch 0x015E4C "investigate these    "
%patch 0x015E63 "ships!        "

#Exit tiles don't change when Keen exits
%patch $417E $90 #Top blank tile doesn't appear
%patch $41A7 $90 #Bottom blank tile doesn't appear

# Keen does not lose ship parts on dying in their levels.
%patch $4F82 $90 $90 $90 $90 $90 $90 #Joystick
%patch $4F8A $90 $90 $90 $90 $90 $90 #Battery
%patch $4F92 $90 $90 $90 $90 $90 $90 #Vacum
%patch $4F9A $90 $90 $90 $90 $90 $90 #everclear

%end
