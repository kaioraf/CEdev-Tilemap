#include <ti/getcsc.h>
#include <graphx.h>
#include <time.h>
#include <keypadc.h>
#include <debug.h>
// #include <sys/timers.h>

// #include "vector.h"
// #include <vector>
/* Include the converted graphics file */
#include "gfx/gfx.h"

/* Include the external tilemap data */
extern unsigned char tilemap_map[];
extern unsigned char mainMenu_map[];

/* Tilemap defines */
#define TILE_WIDTH 16
#define TILE_HEIGHT 16

#define snowLevel1_WIDTH 30
#define snowLevel1_HEIGHT 20

#define mainMenu_WIDTH 7
#define mainMenu_HEIGHT 9

#define snowLevel1_DRAW_WIDTH 20
#define snowLevel1_DRAW_HEIGHT 14

#define mainMenu_DRAW_WIDTH 7
#define mainMenu_DRAW_HEIGHT 9

#define snowLevel1_Y_OFFSET 16
#define snowLevel1_X_OFFSET 0

#define mainMenu_Y_OFFSET 32
#define mainMenu_X_OFFSET 0

#define mainMenuItemCount 3

#define PLAYER_START_X ((GFX_LCD_WIDTH - Player_width) / 2)
#define PLAYER_START_Y ((GFX_LCD_HEIGHT - Player_height) / 2)

gfx_UninitedSprite(playerBackground, Player_width, Player_height);
gfx_UninitedSprite(flippedPlayer, Player_width, Player_height);

class Sprite
{
private:
    int oldX;
    int oldY;
    gfx_sprite_t *background;
    gfx_sprite_t *foreground;
    gfx_sprite_t *flippedForeground;

public:
    int x;
    int y;
    int speed;
    bool isFlipped;
    int width;
    int height;
    int walkAnimationLength;
    int walkAnimationSteps;
    float walkAnimationDuration;
    int animationState;
    Sprite(gfx_sprite_t *Foreground, gfx_sprite_t *Background, int startX, int startY, int spriteWidth, int spriteHeight);
    // bool needsToRespawn();
    // void Respawn();
    // void moveDotDown();
    // void playAnimation();
    int Animate(int xPosChange, int yPosChange);
    void FlipSprite();
    void Draw();
    ~Sprite();
};

Sprite::Sprite(gfx_sprite_t *Foreground, gfx_sprite_t *Background, int startX, int startY, int spriteWidth, int spriteHeight)
{
    x = startX;
    y = startY;
    width = spriteWidth;
    height = spriteHeight;
    speed = TILE_WIDTH;
    isFlipped = false;
    oldX = x;
    oldY = y;
    foreground = Foreground;
    background = Background;
    flippedForeground = nullptr;
    walkAnimationLength = 16;
    walkAnimationSteps = 4;
    walkAnimationDuration = 30; // time in ms
    animationState = 0;
    /* Get the original background behind the sprite */
    gfx_GetSprite(background, x, y);
}

// void playAnimation()
// {

// }

int Sprite::Animate(int xPosChange, int yPosChange)
{
    // 0 for up, 1 for right, 2 for down, 3 for left
    if (animationState < walkAnimationLength)
    {
        x += xPosChange;
        y += yPosChange;
        animationState += walkAnimationSteps;
        return 0; // animation still going for next frame
    }
    animationState = 0;
    return 1; // done animating
}

void Sprite::FlipSprite()
{
    if (flippedForeground == nullptr)
    {
        flippedForeground = flippedPlayer;
        gfx_FlipSpriteY(foreground, flippedForeground);
    }
    gfx_sprite_t *oldForeground = foreground;
    foreground = flippedForeground;
    flippedForeground = oldForeground;
    isFlipped = !isFlipped;
}

void Sprite::Draw()
{
    /* Render the original background */
    gfx_Sprite(background, oldX, oldY);

    /* Get the background behind the sprite */
    gfx_GetSprite(background, x, y);

    /* Render the sprite */
    gfx_TransparentSprite(foreground, x, y);

    oldX = x;
    oldY = y;
}

class Tilemap
{
private:
    /* data */
public:
    gfx_tilemap_t tilemap;
    int animationState;
    int animationLength;
    int animationDuration;
    int animationSteps;
    int tilemapExternalXOffset, tilemapExternalYOffset;
    Tilemap(unsigned char currentTileMap[], gfx_sprite_t **tilesetTiles, uint8_t tileType, uint8_t tileSize, uint8_t drawWidth, uint8_t drawHeight, uint8_t tilemapHeight, uint16_t tilemapWidth, uint8_t xOffset, uint8_t yOffset);
    void Animate(int offsetChange, int offset);
    ~Tilemap();
};

Tilemap::Tilemap(unsigned char currentTileMap[], gfx_sprite_t **tilesetTiles, uint8_t tileType, uint8_t tileSize, uint8_t drawWidth, uint8_t drawHeight, uint8_t tilemapHeight, uint16_t tilemapWidth, uint8_t xOffset, uint8_t yOffset)
{
    tilemap.map = currentTileMap;
    tilemap.tiles = tilesetTiles;
    tilemap.type_width = tileType;
    tilemap.type_height = tileType;
    tilemap.tile_height = tileSize;
    tilemap.tile_width = tileSize;
    tilemap.draw_height = drawHeight; 
    tilemap.draw_width = drawWidth;
    tilemap.height = tilemapHeight;
    tilemap.width = tilemapWidth;
    tilemap.x_loc = xOffset;
    tilemap.y_loc = yOffset;
    animationState = 0;
    animationLength = 16; 
    animationSteps = 4;
    animationDuration = 30;// length in millisecond
}

void Tilemap::Animate(int xPosChange, int yPosChange)
{

    if (animationState < animationLength)
    {
        tilemapExternalXOffset += xPosChange;
        tilemapExternalYOffset += yPosChange;
        animationState += animationSteps;
        return; // animation still going for next frame
    }
    animationState = 0;
    return; // done animating
}
    // if (animationState < animationLength)
    // {
    //     offset += offsetChange;
    //     animationState += animationSteps;
    // }
    // else
    // {
    //     animationState = 0; // done animating
    // }
    
    
    // return offset;


Tilemap::~Tilemap()
{
}

class Menu
{
private:
    /* data */
public:
    unsigned long amountOfOptions;
    // Vector<Menu*> directsTo;
    Menu *directsTo[10];
    // Vector<char*> itemText;
    char *itemText[10];
    Tilemap *Map;
    int selectedItem; // 1 indexed
    int x;
    int y;
    Menu(Tilemap *menuMap, unsigned long menuOptionCount, Menu *optionDirectsTo[], char *menuItemText[], int selectedMenuItem);
    void draw();
    ~Menu();
};

Menu::Menu(Tilemap *menuMap, unsigned long menuOptionCount, Menu *optionDirectsTo[], char *menuItemText[], int selectedMenuItem)
{ // vector<char*> or vector<char> *....
    selectedItem = selectedMenuItem;
    Map = menuMap;
    amountOfOptions = menuOptionCount;

    for (unsigned long i = 0; i < amountOfOptions; i++)
    {
        directsTo[i] = optionDirectsTo[i];
    }

    for (unsigned long i = 0; i < amountOfOptions; i++)
    {
        itemText[i] = menuItemText[i];
    }
}

void Menu::draw()
{

    gfx_Tilemap_NoClip(&Map->tilemap, 0, 0);
    gfx_SetTextBGColor(gfx_GetPixel(Map->tilemap.x_loc + TILE_WIDTH, Map->tilemap.y_loc + TILE_HEIGHT)); // set background color as menu bg color gfx background as pallete
    int textMargin = 15;
    for (unsigned long i = 0; i < amountOfOptions; i++)
    {
        gfx_PrintStringXY(itemText[i], Map->tilemap.x_loc + TILE_WIDTH, Map->tilemap.y_loc + (textMargin * i) + TILE_HEIGHT);
    }
}

Menu::~Menu()
{
}

// int* Animate(int xOffset, int yOffset, int x, int y)
// {
//     x += xOffset;
//     y += yOffset;

//     int posVector[2] = {x, y};
//     return posVector;
// }


int main(void)
{
    // uint8_t key;
    kb_key_t arrows;
    // kb_key_t keyGroup1;

    // make tileset tiles thing
    playerBackground->width = Player_width;
    playerBackground->height = Player_height;
    /* Initialize graphics drawing */
    gfx_Begin();

    /* Set the palette */
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);
    gfx_SetColor(0);
    gfx_SetTextFGColor(1);
    gfx_SetTextBGColor(0);

    /* Draw to buffer to avoid tearing */
    gfx_SetDrawBuffer();

    /* Set monospace font with width of 8 */
    gfx_SetMonospaceFont(8);

    Tilemap *mainMenuTilemap = new Tilemap(mainMenu_map, GUI_tiles, gfx_tile_16_pixel, TILE_HEIGHT, mainMenu_DRAW_WIDTH, mainMenu_DRAW_HEIGHT, mainMenu_HEIGHT, mainMenu_WIDTH, mainMenu_X_OFFSET, mainMenu_Y_OFFSET);
    Tilemap *snowLevel_1 = new Tilemap(tilemap_map, snowSet1_tiles, gfx_tile_16_pixel, TILE_HEIGHT, snowLevel1_DRAW_WIDTH, snowLevel1_DRAW_HEIGHT, snowLevel1_HEIGHT, snowLevel1_WIDTH, snowLevel1_X_OFFSET, snowLevel1_Y_OFFSET);
    Sprite *PlayerSprite = new Sprite(Player, playerBackground, PLAYER_START_X, PLAYER_START_Y, Player_width, Player_height);
    
    snowLevel_1->tilemapExternalXOffset = 5;
    snowLevel_1->tilemapExternalYOffset = 0;

    PlayerSprite->Draw();
    unsigned long mainMenuSize = 3;
    Menu *mainMenuItemDirectsTo[3];
    char *mainMenuItems[3] = {"Options", "Continue", "Close Game"};
    int mainMenuSelectedItem = 1;
    Menu *mainMenu = new Menu(mainMenuTilemap, mainMenuSize, mainMenuItemDirectsTo, mainMenuItems, mainMenuSelectedItem);
    mainMenu->draw();
    bool menuOpen = false;
    Menu *currentMenu[1];
    currentMenu[0] = mainMenu;

    clock_t oldClockTime = 0;
    clock_t newClockTime = clock();
    unsigned long timeElapsed;
    bool isModePressed, oldIsModePressed; // make booleans for a single keypress
    bool isAnimatingTilemap, isAnimating = false;
    int animationXChange, animationYChange;
    // int tilemapAnimationPosChange;
    // int tilemapPos;
    // char tilemapAnimationDirection;
    bool print = false;
    bool key, prevkey;

    /* Wait for the enter key to quit */
    do
    {
        timeElapsed = newClockTime - oldClockTime;
        oldClockTime = newClockTime;
        newClockTime = clock();
        uint8_t block_mapped;
        uint8_t block_ptr;

        /* Get the key */
        kb_Scan();
        arrows = kb_Data[7];
        // keyGroup1 = kb_Data[1];
        // key = os_GetCSC();
        /* Draw tilemap and coords */

        /* Do something based on the keypress */
        if (arrows && !isAnimating)
        {
            animationXChange = 0;
            animationYChange = 0;
            isAnimating = true;
            // tilemapPos = 0;
            if (arrows & kb_Down)
            {
                animationXChange = 0;
                animationYChange = PlayerSprite->walkAnimationSteps;

                if (PlayerSprite->y < PLAYER_START_Y)
                {
                    isAnimatingTilemap = false;
                    // PlayerSprite->y = PlayerSprite->y + PlayerSprite->speed; //move the player if not in center of screen
                    PlayerSprite->Animate(animationXChange, animationYChange);
                }
                else
                {
                    if (snowLevel_1->tilemapExternalYOffset < (snowLevel1_HEIGHT * TILE_HEIGHT) - (snowLevel1_DRAW_HEIGHT * TILE_HEIGHT))
                    {
                        // y_offset += TILE_HEIGHT; // move the screen if in center
                        isAnimatingTilemap = true;
                        snowLevel_1->Animate(animationXChange, animationYChange);
                        // tilemapAnimationPosChange = animationYChange;
                        // tilemapPos = snowLevel_1->tilemapExternalYOffset;
                        // tilemapAnimationDirection = 'y';
                    }
                    else
                    {
                        isAnimatingTilemap = false;
                        // PlayerSprite->y = PlayerSprite->y + PlayerSprite->speed;
                        PlayerSprite->Animate(animationXChange, animationYChange);
                    }
                }
            }
            else if (arrows & kb_Left)
            {
                animationXChange = -PlayerSprite->walkAnimationSteps;
                animationYChange = 0;
                if (!PlayerSprite->isFlipped)
                {
                    PlayerSprite->FlipSprite();
                }
                else
                {
                    if (PlayerSprite->x > PLAYER_START_X)
                    {
                        isAnimatingTilemap = false;
                        // PlayerSprite->x = PlayerSprite->x - PlayerSprite->speed;
                        PlayerSprite->Animate(animationXChange, animationYChange);
                    }
                    else
                    {
                        if (snowLevel_1->tilemapExternalXOffset)
                        {
                            // x_offset -= TILE_WIDTH;
                            isAnimatingTilemap = true;
                            snowLevel_1->Animate(animationXChange, animationYChange);
                            // tilemapAnimationPosChange = animationXChange;
                            // tilemapPos = snowLevel_1->tilemapExternalXOffset;
                            // tilemapAnimationDirection = 'x';
                        }
                        else
                        {
                            isAnimatingTilemap = false;
                            // PlayerSprite->x = PlayerSprite->x - PlayerSprite->speed;
                            PlayerSprite->Animate(animationXChange, animationYChange);
                        }
                    }
                }
            }
            else if (arrows & kb_Right)
            {
                animationXChange = PlayerSprite->walkAnimationSteps;
                animationYChange = 0;
                if (PlayerSprite->isFlipped)
                {
                    PlayerSprite->FlipSprite();
                }
                else
                {
                    if (PlayerSprite->x < PLAYER_START_X)
                    {
                        isAnimatingTilemap = false;
                        PlayerSprite->Animate(animationXChange, animationYChange);
                    }
                    else
                    {
                        if (snowLevel_1->tilemapExternalXOffset < (snowLevel1_WIDTH * TILE_WIDTH) - (snowLevel1_DRAW_WIDTH * TILE_WIDTH))
                        {
                            // x_offset += TILE_WIDTH;
                            isAnimatingTilemap = true;
                            snowLevel_1->Animate(animationXChange, animationYChange);
                            // tilemapAnimationPosChange = animationXChange;
                            // tilemapPos = snowLevel_1->tilemapExternalXOffset;
                            // tilemapAnimationDirection = 'x';
                        }
                        else
                        {
                            isAnimatingTilemap = false;
                            PlayerSprite->Animate(animationXChange, animationYChange);
                        }
                    }
                }
            }
            else if (arrows & kb_Up)
            {
                animationXChange = 0;
                animationYChange = -PlayerSprite->walkAnimationSteps;
                if (PlayerSprite->y > PLAYER_START_Y)
                {
                    isAnimatingTilemap = false;
                    PlayerSprite->Animate(animationXChange, animationYChange);
                }
                else
                {
                    if (snowLevel_1->tilemapExternalYOffset)
                    {
                        // y_offset -= TILE_HEIGHT;
                        isAnimatingTilemap = true;
                        snowLevel_1->Animate(animationXChange, animationYChange);
                        // tilemapAnimationPosChange = animationYChange;
                        // tilemapAnimationDirection = 'y';
                        // tilemapPos = snowLevel_1->tilemapExternalYOffset;
                    }
                    else
                    {
                        isAnimatingTilemap = false;
                        PlayerSprite->Animate(animationXChange, animationYChange);
                    }
                }
            }
        }
        else if (isAnimatingTilemap && isAnimating) // direction of movement decided in if thing?
        {
            if (CLOCKS_PER_SEC / (newClockTime - oldClockTime) > snowLevel_1->animationDuration / (snowLevel_1->animationLength / snowLevel_1->animationSteps)) // also implement in playersprite at the bottom also delete the rest
            {
                snowLevel_1->Animate(animationXChange, animationYChange);
            }
            if (snowLevel_1->animationState == 0) {isAnimating = false;}


            // tilemapPos = snowLevel_1->Animate(tilemapAnimationPosChange, tilemapPos);
            // switch (tilemapAnimationDirection)
            // {
            // case 'y':
            //     /* code */
            //     snowLevel_1->tilemapExternalYOffset = tilemapPos;
            //     break;
            // case 'x':
            //     snowLevel_1->tilemapExternalXOffset = tilemapPos;
            //     break;
            // } // draw player as relative to tilemap so that animate can also work with tilemap animation at the same time
            // gfx_Tilemap(&snowLevel_1->tilemap, snowLevel_1->tilemapExternalXOffset, snowLevel_1->tilemapExternalYOffset);
        }
        else if (isAnimating)
        {
            if (CLOCKS_PER_SEC / (newClockTime - oldClockTime) > PlayerSprite->walkAnimationDuration / (PlayerSprite->walkAnimationLength / PlayerSprite->walkAnimationSteps))
            {
                PlayerSprite->Animate(animationXChange, animationYChange);
            }
            if (PlayerSprite->animationState == 0) {isAnimating = false;}
        }

        isModePressed = kb_Data[1] == kb_Mode;

        if (isModePressed && !oldIsModePressed) // check for single keypress
        {
            menuOpen = !menuOpen; // toggle menu state
        }
        oldIsModePressed = isModePressed;

        gfx_Tilemap(&snowLevel_1->tilemap, snowLevel_1->tilemapExternalXOffset, snowLevel_1->tilemapExternalYOffset);
        // gfx_Tilemap_NoClip(&mainMenuTilemap->tilemap, 0, 0);
        gfx_SetColor(gfx_GetPixel(mainMenuTilemap->tilemap.x_loc + TILE_WIDTH, mainMenuTilemap->tilemap.y_loc + TILE_HEIGHT));
        gfx_FillRectangle(0, 0, 320, 16);
            if (print)
            {
                gfx_PrintInt(6 ,1);
            }
        gfx_PrintStringXY("x:", 64, 4);
        gfx_PrintInt(snowLevel_1->tilemapExternalXOffset, 3);
        gfx_PrintString(" y:");
        gfx_PrintInt(snowLevel_1->tilemapExternalYOffset, 3);
        gfx_PrintString(" block:");

        /* Or use gfx_GetTileMapped() and gfx_GetTile() */
        block_mapped = *gfx_TilePtrMapped(&snowLevel_1->tilemap, snowLevel_1->tilemapExternalXOffset / TILE_WIDTH, snowLevel_1->tilemapExternalYOffset / TILE_HEIGHT);
        block_ptr = *gfx_TilePtr(&snowLevel_1->tilemap, snowLevel_1->tilemapExternalXOffset, snowLevel_1->tilemapExternalYOffset);

        gfx_PrintUInt(block_mapped, 3);
        gfx_PrintString("/");
        gfx_PrintUInt(block_ptr, 3);
        gfx_PrintString("  ");
        gfx_PrintInt(CLOCKS_PER_SEC / timeElapsed, 3);
        
        // dbg_printf("var clock old: %lu\n", oldClockTime);
        // ("var clock new: %lu\n", newClockTime);
        PlayerSprite->Draw();
        gfx_SwapDraw();
        // for (long long i = 0; i < 10; i++)
        // {
        //     gfx_FillRectangle(0, 0, 320, 16);
        // }
        // key = kb_Data[1] == kb_2nd;
        // while(!key && prevkey) 
        // {
        //     kb_Scan();
        //     key = kb_Data[1] == kb_2nd;
        // }
        // prevkey = key;



        while (menuOpen)
        {
            timeElapsed = newClockTime - oldClockTime;
            oldClockTime = newClockTime;
            newClockTime = clock();

            kb_Scan();
            arrows = kb_Data[7];

            if (arrows)
            {
                /* Do different directions depending on the keypress */
                if (arrows & kb_Down)
                {
                    if (currentMenu[0]->selectedItem < currentMenu[0]->amountOfOptions)
                    {
                        currentMenu[0]->selectedItem++; // move down the menu
                    }
                }
                if (arrows & kb_Up)
                {
                    if (currentMenu[0]->selectedItem > 1)
                    {
                        currentMenu[0]->selectedItem--; // move up the menu
                    }
                }
            }
            isModePressed = kb_Data[1] == kb_Mode;

            if (isModePressed && !oldIsModePressed) // check for single keypress
            {
                menuOpen = !menuOpen; // toggle menu state
            }
            oldIsModePressed = isModePressed;

            gfx_SetColor(gfx_GetPixel(mainMenuTilemap->tilemap.x_loc + TILE_WIDTH, mainMenuTilemap->tilemap.y_loc + TILE_HEIGHT));
            gfx_FillRectangle(0, 0, 320, 16);
            gfx_PrintStringXY("x:", 64, 4);
            gfx_PrintInt(snowLevel_1->tilemapExternalXOffset, 3);
            gfx_PrintString(" y:");
            gfx_PrintInt(snowLevel_1->tilemapExternalYOffset, 3);
            gfx_PrintString(" block:");
            gfx_PrintUInt(block_mapped, 3);
            gfx_PrintString("/");
            gfx_PrintUInt(block_ptr, 3);
            gfx_PrintString("  ");
            gfx_PrintInt(CLOCKS_PER_SEC / timeElapsed, 3);
            currentMenu[0]->draw();
            gfx_SwapDraw();
        }
        kb_Scan();
    } while (!kb_IsDown(kb_KeyEnter));

    /* End graphics drawing */
    gfx_End();

    return 0;
}
