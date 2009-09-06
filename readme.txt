**********************************************
*                                            *
*             Commander Genius               * 
*              (CloneKeenPlus)               *
*                                            *
*    (c)2008-2009            Gerstrong       *
*    (c)2009                 Albert          *
*    (c)2009                 Pizza2004       *
*    (c)2009                 Pickle          *
*    (c)2009                 Tulip           *
*    (c)2009                 DaVince         *
*    (c)2003-2005 and 2009   Caitlin Shaw    *
*           Released under GNU/GPL           *
*                                            *
*            Release v0.3.0.4                *
*                                            *
**********************************************

Welcome to Commander Genius (aka CloneKeenPlus)

Commander Genius is an almost complete open-source clone of
the old classic DOS game, Commander Keen: Invasion of the
Vorticons by ID Software. Commander Genius requires the original game
data files to work. You can play all three episodes of the game and
some fanmade mods.

Commander Genius currently runs under Linux/X11, Windows 
and MacOS X with SDL. Also portable systems are supported (Wiz)  

Commander Genius tries to mostly replicate the original game,
however there are also a number of improvements like:
- smoother scrolling
- MMX Support
- OpenGL Acceleration (through Hardware on some systems)
- better graphic effects (transparency, fading)
- 2-player support (still very incomplete)
- high quality packs (for better and more sounds and ingame music)
- games menu that allows choosing which one to play
- gamepad support (for using your input devices as you want)
- built in patch system for mods (WIP)
- better logics like keystacking

and much more!

To our knowledge this is the only released attempt at
writing a third-party Keen game that has reached this
stage of completeness.

Commander Genius is a new edition of CloneKeenPlus.
The reason we call it is because this version is compiled
with GNU C++ (previously GNU C). Some functions are 
now classes. The idea behind that is to provide a faster
and more stable program. Features like the implementation
of resolution changes was achieved through classes. 
Although the program itself looks very similar 
the skeleton of the program is very different 
to the one in CloneKeen. 

The Commander Genius source code may be freely distributed and
modified as per the GPL, just please remember to give
credit to the original authors.

Setting up the data of the game:

 * Copy all of the data files from the original game(s) into the
   "./data/data" folder. This means all of the files with the extension
   .ck?. If you have the registered versions, you can play the whole game,
   otherwise download the shareware version and copy the data from it.
   You also can use mods.  In Macintosh you can find this folder by right
   clicking on the game and clicking open package then going into resources.
 * If you want to use some patched mods, you can put them into subdirectories.
   You only have to modify the "games.cfg" which can be found in the
   "./data/data" folder along with the Keen 1 files. 
 * Go into the Release folder and start the interpreter. If you have downloaded 
   the binary version of Commander Genius (for Windows, Linux32/64), then you 
   will find the executable in the root directory of the extracted game.
   Under Linux it is called "CommanderGenius", under Windows "CGenius.exe".    
 * If you want better sounds and music, please download the high quality pack (HQP).
   Extract its contents to the "data" directory together with your game data.
 * If your monitor and graphic card support a special resolution, you can add it by
   modifying "resolutions.cfg" with your favorite text-editor, it can be found in the
   "./data" folder and you simply have to add in resolutions as "width"x"height"x"depth"
   (depth should always be 32 in the case of a computer).
 
Howto use Commander Genius (If you didn't compile it):
 * Go to the game directory.
 * Under Windows you start "cgenius.exe", under Linux "./CommanderGenius"

Howto compile it under Ubuntu (testet on Jaunty):
 * This is based on the source code that we provide on our website
 * As I know which packages are needed, because I use Ubuntu, here is an extra guide. 
 * Install through Synaptic or "sudo apt-get install" following packages:
 	- build-essential
 	- libvorbis-dev
 	- libsdl1.2-dev
 	- libgl1-mesa-dev
 * extract my source code into "CGenius".
 * cd into the "CGenius/Release" subdirectory.
 * Run "make".
 * Then, you can run "CommanderGenius". Copy the files, wherever you want them together 
   with the data directory. "make install" does not work though.

Compiling CG on Linux/UNIX (other than Ubuntu):
 * If you want to compile the game, you need to have all the standard .
   development libraries and GNU C Compiler (at least version 4) installed 
   on your system.
 * You need to install the "vorbis", "SDL" and "Mesa/OpenGL" development libraries to get 
   it successfully compiled.
 * extract my source code to "ckp". 
 * cd into the "ckp/Release" subdirectory.
 * Run "make".
 * Then, you can run "CommanderGenius". Copy the files, 
   wherever you want them together with the data directory. 
   "make install" does not work though.
   
- VIDEO SYSTEM -
Being cross-platform, there are many methods to detect different resolutions on a system.

Gernerally it is not necessary, because many resolutions are already supported, but if 
you want to support other resolutions that you don't find in the options list just modify the
"resolutions.cfg" file by hand. CG always reads this file and checks whether your card and 
monitor support it.

Caution: This is recommended for advanced users who know what they do. The team won't take 
responsabilities if you use a wrong configuration. If you want to see other resolutions to 
in future versions of CG, mail us ;-).

- GAME PLAY -
Before you play the game, you should take a look at the controls. Normally LAlt, Ctrl, Space and Enter
are the action keys. You can also assign joystick buttons and axes to those actions.

There are some keys, which cannot be changed and are part of the game

KEY        	ACTION             DESCRIPTION
LALT+Q	   				       Both Keys make you quit the game. Depending where you are in the game
LALT+F4		quit			   it will ask you, if you really want that
LALT+Enter	fullscreen/        This will change between fullscreen and window-mode
            windows			   
   
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
Demo doesn't work until you record some. If you want to do that start the interpreter with "-rec" as argument.
For example: "CGenius.exe -rec -game1 -level01" and you should be off to go recording in game 1 level 1.

When you exit, the demo should be recorded. Restart the interpreter/game and wait a while at 
the tile screen and the game will cycle through several demos, an added feature not found on the original game.

You can also see them using the demo option in the main menu. Have fun!

- FAQ -
Q: When I change to opengl-mode, the screen gets white and I can't do anything! I have to kill the program.
   What can I do to restore the normal mode?

A: You can delete "cgenius.cfg" or change it the way you need it! Just put "opengl = 0" and it should normal
   with your previous settings. I'm going to fix that soon! The white screen means, that your graphic card
   doesn't support NPOT Textures, which is required. A video card with OpenGL 2.0 or later support should not
   have that problem anymore.
   
Q: Why not to use POT-Textures?
A: Because it requires more memory to write, and the game would get slower than with software rendering.
   I'm working on a new method to get it working with POT Textures and faster, but for now I cannot
   support it. 

Q: Commander Genius is slow? Please help!
A: You can increase the frameskip under settings->video or you can enable automatic frameskip, what is very
   recommended for any system. OpenGL may also help, but you must have a gfx-card that support NPOT-Textures.
   (OpenGL 2.0 or later). Try to lower the resolution, a lower filter, or decrease some stuff in the sound section.
   Stereo sound requires additional calculations than mono sound. There are many things you can do, to speed up
   the program.

- BUG! -
So you really found one or more bugs? Please report me those per e-mail (gerstrong@gmail.com)
and they will be checked. You can also submit them through the bug tracker at Sourceforge.net.
Depending on the priority, I'm going to fix them as soon as possible. 
Sending me the "CGLog.html" file might help tracking the error.

- ACKNOWLEDGEMENTS -
Many thanks to people who has helped me so far in this project with their
suggestions:


Sciz CT
DSL (Nice icon contribution)
Commander Spleen
Napalm (More Keen Data Structure Information)
Malvineous
Hans de Goede
Levellass (Keen EXE-Information)
ZidaneA (For his music tracks!)
Iv4n

Also many special thanks to Tulip who has been supporting me all the time in testing
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
