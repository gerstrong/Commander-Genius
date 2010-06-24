
%ext ck1


%level.hint 2
Mortimer McMire:

"Ah, if it isn't
 Commander Clown.
 Trying to ruin my
 fun, are you?
 We'll see... ha!"

%level.hint 6
Mortimer McMire:

"Have you met the
 Sparky Mark II?
 I've improved the
 original Shikadi
 design--faster."

%level.hint 9
Mortimer McMire:

"Remember the
 Mangling Machine,
 314?  My android
 dummies are back
 to ruin you!"

%level.hint 10
Mortimer McMire:

"Enjoy the ride on
 the Friction
 Dispersal Pads?  
 The Cargo Bay is
 no place for you."

%level.hint 11
Mortimer McMire:

"Alright, Keen--
  Prepare to Die!"

%level.hint 12
Mortimer McMire:

"You're probably
 running low on
 ammo, eh, Keen? 
 You won't last."

%level.hint 15
Mortimer McMire:

"You might enjoy
 this, Keen--an
 old-style play
 field!  Ha!"

#################
# XKY GOES HACK #
#################

# Your ship is missing...
%patch 0x0001659B "Keen needs to steal "

# SpaceBar Status Menu
%patch 0x00014e66 "LIFE"
%patch 0x00014e77 "LIVES"
%patch 0x00014e9B "N.STUN"
%patch 0x00014eac "KEYGEMS:"
%patch 0x00014ec8 "SHOTS:"


# About ID Screen:
%patch 0x00015f80 "Xky Rauh is a long-time Commander "
%patch 0x00015fa8 "Keen fan whose goal with this mod"
%patch 0x00015fd0 "was to uphold the spirit and  "
%patch 0x00015ff8 "fun of the original."
%patch 0x00016020 "This effort was only possible   "
%patch 0x00016048 "with support from people like Andy "
%patch 0x00016070 "Durdin and the many Keen  "
%patch 0x00016098 "fans at CC314s EzBoard"
%patch 0x000160c0 "Thank you in advance for your"
%patch 0x000160e8 "interest in my work, and being so"
%patch 0x00016110 "patient for its release! "

#the OrderInfo screen... hack hack hack
%patch 0x0001633c "The Uni-"
%patch 0x00016344 0x0A 0x00
%patch 0x00016346 "verse is Toast!!"
%patch 0x00016392 "Unfortunately, only id   "
%patch 0x000163ab 0x0A 0x00
%patch 0x000163ad "Software knows for sure  "
%patch 0x000163c6 0x0A 0x00
%patch 0x000163c8 "when the trilogy will be "
%patch 0x000163e1 0x0A 0x00
%patch 0x000163e4 "      more than just a dream...      "
%patch 0x00016409 0x0A 0x00
%patch 0x0001640b "                                 :-( "
%patch 0x00016430 0x0A 0x00
%patch 0x00016432 "                                     "
%patch 0x00016457 0x0A 0x00
%patch 0x00016459 "We may never see a true conclusion,"
%patch 0x0001647c 0x0A 0x00
%patch 0x0001647e "    but Modding is still fun!"
%patch 0x0001649b 0x0A 0x0A 0x00                

#Check out CC314...
%patch 0x000164b3 " Check out CC314:"
%patch 0x000164c4 0x0A 0x00
%patch 0x000164c6 "(a 56k or cable           http://www. "
%patch 0x000164ec 0x0A 0x00
%patch 0x000164ee " modem both work        ClassicGaming  "
%patch 0x00016514 0x0A 0x00
%patch 0x00016516 " just as well.)           .com/cc314/   "
%patch 0x0001653e 0x0A 0x0A 0x0A 0x00
%patch 0x00016542 " Lots of Commander Keen info, maps, and "
%patch 0x0001656a 0x0A 0x00
%patch 0x0001656b "  downloads like SGA or Level Editors!   "
%patch 0x00016599 0x0A 0x00

#Title Screen's Menu Options
#wonderful--i've got to shift everything back a hex to fit my name... *laugh*
%patch 0x00015ef6 "Restore Game"
%patch 0x00015f02 0x0A 0x00 0x20 0x20 0x20 0x20 "Story" 0x0A 0x00 0x20 0x20 0x20 "About Xky"
%patch 0x00015f22 "The Keenest"
%patch 0x00015f32 "Keen 7 8 & 9?"
%patch 0x00015f44 "Old Stuff"
%patch 0x00015f52 "Title Screen"

##################
##### ENDING #####
##################

# This is the Ending Sequence... 
# it was a pain in the butt to decode 
# with my useless HEX editor... 
# but at least now it's all figured
# out for other people! 
#
# be aware that you're very cramped
# for space, and the carridge returns
# are practically hard-coded.

# screen on Mars, at coordinates 9, 37 on World Map.
%patch 0x00015c24 "Commander Keen returns to his B-w-B"
%patch 0x00015c47 0x0A 0x00
%patch 0x00015c49 "Megarocket with the stolen parts   "
%patch 0x00015c6a 0x0A 0x00
%patch 0x00015c6c "and heads home, finding it odd that"
%patch 0x00015c8F 0x0A 0x00
%patch 0x00015c91 "Mort's ship was so vulnerable.  "

# i think this triggers the Flight Sequence
# for when Keen flies from Mars to Earth,
# and spots the Vorticon Mothership.
%patch 0x00015cb0 0x0A 0x00 0x46 0x49 0x4E 0x41 0x4C 0x45 0x2E 0x43 0x4B 0x31


# First Dialogue box at home, and so on
%patch 0x00015cbd "Keen makes it home and"
%patch 0x00015cd3 0x0A 0x00
%patch 0x00015cd5 "ponders things in "
%patch 0x00015ce7 0x0A 0x00
%patch 0x00015ce9 "his workshop.    "
%patch 0x00015cf9 0x00 0x00

%patch 0x00015cfb "'The Photachyon      "
%patch 0x00015d0e 0x0A 0x00
%patch 0x00015d10 "Tranciever might be"
%patch 0x00015d23 0x0A 0x00
%patch 0x00015d25 "of use.'  "
%patch 0x00015d2F 0x00

%patch 0x00015d30 "Keen turned it on... "
%patch 0x00015d45 0x0A 0x00
# WHAT IS THAT ONE-EYED GREEN THING IN YOUR ROOM?
# this part below is the sudden jump in text. :-)
%patch 0x00015d47 "'Hahaha, Keen!  Your "
%patch 0x00015d5c 0x0A 0x00
%patch 0x00015d5e "little caper was in"
%patch 0x00015d71 0x0a 0x00
%patch 0x00015d73 "vain!!!        "
%patch 0x00015d7E 0x0a 0x00

%patch 0x00015d80 "The Universe is "
%patch 0x00015d90 0x0A 0x00
%patch 0x00015d92 "Toast!!   "
%patch 0x00015d9B 0x00

%patch 0x00015d9c "As you hear this, my  "
%patch 0x00015db2 0x0A 0x00
%patch 0x00015db4 "second fleet moves  "
%patch 0x00015dc8 0x0A 0x00
%patch 0x00015dcA "out--can you find it?'"
%patch 0x00015de0 0x00 0x00

# OK, Mom. Goodnight.
%patch 0x00015de2 "'Yikes!' said Keen."
%patch 0x00015df5 0x00

# Goodnight, Dear.
%patch 0x00015df6 "'A whole FLEET?'"
%patch 0x00015e06 0x00

%patch 0x00015e07 "Keen quickly started "
%patch 0x00015e1C 0x0A 0x00
%patch 0x00015e1E "scanning the spacewaves"
%patch 0x00015e35 0x0A 0x00
%patch 0x00015e37 "for information on "
%patch 0x00015e4a 0x0A 0x00
%patch 0x00015e4c "the 2nd fleet, eager "
%patch 0x00015e61 0x0A 0x00
%patch 0x00015e63 "to stop it!   "
%patch 0x00015e71 0x00


%end
