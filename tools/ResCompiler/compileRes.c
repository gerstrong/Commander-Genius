#include <stdio.h>

/**
 * @brief compileBinIntoCharArray   Given a file generates a char array
 *                                  that can be used in the main sources
 * @param filename
 * @param output
 */
void compileBinIntoCharArray(const char *filename,
                             const char *globVar,
                             const char *output)
{

    FILE *file = fopen(filename, "rb");
    FILE *out = fopen(output, "w");

    unsigned char buf[16];
    fprintf(out, "#pragma once \n\n static unsigned char %s[] = { ", globVar);
    while(!feof(file))
    {
            size_t count = fread(buf, 1, 16, file);

            for(int n = 0; n < count; ++n)
            {
                    fprintf(out, "0x%02X, ", buf[n]);
            }

            fprintf(out, "\n");
    }
    fprintf(out, "};");
    fclose(file);
    fclose(out);
}

int main()
{
    compileBinIntoCharArray("dpad.png", "gDPadPng", "dpad.h");
    compileBinIntoCharArray("up.png", "gUp", "up.h");
    compileBinIntoCharArray("down.png", "gDown", "down.h");
    compileBinIntoCharArray("1.png","gButton1Png", "button1.h");
    compileBinIntoCharArray("2.png","gButton2Png", "button2.h");
    compileBinIntoCharArray("3.png","gButton3Png", "button3.h");
    compileBinIntoCharArray("4.png","gButton4Png", "button4.h");
    compileBinIntoCharArray("bckgrnd.png","gButtonBgPng", "buttonBg.h");
    compileBinIntoCharArray("confirm.png","gButtonConfirmPng", "buttonConfirm.h");
    compileBinIntoCharArray("start.png","gButtonStartPng", "buttonStart.h");
    compileBinIntoCharArray("closeBtn.png","gButtonClosePng", "buttonClose.h");
    compileBinIntoCharArray("menu.png","gButtonMenuPng", "buttonMenu.h");
    compileBinIntoCharArray("cgttf.ttf","gCgTtf", "cgttf.h");
    compileBinIntoCharArray("TrophyMsg.png","gTrophyMsg", "TrophyMsg.h");
    compileBinIntoCharArray("Trophy_Sound.ogg","gTrophy_Sound", "Trophy_Sound.h");
    return 0;
}

