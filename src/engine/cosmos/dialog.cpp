//
// Created by Eric Fry on 31/10/2017.
//


#include "sound/sfx.h"
#include <SDL_events.h>
#include "sound/music.h"
#include "game.h"
#include "dialog.h"
#include "input.h"
#include "video.h"
#include "map.h"
#include "font.h"
#include "player.h"
#include "actor.h"
#include "save.h"
#include "fullscreen_image.h"
#include "cartoon.h"
#include "high_scores.h"

//Data
uint8 show_one_moment_screen_flag = 0;
sint8 selected_ingame_hint_dialog = -1;
uint8 byte_28BE3 = 0;
uint8 byte_2E21C = 0;
int cur_selected_item = 0;

typedef struct MenuItem {
    uint16 x_pos;
    uint16 y_pos;
    const char *text;
    SDL_Keycode action_key;
} MenuItem;

int cleanup_and_exit();
uint16 restore_savegame_dialog();
void cosmic_hints_dialog(uint16 y_pos);
void display_clear_tile_to_gray(uint16 x, uint16 y);

SDL_Keycode wait_for_input_with_repeat(int spinner_x, int spinner_y, bool allow_key_repeat)
{
    video_update();
    reset_player_control_inputs();
    flush_input();

    uint16 spinner_idx = 0;
    SDL_Keycode keycode = poll_for_key_press(allow_key_repeat);
    while(keycode == SDLK_UNKNOWN)
    {
        //Draw spinning cursor
        video_draw_tile(map_get_bg_tile(1987+spinner_idx), spinner_x * TILE_WIDTH, spinner_y * TILE_HEIGHT);
        video_update();
        if(spinner_idx == 3)
            spinner_idx = 0;
        else
            spinner_idx++;
        cosmo_wait(5);
        keycode = poll_for_key_press(allow_key_repeat);
    }
    display_clear_tile_to_gray(spinner_x, spinner_y);
    return keycode;
}

void wait_for_input(int spinner_x, int spinner_y)
{
    wait_for_input_with_repeat(spinner_x, spinner_y, false);
}

SDL_Keycode wait_for_input_get_key(int spinner_x, int spinner_y)
{
    return wait_for_input_with_repeat(spinner_x, spinner_y, false);
}

uint16 dialog_text_extract_num(const char *text)
{
    char buf[4];

    buf[0] = text[0];
    buf[1] = text[1];
    buf[2] = text[2];
    buf[3] = 0;

    return (uint16)strtol(buf, NULL, 10);
}

void display_dialog_text_with_color(uint16 x_pos, uint16 y_pos, const char *text, uint8 text_color)
{
    HintDialogInput input = NO_INPUT;
    uint16 typewriter_keys_count = 0;
    uint16 typewriter_delay_counter = 0;
    int len = strlen(text);
    int x=0;
    for(int i=0; i < len; i++)
    {
        unsigned char c = (unsigned char)text[i];
        if(c == '\n')
        {
            x = 0;
            y_pos++;
            continue;
        }

        if(c < 123)
        {
            if (typewriter_delay_counter > 0)
            {
                input = hint_dialog_get_input(input);

                if(input == FAST_FORWARD)
                {
                    cosmo_wait(1);
                }
                else
                {
                    if(input == NO_INPUT)
                    {
                        for(; typewriter_keys_count > 0; typewriter_keys_count--)
                        {
                            cosmo_wait(3);
                        }
                    }
                }

                typewriter_keys_count = typewriter_delay_counter;
                if (c != 0x20)
                {
                    play_sfx(0x2c);
                }
                video_update();
                display_char(x_pos + x, y_pos, c, text_color);
            }
            else
            {
                display_char(x_pos + x, y_pos, c, text_color);
            }
            x++;
        }
        if(c >= 0xfb && c < 0xff)
        {
            uint16 frame_num = dialog_text_extract_num(&text[i+1]);
            switch(c)
            {
                case 0xfb : display_cartoon_sequence(frame_num, x_pos + x, y_pos); break;
                case 0xfc :
                    typewriter_keys_count = frame_num;
                    typewriter_delay_counter = frame_num;
                    break;
                case 0xfd : display_player_sprite(frame_num, x_pos + x, y_pos, 6); break;
                case 0xfe :
                    display_actor_sprite_maybe(frame_num, dialog_text_extract_num(&text[i+4]), x_pos + x - 1, y_pos, 6);
                    i += 3;
                    break;
            }
            i += 3;
        }
    }
    video_update();
}

void display_dialog_text(uint16 x_pos, uint16 y_pos, const char *text)
{
    display_dialog_text_with_color(x_pos, y_pos, text, FONT_WHITE);
}

void display_menu_items(int x_offset, MenuItem *menu_items)
{
    MenuItem *item = menu_items;
    for(int i=0; item->text != NULL; item++, i++)
    {
        display_dialog_text_with_color(x_offset + item->x_pos, item->y_pos, item->text, i == cur_selected_item ? 2 : FONT_WHITE);
    }
}

void menu_handle_arrow_key(SDL_Keycode key, MenuItem *items) {
    int num_items = 0;
    for(MenuItem *item = items; item->text != NULL; item++)
    {
        num_items++;
    }

    if(key == SDLK_DOWN)
    {
        cur_selected_item = (cur_selected_item + 1) % num_items;
    }
    if(key == SDLK_UP)
    {
        cur_selected_item = cur_selected_item - 1;
        if(cur_selected_item < 0)
            cur_selected_item = num_items - 1;
    }

}

SDL_Keycode display_menu_items_in_dialog(int x_offset,
                         MenuItem *menu_items,
                         int spinner_x, int spinner_y)
{
    cur_selected_item = 0;
    SDL_Keycode key;
    do {
        display_menu_items(x_offset, menu_items);
        key = wait_for_input_with_repeat(spinner_x, spinner_y, true);
        if(key == SDLK_UP || key == SDLK_DOWN)
        {
            menu_handle_arrow_key(key, menu_items);
        }
    } while(key == SDLK_UP || key == SDLK_DOWN);

    if(is_return_key(key))
    {
        key = menu_items[cur_selected_item].action_key;
    }

    return key;
}

void you_havent_found_any_bombs_dialog() {
    if(game_play_mode == PLAY_GAME)
    {
        play_sfx(0x1e);
        uint16 x = create_text_dialog_box(2, 4, 0x1c, "", "");
        x++;
        display_dialog_text(x, 3, "You haven't found any");
        display_dialog_text(x, 4, "bombs to use yet!     \xfe""056000");
        cosmo_wait(0x3c);
        wait_for_input(x-1 + 0x19, 4);
    }
}

void monster_attack_hint_dialog()
{
    if(game_play_mode == PLAY_GAME)
    {
        play_sfx(0x1e);
        uint16 si = create_text_dialog_box(2, 5, 0x16, "REMINDER:  Jump on", "defend yourself.  ");
        display_dialog_text(si, 4, " top of creatures to");
        cosmo_wait(0x3c);
        wait_for_input(si + 0x13, 5);
        si = create_text_dialog_box(2, 13, 0x14, "Like this...", "Press ANY key.");
        display_dialog_text(si + 5, 9, "   \xfd""036");
        display_dialog_text(si + 5, 11, "   \xfe""118000");
        cosmo_wait(0x3c);
        wait_for_input(si + 0x11, 13);
    }
}

int quit_game_dialog()
{
    uint16 x = create_text_dialog_box(11, 4, 0x12, "Are you sure you", "want to quit? ");
    SDL_Keycode key = wait_for_input_get_key(x + 14, 13);
    if (key == SDLK_y || is_return_key(key))
    {
        return 1;
    }

    return 0;
}

void apogee_bbs_dialog()
{
    fade_to_black_speed_3();
    video_fill_screen_with_black();
    uint16 x = create_text_dialog_box(1, 0x16, 0x26, "THE OFFICIAL APOGEE BBS", "Press ANY key.");
    x += 3;
    display_dialog_text(x, 3, "    -----------------------");
    display_dialog_text(x, 5, "The SOFTWARE CREATIONS BBS is");
    display_dialog_text(x, 6, " the home BBS for the latest");
    display_dialog_text(x, 7, " Apogee games.  Check out our");
    display_dialog_text(x, 8, "FREE 'Apogee' file section for");
    display_dialog_text(x, 9, "  new releases and updates.");
    display_dialog_text(x, 11, "       BBS phone lines:");
    display_dialog_text(x, 13, "(508) 365-2359  2400 baud");
    display_dialog_text(x, 14, "(508) 365-9825  9600 baud");
    display_dialog_text(x, 15, "(508) 365-9668  14.4k dual HST");
    display_dialog_text(x, 0x11, "Home of the Apogee BBS Network!");
    display_dialog_text(x, 0x13, "    A Major Multi-Line BBS.");

    fade_in_from_black_with_delay_3();
    wait_for_input(x + 0x20, 0x15);
    fade_to_black_speed_3();
    video_fill_screen_with_black();

    x = create_text_dialog_box(0, 0x19, 0x28, "APOGEE ON AMERICA ONLINE! ", "Press ANY key.");
    display_dialog_text(x, 2, "      -------------------------");
    display_dialog_text(x, 4, "   America Online (AOL) is host of");
    display_dialog_text(x, 5, " the Apogee Forum, where you can get");
    display_dialog_text(x, 6, "   new Apogee games. Use the Apogee");
    display_dialog_text(x, 7, "  message areas to talk and exchange");
    display_dialog_text(x, 8, "   ideas, comments and secrets with");
    display_dialog_text(x, 9, "   our designers and other players.");
    display_dialog_text(x, 11, "  If you are already a member, after");
    display_dialog_text(x, 12, " you log on, use the keyword \"Apogee\"");
    display_dialog_text(x, 13, " (Ctrl-K) to jump to the Apogee area.");
    display_dialog_text(x, 15, "  If you'd like to know how to join,");
    display_dialog_text(x, 0x10, "        please call toll free:");
    display_dialog_text(x, 0x12, "            1-800-827-6364");
    display_dialog_text(x, 0x13, "    Please ask for extension 5703.");
    display_dialog_text(x, 0x15, "   You'll get the FREE startup kit.");

    fade_in_from_black_with_delay_3();
    wait_for_input(x + 0x25, 0x17);
}

void ordering_info_dialog()
{
    fade_to_black_speed_3();
    video_fill_screen_with_black();
    uint16 x = create_text_dialog_box(0, 0x18, 0x26, "Ordering Information", "Press ANY key.");
    display_dialog_text(x, 2, "  \xfe""223000                              ""\xfe""223000");
    display_dialog_text(x, 4, "      COSMO'S COSMIC ADVENTURE");
    display_dialog_text(x, 5, "    consists of three adventures.");
    display_dialog_text(x, 7, "    Only the first adventure is");
    display_dialog_text(x, 8, " available as shareware.  The final");
    display_dialog_text(x, 9, "   two amazing adventures must be");
    display_dialog_text(x, 10, "    purchased from Apogee, or an");
    display_dialog_text(x, 11, "          authorized dealer.");
    display_dialog_text(x, 13, "  The last two adventures of Cosmo");
    display_dialog_text(x, 14, "   feature exciting new graphics,");
    display_dialog_text(x, 15, "  new creatures, new puzzles, new");
    display_dialog_text(x, 0x10, "   music and all-new challenges!");
    display_dialog_text(x, 0x12, "    The next few screens provide");
    display_dialog_text(x, 0x13, "       ordering instructions.");
    display_dialog_text(x, 0x16, "  \xfe""155000                              ""\xfe""154001");
    fade_in_from_black(1);
    wait_for_input(x + 0x23, 0x16);
    fade_to_black(1);
    video_fill_screen_with_black();

    x = create_text_dialog_box(1, 0x16, 0x26, "Ordering Information", "Press ANY key.");
    display_dialog_text(x, 4, "       Order now and receive:");
    display_dialog_text(x, 6, "   * All three exciting adventures");
    display_dialog_text(x, 7, "   * The hints and tricks sheet");
    display_dialog_text(x, 8, "   * The Secret Cheat password");
    display_dialog_text(x, 9, "   * Exciting new bonus games");
    display_dialog_text(x, 11, "      To order, call toll free:");
    display_dialog_text(x, 12, "           1-800-426-3123");
    display_dialog_text(x, 13, "   (Visa and MasterCard Welcome)");
    display_dialog_text(x, 15, "   Order all three adventures for");
    display_dialog_text(x, 0x10, "     only $35, plus $4 shipping.");
    display_dialog_text(x, 0x13, "              \xfe""129002");
    display_dialog_text(x, 0x14, "  \xfb""014                          ""\xfb""015");
    fade_in_from_black(1);
    wait_for_input(x + 0x23, 0x15);
    fade_to_black(1);
    video_fill_screen_with_black();

    x = create_text_dialog_box(1, 0x16, 0x26, "Ordering Information", "Press ANY key.");
    display_dialog_text(x, 4, "      Please specify disk size:");
    display_dialog_text(x, 5, "           5.25\"  or  3.5\"");
    display_dialog_text(x, 7, "     To order send $35, plus $4");
    display_dialog_text(x, 8, "      shipping, USA funds, to:");
    display_dialog_text(x, 10, "           Apogee Software");
    display_dialog_text(x, 11, "           P.O. Box 476389");
    display_dialog_text(x, 12, "       Garland, TX 75047  (USA)");
    display_dialog_text(x, 14, "\xfe""101003       Or CALL NOW toll free:  ""\xfe""101000");
    display_dialog_text(x, 15, "           1-800-426-3123");
    display_dialog_text(x, 0x12, "         ORDER COSMO TODAY!");
    display_dialog_text(x, 0x13, "           All 3 for $39!");
    display_dialog_text(x, 0x14, "  \xfb""014                          ""\xfb""015");
    fade_in_from_black(1);
    wait_for_input(x + 0x23, 0x15);
    fade_to_black(1);
    video_fill_screen_with_black();

    x = create_text_dialog_box(4, 15, 0x26, "USE YOUR FAX MACHINE TO ORDER!", "Press ANY key.");
    display_dialog_text(x, 7, "  You can now use your FAX machine");
    display_dialog_text(x, 8, "   to order your favorite Apogee");
    display_dialog_text(x, 9, "     games quickly and easily.");
    display_dialog_text(x, 11, "   Simply print out the ORDER.FRM");
    display_dialog_text(x, 12, "    file, fill it out and FAX it");
    display_dialog_text(x, 13, "    to us for prompt processing.");
    display_dialog_text(x, 15, "     FAX Orders: (214) 278-4670");
    fade_in_from_black(1);
    wait_for_input(x + 0x23, 0x11);
    fade_to_black(1);
    video_fill_screen_with_black();

    x = create_text_dialog_box(1, 0x14, 0x26, "About Apogee Software", "Press ANY key.");
    x += 2;
    display_dialog_text(x, 4, "Our goal is to establish Apogee");
    display_dialog_text(x, 5, "  as the leader in commercial");
    display_dialog_text(x, 6, " quality shareware games. With");
    display_dialog_text(x, 7, " enthusiasm and dedication we");
    display_dialog_text(x, 8, "think our goal can be achieved.");
    display_dialog_text(x, 10, "However,  we need your support.");
    display_dialog_text(x, 11, "Shareware is not free software.");
    display_dialog_text(x, 13, "  We thank you in advance for");
    display_dialog_text(x, 14, "   your contribution to the");
    display_dialog_text(x, 15, "  growing shareware community.");
    display_dialog_text(x - 2, 0x11, "\xfd""010        Your honesty pays...     ""\xfd""033");
    fade_in_from_black(1);
    wait_for_input(x + 0x21, 0x13);
}

void foreign_orders_dialog()
{
    fade_to_black_speed_3();
    video_fill_screen_with_black();
    uint16 si = create_text_dialog_box(1, 0x13, 0x26, "FOREIGN CUSTOMERS", "Press ANY key.");
    si += 2;
    display_dialog_text(si, 3, "        -----------------");
    display_dialog_text(si, 5, " The following screens list our");
    display_dialog_text(si, 6, "   dealers outside the United");
    display_dialog_text(si, 7, " States, for Australia, Germany,");
    display_dialog_text(si, 8, " Canada and the United Kingdom.");
    display_dialog_text(si, 10, "   These are official Apogee");
    display_dialog_text(si, 11, "    dealers with the latest");
    display_dialog_text(si, 12, "\xfe""153000       games and updates.    ""\xfe""153001");
    display_dialog_text(si, 14, " If your country is not listed,");
    display_dialog_text(si, 15, "  you may order directly from");
    display_dialog_text(si, 0x10, "Apogee by phone: (214) 278-5655.");
    fade_in_from_black(1);
    wait_for_input(si + 0x21, 0x12);
    fade_to_black(1);
    video_fill_screen_with_black();

    si = create_text_dialog_box(1, 0x13, 0x26, "AUSTRALIAN CUSTOMERS", "Press ANY key.");
    si += 3;
    display_dialog_text(si, 4, "PRICE: $45 + $5 shipping.");
    display_dialog_text(si, 6, "BudgetWare");
    display_dialog_text(si, 7, "P.O. Box 496");
    display_dialog_text(si, 8, "Newtown, NSW  2042        \xfe""113000");
    display_dialog_text(si, 10, "Phone:      (02) 519-4233");
    display_dialog_text(si, 11, "Toll free:  (008) 022-064");
    display_dialog_text(si, 12, "Fax:        (02) 516-4236");
    display_dialog_text(si, 13, "CompuServe: 71520,1475");
    display_dialog_text(si, 15, "Use MasterCard, Visa, Bankcard,");
    display_dialog_text(si, 0x10, "cheques.");
    fade_in_from_black(1);
    wait_for_input(si + 0x20, 0x12);
    fade_to_black(1);
    video_fill_screen_with_black();

    si = create_text_dialog_box(1, 0x14, 0x26, "CANADIAN CUSTOMERS", "Press ANY key.");
    si += 3;
    display_dialog_text(si, 4, "PRICE: $42 Canadian.       \xfe""146000");
    display_dialog_text(si, 6, "Distant Markets");
    display_dialog_text(si, 7, "Box 1149");
    display_dialog_text(si, 8, "194 - 3803 Calgary Trail S.");
    display_dialog_text(si, 9, "Edmondton, Alb.  T6J 5M8");
    display_dialog_text(si, 10, "CANADA");
    display_dialog_text(si, 12, "Orders:    1-800-661-7383");
    display_dialog_text(si, 13, "Inquiries: (403) 436-3009");
    display_dialog_text(si, 14, "Fax:       (403) 435-0928  \xfe""086002");
    display_dialog_text(si, 0x10, "Use MasterCard, Visa or");
    display_dialog_text(si, 0x11, "money orders.");
    fade_in_from_black(1);
    wait_for_input(si + 0x20, 0x13);
    fade_to_black(1);
    video_fill_screen_with_black();

    si = create_text_dialog_box(1, 0x14, 0x26, "GERMAN CUSTOMERS", "Press ANY key.");
    si += 3;
    display_dialog_text(si, 4, "Price: 49,-- DM plus 10,-- DM");
    display_dialog_text(si, 5, "Total: 59,-- DM (Deutsche Mark)");
    display_dialog_text(si, 7, "CDV-Software");
    display_dialog_text(si, 8, "Ettlingerstr. 5");
    display_dialog_text(si, 9, "7500 Karlsruhe 1  GERMANY");
    display_dialog_text(si, 11, "Phone: 0721-22295");
    display_dialog_text(si, 12, "Fax:   0721-21314            \xfe""127004");
    display_dialog_text(si, 13, "Compuserve: 1000022,274");
    display_dialog_text(si, 15, "Use Visa, MasterCard, EuroCard,");
    display_dialog_text(si, 0x10, "American Express, cheque, money");
    display_dialog_text(si, 0x11, "order, or C.O.D.");
    fade_in_from_black(1);
    wait_for_input(si + 0x20, 0x13);
    fade_to_black(1);
    video_fill_screen_with_black();

    si = create_text_dialog_box(1, 0x14, 0x26, "UNITED KINGDOM CUSTOMERS", "Press ANY key.");
    si += 3;
    display_dialog_text(si, 4, "Price: /29 + VAT + 2 P&P     \xfe""085000");
    display_dialog_text(si, 6, "Precision Software Applications");
    display_dialog_text(si, 7, "Unit 3, Valley Court Offices");
    display_dialog_text(si, 8, "Lower Rd");
    display_dialog_text(si, 9, "Croydon, Near Royston");
    display_dialog_text(si, 10, "Herts. SG8 0HF, United Kingdom");
    display_dialog_text(si, 12, "Phone: +44 (0) 223 208 288");
    display_dialog_text(si, 13, "FAX:   +44 (0) 223 208 089");
    display_dialog_text(si, 15, "Credit cards, Access, cheques,");
    display_dialog_text(si, 0x10, "postal & Bankers orders.");
    display_dialog_text(si, 0x11, "Make cheques payable to PSA.");
    fade_in_from_black(1);
    wait_for_input(si + 0x20, 0x13);
}

MenuItem main_menu_items[14] = {
        {0, 5, " B)egin New Game", SDLK_b},
        {0, 6, " R)estore A Game", SDLK_r},
        {0, 7, " S)tory", SDLK_s},
        {0, 8, " I)nstructions", SDLK_i},
        {0, 9, " H)igh Scores", SDLK_h},
        {0, 10, " G)ame Redefine", SDLK_g},
        {0, 12, " O)rdering Info.", SDLK_o},
        {0, 14, " F)oreign Orders", SDLK_f},
        {0, 15, " A)pogee's BBS", SDLK_a},
        {0, 0x10, " D)emo", SDLK_d},
        {0, 0x11, " C)redits", SDLK_c},
        {0, 0x12, " T)itle Screen", SDLK_t},
        {0, 0x14, " Q)uit Game", SDLK_q},
        {0, 0, NULL, 0}
};

void main_menu_dialog()
{
    cur_selected_item = 0;
    int x = create_text_dialog_box(2, 0x15, 0x14, "MAIN MENU", "");
    display_menu_items(x, main_menu_items);
}

void story_dialog()
{
    fade_to_black_speed_3();
    video_fill_screen_with_black();
    uint16 x = create_text_dialog_box(1, 0x17, 0x26, "STORY", "Press ANY key.");
    display_dialog_text(x + 1, 8, "\xfb""000");
    display_dialog_text(x + 1, 0x14, "\xfb""002");
    display_dialog_text(x + 0x10, 5, "Tomorrow is Cosmo's");
    display_dialog_text(x + 0x10, 7, "birthday, and his");
    display_dialog_text(x + 0x10, 9, "parents are taking");
    display_dialog_text(x + 0x10, 11, "him to the one place");
    display_dialog_text(x + 0x10, 13, "in the Milky Way");
    display_dialog_text(x + 0x10, 15, "galaxy that all kids");
    display_dialog_text(x + 0x10, 0x11, "would love to go to:");
    display_dialog_text(x + 0x10, 0x13, "   Disney World!");
    fade_in_from_black_with_delay_3();
    wait_for_input(x + 0x23, 0x16);
    fade_to_black_speed_3();
    video_fill_screen_with_black();

    x = create_text_dialog_box(1, 0x17, 0x26, "STORY", "Press ANY key.");
    display_dialog_text(x + 3, 12, "\xfb""003");
    display_dialog_text(x + 0x19, 12, "\xfb""004");
    display_dialog_text(x + 3, 5, "Suddenly a blazing comet zooms");
    display_dialog_text(x + 4, 7, "toward their ship--leaving no");
    display_dialog_text(x + 0x10, 10, "time");
    display_dialog_text(x + 0x11, 12, "to");
    display_dialog_text(x + 10, 15, "change course...");
    fade_in_from_black_with_delay_3();
    wait_for_input(x + 0x23, 0x16);
    fade_to_black_speed_3();
    video_fill_screen_with_black();

    x = create_text_dialog_box(1, 0x17, 0x26, "STORY", "Press ANY key.");
    display_dialog_text(x + 1 + 1, 7, "\xfb""005");
    display_dialog_text(x + 0x19, 0x14, "\xfb""006");
    display_dialog_text(x + 15, 7, "The comet slams into");
    display_dialog_text(x + 1, 10, "the ship and forces Cosmo's");
    display_dialog_text(x + 1, 13, "dad to make an");
    display_dialog_text(x + 1, 15, "emergency landing");
    display_dialog_text(x + 1, 0x11, "on an uncharted");
    display_dialog_text(x + 1, 0x13, "planet.");
    fade_in_from_black_with_delay_3();
    wait_for_input(x + 0x23, 0x16);
    fade_to_black_speed_3();
    video_fill_screen_with_black();

    x = create_text_dialog_box(1, 0x17, 0x26, "STORY", "Press ANY key.");
    display_dialog_text(x + 0x11, 9, "\xfb""007");
    display_dialog_text(x + 1, 0x14, "\xfb""008");
    display_dialog_text(x + 1 + 1, 5, "While Cosmo's");
    display_dialog_text(x + 1 + 1, 7, "dad repairs");
    display_dialog_text(x + 1 + 1, 9, "the ship,");
    display_dialog_text(x + 11, 15, "Cosmo heads off to");
    display_dialog_text(x + 11, 0x11, "explore and have");
    display_dialog_text(x + 11, 0x13, "some fun.");
    fade_in_from_black_with_delay_3();
    wait_for_input(x + 0x23, 0x16);
    fade_to_black_speed_3();
    video_fill_screen_with_black();

    x = create_text_dialog_box(1, 0x17, 0x26, "STORY", "Press ANY key.");
    display_dialog_text(x + 3, 15, "\xfb""009");
    display_dialog_text(x + 6, 7, "Returning an hour later,");
    display_dialog_text(x + 0x11, 11, "Cosmo cannot find");
    display_dialog_text(x + 0x11, 13, "his Mom or Dad.");
    display_dialog_text(x + 0x11, 15, "Instead, he finds");
    display_dialog_text(x + 8, 0x12, "strange foot prints...");
    fade_in_from_black_with_delay_3();
    wait_for_input(x + 0x23, 0x16);
    fade_to_black_speed_3();
    video_fill_screen_with_black();

    x = create_text_dialog_box(1, 0x17, 0x26, "STORY", "Press ANY key.");
    display_dialog_text(x + 0x15, 0x13, "\xfb""010");
    display_dialog_text(x + 1 + 1, 5, "...oh no!  Has his");
    display_dialog_text(x + 1 + 1, 7, "family been taken");
    display_dialog_text(x + 1 + 1, 9, "away by a hungry");
    display_dialog_text(x + 1 + 1, 11, "alien creature to");
    display_dialog_text(x + 1 + 1, 13, "be eaten?  Cosmo");
    display_dialog_text(x + 1 + 1, 15, "must rescue his");
    display_dialog_text(x + 1 + 1, 0x11, "parents before");
    display_dialog_text(x + 1 + 1, 0x13, "it's too late...!");
    fade_in_from_black_with_delay_3();
    wait_for_input(x + 0x23, 0x16);
}

void missing_savegame_dialog()
{
    uint16 x = create_text_dialog_box(5, 4, 0x14, "Can't find that", "game to restore! ");
    wait_for_input(x + 0x11, 7);
}

void instructions_page1()
{
    uint16 x = create_text_dialog_box(0, 0x18, 0x26, "Instructions  Page One of Five", "Press PgDn for next.  ESC to Exit.");
    display_dialog_text(x, 4, " OBJECT OF GAME:");
    display_dialog_text(x, 6, " On a strange and dangerous planet,");
    display_dialog_text(x, 8, " Cosmo must find and rescue his");
    display_dialog_text(x, 10, " parents.");
    display_dialog_text(x, 13, " Cosmo, having seen big scary alien");
    display_dialog_text(x, 15, " footprints, believes his parents");
    display_dialog_text(x, 17, " have been captured and taken away");
    display_dialog_text(x, 19, " to be eaten!");
}

void instructions_page2()
{
    uint16 x = create_text_dialog_box(0, 0x18, 0x26, "Instructions  Page Two of Five", "Press PgUp or PgDn.  Esc to Exit.");
    display_dialog_text(x, 4, " Cosmo has a very special ability:");
    display_dialog_text(x, 6, " He can use his suction hands to");
    display_dialog_text(x, 8, " climb up walls.");
    display_dialog_text(x, 11, " Warning:  Some surfaces, such as");
    display_dialog_text(x, 13, " ice, might be too slippery for");
    display_dialog_text(x, 15, " Cosmo to cling on firmly.");
    display_dialog_text(x, 20, "\xfd""011                                 \xfd""034");
}

void instructions_page3()
{
    uint16 x = create_text_dialog_box(0, 0x18, 0x26, "Instructions  Page Three of Five", "Press PgUp or PgDn.  Esc to Exit.");
    display_dialog_text(x, 4, " Cosmo can jump onto attacking");
    display_dialog_text(x, 6, " creatures without being harmed.");
    display_dialog_text(x, 8, " This is also Cosmo's way of");
    display_dialog_text(x, 10, " defending himself.");
    display_dialog_text(x, 13, " Cosmo can also find and use bombs.");
    display_dialog_text(x+5, 18, "   \xfd""036");
    display_dialog_text(x+5, 20, "         \xfd""024          \xfd""037");
    display_dialog_text(x+5, 20, "   \xfe""118000         \xfe""057000         \xfe""024000");
}

void instructions_page4()
{
    uint16 x = create_text_dialog_box(0, 0x18, 0x26, "Instructions  Page Four of Five", "Press PgUp or PgDn.  Esc to Exit.");
    display_dialog_text(x, 5, " Use the up and down arrow keys to");
    display_dialog_text(x, 7, " make Cosmo look up and down,");
    display_dialog_text(x, 9, " enabling him to see areas that");
    display_dialog_text(x, 11, " might be off the screen.");
    display_dialog_text(x + 4, 18, "   \xfd""028                  \xfd""029");
    display_dialog_text(x, 19, "      Up Key           Down Key");
}

void instructions_page5()
{
    uint16 x = create_text_dialog_box(0, 0x18, 0x26, "Instructions  Page Five of Five", "Press PgUp.  Esc to Exit.");
    display_dialog_text(x, 5, " In Cosmo's Cosmic Adventure, it's");
    display_dialog_text(x, 7, " up to you to discover the use of");
    display_dialog_text(x, 9, " all the neat and strange objects");
    display_dialog_text(x, 11, " you'll encounter on your journey.");
    display_dialog_text(x, 13, " Secret Hint Globes will help");
    display_dialog_text(x, 15, " you along the way.");
    display_dialog_text(x, 18, "                 \xfe""125000");
    display_dialog_text(x, 20, "              \xfd""027   \xfe""125002");
}

void instructions_dialog()
{
    fade_in_from_black_with_delay_3();
    video_fill_screen_with_black();

    for(uint8 page_num = 1; page_num < 6; )
    {
        fade_to_black(1);

        switch(page_num)
        {
            case 1: instructions_page1(); break;
            case 2: instructions_page2(); break;
            case 3: instructions_page3(); break;
            case 4: instructions_page4(); break;
            case 5: instructions_page5(); break;
            default: break;
        }

        fade_in_from_black(1);

        SDL_Keycode keycode = wait_for_input_get_key(0x25, 0x16);
        while(page_num == 1 && (keycode == SDLK_PAGEUP || keycode == SDLK_UP))
        {
            keycode = wait_for_input_get_key(0x25, 0x16);
        }

        if(keycode == SDLK_ESCAPE)
        {
            return;
        }

        if(keycode == SDLK_PAGEUP || keycode == SDLK_UP)
        {
            if(page_num > 1)
            {
                page_num--;
            }
        }
        else
        {
            page_num++;
        }
    }

    video_fill_screen_with_black();
    cosmic_hints_dialog(3);
}

game_play_mode_enum main_menu() {
    set_initial_game_state();
    show_one_moment_screen_flag = 0;
    load_music(0x12);
    display_fullscreen_image(1);
    flush_input();

    for(int i=0;;i+=3)
    {
        SDL_Keycode key = poll_for_key_press(false);
        if(key != SDLK_UNKNOWN)
        {
            if (key == SDLK_q || key == SDLK_ESCAPE)
            {
                if (quit_game_dialog())
                {
                    return QUIT_GAME;
                }
            }

            uint16 restore_status = 0;
            for(int return_to_title=0;!return_to_title;)
            {
                main_menu_dialog();
                bool key_handled = true;
                do {
                    key = display_menu_items_in_dialog(11, main_menu_items, 0x1c, 0x15);
                    switch(key)
                    {
                        case SDLK_ESCAPE :
                        case SDLK_q :
                            if(quit_game_dialog())
                            {
                                return QUIT_GAME;
                            }
                            return_to_title = 1;
                            i = 0;
                            break;

                        case SDLK_b :
                        case SDLK_SPACE :
                            stop_music();
                            show_one_moment_screen_flag = 1;
                            show_monster_attack_hint = 0;
                            play_sfx(0x30);
                            return PLAY_GAME;

                        case SDLK_r :
                            restore_status = restore_savegame_dialog();
                            if(restore_status == 1)
                            {
                                stop_music();
                                return PLAY_GAME;
                            }

                            if(restore_status == 0)
                            {
                                missing_savegame_dialog();
                            }
                            break;

                        case SDLK_i :
                            instructions_dialog();
                            break;

                        case SDLK_s :
                            story_dialog();
                            break;

                        case SDLK_g :
                            game_redefine();
                            break;

                        case SDLK_F11 :
                            if (cheat_mode_flag)
                            {
                                return RECORD_DEMO;
                            }
                            break;

                        case SDLK_o :
                            if (get_episode_number() == 1) {
                                ordering_info_dialog();
                            } else {
                                commercial_ordering_information_dialog();
                            }
                            break;

                        case SDLK_f :
                            foreign_orders_dialog();
                            break;

                        case SDLK_a :
                            apogee_bbs_dialog();
                            break;

                        case SDLK_d :
                            return PLAY_DEMO;

                        case SDLK_h :
                            fade_to_black_speed_3();
                            video_fill_screen_with_black();
                            display_high_score_dialog(true);
                            break;

                        case SDLK_c :
                            display_fullscreen_image(2);
                            while(poll_for_key_press(false)==SDLK_UNKNOWN)
                            {}
                            break;

                        case SDLK_t :
                            return_to_title = 1;
                            i = 0;
                            break;

                        default :
                            key_handled = false;
                            break;
                    }
                } while(!key_handled && !return_to_title);

                display_fullscreen_image(1);
            }
        }
        else
        {
            if(i==600)
            {
                display_fullscreen_image(2);
            }
            if(i == 1200)
            {
                return PLAY_DEMO;
            }
        }
        cosmo_wait(3);
    }
}

const static uint8 level_numbers_tbl[] ={ 1, 2, 0, 0, 3, 4, 0, 0, 5, 6, 0, 0, 7, 8, 0, 0, 9, 10 };
void now_entering_level_n_dialog(uint16 level_number)
{
    if(game_play_mode == 0)
    {
        uint16 x = create_text_dialog_box(7, 3, 0x18, "\xfc""003  Now entering level", "");
        cosmo_wait(0x14);
        play_sfx(0x40);
        if(level_numbers_tbl[level_number] != 10)
        {
            display_number(x + 0x14, 8, level_numbers_tbl[level_number]);
        }
        else
        {
            display_number(x + 0x15, 8, level_numbers_tbl[level_number]);
        }
        video_update();
    }
}

void ingame_hint_dialogs_ep1(uint16 hint_number) {
    cosmo_wait(0x1e);
    flush_input();
    uint16 x;
    if(hint_number != 0 && hint_number < 15)
    {
        x = create_text_dialog_box(2, 9, 0x1c, "COSMIC HINT!", "Press any key to exit.");
        display_dialog_text(x, 8, " Press SPACE to hurry or");
    }

    switch (hint_number)
    {
        case 0:
            x = create_text_dialog_box(2, 11, 0x1c, "COSMIC HINT!", "Press any key to exit.");
            display_dialog_text(x, 10, " Press SPACE to hurry or");
            display_dialog_text(x, 5, "\xfc""003 These hint globes will\n"
                                               " help you along your\n"
                                               " journey.  Press the up\n"
                                               " key to reread them");

            wait_for_input(x + 0x19, 11);
            break;

        case 1:
            display_dialog_text(x, 5, "\xfc""003 Bump head into switch\n above!");
            break;

        case 2:
            display_dialog_text(x, 5, "\xfc""003 The ice in this cave is\n very, very slippery.");
            break;

        case 3:
            display_dialog_text(x, 5, "\xfc""003 Use this shield for\n temporary invincibility.");
            break;

        case 4:
            display_dialog_text(x, 5, "\xfc""003 You found a secret\n area!!!  Good job!");
            break;

        case 5:
            display_dialog_text(x, 5, "\xfc""003 In high places look up\n to find bonus objects.");
            break;

        case 6:
            display_dialog_text(x, 5, "\xfc""003      Out of Order...");
            break;

        case 7:
            display_dialog_text(x, 5, "\xfc""003 This might be a good\n time to save your game!");
            break;

        case 8:
            display_dialog_text(x, 5, "\xfc""003 Press your up key to\n use the transporter.");
            break;

        case 9:
            display_dialog_text(x, 5, "\xfc""003  (1) FOR...");
            break;

        case 10:
            display_dialog_text(x, 5, "\xfc""003  (2) EXTRA...");
            break;

        case 11:
            display_dialog_text(x, 5, "\xfc""003  (3) POINTS,...");
            break;

        case 12:
            display_dialog_text(x, 5, "\xfc""003  (4) DESTROY...");
            break;

        case 13:
            display_dialog_text(x, 5, "\xfc""003  (5) HINT...");
            break;

        case 14:
            display_dialog_text(x, 5, "\xfc""003  (6) GLOBES!!!");
            break;

        case 15:
            x = create_text_dialog_box(2, 11, 0x1c, "COSMIC HINT!", "Press any key to exit.");
            display_dialog_text(x + 0x16, 8, "\xfe""083000");
            display_dialog_text(x, 10, " Press SPACE to hurry or");
            display_dialog_text(x, 5, "\xfc""003  The Clam Plants won't\n  hurt you if their\n  mouths are closed.");
            wait_for_input(x + 0x19, 11);
            break;

        case 16:
            x = create_text_dialog_box(2, 10, 0x1c, "COSMIC HINT!", "Press any key to exit.");
            display_dialog_text(x + 0x17, 7, "\xfe""001002");
            display_dialog_text(x, 9, " Press SPACE to hurry or");
            display_dialog_text(x, 5, "\xfc""003  Collect the STARS to\n  advance to BONUS\n  STAGES.");
            wait_for_input(x + 0x19, 10);
            break;

        case 17:
            x = create_text_dialog_box(2, 10, 0x1c, "COSMIC HINT!", "Press any key to exit.");
            display_dialog_text(x, 9, " Press SPACE to hurry or");
            display_dialog_text(x, 5, "\xfc""003  Some creatures require\n  more than one pounce\n  to defeat!");
            wait_for_input(x + 0x19, 10);
            break;

        case 18:
            x = create_text_dialog_box(2, 9, 0x1e, "COSMIC HINT!", "Press any key to exit.");
            display_dialog_text(x + 0x19, 8, "\xfd""032");
            display_dialog_text(x, 8, "  Press SPACE to hurry or");
            display_dialog_text(x, 5, "\xfc""003 Cosmo can climb wall's\n with his suction hands.");
            wait_for_input(x + 0x1b, 9);
            break;
    }

    if(hint_number != 0 && hint_number < 15)
    {
        wait_for_input(x + 0x19, 9);
    }
}

void ingame_hint_dialogs_ep2(uint16 hint_number) {
    cosmo_wait(0x1e);
    flush_input();
    uint16 x = create_text_dialog_box(2, 9, 0x1c, "COSMIC HINT!", "Press any key to exit.");
    display_dialog_text(x, 8, " Press SPACE to hurry or");

    switch (hint_number) {
        case 0 :
            display_dialog_text(x, 5, "\xfc""003 Look out for enemies");
            display_dialog_text(x, 6, "\xfc""003 from above!");
            break;
        case 1 :
            display_dialog_text(x, 5, "\xfc""003    Don't...");
            break;
        case 2 :
            display_dialog_text(x, 5, "\xfc""003    step...");
            break;
        case 3 :
            display_dialog_text(x, 5, "\xfc""003    on...");
            break;
        case 4 :
            display_dialog_text(x, 5, "\xfc""003    worms...");
            break;
        case 5 :
            display_dialog_text(x, 5, "\xfc""003 There is a secret area");
            display_dialog_text(x, 6, "\xfc""003 in this level!");
            break;
        case 6 :
            display_dialog_text(x, 5, "\xfc""003 You found the secret");
            display_dialog_text(x, 6, "\xfc""003 area.  Well done.");
            break;
        case 7 :
            display_dialog_text(x, 5, "\xfc""    Out of order.");
            break;
        default :
            break;
    }

    wait_for_input(x + 0x19, 9);
}

void ingame_hint_dialogs_ep3(uint16 hint_number) {
    cosmo_wait(0x1e);
    flush_input();
    uint16 x = create_text_dialog_box(2, 9, 0x1c, "COSMIC HINT!", "Press any key to exit.");
    display_dialog_text(x, 8, " Press SPACE to hurry or");

    switch (hint_number) {
        case 0 :
            display_dialog_text(x, 5, "\xfc""003 Did you find the");
            display_dialog_text(x, 6, "\xfc""003 hamburger in this level?");
            break;
        case 1 :
            display_dialog_text(x, 5, "\xfc""003 This hint globe being");
            display_dialog_text(x, 6, "\xfc""003 upgraded to a 80986.");
            break;
        case 2 :
            display_dialog_text(x, 5, "\xfc""003 WARNING:  Robots shoot");
            display_dialog_text(x, 6, "\xfc""003 when the lights are on!");
            break;
        case 3 :
            display_dialog_text(x, 5, "\xfc""003 There is a hidden scooter");
            display_dialog_text(x, 6, "\xfc""003 in this level.");
            break;
        case 4 :
            display_dialog_text(x, 5, "\xfc""003 Did you find the");
            display_dialog_text(x, 6, "\xfc""003 hamburger in level 8!");
            break;
        case 5 :
            display_dialog_text(x, 5, "\xfc""003   Out of order...!");
            break;
        default :
            break;
    }

    wait_for_input(x + 0x19, 9);
}

void ingame_hint_dialogs(uint16 hint_number) {
    switch (get_episode_number()) {
        case 1 : ingame_hint_dialogs_ep1(hint_number); break;
        case 2 : ingame_hint_dialogs_ep2(hint_number); break;
        case 3 : ingame_hint_dialogs_ep3(hint_number); break;
        default : break;
    }
}

void power_up_module_dialog()
{
    if(game_play_mode == PLAY_GAME)
    {
        play_sfx(0x1e);
        int si = create_text_dialog_box(2, 5, 0x16, "", "");
        display_dialog_text(si, 3, " Power Up modules");
        display_dialog_text(si, 4, " increase Cosmo's");
        display_dialog_text(si, 5, " health.         \xfe""028002");
        cosmo_wait(0x3c);
        flush_input();
        wait_for_input(si + 8, 5);
    }
}

void display_clear_tile_to_gray(uint16 x, uint16 y)
{
    video_draw_tile(map_get_bg_tile(1999), x * TILE_WIDTH, y * TILE_HEIGHT);
}

static const char score_text_tbl[][17] = {
        "    Not Bad!    ",
        "    Way Cool    ",
        "     Groovy     ",
        "    Radical!    ",
        "     Insane     ",
        "     Gnarly     ",
        "   Outrageous   ",
        "   Incredible   ",
        "    Awesome!    ",
        "   Brilliant!   ",
        "    Profound    ",
        "    Towering    ",
        "Rocket Scientist"
};

void display_score_from_level()
{
    stop_music();

    if(num_stars_collected == 0)
    {
        fade_in_from_black_with_delay_3();
        return;
    }

    fade_to_white(3);
    video_fill_screen_with_black();
    create_text_dialog_box(2, 0xe, 0x1e, "Super Star Bonus!!!!", "");

    display_actor_sprite_maybe(1, 2, 8, 8, 6);

    display_dialog_text(0xe, 7, "X 1000 =");

    display_number(0x1b, 7, num_stars_collected * 1000);

    cosmo_wait(0x32);
    display_dialog_text(0xa, 0xc, "YOUR SCORE =  ");
    display_number(0x1d, 0xc, score);

    fade_in_from_black_with_delay_3();

    cosmo_wait(0x64);

    int star_counter = 0;
    for(int i=num_stars_collected; i > 0; i--)
    {
        score += 1000;
        cosmo_wait(0xf);

        for(int j=0; j < 7; j++)
        {
            display_clear_tile_to_gray(0x17 + j, 0xc);
        }
        play_sfx(1);
        display_number(0x1d, 0xc, score);

        if (star_counter / 6 < 13)
        {
            star_counter++;
        }

        for(int j=0; j < 16; j++)
        {
            if(j < 7)
            {
                display_clear_tile_to_gray(0x16 + j, 7);
            }

            if((star_counter & 7) == 1)
            {
                display_clear_tile_to_gray(0xd + j, 0xe);

            }
        }

        display_number(0x1b, 7, i * 1000);

        video_update();

        if((star_counter & 7) == 1)
        {
            uint8 score_text_idx = (uint8)(star_counter/6);
            if(score_text_idx > 12)
            {
                score_text_idx = 12;
            }

            display_dialog_text(0xd, 0xe, score_text_tbl[score_text_idx]);
        }

        video_update();
    }

    cosmo_wait(0x190);
    num_stars_collected = 0;
}

void display_end_of_level_score_dialog(const char *header_text, const char *footer_text)
{
    fade_to_black_speed_3();
    video_fill_screen_with_black();
    uint16 x = create_text_dialog_box(6, 4, 0x1e, header_text, footer_text);
    fade_in_from_black_with_delay_3();
    wait_for_input(x + 0x1b, 8);
    display_score_from_level();
    fade_to_black_speed_3();
    video_fill_screen_with_black();
}

void end_game_dialog(uint16 dialog_number) {
    if(get_episode_number() != 1)
    {
        return;
    }

    flush_input();
    reset_player_control_inputs();
    if(dialog_number == 0xa6)
    {
        finished_game_flag_maybe = 1;
        return;
    }
    if(dialog_number == 0xa4 || dialog_number == 0xa5)
    {
        uint16 x = create_text_dialog_box(2, 8, 0x1c, "", "Press any key to exit.");
        if(dialog_number == 0xa4)
        {
            display_dialog_text(x, 4, "\xfc""003 What's happening?  Is\n Cosmo falling to his\n Doom?");
        }
        else
        {
            display_dialog_text(x, 4, "\xfc""003 Is there no end to this\n pit?  And what danger\n awaits below?!");
        }
        wait_for_input(x + 0x19, 8);
    }
}

uint16 draw_dialog_frame(uint16 x_pos, uint16 y_pos, uint16 height, uint16 width, const char *top_text, const char *bottom_text, uint8 display_text)
{
    video_draw_tile(map_get_bg_tile(1991), x_pos * TILE_WIDTH, y_pos * TILE_HEIGHT);
    video_draw_tile(map_get_bg_tile(1992), (x_pos + width-1) * TILE_WIDTH, y_pos * TILE_HEIGHT);

    for(int x=1;x < width-1;x++)
    {
        video_draw_tile(map_get_bg_tile(1995), (x_pos + x) * TILE_WIDTH, y_pos * TILE_HEIGHT);
        video_draw_tile(map_get_bg_tile(1996), (x_pos + x) * TILE_WIDTH, (y_pos + height-1) * TILE_HEIGHT);

        for(int y=1;y < height-1;y++)
        {
            video_draw_tile(map_get_bg_tile(1997), x_pos * TILE_WIDTH, (y_pos + y) * TILE_HEIGHT);
            video_draw_tile(map_get_bg_tile(1998), (x_pos + width-1) * TILE_WIDTH, (y_pos + y) * TILE_HEIGHT);

            for(int x1=1;x1 < width-1;x1++)
            {
                video_draw_tile(map_get_bg_tile(1999), (x_pos + x1) * TILE_WIDTH, (y_pos + y) * TILE_HEIGHT);
            }
        }
    }

    video_draw_tile(map_get_bg_tile(1993), x_pos * TILE_WIDTH, (y_pos + height-1) * TILE_HEIGHT);
    video_draw_tile(map_get_bg_tile(1994), (x_pos + width-1) * TILE_WIDTH, (y_pos + height-1) * TILE_HEIGHT);

    if(display_text)
    {
        display_dialog_text(0x14 - (strlen(top_text)/2), y_pos + 1, top_text);
        display_dialog_text(0x14 - (strlen(bottom_text)/2), y_pos + height - 2, bottom_text);
    }

    video_update();
    return x_pos + 1;
}

uint16 create_text_dialog_box(uint16 y_offset, uint16 height, uint16 width, const char *top_message, const char *bottom_message)
{
    int dialog_x = 0x14 - (width / 2);
    int var_4 = (height / 2) + y_offset;
    int cur_width = 1;
    for(int cur_x = 0x13; cur_x > dialog_x; cur_x--)
    {
        cur_width = cur_width + 2;
        draw_dialog_frame(cur_x, var_4, 2, cur_width, "", "", 0);
        cosmo_wait(1);
    }

    int cur_height = 0;
    for(int cur_y = var_4;cur_y >= ((height & 1) ? 1 : 2) + y_offset; cur_y--)
    {
        cur_height += 2;
        draw_dialog_frame(dialog_x, cur_y, cur_height, width, "", "", 0);
        cosmo_wait(1);
    }

    return draw_dialog_frame(dialog_x, y_offset, height, width, top_message, bottom_message, 1);
}

void savegame_load_savegame_slot_text(char *text, uint8 position)
{
    SaveGameData save_data;
    SaveStatus status = load_savegame_data_from_file('1' + position, &save_data);
    if(status == LOADED)
    {
        sprintf(text, "%d) L%-2d H%d B%d *%-2d %8d",
                position+1, level_numbers_tbl[save_data.current_level], save_data.health-1, save_data.num_bombs,
                save_data.num_stars_collected, save_data.score);
    }
}

void savegame_dialog()
{
    char slot_text[][26] = { //"3) L13 H3 B6 *50 10000000\0"
            "1) -- Empty Slot --",
            "2) -- Empty Slot --",
            "3) -- Empty Slot --",
            "4) -- Empty Slot --",
            "5) -- Empty Slot --",
            "6) -- Empty Slot --",
            "7) -- Empty Slot --",
            "8) -- Empty Slot --",
            "9) -- Empty Slot --",
    };

    MenuItem items[] = {
            {1, 5, slot_text[0], SDLK_1},
            {1, 6, slot_text[1], SDLK_2},
            {1, 7, slot_text[2], SDLK_3},
            {1, 8, slot_text[3], SDLK_4},
            {1, 9, slot_text[4], SDLK_5},
            {1, 10, slot_text[5], SDLK_6},
            {1, 11, slot_text[6], SDLK_7},
            {1, 12, slot_text[7], SDLK_8},
            {1, 13, slot_text[8], SDLK_9},
            {0, 0, NULL, 0}
    };

    for(int i=0; i < 9; i++)
    {
        savegame_load_savegame_slot_text(slot_text[i], i);
    }

    uint16 x = create_text_dialog_box(1, 18, 29, "Save a game.", "Press ESC to quit.");
    display_dialog_text(x, 3, " What game number (1-9)?");
    display_dialog_text(x, 15, " NOTE: Game is saved at");
    display_dialog_text(x, 16, " BEGINNING of level.");
    SDL_Keycode keycode = display_menu_items_in_dialog(x, items, x + 0x18, 3); //wait_for_input(x + 0x18, 11);
    if(keycode == SDLK_ESCAPE || keycode == SDLK_SPACE || is_return_key(keycode))
    {
        return;
    }

    if(keycode >= SDLK_1 && keycode <= SDLK_9)
    {
        display_char(x + 0x18, 3, keycode, FONT_WHITE);
        uint16 tmp_num_bombs = num_bombs;
        uint32 tmp_num_stars_collected = num_stars_collected;
        uint16 tmp_current_level = current_level;
        uint8 tmp_num_health_bars = num_health_bars;
        uint32 tmp_score = score;
        uint8 tmp_health = health;

        load_savegame_file('T');
        write_savegame_file(keycode);

        health = tmp_health;
        num_bombs = tmp_num_bombs;
        num_stars_collected = tmp_num_stars_collected;
        current_level = tmp_current_level;
        score = tmp_score;
        num_health_bars = tmp_num_health_bars;
        x = create_text_dialog_box(7, 4, 0x14, "Game Saved.", "Press ANY key.");
        wait_for_input(x + 0x11, 9);
    }
    else
    {
        x = create_text_dialog_box(11, 4, 0x1c, "Invalid game number!", "Press ANY key.");
        wait_for_input(x + 0x19, 13);
    }
}

uint16 restore_savegame_dialog()
{
    char slot_text[][26] = { //"3) L13 H3 B6 *50 10000000\0"
            "1) -- Empty Slot --",
            "2) -- Empty Slot --",
            "3) -- Empty Slot --",
            "4) -- Empty Slot --",
            "5) -- Empty Slot --",
            "6) -- Empty Slot --",
            "7) -- Empty Slot --",
            "8) -- Empty Slot --",
            "9) -- Empty Slot --",
    };

    MenuItem items[] = {
            {1, 7, slot_text[0], SDLK_1},
            {1, 8, slot_text[1], SDLK_2},
            {1, 9, slot_text[2], SDLK_3},
            {1, 10, slot_text[3], SDLK_4},
            {1, 11, slot_text[4], SDLK_5},
            {1, 12, slot_text[5], SDLK_6},
            {1, 13, slot_text[6], SDLK_7},
            {1, 14, slot_text[7], SDLK_8},
            {1, 15, slot_text[8], SDLK_9},
            {0, 0, NULL, 0}
    };

    for(int i=0; i < 9; i++)
    {
        savegame_load_savegame_slot_text(slot_text[i], i);
    }

    uint16 x = create_text_dialog_box(3, 16, 29, "Restore a game.", "Press ESC to quit.");
    display_dialog_text(x, 5, " What game number (1-9)?");
    SDL_Keycode character = display_menu_items_in_dialog(x, items, x + 0x18, 5);
    if(character != SDLK_ESCAPE && character != SDLK_SPACE && !is_return_key(character))
    {
        if(character >= SDLK_1 && character <= SDLK_9)
        {
            display_char(x + 0x18, 5, character, FONT_WHITE);
            if(load_savegame_file(character))
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        x = create_text_dialog_box(11, 4, 0x1c, "Invalid game number!", "Press ANY key.");
        wait_for_input(x + 0x19, 13);
    }
    return 2;
}

void sound_test_dialog()
{
    uint16 cur_sfx_num = 1;
    uint8 tmp_sfx_flag = sfx_on_flag;
    sfx_on_flag = 1;
    uint16 x = create_text_dialog_box(2, 7, 0x22, "Test Sound", "Press ESC to quit.");
    display_dialog_text(x, 4, " Press \x18 or \x19 to change sound #.");
    display_dialog_text(x, 5, "   Press Enter to hear sound.");

    for(;;)
    {
        display_clear_tile_to_gray(x + 15, 6);
        display_clear_tile_to_gray(x + 16, 6);

        display_number(x + 16, 6, cur_sfx_num);
        SDL_Keycode key = wait_for_input_with_repeat(x + 0x1f, 7, true);
        switch(key)
        {
            case SDLK_DOWN :
                if(cur_sfx_num > 1)
                {
                    cur_sfx_num--;
                }
                break;
            case SDLK_UP :
                if(cur_sfx_num < 65)
                {
                    cur_sfx_num++;
                }
                break;
            case SDLK_RETURN :
            case SDLK_RETURN2 :
            case SDLK_KP_ENTER :
                play_sfx(cur_sfx_num);
                break;
            case SDLK_ESCAPE :
                sfx_on_flag = tmp_sfx_flag;
                return;
            default :
                break;
        }
    }
}

void enter_new_key(uint16 x, const char *text, InputCommand command)
{
    display_dialog_text(x + 4, 12, text);
    display_dialog_text(x + 4, 13, "Enter new key:");
    SDL_Keycode keycode = wait_for_input_get_key(x + 0x12, 13);
    if(keycode != SDLK_ESCAPE)
    {
        set_input_command_key(command, keycode);
    }
}

static const MenuItem keyboard_config_items[] = {
        {1, 6, " #1) Up key is:", SDLK_1},
        {1, 7, " #2) Down key is:", SDLK_2},
        {1, 8, " #3) Left key is:", SDLK_3},
        {1, 9, " #4) Right key is:", SDLK_4},
        {1, 10, " #5) Jump key is:", SDLK_5},
        {1, 11, " #6) Bomb key is:", SDLK_6},
        {0, 0, NULL, 0}
};
void keyboard_config()
{
    uint16 x = create_text_dialog_box(3, 15, 0x1b, "Keyboard Config.", "Press ESC to quit.");

    for(;;)
    {
        display_dialog_text(x + 0x13, 6, get_command_key_string(CMD_KEY_UP));
        display_dialog_text(x + 0x13, 7, get_command_key_string(CMD_KEY_DOWN));
        display_dialog_text(x + 0x13, 8, get_command_key_string(CMD_KEY_LEFT));
        display_dialog_text(x + 0x13, 9, get_command_key_string(CMD_KEY_RIGHT));
        display_dialog_text(x + 0x13, 10, get_command_key_string(CMD_KEY_JUMP));
        display_dialog_text(x + 0x13, 11, get_command_key_string(CMD_KEY_BOMB));

        display_dialog_text(x, 15, "Select key # to change or");

        SDL_Keycode keycode = display_menu_items_in_dialog(x, (MenuItem *)keyboard_config_items, x + 0x15, 16);
        switch (keycode)
        {
            case SDLK_ESCAPE : return;
            case SDLK_1 :
                enter_new_key(x, "Modifying UP.", CMD_KEY_UP);
                break;
            case SDLK_2 :
                enter_new_key(x, "Modifying DOWN.", CMD_KEY_DOWN);
                break;
            case SDLK_3 :
                enter_new_key(x, "Modifying LEFT.", CMD_KEY_LEFT);
                break;
            case SDLK_4 :
                enter_new_key(x, "Modifying RIGHT.", CMD_KEY_RIGHT);
                break;
            case SDLK_5 :
                enter_new_key(x, "Modifying JUMP.", CMD_KEY_JUMP);
                break;
            case SDLK_6 :
                enter_new_key(x, "Modifying BOMB.", CMD_KEY_BOMB);
                break;
            default : break;
        }

        draw_dialog_frame(7, 3, 15, 0x1b, "Keyboard Config.", "Press ESC to quit.", 1);
    }
}

static MenuItem game_redefine_items[] = {
        {0, 7, " K)eyboard redefine", SDLK_k},
        {0, 8, " J)oystick redefine", SDLK_j},
        {0, 9, " S)ound toggle", SDLK_s},
        {0, 10, " T)est sound", SDLK_t},
        {0, 11, " M)usic toggle", SDLK_m},
        {0, 0, NULL, 0}
};

void game_redefine()
{
    uint16 si = create_text_dialog_box(4, 0xb, 0x16, "Game Redefine", "Press ESC to quit.");

    SDL_Keycode key = display_menu_items_in_dialog(si, game_redefine_items, 0x1d, 0xd);

    switch (key)
    {
        case SDLK_k :
            keyboard_config();
            break;
        case SDLK_j :
            //FIXME joystick_config() Do we even need this dialog?
            break;
        case SDLK_s :
            sound_toggle_dialog();
            break;
        case SDLK_t :
            sound_test_dialog();
            break;
        case SDLK_m :
            music_toggle_dialog();
            break;
        default :
            break;
    }
}

void cosmic_hints_dialog(uint16 y_pos)
{
    uint16 y = y_pos - 1;
    uint16 x = create_text_dialog_box(y_pos, 0x12, 0x26, "Cosmic Hints", "Press ANY key.");
    display_dialog_text(x, y + 4, " * Usually jumping in the paths of");
    display_dialog_text(x, y + 5, "   bonus objects will lead you in");
    display_dialog_text(x, y + 6, "   the right direction.");
    display_dialog_text(x, y + 8, " * There are many secret bonuses in");
    display_dialog_text(x, y + 9, "   this game, such as bombing 15 of");
    display_dialog_text(x, y + 10, "   the Eye Plants.  (Registered");
    display_dialog_text(x, y + 11, "   players will get the full list.)");
    display_dialog_text(x, y + 13, " * When clinging to a wall, tap the");
    display_dialog_text(x, y + 14, "   jump key to let go and fall.  To");
    display_dialog_text(x, y + 15, "   re-cling to the wall, push");
    display_dialog_text(x, y + 0x10, "   yourself into the wall again.");
    wait_for_input(x + 0x23, y + 0x11);
    x = create_text_dialog_box(y_pos, 0x12, 0x26, "Key Definition Screen", "");
    display_dialog_text(x, y + 4, "                     Look");
    display_dialog_text(x, y + 5, "                      UP");
    display_dialog_text(x, y + 7, "              Walk            Walk");
    display_dialog_text(x, y + 8, "  Jump  Drop  LEFT            RIGHT");
    display_dialog_text(x, y + 9, "   UP   BOMB");
    display_dialog_text(x, y + 10, "                     \xfd""028");
    display_dialog_text(x, y + 11, "                     Look");
    display_dialog_text(x, y + 12, "                     DOWN");
    display_dialog_text(x, y + 13, "              \xfd""001                 ""\xfd""023");
    display_dialog_text(x, y + 14, "  \xfd""030      ""\xfd""037   ""\xfe""024000");
    display_dialog_text(x, y + 0x11, "                     \xfd""029");

    display_dialog_text(x + 0x18, y + 7, get_command_key_string(CMD_KEY_UP));
    display_dialog_text(x + 0x18, y + 14, get_command_key_string(CMD_KEY_DOWN));
    display_dialog_text(x + 14, y + 14, get_command_key_string(CMD_KEY_LEFT));
    display_dialog_text(x + 0x1e, y + 14, get_command_key_string(CMD_KEY_RIGHT));
    display_dialog_text(x + 1 + 1, y + 15, get_command_key_string(CMD_KEY_JUMP));
    display_dialog_text(x + 8, y + 15, get_command_key_string(CMD_KEY_BOMB));

    wait_for_input(x + 0x23, y + 0x11);
    x = create_text_dialog_box(4, 11, 0x22, "During the game, you can...", "Press ANY key.");
    display_dialog_text(x, 7, " Press 'P' to PAUSE GAME");
    display_dialog_text(x, 8, " Press 'ESC' or 'Q' to QUIT game");
    display_dialog_text(x, 9, " Press 'S' to toggle SOUND");
    display_dialog_text(x, 10, " Press 'M' to toggle MUSIC");
    display_dialog_text(x, 11, " Press 'F1' to show HELP");
    wait_for_input(x + 0x1f, 13);
}

void malformed_savegame_dialog()
{
    uint16 x = create_text_dialog_box(2, 4, 0x1c, "Altered file error!!", "Now exiting game!");
    wait_for_input(x + 0x19, 4);
}

static MenuItem help_menu_items[] = {
        {0, 5, " S)ave your game", SDLK_s},
        {0, 6, " R)estore a game", SDLK_r},
        {0, 7, " H)elp", SDLK_h},
        {0, 8, " G)ame redefine", SDLK_g},
        {0, 9, " V)iew High Scores", SDLK_v},
        {0, 10, " Q)uit Game", SDLK_q},
        {0, 0, NULL, 0}
};

uint16 help_menu_dialog()
{
    uint16 x_offset = create_text_dialog_box(2, 12, 0x16, "HELP MENU", "Press ESC to quit.");

    for(;;)
    {
        uint16 status = 0;
        switch (display_menu_items_in_dialog(x_offset, help_menu_items, 0x1d, 12))
        {
            case SDLK_g:
                game_redefine();
                return 0;

            case SDLK_s:
                savegame_dialog();
                return 0;

            case SDLK_r:
                status = restore_savegame_dialog();
                if(status == 1)
                {
                    load_level(current_level);
                    return 1;
                }
                if(status == 0)
                {
                    missing_savegame_dialog();
                }
                return 0;

            case SDLK_v:
                display_high_score_dialog(false);
                return 0;

            case SDLK_q:
                return 2;

            case SDLK_h:
                cosmic_hints_dialog(1);
                return 0;

            case SDLK_ESCAPE:
                return 0;

            default: break;
        }
    }
}

void a_game_by_dialog()
{
    uint16 x = create_text_dialog_box(4, 13, 0x1a, "A game by", "Copyright (c) 1992");
    display_dialog_text(x, 7, "     Todd J Replogle");
    display_dialog_text(x + 11, 9, "and");
    display_dialog_text(x, 11, "\xfd""027   Stephen A Hornback""\xfd""004");
    display_dialog_text(x, 13, "      Version 1.20");
    wait_for_time_or_key(0x2bc);
    fade_to_black_speed_3();
}

void collect_input_string(uint16 x, uint16 y, char *name_buffer, uint8 buf_length)
{
    int i = 0;
    for(; ; )
    {
        SDL_Keycode key = wait_for_input_with_repeat(x + i, y, true);
        display_clear_tile_to_gray(x + i, y);
        if((key >= SDLK_a && key <= SDLK_z)
           || (key >= SDLK_0 && key <= SDLK_9)
           || key == SDLK_SPACE
           || key == SDLK_MINUS
           || key == SDLK_EQUALS
           || key == SDLK_QUOTE
           || key == SDLK_SEMICOLON
           || key == SDLK_SLASH
           || key == SDLK_PERIOD
           || key == SDLK_COMMA)
        {
            if(i < buf_length - 1)
            {
                name_buffer[i] =(char)toupper(key);
                display_char(x + i, y, (char)key, FONT_WHITE);
                i++;
            }
        }
        if(key == SDLK_ESCAPE)
        {
            i = 0;
            break;
        }
        if(is_return_key(key))
        {
            break;
        }
        if(i > 0 && (key == SDLK_BACKSPACE || key == SDLK_DELETE))
        {
            i--;
            name_buffer[i] = ' ';
            display_clear_tile_to_gray(x + i, y);
        }
    }

    name_buffer[i] = '\0';
}

void enter_high_score_name_dialog(char *name_buffer, uint8 buf_length) {
    memset(name_buffer, 0, buf_length);
    uint16 x = create_text_dialog_box(5, 7, 0x24, "You made it into the hall of fame!", "Press ESC to quit.");
    display_dialog_text(x, 8, "Enter your name:");
    fade_in_from_black_with_delay_3();
    play_sfx(0x34);

    collect_input_string(x + 16, 8, name_buffer, buf_length);
}

uint8 warp_level_tbl[] = {0, 1, 4, 5, 8, 9, 12, 13, 16, 17, 20, 2, 3};
int warp_mode_dialog()
{
    char buf[3];
    uint16 x = create_text_dialog_box(2, 4, 0x1c, "Warp Mode!", "Enter level (1-13):");
    collect_input_string(x + 0x16, 4, buf, 3);
    sint16 warp_level = (sint16)strtol(buf, NULL, 10);

    warp_level--;
    if(warp_level < 0 || warp_level > 12)
    {
        return 0;
    }
    else
    {
        current_level = (uint16)warp_level;
        load_savegame_file('T');
        load_level(warp_level_tbl[warp_level]);
    }

    return 1;
}

void display_high_score_dialog(bool use_fading)
{
    for(;;)
    {
        uint16 x = create_text_dialog_box(2, 0x11, 0x1e, "Hall of Fame", "any other key to exit.");

        for(int i = 0; i < NUM_HIGH_SCORE_ENTRIES; i++)
        {
            uint16 y = i + 5;
            HighScore high_score = get_high_score(i);
            display_number(x + 2, y, i + 1);
            display_dialog_text(x + 3, y, ".");
            display_number(x + 11, y, high_score.score);
            display_dialog_text(x + 13, y, high_score.name);
        }

        display_dialog_text(x + 3, 0x10, "Press 'F10' to erase or");

        if(use_fading)
        {
            fade_in_from_black_with_delay_3();
        }
        SDL_Keycode keycode = wait_for_input_get_key(x + 0x1b, 0x11);
        if (keycode != SDLK_F10)
        {
            break;
        }

        x = create_text_dialog_box(5, 4, 0x1c, "Are you sure you want to", "ERASE High Scores?");
        keycode = wait_for_input_get_key(x + 0x16, 7);
        if (keycode == SDLK_y)
        {
            clear_high_score_table();
        }

        if(use_fading)
        {
            fade_to_black_speed_3();
            video_fill_screen_with_black();
        }
    }
}

void sound_toggle_dialog()
{
    uint16 x;
    if(sfx_on_flag)
    {
        sfx_on_flag = 0;
        x = create_text_dialog_box(2, 4, 0x18, "Sound Toggle", "The sound is now OFF!");
    }
    else
    {
        sfx_on_flag = 1;
        x = create_text_dialog_box(2, 4, 0x18, "Sound Toggle", "The sound is now ON!");
    }

    wait_for_input(x + 0x15, 4);
}

void music_toggle_dialog()
{
    uint16 x;
    if(music_on_flag)
    {
        music_on_flag = 0;
        x = create_text_dialog_box(2, 4, 0x18, "Music Toggle", "The music is now OFF!");
        stop_music();
    }
    else
    {
        music_on_flag = 1;
        x = create_text_dialog_box(2, 4, 0x18, "Music Toggle", "The music is now ON!");
        play_music();
    }

    wait_for_input(x + 0x15, 4);
}

void god_mode_toggle_dialog()
{
    uint16 x;
    if(god_mode_flag)
    {
        god_mode_flag = false;
        x = create_text_dialog_box(2, 4, 0x1c, "God Toggle", "The god mode is now OFF!");
    }
    else
    {
        god_mode_flag = true;
        x = create_text_dialog_box(2, 4, 0x1c, "God Toggle", "The god mode is now ON!");
    }

    wait_for_input(x + 0x19, 4);
}

void hack_mover_toggle_dialog()
{
    uint16 x;
    if(cheat_hack_mover_enabled)
    {
        cheat_hack_mover_enabled = false;
        x = create_text_dialog_box(2, 4, 0x1e, "Hack Mover Toggle", "Hack Mover is now OFF!");
    }
    else
    {
        cheat_hack_mover_enabled = true;
        x = create_text_dialog_box(2, 4, 0x1e, "Hack Mover Toggle", "Hack Mover is now ON!");
    }

    wait_for_input(x + 0x1b, 4);
}

void end_sequence_ep1() {
    cosmo_wait(5);
    fade_to_black_speed_3();
    display_fullscreen_image(4);
    wait_for_input(0x27, 0x18);
    fade_to_white(4);
    video_fill_screen_with_black();
    
    uint16 x = create_text_dialog_box(1, 0x18, 0x26, "", "Press ANY key.");
    
    display_dialog_text(x + 4, 13, "\xfb""016");
    display_dialog_text(x + 0x1c, 0x16, "\xfb""017");
    fade_in_from_black_with_delay_3();
    display_dialog_text(x + 14, 4, "\xfc""003Are Cosmo's cosmic ");
    display_dialog_text(x + 14, 5, "\xfc""003adventuring days ");
    display_dialog_text(x + 14, 6, "\xfc""003finally over?    ");
    display_dialog_text(x + 14, 8, "\xfc""003Will Cosmo's parents ");
    display_dialog_text(x + 14, 9, "\xfc""003be lightly seasoned ");
    display_dialog_text(x + 14, 10, "\xfc""003and devoured before ");
    display_dialog_text(x + 14, 11, "\xfc""003he can save them?      ");
    display_dialog_text(x + 1, 15, "\xfc""003Find the stunning ");
    display_dialog_text(x + 1, 0x10, "\xfc""003answers in the next two ");
    display_dialog_text(x + 1, 0x11, "\xfc""003NEW, shocking, amazing, ");
    display_dialog_text(x + 1, 0x12, "\xfc""003horrifying, wacky and ");
    display_dialog_text(x + 1, 0x13, "\xfc""003exciting episodes of...         ");
    display_dialog_text(x + 1, 0x15, "\xfc""003COSMO'S COSMIC ADVENTURE!");
    wait_for_input(x + 0x23, 0x17);
    fade_to_black_speed_3();
    video_fill_screen_with_black();
    x = create_text_dialog_box(6, 4, 0x18, "Thank you", " for playing!");
    fade_in_from_black_with_delay_3();
    cosmo_wait(0x64);
    wait_for_input(x + 0x15, 8);
    ordering_info_dialog();
    display_score_from_level();
}

void end_sequence_ep2() {
    fade_to_black_speed_3();
    video_fill_screen_with_black();
    uint16 x = create_text_dialog_box(1, 0x18, 0x26, "", "Press ANY key.");
    display_dialog_text(x + 0x19, 0xf, "\xfb""021");
    fade_in_from_black_with_delay_3();
    display_dialog_text(x + 1, 7, "\xfc""003 Young Cosmo leaps ");
    display_dialog_text(x + 1, 9, "\xfc""003 through a small hole ");
    display_dialog_text(x + 1, 0xb, "\xfc""003 in the cave ceiling, ");
    display_dialog_text(x + 1, 0xd, "\xfc""003 and finally sees what ");
    display_dialog_text(x + 1, 0xf, "\xfc""003 he's searching for... ");
    wait_for_input(x + 0x23, 0x17);
    display_fullscreen_image(4);
    load_music(7);
    x = create_text_dialog_box(0x12, 5, 0x26, "", "");
    display_dialog_text(x + 1, 0x13, "\xfc""003 ...the city where his parents are ");
    display_dialog_text(x + 1, 0x14, "\xfc""003  held captive--undoubtedly being");
    display_dialog_text(x + 1, 0x15, "\xfc""003     readied for the big feast!");
    wait_for_input(0x25, 0x15);
    x = create_text_dialog_box(0x12, 5, 0x26, "", "");
    display_dialog_text(x + 1, 0x13, "\xfc""003    Cosmo knows what he must do.");
    display_dialog_text(x + 1, 0x14, "\xfc""003    Enter the city and save his ");
    display_dialog_text(x + 1, 0x15, "\xfc""003   parents before it's too late!");
    wait_for_input(0x25, 0x15);
    fade_to_white(7);
    video_fill_screen_with_black();
    x = create_text_dialog_box(6, 4, 0x18, "Thank you", " for playing!");
    fade_in_from_black_with_delay_3();
    wait_for_input(x + 0x15, 8);
    commercial_ordering_information_dialog();
}

void end_sequence_ep3() {
    load_music(8);
    fade_to_black_speed_3();
    video_fill_screen_with_black();
    uint16 x = create_text_dialog_box(1, 0x16, 0x26, "", "Press ANY key.");
    display_dialog_text(x + 1, 0x11, "\xfb""018");
    display_dialog_text(x + 0x11, 0x6, "The creature is");
    display_dialog_text(x + 0x11, 0x7, "finally defeated");
    display_dialog_text(x + 0x11, 0x8, "and flies away.");
    display_dialog_text(x + 0x11, 0x9, "Suddenly, a door");
    display_dialog_text(x + 0x11, 0xa, "opens and Cosmo");
    display_dialog_text(x + 0x11, 0xb, "enters slowly.");
    display_dialog_text(x + 0x11, 0xd, "A big, scary blue");
    display_dialog_text(x + 0x11, 0xe, "alien creature");
    display_dialog_text(x + 0x11, 0xf, "wraps his arm");
    display_dialog_text(x + 0x11, 0x10, "around Cosmo...");
    fade_in_from_black_with_delay_3();
    wait_for_input(x + 0x23, 0x15);
    fade_to_black_speed_3();

    x = create_text_dialog_box(1, 0x16, 0x26, "", "Press ANY key.");
    display_dialog_text(x + 1, 0x10, "\xfb""019");
    display_dialog_text(x + 10, 3, "\"Hi Cosmo,\" says the blue");
    display_dialog_text(x + 10, 4, "alien, \"I'm Zonk,\" and");
    display_dialog_text(x + 10, 5, "we've been looking all");
    display_dialog_text(x + 10, 6, "over the planet for you\"");
    display_dialog_text(x + 10, 8, "\"This is a very dangerous");
    display_dialog_text(x + 10, 9, "planet, and when we found");
    display_dialog_text(x + 10, 10, "your parents, we brought");
    display_dialog_text(x + 10, 0xb, "them here for safety.\"");
    display_dialog_text(x + 10, 0xd, "\"We have been looking for");
    display_dialog_text(x + 10, 0xe, "you all this time, but");
    display_dialog_text(x + 10, 0xf, "it looks like you did a");
    display_dialog_text(x + 10, 0x10, "better job finding us!\"");
    display_dialog_text(x + 10, 0x12, "\"Here, I got a surprise");
    display_dialog_text(x + 10, 0x13, "for you...\"");
    fade_in_from_black_with_delay_3();
    wait_for_input(x + 0x23, 0x15);
    fade_to_black_speed_3();

    x = create_text_dialog_box(1, 0x16, 0x26, "", "Press ANY key.");
    display_dialog_text(x + 0x1b, 0xe, "\xfb""020");
    display_dialog_text(x + 2, 7, "\"Mommy!  Daddy!\"");
    display_dialog_text(x + 2, 8, "Cosmo and his parents");
    display_dialog_text(x + 2, 9, "are finally reunited,");
    display_dialog_text(x + 2, 10, "and hugs are passed");
    display_dialog_text(x + 2, 0xb, "all around.");
    display_dialog_text(x + 2, 0xd, "Daddy explains that");
    display_dialog_text(x + 2, 0xe, "Zonk helped fix their");
    display_dialog_text(x + 2, 0xf, "ship, and they can");
    display_dialog_text(x + 2, 0x10, "resume their trip to");
    display_dialog_text(x + 2, 0x11, "Disney World.");
    fade_in_from_black_with_delay_3();
    wait_for_input(x + 0x23, 0x15);
    fade_to_black_speed_3();

    x = create_text_dialog_box(1, 0x16, 0x26, "", "Press ANY key.");
    display_dialog_text(x + 0x1b, 0x13, "\xfb""003");
    display_dialog_text(x + 1, 10, "\xfb""011");
    display_dialog_text(x + 0xc, 6, "After saying goodbye");
    display_dialog_text(x + 0xc, 7, "to Zonk,");
    display_dialog_text(x + 1, 0x11, "Cosmo and his family");
    display_dialog_text(x + 1, 0x12, "blast off toward earth...");
    fade_in_from_black_with_delay_3();
    wait_for_input(x + 0x23, 0x15);
    fade_to_black_speed_3();

    x = create_text_dialog_box(1, 0x16, 0x26, "", "Press ANY key.");
    display_dialog_text(x + 0xd, 0x13, "\xfb""012");
    display_dialog_text(x + 2, 5, "    ...and arrive just four");
    display_dialog_text(x + 2, 6, "     galactic hours later!!");
    display_dialog_text(x + 2, 7, "Using their inviso-cloak device,");
    display_dialog_text(x + 2, 8, " they park their ship on top of");
    display_dialog_text(x + 2, 9, "        Space Mountain.");
    fade_in_from_black_with_delay_3();
    wait_for_input(x + 0x23, 0x15);
    fade_to_black_speed_3();

    x = create_text_dialog_box(1, 0x16, 0x26, "", "Press ANY key.");
    display_dialog_text(x + 0xc, 0xc, "\xfb""013");
    display_dialog_text(x + 2, 0xf, "  Disney World has always been");
    display_dialog_text(x + 2, 0x10, "    a great place for aliens");
    display_dialog_text(x + 2, 0x11, "  to visit on their vacations!");
    fade_in_from_black_with_delay_3();
    wait_for_input(x + 0x23, 0x15);
    display_fullscreen_image(4);
    x = create_text_dialog_box(0, 3, 0x18, "WEEEEEEEE!", "");
    play_sfx(0x2a);
    cosmo_wait(200);
    load_music(0x12);
    wait_for_input(x + 0x15, 1);
    fade_to_white(4);
    x = create_text_dialog_box(0, 5, 0x18, "Cosmo has the best", "The End!");
    display_dialog_text(x + 1, 2, "birthday of his life.");
    fade_in_from_black_with_delay_3();
    cosmo_wait(100);
    wait_for_input(x + 0x15, 3);

    //congratulations page
    fade_to_black_speed_3();
    video_fill_screen_with_black();
    x = create_text_dialog_box(0, 0x17, 0x26, "CONGRATULATIONS!", "Press ANY key.");
    x += 2;
    display_dialog_text(x, 3, "You saved Cosmo's parents and");
    display_dialog_text(x, 4, "landed at Disney World for the");
    display_dialog_text(x, 5, "best birthday of your life.");
    display_dialog_text(x, 7, "After a great birthday on Earth,");
    display_dialog_text(x, 8, "you headed home and told all of");
    display_dialog_text(x, 9, "your friends about your amazing");
    display_dialog_text(x, 10, "adventure--no one believed you!");
    display_dialog_text(x, 0xc, "Maybe on your next adventure you");
    display_dialog_text(x, 0xd, "can take pictures!");
    display_dialog_text(x, 0xf, "Coming Dec. 92: Duke Nukum II --");
    display_dialog_text(x, 0x10, "The amazing sequel to the first");
    display_dialog_text(x, 0x11, "Nukum trilogy, in which Duke is");
    display_dialog_text(x, 0x12, "kidnapped by an alien race to");
    display_dialog_text(x, 0x13, "save them from termination...");
    fade_in_from_black(1);
    wait_for_input(x + 0x21, 0x15);

    commercial_ordering_information_dialog();
}

void end_sequence() {
    switch(get_episode_number()) {
        case 1 : end_sequence_ep1(); break;
        case 2 : end_sequence_ep2(); break;
        case 3 : end_sequence_ep3(); break;
        default : break;
    }
}

void display_ingame_hint_dialog() {
    if (selected_ingame_hint_dialog != -1) {
        ingame_hint_dialogs(selected_ingame_hint_dialog);
        selected_ingame_hint_dialog = -1;
    }
}

void show_ingame_hint_dialog(int dialog_num) {
    selected_ingame_hint_dialog = dialog_num;
}

void commercial_ordering_information_dialog() {
    fade_to_black_speed_3();
    video_fill_screen_with_black();
    int x = create_text_dialog_box(0,0x18,0x26,"Ordering Information", "Press ANY key.");
    display_dialog_text(x,4,"      COSMO\'S COSMIC ADVENTURE");
    display_dialog_text(x,6,"  This game IS commercial software.");
    display_dialog_text(x,8,"    This episode of Cosmo is NOT");
    display_dialog_text(x,9," available as shareware.  It is not");
    display_dialog_text(x,10,"  freeware, nor public domain.  It");
    display_dialog_text(x,0xb,"  is only available from Apogee or");
    display_dialog_text(x,0xc,"        authorized dealers.");
    display_dialog_text(x,0xe," If you are a registered player, we");
    display_dialog_text(x,0xf,"    thank you for your patronage.");
    display_dialog_text(x,0x11,"  Please report any illegal selling");
    display_dialog_text(x,0x12,"  and distribution of this game to");
    display_dialog_text(x,0x13,"  Apogee by calling 1-800-GAME123.");
    fade_in_from_black(1);
    wait_for_input(x + 0x23,0x16);
}

int cosmo_speaking_dialog() {
    int x = create_text_dialog_box(2,8,0x22,"", "Press a key to continue.");
    display_dialog_text(x + 0x1d,7,"\xfd""004");
    return x + 1;
}

int duke_speaking_dialog() {
    int x = create_text_dialog_box(2,8,0x22,"", "Press a key to continue.");
    display_dialog_text(x + 1,7,"\xfe""221003");
    return x + 4;
}

void cosmo_duke_dialog() {
    int x = cosmo_speaking_dialog();
    display_dialog_text(x,5,"\xfc""003  Yikes, who are you?");
    wait_for_input(x + 0x1b,8);

    x = duke_speaking_dialog();
    display_dialog_text(x, 4, "\xfc""003 I'm Duke Nukum, green");
    display_dialog_text(x, 5, "\xfc""003 alien dude.              ");
    wait_for_input(x + 0x1b,8);

    x = duke_speaking_dialog();
    display_dialog_text(x, 4, "\xfc""003 Until you rescued me, I");
    display_dialog_text(x, 5, "\xfc""003 was stopped cold by an");
    display_dialog_text(x, 6, "\xfc""003 alien invasion force!");
    wait_for_input(x + 0x1b,8);

    x = cosmo_speaking_dialog();
    display_dialog_text(x,4,"\xfc""003 Wow!  Can you help rescue ");
    display_dialog_text(x,5,"\xfc""003 my parents?");
    wait_for_input(x + 0x1b,8);

    x = duke_speaking_dialog();
    display_dialog_text(x, 4, "\xfc""003 Sorry, kid, I've got to");
    display_dialog_text(x, 5, "\xfc""003 save the galaxy...");
    wait_for_input(x + 0x1b,8);

    x = duke_speaking_dialog();
    display_dialog_text(x, 4, "\xfc""003 ...but, I can give you");
    display_dialog_text(x, 5, "\xfc""003 something that will help");
    display_dialog_text(x, 6, "\xfc""003 you out.");
    wait_for_input(x + 0x1b,8);

    x = cosmo_speaking_dialog();
    display_dialog_text(x,4,"\xfc""003 Thanks, Mr. Nukum, and");
    display_dialog_text(x,5,"\xfc""003 good luck on your mission.");
    wait_for_input(x + 0x1b,8);

    x = duke_speaking_dialog();
    display_dialog_text(x, 4, "\xfc""003 Just look for me in my");
    display_dialog_text(x, 5, "\xfc""003 next exciting adventure,");
    display_dialog_text(x, 6, "\xfc""003 Duke Nukum II!");
    wait_for_input(x + 0x1b,8);

    x = cosmo_speaking_dialog();
    display_dialog_text(x,5,"\xfc""003             Bye.");
    wait_for_input(x + 0x1b,8);

    x = duke_speaking_dialog();
    display_dialog_text(x, 4, "\xfc""003 See ya... and have those");
    display_dialog_text(x, 5, "\xfc""003 spots checked...!");
    wait_for_input(x + 0x1b,8);
}
