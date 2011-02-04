----------------------------
IMF Player by Gerstrong v2.0
----------------------------

This IMF Player was built for Commander Genius to support that format.

Special thanks to NY00123! He implemented the support for the frequencies 700 hz and 280 hz which are used by games like Duke Nukem II and Wolfenstein 3D.
He also put more text to be printed out when you just type in the command.

Usage:

IMFPlayer <imffile>
IMFPlayer <imffile> <imf clock rate>
IMFPlayer <imffile> <imf clock rate> <mixer sample rate> <opl rate>
All rates are in Hz

Defaults: <imf clock rate> = 560, <mixer sample rate> = <opl rate> = 49716.\n\n");

Examples:

IMFPlayer K4T01.imf 560 49716 49716\n", argv[0]);
IMFPlayer K4T01.imf 560\n", argv[0]);
IMFPlayer K4T01.imf\n\n", argv[0]);

IMFPlayer track01.wlf 700 44100 44100
IMFPlayer DUKINA.IMF 280 49716 49716
IMFPlayer DUKINA.IMF 280

Note that the rate of 49716Hz is more true to the original OPL chip,
but in case of trouble a different rate can be set.
Also note that if you mix two different rates like "44100 49716", then
sound might play with a wrong pitch!
For possible `futural` reasons, it's still supported, though.

Enjoy it!

The Commander Genius Team :-)
