*************************************
*                                   *
*        C L O N E K E E N          *
*             PLUS					* 
*       (Commander Genius)		    *
*                                   *
*    (c)2008-2009  Gerstrong		*
*	 (c)2003-2005  Caitlin Shaw     *
*      Released under GNU/GPL       *
*                                   *
*          Beta v0.2.6              *
*                                   *
*************************************
Welcome to CloneKeenPlus (Commander Genius)!

CloneKeenPlus is an almost complete open-source clone of
the old classic DOS game, Commander Keen: Invasion of the
Vorticons by ID Software. CloneKeenPlus requires the original game
data files to work. You can play all three episodes of the game and
some fanmade mods.

CloneKeenPlus currently runs under Linux/X11 and Windows with SDL. 
In Mac OS X it also might run, but this has not been tested.

CloneKeenPlus tries to mostly replicate the original game,
however there are also a number of improvements,
including smoother scrolling, better graphic effects
in some parts of the game, 2-player support, high quality 
packs, games menu, gamepad support (More than two joystick
buttons) and more.

To my knowledge this is the only released attempt at
writing a third-party Keen game that has reached this
stage of completedness.

Commander Genius is a new edition of CloneKeenPlus.
It will be the new game of the interpreter, but it is still
not sure. This edition is compiled with GNU C++.

The CloneKeen source may be freely distributed and
modified as per the GPL, just please remember to give
credit to the original authors.

CloneKeenPlus has been developed by Gerstrong implementing 
mayor improvements basing on the original 
source code of CloneKeen by Caitlin Shaw. Many 
thanks to him, for bringing us this great program.

Obviously it is not the original version of CloneKeen 
or any update. It is another version from a different
author. Other guys and me had many improvements and 
I tried to post them to the author. However, until 
today he has not responded. It is also known, that there
have not been updates of CloneKeen since 2005. It stayed
in beta 0.83.
 
I also would like to see those changes and improvements 
(or some of them) in his original project.
So I hope he will contact me some day. Most of the code is
based on his work and I acknow him a lot for that!


Setting up the data of the game:

 * Copy all of the data files from the original game(s) into the
   "./data" folder. This means all of the files with the extension
   .ck?. If you have the registered versions, you can play the whole game,
   otherwise download the shareware version and copy the data from it.
   You also can use mods.
 * If you want to use some patched mods, you can put them into subdirectories.
   You only have modify the "games.cfg".
 * Only Linux/Unix Users: If you have many data files which are not lower-case, 
   use "./data/renlower.sh" shell script to rename them. 
 * Go into the "Release" folder and drop to the command line.
 * If you want better sounds and music, please download the high quality pack.
   Extract its contents to the "data" directory together with your game data.
 
Howto use CloneKeenPlus (If you didn't compile it):
 * Under Windows you start "clonekeen.exe", under Linux "./clonekeen"

Howto compile it under Ubuntu (testet on Intrepid):
 * As I know which packages are needed, because I use Ubuntu, here is an extra guide. 
 * Install through Synaptic or "sudo apt-get install" following packages:
 	- build-essential
 	- libvorbis-dev
 	- libsdl1.2-dev
 * extract my source code into "ckp".
 * cd into the "cpk/Release" subdirectory.
 * Run "make".
 * Then, you can run "clonekeen". Copy the files, wherever you want them together 
   with the data directory. "make install" does not work though.

Compiling CKP on Linux/UNIX (other than Ubuntu):
 * If you want to compile the game, you need to have all the standard .
   development libraries and GNU C Compiler (at least version 4) installed 
   on your system.
 * You need to install the "vorbis" and "SDL" development libraries to get 
   it successfully compiled.
 * extract my source code to "ckp". 
 * cd into the "ckp/Release" subdirectory.
 * Run "make".
 * Then, you can run "clonekeen". Copy the files, 
   wherever you want them together with the data directory. 
   "make install" does not work though.

HINTS & SECRETS


- CHEAT CODES -

There are a full assortment of cheat codes available. Some cheats must first
be enabled with -cheat or through the options->game menu. C+T+SPACE and G+O+D will
always work.

KEY        	ACTION             DESCRIPTION
G+O+D	   	godmode			   In god mode you are invincible!
C+T+SPACE  	All Items          Gives all items, similar to the original.
LSHIFT+TAB 	Skip Level         Holding down this key you can skip levels and walk through tiles
							   on the map.
TAB			No clipping		   If you hold down this key you will disable clipping.
							   However, you cannot fall though floors. 

- SAVE/LOAD GAME -

The game can be saved by pressing F3 at any point during the game. The game will
be saved in the exact position it was left (unlike the original which only
allowed save at the map).

- DEMO -

Wait a while at the tile screen and the game will cycle through several
demos, an added feature not found on the original game.

- BUG! -

So you really found one or more bugs? Please report me those per e-mail 
and they will be checked. If necesarry we are going to fix them as soon
as possible.

- ACKNOWLEDGEMENTS -
Many thanks to people who has helped me so far in this project with their
suggestions:


Sciz CT
Commander Spleen
Malvineous
Tulip
Hans de Goede
Levelass (Keen EXE-Information)
ZidaneA (For his music tracks!)
Iv4n

Special thanks to Tulip, who has been supporting me all the time in testing
the Windows version of CKP. Without you it wouldn't be so great now! 

I'm sure, there were more people who helped me and I forgot to mention, but
many thanks to them, too.

Many thanks to all of you, the project has gone farther, and will be a nice
port to Commander Keen!
                            
****************************************************************************
For questions, comments, or updates to the source tree
please contact: gerstrong@gmail.com or go thru
SourceForge at http://clonekeenplus.sourceforge.net/
****************************************************************************
