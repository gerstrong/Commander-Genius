%ext ck4
%version 1.4
# Load the modified graphics
%egahead EGAHEAD.CK4
%patch $10DEC $08 $00

#Load the custom maps
%gamemaps editmaps.ck4
%maphead maphead.ck4

#underwater level exits normally
#%patch $905B $90 $90

#Enemy Patches

#Bounder/Hipalong
#Jump height
%patch $111BC $DA $FF
# How high the Bounder leaps when shot while moving upward (default #is 32, which is $20h) 
%patch $11179 $40 

#mad mushroom/Hopper
# How many bounces until the "high" bounce
%patch $FF90 $02 $00 


#Oracle Members/key carriers
# Require only 7 members to be rescued. 
%patch $6AE6 $07 $7D 

#what oracle members say
%patch $30C17 "Alright! This is a key" $0A 
   "to Mort's base!  " $0A 
   "                 " $0A 
   "              " $0A 
   "           "

%patch $30BB3 "Alright! This is a key" $0A 
              "to Mort's base!  " $0A 
              "                 " $0A 
              "              " $0A 
              "                         "

#Keen's reply
%patch $30A71 "Now to look for more " $0A "keys!     " 

%patch $30A92 "I still need more  " $0A 
              "keys!       " $00 

%patch $30AB3 "Woo-hoo!    " $00

%patch $30AC0 "I wonder where       " $0A 
              "the rest of the    " $0A 
              "keys are?            " $00 

%patch $30B00 "I need more keys! " $00 

%patch $30B13 "I wonder where   " $0A 
              "the rest of the   " $0A 
              "keys are. " $00 

%patch $30B44 "Great, the last key!" $0A 

              "Mort is Toast!   " $00 

%patch $30B6B "This text shouldnt" $0A 
              "be here. Did you  " $0A 
              "modify the     " $0A 
              "patch file?    " $00





#Licks/Slurple
# Height for short hops 
%patch $113DA $C0 $FF  
# default is -32 
# Making this number smaller will make the Lick's leap less high, and shorter as a result.
# Height for long hops 
%patch $113F5 $E0 $FF  
# default is -16 
# Making this number smaller will make the Lick's leap taller, and longer as a result.

#eggs
# What comes out of the Eggs? 
%patch $10135 $F0 $23
%patch $10106 $00 $01  #80 #00


# You can make a line break with $0A 
# The story ends at $1F03B, so you can only have a story that's 507 bytes long. 
%patch $1EE40 "Episode Seven" 
$0A "        "
$0A "The Keys of Krodacia" 
$0A "        "
$0A "After learning that" 
$0A "his arch enemy"
$0A "Mortimer McMire is "
$0A "  still alive Commander  "
%patch 0x0001EEC6 "Keen looks for"
$0A "clues and makes the"
$0A "discovery that"
$0A "Mortimer plans to"
%patch 0x0001EF0A "destroy the Universe!"
$0A "                     "
$0A "Once at Mortimer's"
$0A "base, located"
$0A "on planet Krodacia,"
$0A "he"
%patch 0x0001EF6D " finds that the"
$0A "entrance to the base"
$0A "is locked."
$0A "The only way to"
$0A " open it is with "
%patch 0x0001EFBF "the seven keys of"
$0A "Krodacia. He must"
$0A "find the keys, the"
$0A "fate of the "
$0A "Universe is in"
$0A "       his hands...       "
$0A "               "

#Princess Lindsey



#Level where Lindsey gives the Miragia hint
%patch $EFB6 $06

#Level where Keen gives the Miragia reply instead of the Pyramid one: 
%patch $F033 $06 


%patch $30A25 "Lt. Barker says:     " $0A $00 
               
%patch $3094C "Thanks for rescuing " $0A
              "me! I would have jumped" $0A
              "out, but its too dark" $0A
              "in here!" $0A $00

%patch $30999 "You can get a swim    " $0A 
              "coin in Monk's house." $0A 
              "Good luck Commander " $0A 
              "Keen! " $0A $00 

%patch $309E3 "No problem. Lets go!  " $00 

%patch $309FA "Thanks for the" $0A 
              "good advice, Lt." $0A 
              "Barker!  " $0A $00

#How much Lindsey moves up/down 
%patch $12829 $00 $00 #Orig $08

#wetsuit
%patch $30A3D "I need a coin" $00 
 
%patch $30A4B "A swim coin! I bet I" $0A 
              "can use this!   " $00 

#level names
%patch $1F040 "Krodacia    " $00

%level.name 0 Krodacia
%level.entry 0 Keen explores\n planet Krodacia 

%level.name 1 Blue Forest
%level.entry 1 Keen wanders through \n the Blue Forest

%level.name 2 Forest Tunnels
%level.entry 2 Keen decends the\n Forest Tunnel

%level.name 3 Private Manor
%level.entry 3 Keen breaks \n into the Private Manor

%level.name 4 Mansion
%level.entry 4 Keen shoots his way \n through the Mansion          

%level.name 5 Complex
%level.entry 5 Keen sneaks into \n the complex         

%level.name 6 Mushroom Forest
%level.entry 6 Keen nervously enters\n the Mushroom Forest

%level.name 7 Cliff Hanger
%level.entry 7 Keen scales the\n Cliff Hanger

%level.name 8 The Underground
%level.entry 8 Keen wanders through\n the Underground

%level.name 9 Monk's House
%level.entry 9 Keen ransacks\n Monk's House

%level.name 10 Gem Temple
%level.entry 10 Keen purposefully\n enters the Gem Temple

%level.name 11 Forest Skies
%level.entry 11 Keen regrets going\n into the Forest Skies 

%level.name 12 Slime Temple
%level.entry 12 Keen dodges slime\n pools in the Slime Temple

%level.name 13 Fun House
%level.entry 13 Keen gets lost\n in the Fun House

%level.name 14 Stone Temple
%level.entry 14 Keen hesitently enters\n the Stone Temple

%level.name 15 Slime Lake
%level.entry 15 Keen takes a swim\n in Slime Lake

%level.name 16 Shoal
%level.entry 16 Keen dives into\n the Shoal 

%level.name 17  Reef
%level.entry 17 Keen treads water\n in the Reef


%patch $2F191 "Exit to Krodacia" $00


#menu patches
%patch $2F49C " Found "
%patch $2F4CB "Gems " 
%patch $2F4D1 " Coin  "


%patchfile $1fe47 ck4load.bin 

##musics##

#To patch AUDIODCT.CK4:
%patchfile $354F6 "AUDIODCT.CK4"

#To patch AUDIOHED.CK4:
%patchfile $20DF0 "AUDIOHED.CK4"

#00 SONG 157 On the Run
#04 SONG 161 Into the Inferno
#03 SONG 160 It's Quiet...Too Quiet
#02 SONG 159 Keen Rushin'
#01 SONG 158 Ceilick Vodoo
#FF SONG 162 The Key to Success



%patch $2F206 $00 $00 #Map       
              $03 $00 #Blue Forest      
              $01 $00 #Forest Tunnels     
              $04 $00 #Private Manor       
              $02 $00 #Mansion 
              $04 $00 #Complex   
              $03 $00 #Mushroom Forest      
              $01 $00 #Cliff Hangar    
              $02 $00 #Underground       
              $02 $00 #Monk's House       
              $01 $00 #Gem Temple   
              $03 $00 #Forest Skies    
              $04 $00 #Slime Temple  
              $02 $00 #Fun House 
              $04 $00 #Stone Temple 
              $03 $00 #Slime Lake   
              $02 $00 #Shoal
              $01 $00 #Reef
              $03 $00 #Bwb Megarocket
              $FF $FF #High scores



#Write custom CK4 TileInfo: 
%patchfile $249C2 keen4.tli


%patch $33234 "Commander Keen   " $00
%patch $33274 "Tom Hall         " $00
%patch $332B4 "Ceilick          " $00
%patch $332F4 "Keenrush         " $00
%patch $33334 "Levellass        " $00
%patch $33374 "Xky Rauh         " $00
%patch $333B4 "Adurdin          " $00
%patch $333F4 "Commander Spleen " $00




%end
