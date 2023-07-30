#ifndef GAMESELECTOR_H
#define GAMESELECTOR_H

#include <base/GsEngine.h>
#include <widgets/GsDialog.h>
#include <memory>


struct GameEntry
{
    std::string path;
    std::string exefilename;
    std::string name;
    short version = 0;
    bool supported = false;
    Uint16 episode = 0;
    bool demo = false;
    bool crcpass = false;
};



class GameSelector : public GsEngine
{
public:
    GameSelector();

protected:
    /**
     * @brief showMessageBox Just show a simple Modal Message Box
     *                       so user gets some information
     *                       and clicks on "ok"
     * @param text
     */
    void showMessageBox(const std::string &text);

    /**
     * @brief mpMsgDialog
     */
    std::unique_ptr<CGUIDialog> mpMsgDialog;

};

#endif // GAMESELECTOR_H
