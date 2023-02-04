#include <ti/getcsc.h>
#include <graphx.h>
#include <time.h>
#include <keypadc.h>
#include <math.h>
#include <debug.h>
#include "tilemap/menuItems.cpp"
#include <ti/screen.h>
// #include <sys/timers.h>

// #include "vector.h"
// #include <vector>
/* Include the converted graphics file */
#include "gfx/gfx.h"

/* Include the external tilemap data */
// extern unsigned char tilemap_map[];
extern unsigned char snowLevel_1_map[];
extern unsigned char mainMenu_map[];
extern unsigned char sword_map[];

/* Tilemap defines */
#define TILE_WIDTH 16
#define TILE_HEIGHT 16

#define swordTile_WIDTH 27
#define swordTile_HEIGHT 27

#define snowLevel1_WIDTH 40
#define snowLevel1_HEIGHT 30

#define mainMenu_WIDTH 7
#define mainMenu_HEIGHT 9

#define sword_WIDTH 6
#define sword_HEIGHT 2

#define snowLevel1_DRAW_WIDTH 21
#define snowLevel1_DRAW_HEIGHT 15

#define mainMenu_DRAW_WIDTH 7
#define mainMenu_DRAW_HEIGHT 9

#define sword_DRAW_WIDTH 1
#define sword_DRAW_HEIGHT 1

#define snowLevel1_Y_OFFSET 16
#define snowLevel1_X_OFFSET 0

#define mainMenu_Y_OFFSET 32
#define mainMenu_X_OFFSET 0

#define sword_Y_OFFSET 130
#define sword_X_OFFSET 130

#define optionsMenu_Y_OFFSET 32
#define optionsMenu_X_OFFSET (GFX_LCD_HEIGHT - mainMenu_WIDTH) / 2

#define mainMenuItemCount 3

#define PLAYER_START_X GFX_LCD_WIDTH  / 2
#define PLAYER_START_Y (GFX_LCD_HEIGHT - Player_height) / 2


// gfx_UninitedSprite(playerBackground, Player_width, Player_height);
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
    bool isDead;
    int score;
    clock_t timeAtCreation;
    // float walkAnimationDuration;
    // int animationState;
    Sprite(gfx_sprite_t *Foreground, int startX, int startY, int spriteWidth, int spriteHeight);
    // bool needsToRespawn();
    // void moveDotDown();
    // void playAnimation();
    void FlipSprite();
    void Draw();
    ~Sprite();
};

Sprite::Sprite(gfx_sprite_t *Foreground, int startX, int startY, int spriteWidth, int spriteHeight)
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
    flippedForeground = nullptr;
    isDead = false;
    score = 0;
    timeAtCreation = clock();
    // animationLength = 16;
    // animationSteps = 2; // divides into 16
    // walkAnimationDuration = 30; // time in ms
    // animationState = 0;
    /* Get the original background behind the sprite */
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
    // gfx_Sprite(background, oldX, oldY);

    // /* Get the background behind the sprite */
    // gfx_GetSprite(background, x, y);

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
    int animationState;
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
    animationState = 0;
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


bool isWalkable(int tileID)
{
    switch (tileID)
    {
    case 15:
    case 16:
    case 17:    
    case 18:
    case 19:
    case 22:
    case 23:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 41:
    case 42:
    case 43:
    case 45:
    case 46:
    case 47:
    case 48:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
    case 65:
    case 66:
    case 67:
    case 68:
    case 88:
    case 98:
    case 99:
    case 101:
    case 108:
    case 111:
    case 113:
    case 116:
    case 128:
    case 133:
        return true;
        break;
    default:
        return false;
        break;
    }
}

int ConvertSpritePosition(Tilemap *snowLevel_1, int playerX, int playerY)
{
    int tileAtSpritePos = gfx_GetTile(&snowLevel_1->tilemap, snowLevel_1->x + playerX - snowLevel1_X_OFFSET, snowLevel_1->y + playerY - snowLevel1_Y_OFFSET);
    return tileAtSpritePos;

}

void checkPlayerEnemy(Sprite *KnightSprite, Sprite *PlayerSprite)
{
    int playerTileX = (int)floor(PlayerSprite->x / TILE_WIDTH);
    int playerTileY = (int)floor(PlayerSprite->y / TILE_WIDTH);
    int knightTileX = (int)floor(KnightSprite->x / TILE_WIDTH);
    int knightTileY = (int)floor(KnightSprite->y / TILE_WIDTH);

    bool xMatches = playerTileX == knightTileX;
    bool yMatches = playerTileY == knightTileY;
    if (xMatches && yMatches)
    {
        PlayerSprite->isDead = true;
    }
}

bool isAliveFor60Seconds(Sprite *KnightSprite)
{
    clock_t timeAlive = clock() - KnightSprite->timeAtCreation;
    int secondsAlive = timeAlive / CLOCKS_PER_SEC;
    if (secondsAlive > 60)
    {
        return true;
    }
    return false;
}

bool needsToRespawn(Sprite* PlayerSprite, Sprite *KnightSprite, bool isFlipped)
{

    bool knightYMatches = PlayerSprite->y == KnightSprite->y || PlayerSprite->y == KnightSprite->y + 16;
    if (isAliveFor60Seconds(KnightSprite))
    {
        return true;
    }
    else if (isFlipped)
    {
        if (KnightSprite->x == PlayerSprite->destX - 16 && knightYMatches)
        {
            PlayerSprite->score ++;
            return true;
        }
    }
    else
    {
        if (KnightSprite->x == PlayerSprite->destX + 16 && knightYMatches)
        {
            PlayerSprite->score ++;
            return true;
        }
    }
    return false;
}

void Respawn(Tilemap *snowLevel_1, Sprite *knightSprite)
{
    bool foundRespawnPos = false;

    while (foundRespawnPos == false)
    {
        knightSprite->x = ((rand() % snowLevel1_WIDTH) * 16) - snowLevel_1->x;
        knightSprite->destX = knightSprite->x;
        // dbg_printf("%d", knightSprite->x);
        // dbg_printf("\n");
        knightSprite->y = ((rand() % snowLevel1_HEIGHT) * 16) - snowLevel_1->y;
        knightSprite->destY = knightSprite->y;
        knightSprite->timeAtCreation = clock();
        // dbg_printf("\n %d", knightSprite->y);
        // dbg_printf("\n");

        if (isWalkable(ConvertSpritePosition(snowLevel_1, knightSprite->x, knightSprite->y))) 
        {
            foundRespawnPos = true;
        }
    }
}

void DrawStatusText(Tilemap *snowLevel_1, Sprite *PlayerSprite, Sprite *KnightSprite ,clock_t timeSincelastUpdate)
{
    // uint8_t block_mapped, block_ptr;
    gfx_FillRectangle(0, 0, 320, 16);
    gfx_PrintStringXY("Score: ", 8, 4);
    gfx_PrintInt(PlayerSprite->score, 3);
    // gfx_PrintInt(snowLevel_1->x, 3);
    // gfx_PrintString(" y:");
    // gfx_PrintInt(snowLevel_1->y, 3);
    // gfx_PrintString(" tile:");
    
    // gfx_PrintString(" block:");

    // /* Or use gfx_GetTileMapped() and gfx_GetTile() */
    // block_mapped = *gfx_TilePtrMapped(&snowLevel_1->tilemap, snowLevel_1->x / TILE_WIDTH, snowLevel_1->y / TILE_HEIGHT);
    // block_ptr = *gfx_TilePtr(&snowLevel_1->tilemap, snowLevel_1->y, snowLevel_1->y);

    // gfx_PrintUInt(block_mapped, 3);
    // gfx_PrintUInt(block_ptr, 3);
    gfx_PrintString(" -- ");
    gfx_PrintInt(CLOCKS_PER_SEC / timeSincelastUpdate, 3);
    gfx_PrintString(" Fps");
    // gfx_PrintString(" x:");
    // gfx_PrintInt(PlayerSprite->x, 3);
    // gfx_PrintString(" y:");
    // gfx_PrintInt(PlayerSprite->y, 3);
    gfx_PrintString(" -- Press mode for menu");

    gfx_SetColor(14);
    gfx_Line(PlayerSprite->x, PlayerSprite->y, KnightSprite->x, KnightSprite->y);
}


void update(Tilemap *snowLevel1, Sprite *PlayerSprite, clock_t timeSinceLastUpdate, Tilemap *sword, Sprite *KnightSprite)
{
    snowLevel1->Animate();
    gfx_Tilemap(&snowLevel1->tilemap, snowLevel1->x, snowLevel1->y);
    if (needsToRespawn(PlayerSprite, KnightSprite, false)) Respawn(snowLevel1, KnightSprite);
    KnightSprite->Animate();
    KnightSprite->Draw();
    PlayerSprite->Animate();
    checkPlayerEnemy(KnightSprite, PlayerSprite);
    PlayerSprite->Draw();
    sword->Animate();
    if (sword->isMoving)
    {
        if (PlayerSprite->isFlipped)
        {
            if (needsToRespawn(PlayerSprite, KnightSprite, true)) Respawn(snowLevel1, KnightSprite);
            sword->tilemap.x_loc = PlayerSprite->x - 20;
        }
        else
        {
            if (needsToRespawn(PlayerSprite, KnightSprite, false)) Respawn(snowLevel1, KnightSprite);
            sword->tilemap.x_loc = PlayerSprite->x + 9;
        }
        sword->tilemap.y_loc = PlayerSprite->y - 11;
        gfx_TransparentTilemap(&sword->tilemap, sword->x, sword->y);
    }

    DrawStatusText(snowLevel1, PlayerSprite, KnightSprite ,timeSinceLastUpdate);
    gfx_SwapDraw();
}



void menuUpdate(Menu *currentMenu)
{
    currentMenu->draw();
    gfx_SwapDraw();
}

void setEnemyDirection(Sprite *KnightSprite, Tilemap *snowLevel_1)
{
    int knightDirection = rand() % 4;
    int moveX = 0;
    int moveY = 0;
    switch (knightDirection)
    {
    case 0: // up right down left, fall through if not walkable 
        moveX = 0;
        moveY = -16;
        if (isWalkable(ConvertSpritePosition(snowLevel_1, KnightSprite->x + moveX, KnightSprite->y + moveY)))
        {
            KnightSprite->MoveDestination(moveX, moveY);
            break;
        }
    case 1:
        moveX = 16;
        moveY = 0;
        if (isWalkable(ConvertSpritePosition(snowLevel_1, KnightSprite->x + moveX, KnightSprite->y + moveY)))
        {
            KnightSprite->MoveDestination(moveX, moveY);
            break;
        }
    case 2:
        moveX = 0;
        moveY = 16;
        if (isWalkable(ConvertSpritePosition(snowLevel_1, KnightSprite->x + moveX, KnightSprite->y + moveY)))
        {
            KnightSprite->MoveDestination(moveX, moveY);
            break;
        }
    case 3:
        moveX = -16;
        moveY = 0;
        if (isWalkable(ConvertSpritePosition(snowLevel_1, KnightSprite->x + moveX, KnightSprite->y + moveY)))
        {
            KnightSprite->MoveDestination(moveX, moveY);
            break;  
        }
    default:
        moveX = 0;
        moveY = 0;
        KnightSprite->MoveDestination(moveX, moveY);
        break;
    }
}

int menuInput(Menu *currentMenu)
{
    uint8_t key = os_GetCSC();
    dbg_printf("%d",currentMenu->selectedItem);

    switch (key)
    {
    case sk_Down:
        if (currentMenu->selectedItem < currentMenu->amountOfOptions)
        {
            currentMenu->selectedItem++; // move down the menu
        }
        break;
    case sk_Up:
        if (currentMenu->selectedItem > 1)
        {
            currentMenu->selectedItem--; // move up the menu
        }
        break;
    case sk_2nd:
        return currentMenu->items[currentMenu->selectedItem - 1]->id;
        break;
    case sk_Mode:
        return 0; // toggle menu state
        break;
    default:
        break;
    }
    return -1;//do nothing
}

void input(kb_key_t arrows, kb_key_t controlButtons, Tilemap *snowLevel_1, Sprite *PlayerSprite, Tilemap *Sword, Sprite *KnightSprite)
{
    bool animatingTilemap = false;
    bool tileIsWalkable = false;
    bool walking; // add sword input fix make error
    int tileToWalkTo;
    int movementX, movementY;
    
    if (arrows && !PlayerSprite->isMoving && !snowLevel_1->isMoving)
    {
        walking = true;
        if (arrows & kb_Down)
        {
            movementX = 0;
            movementY = 16;
            tileToWalkTo = ConvertSpritePosition(snowLevel_1, PlayerSprite->x + movementX, PlayerSprite->y + movementY);
            tileIsWalkable = isWalkable(tileToWalkTo) && PlayerSprite->y + movementY < GFX_LCD_HEIGHT;
            if (PlayerSprite->y < PLAYER_START_Y)
            {
                animatingTilemap = false;
            }  
            else if (snowLevel_1->y < (snowLevel1_HEIGHT * TILE_HEIGHT) - ((snowLevel1_DRAW_HEIGHT - 1) * TILE_HEIGHT)) // - 1 to move properly
            {
                animatingTilemap = true;
            }
            else 
            {
                animatingTilemap = false;
            }
        }
        else if (arrows & kb_Left)
        {
            movementX = -16;
            movementY = 0;
            tileToWalkTo = ConvertSpritePosition(snowLevel_1, PlayerSprite->x + movementX, PlayerSprite->y + movementY);
            tileIsWalkable = isWalkable(tileToWalkTo) && PlayerSprite->x + movementX >= 0;
                    dbg_printf("%d", snowLevel_1->x);
            if (!PlayerSprite->isFlipped)
            {
                PlayerSprite->FlipSprite();
                Sword->y = Sword->destY = swordTile_HEIGHT;
            }
            else
            {
                if (PlayerSprite->x > PLAYER_START_X)
                {
                    animatingTilemap = false;
                }
                else if (snowLevel_1->x > 0)
                {
                    animatingTilemap = true;
                }
                else
                {
                    animatingTilemap = false;
                }
            }
        }
        else if (arrows & kb_Right)
        {
            movementX = 16;
            movementY = 0;
            tileToWalkTo = ConvertSpritePosition(snowLevel_1, PlayerSprite->x + movementX, PlayerSprite->y + movementY);
            tileIsWalkable = isWalkable(tileToWalkTo) && PlayerSprite->x + movementX < GFX_LCD_WIDTH;
            if (PlayerSprite->isFlipped)
            {
                PlayerSprite->FlipSprite();
                Sword->y = Sword->destY = 0;
            }
            else
            {
                if (PlayerSprite->x < PLAYER_START_X)
                {
                    animatingTilemap = false;
                }  
                else if (snowLevel_1->x < (snowLevel1_WIDTH * TILE_WIDTH) - ((snowLevel1_DRAW_WIDTH - 1) * TILE_WIDTH)) // - 1 to move properly
                {
                    animatingTilemap = true;
                }
                else 
                {
                    animatingTilemap = false;
                }
            }
        }
        else if (arrows & kb_Up)
        {
            movementX = 0;
            movementY = -16;
            tileToWalkTo = ConvertSpritePosition(snowLevel_1, PlayerSprite->x + movementX, PlayerSprite->y + movementY);
            tileIsWalkable = isWalkable(tileToWalkTo) && PlayerSprite->y + movementY > 0;
            if (PlayerSprite->y > PLAYER_START_Y)
            {
                animatingTilemap = false;
            }
            else if (snowLevel_1->y > 0)
            {
                animatingTilemap = true;
            }
            else
            {
                animatingTilemap = false;
            }
        }

        if (tileIsWalkable && walking)
        {
            if (animatingTilemap)
            {
                snowLevel_1->MoveDestination(movementX, movementY);
                KnightSprite->MoveDestination(-movementX, -movementY);
            }
            else 
            {
                PlayerSprite->MoveDestination(movementX, movementY);
            }
        }
    } 
    // if player standing still effectivly and sword is not animating
    if (!Sword->isMoving)
    {
        if (controlButtons & kb_2nd)
        {
            Sword->x = 0;
            Sword->destX = swordTile_WIDTH * (sword_WIDTH - 1);
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
    bool stopGame = false;
    clock_t clockTime = clock();
    clock_t lastUpdate = 0;
    clock_t timeSinceLastUpdate;
    srand(time(NULL));
    rand();
    GraphxSetup();

    Tilemap *snowLevel_1 = new Tilemap(snowLevel_1_map, snowSet1_tiles, gfx_tile_16_pixel, TILE_HEIGHT, snowLevel1_DRAW_WIDTH, snowLevel1_DRAW_HEIGHT, snowLevel1_HEIGHT, snowLevel1_WIDTH, snowLevel1_X_OFFSET, snowLevel1_Y_OFFSET);
    Tilemap *sword = new Tilemap(sword_map, SwordMap_tiles, gfx_tile_no_pow2, swordTile_HEIGHT, sword_DRAW_WIDTH, sword_DRAW_HEIGHT, sword_HEIGHT, sword_WIDTH, sword_X_OFFSET, sword_Y_OFFSET);
    sword->animationSteps = swordTile_WIDTH;
    
    // knightEnemy->width = knightEnemy_width;
    // knightEnemy->height = knightEnemy_height;
    Sprite *KnightSprite = new Sprite(knightEnemy, 144, 128, knightEnemy_width, knightEnemy_height);

    // playerBackground->width = Player_width;
    // playerBackground->height = Player_height;
    Sprite *PlayerSprite = new Sprite(Player, PLAYER_START_X, PLAYER_START_Y, Player_width, Player_height);

    Tilemap *mainMenuTilemap = new Tilemap(mainMenu_map, GUI_tiles, gfx_tile_16_pixel, TILE_HEIGHT, mainMenu_DRAW_WIDTH, mainMenu_DRAW_HEIGHT, mainMenu_HEIGHT, mainMenu_WIDTH, mainMenu_X_OFFSET, mainMenu_Y_OFFSET);
    Tilemap *optionsMenuTilemap = new Tilemap(mainMenu_map, GUI_tiles, gfx_tile_16_pixel, TILE_HEIGHT, mainMenu_DRAW_WIDTH, mainMenu_DRAW_HEIGHT, mainMenu_HEIGHT, mainMenu_WIDTH, optionsMenu_X_OFFSET, optionsMenu_Y_OFFSET);
    menuItem *menuContinue = new menuItem(0, "Continue");
    menuItem *menuOptions = new menuItem(1, "Options");
    menuItem *menuClosegame = new menuItem(2, "Close game");

    int mainMenuSize = 3;
    menuItem *mainMenuItems[3] = {menuContinue, menuOptions, menuClosegame};
    int mainMenuSelectedItem = 1;
    Menu *mainMenu = new Menu(mainMenuTilemap, mainMenuSize, mainMenuItems, mainMenuSelectedItem);
    // mainMenu->draw();

    int optionsMenuSize = 1;
    menuItem *optionsMenuItems[1] = {menuContinue};
    int optionsSelectedItem = 1;
    Menu *optionsMenu = new Menu(optionsMenuTilemap, optionsMenuSize, optionsMenuItems, optionsSelectedItem); 

    int gameOverSize = 1;
    menuItem *gameOverItems[1] = {menuClosegame};
    int gameOverSelectedItem = 1;
    Menu *gameOverMenu = new Menu(optionsMenuTilemap, gameOverSize, gameOverItems, gameOverSelectedItem); 

    Menu *currentMenu;
    // Menu *currentMenu[1];
    currentMenu = mainMenu;
    bool menuOpen = false;

    // int currentTile;
    kb_key_t arrows;
    kb_key_t controlButtons;
    do
    {
        clockTime = clock();
        timeSinceLastUpdate = clockTime - lastUpdate;

        kb_Scan();
        arrows = kb_Data[7];
        controlButtons = kb_Data[1];
        // currentTile = ConvertSpritePosition(snowLevel_1, PlayerSprite->x, PlayerSprite->y);

        if (timeSinceLastUpdate > 1092) //update 30 times per second
        {
            if (!KnightSprite->isMoving) setEnemyDirection(KnightSprite, snowLevel_1);

            input(arrows, controlButtons, snowLevel_1, PlayerSprite, sword, KnightSprite);
            update(snowLevel_1 , PlayerSprite, timeSinceLastUpdate, sword, KnightSprite);
            lastUpdate = clockTime;
            if (PlayerSprite->isDead)
            {
                currentMenu = gameOverMenu;
                menuOpen = true;
            }
            if (os_GetCSC() == sk_Mode) 
            {
                menuOpen = true; 
                currentMenu = mainMenu;
            }
            while (menuOpen)
            {
                switch (menuInput(currentMenu))
                {
                case 0:
                    if (PlayerSprite->isDead) // don't exit gameover menu
                    {
                        break;
                    }
                    menuOpen = false;
                    break;
                case 1:
                    currentMenu = optionsMenu;
                    break;
                case 2:
                    stopGame = true;
                    menuOpen = false;
                    break;
                default:
                    break;
                }
                menuUpdate(currentMenu);
            }
        }
    } while (/*!kb_IsDown(kb_KeyEnter) ||*/ !stopGame);
    gfx_End();
    return 0;
}