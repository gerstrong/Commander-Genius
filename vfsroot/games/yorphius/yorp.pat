%ext CK1

%dump DUMP.BEH

#Setting patches 
%patch $13D5A $00 
%patch $14220 $00 
%patch $146E6 $00 
%patch $14BAC $00 

#Menu Patch
#Menu text
%patch $15EE9 "New Game"
%patch $15EF6 "Continue Game"
%patch $15F08 "Story"
%patch $15F12 "Hey ID!" $0A $00
%patch $15F22 "High Scores"
%patch $15F32 "Ordering Info"
%patch $15F44 "Previews!"
%patch $15F52 "Restart Demo"

#About ID text
%patch $15F80 "     Thanx for the games, ID!" $0A $00
%patch $15FA8  $0A $00
%patch $15FD0  $0A $00
%patch $15FF8  $0A $0A $00
%patch $16020  $0A $00
%patch $16048  $0A $00
%patch $16070  $0A $00
%patch $16098  $0A $0A $00
%patch $160C0  $0A $00
%patch $160E8  $0A $00
%patch $16110  $0A $00

#Ship
%patch $1659B "You need these    " $0A "  activators:" $0A $0A $0A $0A $00
%patch $165C2 "    Go get them!" $0A $00
%patch $AE31 $07
%patch $AE35 $12

%patch $14EF6 "Super Powers!" $00
%patch $14F07 "No Super Powers.." $00

%patch $14DEE  $0A $00
%patch $14E05 "Hey cheater," $0A $00
%patch $14E21 "Have some stuff, for free!" $0A $00
%patch $14E39 $00

#status screen
%patch $14E83 "ACTIVATORS"
%patch $14E9B "RAWGUN"
%patch $14EDB "AMMO"

%level.hint 10
Do you have
all the activators?

%level.hint 15
You hear an evil
laugh:
You mental wimp! I
I was behind this
all!

# Change tank bot's shooting height in Keen 1. 
%patch $4900 $00

#This makes those darn textboxes that much easier to patch
%patch $8238 $D3 $2B
%patch $8241 $D3 $2B
%patch $824A $D3 $2B
%patch $8568 $D3 $2B
%patch $8571 $D3 $2B
%patch $85F4 $D3 $2B
%patch $85FD $D3 $2B
%patch $865D $D3 $2B
%patch $8666 $D3 $2B
%patch $869A $D3 $2B
%patch $86CE $D3 $2B
%patch $86D7 $D3 $2B
%patch $87A4 $D3 $2B
%patch $87AD $D3 $2B
%patch $87B6 $D3 $2B
%patch $87BF $D3 $2B

#Map
%patch $15C24 "Commander Keen uses the activators" $0A
              "and releases Spot from it's cage." $0A
              "Soon Keen starts the rocket" $0A
              "and they leave planet Yorpius II.." $00

# First dialogue box at home
%patch $15CBD "What was that, Keen?" $0A
              "Spot asked. I" $0A
              "don't know yet.." $00

%patch $15CFB "Get your teeth off" $0A
              "that lightswitch" $0A
              "or else.." $00

%patch $15D30 "Ok ok, said Spot" $0A $00
#Text jump
%patch $15D47 "But we have to " $0A
              "go out and check" $0A
              "what that is." $00

%patch $15D80 "Do we have to?" $0A
              "Keen?" $00

%patch $15D9C "Of course we have to." $0A
              "It could be dangerous." $0A
              "I'm the defender, ok?" $00

%patch $15DE2 "Sure you are.." $00

%patch $15DF6 "Yep, of course." $00

%patch $15E07 "Spot! Let the light-" $0A
              "switch be. Don't" $0A
              "touch it anymore today." $0A
              "Now we we have to go" $0A
              "and find out.." $00

%patch $15E72 "what it will be." $00

%end