#include <ti/getcsc.h>
#include <graphx.h>
#include <time.h>
#include <keypadc.h>
#include <math.h>
#include <debug.h>
#include <ti/screen.h>

class Menu
{
private:
    /* data */
public:
    int amountOfOptions;
    menuItem *items[10];
    // Menu *directsTo[10]; //10 is max size
    // char *itemText[10];
    Tilemap *Map;
    int selectedItem; // 1 indexed
    int x;
    int y;
    Menu(Tilemap *menuMap, int menuOptionCount, menuItem *menuItems[], int selectedMenuItem);
    void draw();
    ~Menu();
};

Menu::Menu(Tilemap *menuMap, int menuOptionCount, menuItem *menuItems[], int selectedMenuItem)
{ // vector<char*> or vector<char> *....
    selectedItem = selectedMenuItem;
    Map = menuMap;
    amountOfOptions = menuOptionCount;

    for (int i = 0; i < amountOfOptions; i++)
    {
        items[i] = menuItems[i];
    }
}

void Menu::draw()
{

    gfx_Tilemap_NoClip(&Map->tilemap, 0, 0);
    gfx_SetTextBGColor(gfx_GetPixel(Map->tilemap.x_loc + TILE_WIDTH, Map->tilemap.y_loc + TILE_HEIGHT)); // set background color as menu bg color gfx background as pallete
    int textMargin = 15;
    for (int i = 0; i < amountOfOptions; i++)
    {
        if (i + 1 == selectedItem)
        {
            gfx_SetTextFGColor(0);//white if selected
        }
        else
        {
            gfx_SetTextFGColor(1);
        }
        gfx_PrintStringXY(items[i]->itemText, Map->tilemap.x_loc + TILE_WIDTH, Map->tilemap.y_loc + (textMargin * i) + TILE_HEIGHT);
    }
}

Menu::~Menu()
{
}
