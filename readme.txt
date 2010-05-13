**********************************************
*                                            *
*             Commander Genius               * 
*              (CloneKeenPlus)               *
*                                            *
*    (c)2008-2010            Gerstrong       *
*    (c)2009-2010            Albert          *
*    (c)2009-2010            Pizza2004       *
*    (c)2009-2010            Pickle          *
*    (c)2009-2010            Tulip           *
*    (c)2009-2010            DaVince         *
*    (c)2003-2010            Caitlin Shaw    *
*           Released under GNU/GPL           *
*                                            *
*            Release v0.3.1.3                *
*                                            *
**********************************************

Thanks for downloading Commander Genius!

Commander Genius is an open-source clone of
Commander Keen, which allows you to play the
original three games, and a majority of the mods
made for it.  All of the original data files
are required to do so, however, we convienently
include Episode 1 for you enjoyment.

Commander Genius runs on Linux/X11, Windows,
MacOS X, Wiz, GP2X and the iPhone, with plans
to release on other platforms in the future.
If you think you would like to port it,
please send us a message and we will do our
best to help you understand the code.

The main goal of Commander Genius is to copy
the original game play feeling as much as possible, 
although there are also a number of improvements like:
- MMX Support
- OpenGL Acceleration
- Better graphical Effects
- Multiplayer Support
- High Quality Packs which make the game look better
- In-Game Menu
- Named Save Slots
- Unrestricted Joystick Support
- and more!

Before it was called CloneKeenPlus, but some 
major rewrites in the code had to be done.
We now call it Commander Genius!  
After all this coding, we have retained 
only a very small portion of the
original CloneKeen code, but it was its base 
that allowed it to happen. Due that fact
we still honor Caitlin Shaw and all that
effort she put onto that engine in the past.

The Commander Genius source code may be freely
distributed and modified as per the GPL, just
please remember to give credit to the original
authors. We worked very hard on it!

Setting up the data of the game:

 * Copy the directory of the games to the "games" subdirectory of CG.
   If you own the registered versions, you can play all episodes, 
   otherwise download the shareware version and copy them there. 
   You also can use mods. If you have a bundled version, 
   you already should be able to play episode 1, as it is in that directory.
 * If you want to use some patched mods just copy them as separate games. 
 * Go into the root directory and start the interpreter. If you have downloaded 
   the binary version of Commander Genius (for Windows, Linux32/64), then you 
   will find the executable in the root directory of the extracted game.
   Under Linux it is called "CGenius", under Windows "CGenius.exe".    
 * If you want better sounds and music, please download the high quality pack (HQP).
   Read that readme of HQP for more information in how to use those resources.
   Also try out our new tilesets. They are awesome!
   Using HQP you might have to copy some resources to the specific game directory so
   CG would detect it. For example you might want to copy ck1tiles.bmp to ./games/keen1/
 * If your monitor and graphic card support a special resolution, you can add it by
   modifying "resolutions.cfg" with your favorite text-editor. It can be found in the
   "./data" directory and you simply have to add in resolutions as "width"x"height"x"depth"
   (depth should always be 32 in the case of a normal computer. Embedded System can use 16 
   for better performance).
 
Howto use Commander Genius (If you didn't compile it):
 * Go to the game directory.
 * Under Windows you start "CGenius.exe", under Linux "./CGenius"

Howto compile it under Ubuntu (testet on Lucid):
 * This is based on the source code that we provide on our website
 * As we know which packages are needed, because we use Ubuntu, here is an extra guide. 
 * Install through Synaptic or "sudo apt-get install" following packages:
 	- build-essential
 	- libvorbis-dev
 	- libsdl1.2-dev
 	- libgl1-mesa-dev
 * extract my source code into "CGenius".
 * cd into the "CGenius/Linux32" or "CGenius/Linux64" subdirectory depending on your system.
 * Run "make".
 * Then, you can run "CommanderGenius". Copy the files, wherever you want them together 
   with the data directory. "make install" does not work though.
 * We also provide CMake for wiz. If you want to compile it for embedded systems.
   We had support for Linux and Win32, but since there is a bug when build, for
   different Linux build (32-bit or 64-bit) we only use it for embedded systems.

Compiling CG on any Linux/UNIX (other than Ubuntu):
 * If you want to compile the game, you need to have all the standard
   development libraries and GNU C Compiler (at least version 4) installed 
   on your system.
 * You need to install the "vorbis", "SDL" and "Mesa/OpenGL" development libraries to get 
   it successfully compiled.
 * extract my source code to "cgenius". 
 * cd into the "cgenius/Release" subdirectory.
 * Run "make".
 * Then, you can run "CommanderGenius". Copy the files, 
   wherever you want them together with the data directory. 
   "make install" does not work though.
   
- VIDEO SYSTEM -
Being cross-platform, there are many methods to detect different resolutions on a system.

Generally it is not necessary, because many resolutions are already supported, but if 
you want to support other resolutions that you don't find in the options list just modify the
"resolutions.cfg" file by hand. CG always reads this file and checks whether your card and 
monitor support it.

CAUTION: This is recommended for advanced users who know what they do. The team won't take 
any responsibility if you use a wrong configuration. If you want to see other resolutions to 
in future versions of CG, mail us ;-).

- GAME PLAY -
Before you play the game, you should take a look at the controls. Normally LAlt, Ctrl, Space and Enter
are the action keys. You can also assign joystick buttons and axes to those actions.

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
The game can be saved by pressing F3 at any point during the game or using the menu.
The game will be saved in the exact position it was left (unlike the original which only
allowed save at the map).

- DEMO -
This feature is disabled for now, but we hope to reenable it soon!

- FAQ -
Q: When I change to opengl-mode, the screen gets white and I can't do anything! I have to kill the program.
   What can I do to restore the normal mode?

A: You can delete "cgenius.cfg" or change it the way you need it! Just put "opengl = 0" and it should normal
   with your previous settings. We are working on a solution for that! The white screen means, that your 
   graphic card doesn't support NPOT Textures, which is required. 
   A video card with OpenGL 2.0 or later support should not have that problem.
   
Q: Why not to use POT-Textures?
A: Because it requires more memory to write, and the game would get slower than with software rendering.
   We are working on a new method to get it working with POT Textures and faster, but for now we cannot
   support it. 

Q: Commander Genius is slow? Please help!
A: You can decrease the fps under settings->video what is very
   recommended for any system. OpenGL may also help, but you must have a gfx-card that support NPOT-Textures.
   (OpenGL 2.0 or later). Try to lower the resolution, a lower filter, or decrease some stuff in the sound section.
   Stereo sound requires additional calculations than mono sound. There are many things you can do, to speed up
   the program.
   
Q: When can I play Keen 4, 5 and 6 on CG?
A: That is a good question! We currently are working on the support and piece by piece more functionalities of those
   great games will be there. Just get every update we release and more stuff for those games will be starting
   to work. ;-) 
  
  
- BUG! -
So you really found one or more bugs? Please report me those per e-mail (gerstrong@gmail.com) or at
the SourceForge.Net Trac system we offer and they will be checked by all of us. 
Depending on the priority, we are going to fix them as soon as possible. 
Sending us the "CGLog.html" or "stdout.txt" file might help tracking the error.
Also remember to test the version from the testing repo we sometimes provide. 
Your error might have been already fixed there. If there is no testing version after the release, 
just report it. ;-)

http://sourceforge.net/apps/trac/clonekeenplus/

- ACKNOWLEDGEMENTS -
Many thanks to people who has helped me so far in this project with their
suggestions:

Sciz CT
DSL (Nice icon contribution)
Motorfingers
Chrisfischtopher
Commander Spleen
Dreams
Napalm (More Keen Data Structure Information)
Malvineous
Hans de Goede
Levellass (Keen EXE-Information)
ZidaneA (For his music tracks!)
Iv4n

Also many special thanks to Martin Hauber (Tulip) who has been supporting me all the time in testing
the Windows version of CKP and giving me access to new information. Without you it wouldn't be so great now!

And also special thanks to Chad Ian Anderson (Pizza2004) in helping us in many points of the game. Even in 
difficult times he always supported us.

Furthermore special thanks to Scott (Pickle) and Albert Zeyer
for his changes. They helped out a lot on the quality of the code and performance incrate.

I'm sure, there were more people who helped us and we forgot to mention, but
many thanks to them also.

Also lots of thanks to all of you, the project has become so great, and will be a nice
port for Commander Keen games!

And yes, it's no rumor, we are working on Keen 4 support! Stay tuned!
                            
****************************************************************************
For questions, comments, or updates to the source tree
please contact: gerstrong@gmail.com or go thru
SourceForge at http://clonekeenplus.sourceforge.net/
****************************************************************************
