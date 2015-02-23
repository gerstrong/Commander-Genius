%ext ck4
%version 1.4 
%maphead maphead.ck4%patchfile

       # Tile Properties

%patchfile $249C2 keen4.tli

%patchfile $1fe47 ck4load.bin


       #level names:

#Level 0 name
%level.name 0 Aspermontis
#Level 1 name
%level.name 1 Dusty Fort
#Level 2 name
%level.name 2 Pyramid of Time
#Level 3 name
%level.name 3 The Oracle
#Level 4 name
%level.name 4 The Perspicuus City
#Level 5 name
%level.name 5 The Damp Woods
#Level 6 name
%level.name 6 Blue Bird Caves
#Level 7 name
%level.name 7 Vitrium
#Level 8 name
%level.name 8 Defluo
#Level 9 name
%level.name 9 Pyramid of Heat
#Level 10 name
%level.name 10 Lichen Caves
#Level 11 name
%level.name 11 Gnosticene Village
#Level 12 name
%level.name 12 The Pyramid the Light
#Level 13 name
%level.name 13 Footsville
#Level 14 name
%level.name 14 The Desert Creek
#Level 15 name
%level.name 15 Pyramid of Pain
#Level 16 name
%level.name 16 The Crack
#Level 17 name
%level.name 17 Well of Wonders
#Level 18 name
%level.name 18 The Bean-With-Bacon Megarocket


       #Level Entries:

#Level 0 entry text
%level.entry 0 Keen treks around 
Aspermontis

#Level 1 entry text
%level.entry 1 Keen crunches through
Dusty Fort

#Level 2 entry text
%level.entry 2 Keen ticks over
Pyramid of Time

#Level 3 entry text
%level.entry 3 Keen pays a visits 
to The Oracle

#Level 4 entry text
%level.entry 4 Keen fades into
The Perspicuus City

#Level 5 entry text
%level.entry 5 Keen gets lost in 
The Damp Woods

#Level 6 entry text
%level.entry 6 Keen swoops into
The Blue Bird Caves

#Level 7 entry text
%level.entry 7 Keen contemplates on 
Vitrium

#Level 8 entry text
%level.entry 8 Keen blinks
 in and out of existance
 in Defluo

#Level 9 entry text
%level.entry 9 Keen boils in 
Pyramid of Heat

#Level 10 entry text
%level.entry 10 Keen seeps into
The Lichen Caves

#Level 11 entry text
%level.entry 11 Keen creeps into 
Gnosticene Village

#Level 12 entry text
%level.entry 12 Keen blindly enters 
The Pyramid of Light

#Level 13 entry text
%level.entry 13 Keen approaches 
Footsville

#Level 14 entry text
%level.entry 14 Keen wades through the 
Desert Creek

#Level 15 entry text
%level.entry 15 Keen is fearful of 
The Pyramid of Pain

#Level 16 entry text
%level.entry 16 Keen squeezes into 
The Crack

#Level 17 entry text
%level.entry 17 Keen ponders upon
The Well of Wonders

       # Load the modified graphics

       #This is the music played in each level: 

#             VALUE   LEVEL                 MUSIC PLAYED         LEVEL No 
%patch $2F206 $00 $00 #Map                  (Shadowlands)          0 
              $03 $00 #Dusty Fort           (slug village)         1 
              $02 $00 #Depth Pyramid        (Too Hot)              2 
              $03 $00 #Oracle               (Slug Village)         3 
              $01 $00 #Illuseon             (Eat your veggies)     4 
              $03 $00 #Damp and Dark Village(Slug Village)         5 
              $04 $00 #Blue Burd Cavern     (Kick in the Pants)    6 
              $04 $00 #Vitrium              (Kick in the pants)    7 
              $01 $00 #Sand Yego            (Eat your Veggies)     8 
              $02 $00 #Desert Pyramid       (Too Hot)              9 
              $03 $00 #Lawned Cave          (Slug village)        10 
              $03 $00 #Gnosticene Village   (Slug Village)        11 
              $03 $00 #Light Pyramid        (Slug Village)        12 
              $04 $00 #Footsville           (Kick in the pants)   13 
              $02 $00 #Pyramid of Forbidden (Too Hot)             14 
              $02 $00 #Isle of Tar          (Too Hot)             15 
              $02 $00 #Isle of Fire         (Too Hot)             16 
              $01 $00 #Well of Wishes       (Eat your Veggies)    17 
              $04 $00 #Bwb Megarocket       (Kick in the Pants)   18 
              $01 $00 #High scores          (Eat your veggies)    19 

           #Council Members text

%patch $30C17 "Thanks once again,  "
%patch $30C2C "Commander Keen. "
%patch $30C3D "That explosion    "
%patch $30C50 "really shook  "
%patch $30C5F "me around.  "

%patch $30BB3 "Thigs oncg aggiag,   "
%patch $30BC9 "Commanger Geen.     "
%patch $30BDE "Thag gexplogion    "
%patch $30BF2 "greallgy gook  "
%patch $30C02 "meeg garougnd.      "

#Council Members text - Keen's replies
%patch $30A71 "What ever            "
%patch $30A87 "you say.  "

%patch $30A92 "Back to the Oracle,"
%patch $30AA6 "pronto.     "

%patch $30AB3 "I dont care."

%patch $30AC0 "I don't really care, "
%patch $30AD6 "just get back to   "
%patch $30AEA "the Oracle chamber..."

%patch $30B13 "Stop yapping and "
%patch $30B25 "get back to the   "
%patch $30B38 "Oracle...  "

%patch $30B44 "I am not having a   "
%patch $30B59 "good day, mate.  "

%patch $30B00 "Yep. Thats good.  "

%patch $30B6B "Finally! I've just  "
%patch $30B80 "about had enough  "
%patch $30B93 "of you guys. To"
%patch $30BA3 "The Oracle!    "


         #Council Janitor text

%patch $1F580 "Oh... Hi...            "
%patch $1F598 "Umm...               "
%patch $1F5AE "Yeah...                 "
%patch $1F5C7 "I kind of... "

%patch $1F5E0 "I decided to help  "
%patch $1F5F4 "you out.             "
%patch $1F60A "But I got stuck here..."

%patch $1F630 "Stuck?         "
%patch $1F640 "How?      "

%patch $1F650 "Giant flying      "
%patch $1F663 "stone feet..."


         #Lindsey:

#Hint level 5
%patch $30A25 "Princess Lindsey says:"
%patch $30999 "Footsville holds the  "
%patch $309B0 "path to The Isle of  "
%patch $309C6 "Periculum within a  "
%patch $309DB "hut.  "
%patch $309FA "Thanks. I'll  "
%patch $30A09 "keep that in    "
%patch $30A1A "mind!    "


#Hint level 7
%patch $30A25 "Princess Lindsey says:"
%patch $3094C "It seems you dropped"
%patch $30961 "your wetsuit over at   "
%patch $30979 "the Pyramid of Light."
%patch $3098F "        "
%patch $309E3 "Umm. Are you sure?    "

          #Wetsuit Text

%patch $30A3D "Wetsuit?     "
%patch $30A4B "Wow! I didn't think "
%patch $30A60 "I'd find this!  "

%patchfile $1fe47 ck4load.bin

          #Star wars text:

%patchfile $1EE40 "STORY.CK4"

#Names
%patch $33234 "Id Software      " $00
%patch $33274 "BubbaTom         " $00
%patch $332B4 "Tom Hall         " $00
%patch $332F4 "Mortimer         " $00
%patch $33334 "Janitor          " $00
%patch $33374 "Lindsey          " $00
%patch $333B4 "Spot             " $00
%patch $333F4 "Lt. Barker       " $00

#Level end text
%patch $2F16F "You didn't make it past"
%patch $2F187 "Try Again"
%patch $2F191 "Exit to Aspermontis"
         

%egahead EGAHEAD.CK4
%end
