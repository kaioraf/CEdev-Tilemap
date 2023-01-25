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
// extern unsigned char tilemap_map[];
extern unsigned char snowLevel_1_map[];
extern unsigned char mainMenu_map[];

/* Tilemap defines */
#define TILE_WIDTH 16
#define TILE_HEIGHT 16

#define snowLevel1_WIDTH 40
#define snowLevel1_HEIGHT 30

#define mainMenu_WIDTH 7
#define mainMenu_HEIGHT 9

#define snowLevel1_DRAW_WIDTH 21
#define snowLevel1_DRAW_HEIGHT 16

#define mainMenu_DRAW_WIDTH 7
#define mainMenu_DRAW_HEIGHT 9

#define snowLevel1_Y_OFFSET 16
#define snowLevel1_X_OFFSET 0

#define mainMenu_Y_OFFSET 32
#define mainMenu_X_OFFSET 0

#define mainMenuItemCount 3

#define PLAYER_START_X GFX_LCD_WIDTH  / 2
#define PLAYER_START_Y (GFX_LCD_HEIGHT - Player_height) / 2

gfx_UninitedSprite(playerBackground, Player_width, Player_height);
gfx_UninitedSprite(flippedPlayer, Player_width, Player_height);

class Animated
{
private:
    /* data */
public:
    int x, destX;
    int y, destY;
    int animationLength = 16;
    int animationSteps = 4;
    bool isMoving;
    void MoveDestination(int dX, int dY);
    void Animate();
    Animated(/* args */);
    ~Animated();
};


void Animated::MoveDestination(int dX, int dY)
{
    destX += dX;
    destY += dY;
    isMoving = dX || dY;
}

void Animated::Animate()
{
    int horizontalMove = destX - x;
    int verticalMove = destY - y;
    if (horizontalMove)
    {
        if (horizontalMove < 0)
        {
            x -= animationSteps;
        }
        else
        {
            x += animationSteps;
        }
    }
    if (verticalMove)
    {
        if (verticalMove < 0)
        {
            y -= animationSteps;
        }
        else
        {
            y += animationSteps;
        }
    }
    isMoving = horizontalMove || verticalMove;
}

Animated::Animated(/* args */)
{
}

Animated::~Animated()
{
}




class Sprite : public Animated
{
private:
    int oldX;
    int oldY;
    gfx_sprite_t *background;
    gfx_sprite_t *foreground;
    gfx_sprite_t *flippedForeground;

public:
    int speed;
    bool isFlipped;
    int width;
    int height;
    // float walkAnimationDuration;
    // int animationState;
    Sprite(gfx_sprite_t *Foreground, gfx_sprite_t *Background, int startX, int startY, int spriteWidth, int spriteHeight);
    // bool needsToRespawn();
    // void Respawn();
    // void moveDotDown();
    // void playAnimation();
    void FlipSprite();
    void Draw();
    ~Sprite();
};

Sprite::Sprite(gfx_sprite_t *Foreground, gfx_sprite_t *Background, int startX, int startY, int spriteWidth, int spriteHeight)
{
    x = destX = startX;
    y = destY = startY;
    width = spriteWidth;
    height = spriteHeight;
    speed = TILE_WIDTH;
    isFlipped = false;
    isMoving = false;
    oldX = x;
    oldY = y;
    foreground = Foreground;
    background = Background;
    flippedForeground = nullptr;
    // animationLength = 16;
    // animationSteps = 2; // divides into 16
    // walkAnimationDuration = 30; // time in ms
    // animationState = 0;
    /* Get the original background behind the sprite */
    gfx_GetSprite(background, x, y);
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
{ // no partial redraw
    /* Render the original background */
    gfx_Sprite(background, oldX, oldY);

    /* Get the background behind the sprite */
    gfx_GetSprite(background, x, y);

    /* Render the sprite */
    gfx_TransparentSprite(foreground, x, y);

    oldX = x;
    oldY = y;
}

class Tilemap : public Animated
{
private:
    /* data */
public:
    gfx_tilemap_t tilemap;
    // int animationState;
    // int animationLength;
    // int animationDuration;
    // int animationSteps;
    Tilemap(unsigned char currentTileMap[], gfx_sprite_t **tilesetTiles, uint8_t tileType, uint8_t tileSize, uint8_t drawWidth, uint8_t drawHeight, uint8_t tilemapHeight, uint16_t tilemapWidth, uint8_t xOffset, uint8_t yOffset);
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
    x = destX = 0;
    y = destY = 0;
    // animationState = 0;
    // animationLength = 16; 
    // animationSteps = 4;
    // animationDuration = 30;// length in millisecond
}


Tilemap::~Tilemap()
{
}

class Menu
{
private:
    /* data */
public:
    int amountOfOptions;
    Menu *directsTo[10]; //10 is max size
    char *itemText[10];
    Tilemap *Map;
    int selectedItem; // 1 indexed
    int x;
    int y;
    Menu(Tilemap *menuMap, int menuOptionCount, Menu *optionDirectsTo[], char *menuItemText[], int selectedMenuItem);
    void draw();
    ~Menu();
};

Menu::Menu(Tilemap *menuMap, int menuOptionCount, Menu *optionDirectsTo[], char *menuItemText[], int selectedMenuItem)
{ // vector<char*> or vector<char> *....
    selectedItem = selectedMenuItem;
    Map = menuMap;
    amountOfOptions = menuOptionCount;

    for (int i = 0; i < amountOfOptions; i++)
    {
        directsTo[i] = optionDirectsTo[i];
    }

    for (int i = 0; i < amountOfOptions; i++)
    {
        itemText[i] = menuItemText[i];
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
        gfx_PrintStringXY(itemText[i], Map->tilemap.x_loc + TILE_WIDTH, Map->tilemap.y_loc + (textMargin * i) + TILE_HEIGHT);
    }
}

Menu::~Menu()
{
}

bool isWalkable(int tileID)
{
    switch (tileID)
    {
    case 21:
    case 22:
        return true;
        break;
    
    default:
        return false;
        break;
    }
}

int ConvertSpritePosition(Tilemap *snowLevel_1, Sprite *PlayerSprite)
{
    // int tilemapX = PLAYER_START_X / TILE_WIDTH;
    // int tilemapY = PLAYER_START_Y / TILE_HEIGHT;
    // gfx_GetTile(snowLevel_1, );
    // gfx_TilePtrMapped();
    int tileAtSpritePos = gfx_GetTile(&snowLevel_1->tilemap, snowLevel_1->x + PlayerSprite->x, snowLevel_1->y + PlayerSprite->y - 16);
    return tileAtSpritePos;

}

void DrawStatusText(Tilemap *snowLevel_1, Sprite *PlayerSprite ,clock_t timeSincelastUpdate, int currentTile)
{
    uint8_t block_mapped, block_ptr;
    gfx_FillRectangle(0, 0, 320, 16);
    gfx_PrintStringXY("x:", 8, 4);
    gfx_PrintInt(snowLevel_1->x, 3);
    gfx_PrintString(" y:");
    gfx_PrintInt(snowLevel_1->y, 3);
    gfx_PrintString(" tile:");
    gfx_PrintInt(currentTile, 3);

    if (isWalkable(currentTile))
    {
        gfx_PrintInt(1, 1);
    }
    else
    {
        gfx_PrintInt(0, 1);
    }
    
    // gfx_PrintString(" block:");

    // /* Or use gfx_GetTileMapped() and gfx_GetTile() */
    // block_mapped = *gfx_TilePtrMapped(&snowLevel_1->tilemap, snowLevel_1->x / TILE_WIDTH, snowLevel_1->y / TILE_HEIGHT);
    // block_ptr = *gfx_TilePtr(&snowLevel_1->tilemap, snowLevel_1->y, snowLevel_1->y);

    // gfx_PrintUInt(block_mapped, 3);
    // gfx_PrintString("/");
    // gfx_PrintUInt(block_ptr, 3);
    gfx_PrintString("  ");
    gfx_PrintInt(CLOCKS_PER_SEC / timeSincelastUpdate, 3);
    gfx_PrintString(" x:");
    gfx_PrintInt(PlayerSprite->x, 3);
    gfx_PrintString(" y:");
    gfx_PrintInt(PlayerSprite->y, 3);
}

void update(Tilemap *snowLevel1, Sprite *PlayerSprite, clock_t timeSinceLastUpdate, int currentTile)
{
    snowLevel1->Animate();
    gfx_Tilemap(&snowLevel1->tilemap, snowLevel1->x, snowLevel1->y);
    PlayerSprite->Animate();
    PlayerSprite->Draw();

    DrawStatusText(snowLevel1, PlayerSprite ,timeSinceLastUpdate, currentTile);
    gfx_SwapDraw();

}

void menuUpdate(Menu *currentMenu[1])
{
    currentMenu[0]->draw();
    gfx_SwapDraw();
}

bool menuInput(Menu *currentMenu[1])
{
    uint8_t key = os_GetCSC();

    switch (key)
    {
    case sk_Down:
        if (currentMenu[0]->selectedItem < currentMenu[0]->amountOfOptions)
        {
            currentMenu[0]->selectedItem++; // move down the menu
        }
        break;
    case sk_Up:
        if (currentMenu[0]->selectedItem > 1)
        {
            currentMenu[0]->selectedItem--; // move up the menu
        }
        break;
    case sk_Mode:
        return false; // toggle menu state
        break;
    default:
        break;
    }
    return true;
}

void input(kb_key_t arrows, Tilemap *snowLevel_1, Sprite *PlayerSprite)
{
    int movementX, movementY;
    
    if (arrows && !PlayerSprite->isMoving && !snowLevel_1->isMoving)
    {
        if (arrows & kb_Down)
        {
            movementX = 0;
            movementY = 16;

            if (PlayerSprite->y < PLAYER_START_Y)
            {
                PlayerSprite->MoveDestination(movementX, movementY);
            }  
            else if (snowLevel_1->y < (snowLevel1_HEIGHT * TILE_HEIGHT) - (snowLevel1_DRAW_HEIGHT * TILE_HEIGHT))
            {
                snowLevel_1->MoveDestination(movementX, movementY);
            }
            else 
            {
                PlayerSprite->MoveDestination(movementX, movementY);
            }
        }
        else if (arrows & kb_Left)
        {
            movementX = -16;
            movementY = 0;
            if (!PlayerSprite->isFlipped)
            {
                PlayerSprite->FlipSprite();
            }
            else
            {
                if (PlayerSprite->x > PLAYER_START_X)
                {
                    PlayerSprite->MoveDestination(movementX, movementY);
                }
                else if (snowLevel_1->x)
                {
                    snowLevel_1->MoveDestination(movementX, movementY);
                }
                else
                {
                    PlayerSprite->MoveDestination(movementX, movementY);
                }
            }
        }
        else if (arrows & kb_Right)
        {
            movementX = 16;
            movementY = 0;
            if (PlayerSprite->isFlipped)
            {
                PlayerSprite->FlipSprite();
            }
            else
            {
                if (PlayerSprite->x < PLAYER_START_X)
                {
                    PlayerSprite->MoveDestination(movementX, movementY);
                }  
                else if (snowLevel_1->x < (snowLevel1_WIDTH * TILE_WIDTH) - (snowLevel1_DRAW_WIDTH * TILE_WIDTH))
                {
                    snowLevel_1->MoveDestination(movementX, movementY);
                }
                else 
                {
                    PlayerSprite->MoveDestination(movementX, movementY);
                }
            }
        }
        else if (arrows & kb_Up)
        {
            movementX = 0;
            movementY = -16;
            if (PlayerSprite->y > PLAYER_START_Y)
            {
                PlayerSprite->MoveDestination(movementX, movementY);
            }
            else if (snowLevel_1->y)
            {
                snowLevel_1->MoveDestination(movementX, movementY);
            }
            else
            {
                PlayerSprite->MoveDestination(movementX, movementY);
            }
        }
    }    
}


void GraphxSetup()
{
    gfx_Begin();
    gfx_SetClipRegion(0, 16, 320, 240);
    /* Set the palette */
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);
    gfx_SetColor(64);
    gfx_SetTextFGColor(1);
    gfx_SetTextBGColor(0);

    /* Draw to buffer to avoid tearing */
    gfx_SetDrawBuffer();
    return;
}

int main(void)
{
    clock_t clockTime = clock();
    clock_t lastUpdate = 0;
    clock_t timeSinceLastUpdate;

    GraphxSetup();

    Tilemap *snowLevel_1 = new Tilemap(snowLevel_1_map, snowSet1_tiles, gfx_tile_16_pixel, TILE_HEIGHT, snowLevel1_DRAW_WIDTH, snowLevel1_DRAW_HEIGHT, snowLevel1_HEIGHT, snowLevel1_WIDTH, snowLevel1_X_OFFSET, snowLevel1_Y_OFFSET);

    playerBackground->width = Player_width;
    playerBackground->height = Player_height;
    Sprite *PlayerSprite = new Sprite(Player, playerBackground, PLAYER_START_X, PLAYER_START_Y, Player_width, Player_height);

    Tilemap *mainMenuTilemap = new Tilemap(mainMenu_map, GUI_tiles, gfx_tile_16_pixel, TILE_HEIGHT, mainMenu_DRAW_WIDTH, mainMenu_DRAW_HEIGHT, mainMenu_HEIGHT, mainMenu_WIDTH, mainMenu_X_OFFSET, mainMenu_Y_OFFSET);

    int mainMenuSize = 3;
    Menu *mainMenuItemDirectsTo[3];
    char *mainMenuItems[3] = {"Options", "Continue", "Close Game"};
    int mainMenuSelectedItem = 1;
    Menu *mainMenu = new Menu(mainMenuTilemap, mainMenuSize, mainMenuItemDirectsTo, mainMenuItems, mainMenuSelectedItem);
    // mainMenu->draw();
    bool menuOpen = false;
    Menu *currentMenu[1];
    currentMenu[0] = mainMenu;

    int currentTile;
    kb_key_t arrows;
    do
    {
        clockTime = clock();
        timeSinceLastUpdate = clockTime - lastUpdate;

        kb_Scan();
        arrows = kb_Data[7];
        // input(arrows, snowLevel_1, PlayerSprite);
        // currentTile = ConvertSpritePosition(snowLevel_1, PlayerSprite);

        if (timeSinceLastUpdate > 1092) //update 30 times per second
        {
            // update(snowLevel_1 , PlayerSprite, timeSinceLastUpdate, currentTile);
            lastUpdate = clockTime;

            if (os_GetCSC() == sk_Mode) {menuOpen = true;}
            while (menuOpen)
            {
                menuOpen = menuInput(currentMenu);
                menuUpdate(currentMenu);
            }
        }

    } while (!kb_IsDown(kb_KeyEnter));
    gfx_End();
    return 0;
}


// int oldmain(void)
// {
//     // uint8_t key;
//     kb_key_t arrows;
//     // kb_key_t keyGroup1;

//     // make tileset tiles thing
    // playerBackground->width = Player_width;
    // playerBackground->height = Player_height;
//     /* Initialize graphics drawing */
//     gfx_Begin();
//     gfx_SetClipRegion(0, 16, 320, 240);
//     /* Set the palette */
//     gfx_SetPalette(global_palette, sizeof_global_palette, 0);
//     gfx_SetColor(0);
//     gfx_SetTextFGColor(1);
//     gfx_SetTextBGColor(0);

//     /* Draw to buffer to avoid tearing */
//     gfx_SetDrawBuffer();

//     /* Set monospace font with width of 8 */
//     gfx_SetMonospaceFont(8);

//     Tilemap *mainMenuTilemap = new Tilemap(mainMenu_map, GUI_tiles, gfx_tile_16_pixel, TILE_HEIGHT, mainMenu_DRAW_WIDTH, mainMenu_DRAW_HEIGHT, mainMenu_HEIGHT, mainMenu_WIDTH, mainMenu_X_OFFSET, mainMenu_Y_OFFSET);
//     Tilemap *snowLevel_1 = new Tilemap(tilemap_map, snowSet1_tiles, gfx_tile_16_pixel, TILE_HEIGHT, snowLevel1_DRAW_WIDTH, snowLevel1_DRAW_HEIGHT, snowLevel1_HEIGHT, snowLevel1_WIDTH, snowLevel1_X_OFFSET, snowLevel1_Y_OFFSET);
//     Sprite *PlayerSprite = new Sprite(Player, playerBackground, PLAYER_START_X, PLAYER_START_Y, Player_width, Player_height);
    
//     snowLevel_1->tilemapExternalXOffset = 0;
//     snowLevel_1->tilemapExternalYOffset = 0;

//     PlayerSprite->Draw();
//     unsigned long mainMenuSize = 3;
//     Menu *mainMenuItemDirectsTo[3];
//     char *mainMenuItems[3] = {"Options", "Continue", "Close Game"};
//     int mainMenuSelectedItem = 1;
//     Menu *mainMenu = new Menu(mainMenuTilemap, mainMenuSize, mainMenuItemDirectsTo, mainMenuItems, mainMenuSelectedItem);
//     // mainMenu->draw();
//     bool menuOpen = false;
//     Menu *currentMenu[1];
//     currentMenu[0] = mainMenu;

//     clock_t oldClockTime = 0;
//     clock_t newClockTime = clock();
//     unsigned long timeElapsed;
//     bool isModePressed, oldIsModePressed; // make booleans for a single keypress
//     bool isAnimatingTilemap, isAnimating = false;
//     int animationXChange, animationYChange;
//     int tilemapAnimationXChange, tilemapAnimationYChange;
//     clock_t lastUpdate = 0;
//     // int tilemapAnimationPosChange;
//     // int tilemapPos;
//     // char tilemapAnimationDirection;
//     bool print = false;
//     bool key, prevkey;

//     /* Wait for the enter key to quit */
//     do
//     {
//         oldClockTime = newClockTime;
//         newClockTime = clock();
//         timeElapsed = newClockTime - oldClockTime;
//         uint8_t block_mapped;
//         uint8_t block_ptr;

//         /* Get the key */
//         kb_Scan();
//         arrows = kb_Data[7];
//         // keyGroup1 = kb_Data[1];
//         // key = os_GetCSC();
//         /* Draw tilemap and coords */

//         /* Do something based on the keypress */
//         if (arrows && !isAnimating)
//         {
//             animationXChange = 0;
//             animationYChange = 0;
//             tilemapAnimationXChange = 0;
//             tilemapAnimationYChange = 0;
//             isAnimating = true;
//             // tilemapPos = 0;
//             if (arrows & kb_Down)
//             {
//                 animationXChange = 0;
//                 animationYChange = PlayerSprite->AnimationSteps;

        //         if (PlayerSprite->y < PLAYER_START_Y)
        //         {
        //             isAnimatingTilemap = false;
        //             // PlayerSprite->y = PlayerSprite->y + PlayerSprite->speed; //move the player if not in center of screen
        //             PlayerSprite->Animate(animationXChange, animationYChange);
        //         }
        //         else
        //         {
        //             if (snowLevel_1->tilemapExternalYOffset < (snowLevel1_HEIGHT * TILE_HEIGHT) - (snowLevel1_DRAW_HEIGHT * TILE_HEIGHT))
        //             {
        //                 // y_offset += TILE_HEIGHT; // move the screen if in center
        //                 isAnimatingTilemap = true;
        //                 snowLevel_1->Animate(animationXChange, animationYChange);
        //                 // tilemapAnimationPosChange = animationYChange;
        //                 // tilemapPos = snowLevel_1->tilemapExternalYOffset;
        //                 // tilemapAnimationDirection = 'y';
        //             }
        //             else
        //             {
        //                 isAnimatingTilemap = false;
        //                 // PlayerSprite->y = PlayerSprite->y + PlayerSprite->speed;
        //                 PlayerSprite->Animate(animationXChange, animationYChange);
        //             }
        //         }
        //     }
        //     else if (arrows & kb_Left)
        //     {
        //         animationXChange = -PlayerSprite->AnimationSteps;
        //         animationYChange = 0;
        //         if (!PlayerSprite->isFlipped)
        //         {
        //             PlayerSprite->FlipSprite();
        //         }
        //         else
        //         {
        //             if (PlayerSprite->x > PLAYER_START_X)
        //             {
        //                 isAnimatingTilemap = false;
        //                 // PlayerSprite->x = PlayerSprite->x - PlayerSprite->speed;
        //                 PlayerSprite->Animate(animationXChange, animationYChange);
        //             }
        //             else
        //             {
        //                 if (snowLevel_1->tilemapExternalXOffset)
        //                 {
        //                     // x_offset -= TILE_WIDTH;
        //                     isAnimatingTilemap = true;
        //                     snowLevel_1->Animate(animationXChange, animationYChange);
        //                     // tilemapAnimationPosChange = animationXChange;
        //                     // tilemapPos = snowLevel_1->tilemapExternalXOffset;
        //                     // tilemapAnimationDirection = 'x';
        //                 }
        //                 else
        //                 {
        //                     isAnimatingTilemap = false;
        //                     // PlayerSprite->x = PlayerSprite->x - PlayerSprite->speed;
        //                     PlayerSprite->Animate(animationXChange, animationYChange);
        //                 }
        //             }
        //         }
        //     }
        //     else if (arrows & kb_Right)
        //     {
        //         animationXChange = PlayerSprite->AnimationSteps;
        //         animationYChange = 0;

        //         if (PlayerSprite->isFlipped)
        //         {
        //             PlayerSprite->FlipSprite();
        //         }
        //         else
        //         {
        //             if (PlayerSprite->x > GFX_LCD_WIDTH - (TILE_WIDTH * 2))
        //             {
        //                 tilemapAnimationXChange = TILE_WIDTH;
        //                 tilemapAnimationYChange = 0;
        //                 isAnimatingTilemap = true;
        //                 snowLevel_1->animationState = 10;
        //             }
        //             else 
        //             {
        //                 isAnimatingTilemap = false;
        //                 PlayerSprite->Animate(animationXChange, animationYChange);
        //             }
        //                 // if (snowLevel_1->tilemapExternalXOffset < (snowLevel1_WIDTH * TILE_WIDTH) - (snowLevel1_DRAW_WIDTH * TILE_WIDTH))
        //                 // {
        //                 //     // x_offset += TILE_WIDTH;
        //                 //     isAnimatingTilemap = true;
        //                 //     snowLevel_1->Animate(animationXChange, animationYChange);
        //                 //     // tilemapAnimationPosChange = animationXChange;
        //                 //     // tilemapPos = snowLevel_1->tilemapExternalXOffset;
        //                 //     // tilemapAnimationDirection = 'x';
        //                 // }
        //                 // else
        //                 // {
        //                 //     isAnimatingTilemap = false;
        //                 //     PlayerSprite->Animate(animationXChange, animationYChange);
        //                 // }
        //         }
        //     }
        //     else if (arrows & kb_Up)
        //     {
        //         animationXChange = 0;
        //         animationYChange = -PlayerSprite->AnimationSteps;
        //         if (PlayerSprite->y > PLAYER_START_Y)
        //         {
        //             isAnimatingTilemap = false;
        //             PlayerSprite->Animate(animationXChange, animationYChange);
        //         }
        //         else
        //         {
        //             if (snowLevel_1->tilemapExternalYOffset)
        //             {
        //                 // y_offset -= TILE_HEIGHT;
        //                 isAnimatingTilemap = true;
        //                 snowLevel_1->Animate(animationXChange, animationYChange);
        //                 // tilemapAnimationPosChange = animationYChange;
        //                 // tilemapAnimationDirection = 'y';
        //                 // tilemapPos = snowLevel_1->tilemapExternalYOffset;
        //             }
        //             else
        //             {
        //                 isAnimatingTilemap = false;
        //                 PlayerSprite->Animate(animationXChange, animationYChange);
        //             }
        //         }
        //     }
        // }
//         if (isAnimating)
//         {
//             float frameRate = CLOCKS_PER_SEC / timeElapsed;
//             int stepCountInAnimation = snowLevel_1->animationLength  / snowLevel_1->animationSteps;
//             if (isAnimatingTilemap) // direction of movement decided in if thing?
//             {
//                 if (newClockTime - lastUpdate > 10000)
//                 {
//                     snowLevel_1->tilemapExternalXOffset += tilemapAnimationXChange;
//                     snowLevel_1->tilemapExternalYOffset += tilemapAnimationYChange;
//                     snowLevel_1->animationState --;
//                     gfx_Tilemap(&snowLevel_1->tilemap, snowLevel_1->tilemapExternalXOffset, snowLevel_1->tilemapExternalYOffset);
//                     gfx_SwapDraw();
//                     // if (frameRate > snowLevel_1->animationDuration / stepCountInAnimation) // also implement in playersprite at the bottom also delete the rest
//                     // {
//                         // snowLevel_1->Animate(animationXChange, animationYChange);
//                     // }
//                     if (snowLevel_1->animationState == 0) 
//                     {
//                         isAnimating = false; isAnimatingTilemap = false;
//                     }
//                     lastUpdate = clock();
//                     /* code */
//                 }
                
//             }
//             else //animate player
//             {
//                 if (frameRate > PlayerSprite->walkAnimationDuration / stepCountInAnimation)
//                 {
//                     PlayerSprite->Animate(animationXChange, animationYChange);
//                 }
//                 if (PlayerSprite->animationState == 0) {isAnimating = false;}
//             }
//             // gfx_Tilemap(&snowLevel_1->tilemap, snowLevel_1->tilemapExternalXOffset, snowLevel_1->tilemapExternalYOffset);
//         }
        

//         // isModePressed = kb_Data[1] == kb_Mode;

//         // if (isModePressed && !oldIsModePressed) // check for single keypress
//         // {
//         //     menuOpen = !menuOpen; // toggle menu state
//         // }
//         oldIsModePressed = isModePressed;

//         // gfx_Tilemap(&snowLevel_1->tilemap, snowLevel_1->tilemapExternalXOffset, snowLevel_1->tilemapExternalYOffset);
        // gfx_SetColor(gfx_GetPixel(mainMenuTilemap->tilemap.x_loc + TILE_WIDTH, mainMenuTilemap->tilemap.y_loc + TILE_HEIGHT));
//         gfx_FillRectangle(0, 0, 320, 16);
//         gfx_PrintStringXY("x:", 64, 4);
//         gfx_PrintInt(snowLevel_1->tilemapExternalXOffset, 3);
//         gfx_PrintString(" y:");
//         gfx_PrintInt(snowLevel_1->tilemapExternalYOffset, 3);
//         gfx_PrintString(" block:");

//         /* Or use gfx_GetTileMapped() and gfx_GetTile() */
//         block_mapped = *gfx_TilePtrMapped(&snowLevel_1->tilemap, snowLevel_1->tilemapExternalXOffset / TILE_WIDTH, snowLevel_1->tilemapExternalYOffset / TILE_HEIGHT);
//         block_ptr = *gfx_TilePtr(&snowLevel_1->tilemap, snowLevel_1->tilemapExternalXOffset, snowLevel_1->tilemapExternalYOffset);

//         gfx_PrintUInt(block_mapped, 3);
//         gfx_PrintString("/");
//         gfx_PrintUInt(block_ptr, 3);
//         gfx_PrintString("  ");
//         gfx_PrintInt(CLOCKS_PER_SEC / timeElapsed, 3);
        
//         PlayerSprite->Draw();
//         // dbg_printf("var clock old: %lu\n", oldClockTime);
//         // ("var clock new: %lu\n", newClockTime);
//         // gfx_SwapDraw();
//         // for (long long i = 0; i < 10; i++)
//         // {
//         //     gfx_FillRectangle(0, 0, 320, 16);
//         // }
//         // key = kb_Data[1] == kb_2nd;
//         // while(!key && prevkey) 
//         // {
//         //     kb_Scan();
//         //     key = kb_Data[1] == kb_2nd;
//         // }
//         // prevkey = key;



//         while (menuOpen)
//         {
//             timeElapsed = newClockTime - oldClockTime;
//             oldClockTime = newClockTime;
//             newClockTime = clock();

//             kb_Scan();
//             arrows = kb_Data[7];

//             if (arrows)
//             {
//                 /* Do different directions depending on the keypress */
//                 if (arrows & kb_Down)
//                 {
//                     if (currentMenu[0]->selectedItem < currentMenu[0]->amountOfOptions)
//                     {
//                         currentMenu[0]->selectedItem++; // move down the menu
//                     }
//                 }
//                 if (arrows & kb_Up)
//                 {
//                     if (currentMenu[0]->selectedItem > 1)
//                     {
//                         currentMenu[0]->selectedItem--; // move up the menu
//                     }
//                 }
//             }
//             isModePressed = kb_Data[1] == kb_Mode;

//             if (isModePressed && !oldIsModePressed) // check for single keypress
//             {
//                 menuOpen = !menuOpen; // toggle menu state
//             }
//             oldIsModePressed = isModePressed;

//             gfx_SetColor(gfx_GetPixel(mainMenuTilemap->tilemap.x_loc + TILE_WIDTH, mainMenuTilemap->tilemap.y_loc + TILE_HEIGHT));
//             gfx_FillRectangle(0, 0, 320, 16);
//             gfx_PrintStringXY("x:", 64, 4);
//             gfx_PrintInt(snowLevel_1->tilemapExternalXOffset, 3);
//             gfx_PrintString(" y:");
//             gfx_PrintInt(snowLevel_1->tilemapExternalYOffset, 3);
//             gfx_PrintString(" block:");
//             gfx_PrintUInt(block_mapped, 3);
//             gfx_PrintString("/");
//             gfx_PrintUInt(block_ptr, 3);
//             gfx_PrintString("  ");
//             gfx_PrintInt(CLOCKS_PER_SEC / timeElapsed, 3);
//             currentMenu[0]->draw();
//             gfx_SwapDraw();
//         }
//         kb_Scan();
//     } while (!kb_IsDown(kb_KeyEnter));

//     /* End graphics drawing */
//     gfx_End();

//     return 0;
// }
