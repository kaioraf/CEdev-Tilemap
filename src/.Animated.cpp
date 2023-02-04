#include <ti/getcsc.h>
#include <graphx.h>
#include <time.h>
#include <keypadc.h>
#include <math.h>
#include <debug.h>
#include <ti/screen.h>

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
    // int speed;
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
    // speed = TILE_WIDTH;
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