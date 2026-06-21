#include "LevelA.h"

LevelA::LevelA()                                      : Scene{ {},     nullptr   } {}
LevelA::LevelA(Vector2 origin, const char *bgHexCode) : Scene{ origin, bgHexCode } {}

LevelA::~LevelA() { shutdown(); }

void LevelA::initialise()
{
    mGameState.nextSceneID = 5; // placeholder 



    mGameState.jumpSound = LoadSound("assets/game/Dirt Jump.wav");
    mGameState.buzzing = LoadSound("assets/game/Bee Buzzing - Nature Sound Effects HD - Daily Moving Videos - DMV (128k).mp3");
    SetSoundVolume(mGameState.buzzing, 0.1f);

    /* ----------- MAP ----------- */
    mGameState.map = new Map(
        LEVEL_WIDTH, LEVEL_HEIGHT,   // map grid cols & rows
        (unsigned int *) mLevelData, // grid data
        "assets/game/tileset.png",   // texture filepath
        TILE_DIMENSION,              // tile size
        4, 1,                        // texture cols & rows
        mOrigin                      // in-game origin
    );

    /* ----------- PROTAGONIST ----------- */
    std::map<Direction, std::vector<int>> xochitlAnimationAtlas = {
        { DOWN,  {  0,1,2,3,4,5,6,7,8,9,10,11  }},
        { LEFT,  {  0,1,2,3,4,5,6,7,8,9,10,11  }},
        { UP,    {  0,1,2,3,4,5,6,7,8,9,10,11  }},
        { RIGHT, {  0,1,2,3,4,5,6,7,8,9,10,11  }},
    };

    float sizeRatio = 48.0f / 64.0f;

    mGameState.xochitl = new Entity(
        { mOrigin.x - 300.0f, mOrigin.y - 200.0f }, // position
        { 110.0f * sizeRatio, 100.0f },              // scale
        "assets/game/bb-sheet.png",                      // texture file address
        ATLAS,                                       // single image or atlas?
        { 16, 12 },                                    // atlas dimensions
        xochitlAnimationAtlas,                       // actual atlas
        PLAYER                                       // entity type
    );

    mGameState.xochitl->setJumpingPower(750.0f);
    mGameState.xochitl->setColliderDimensions({
        mGameState.xochitl->getScale().x / 2.5f,
        mGameState.xochitl->getScale().y / 2.0f
    });
    mGameState.xochitl->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });

    /*
        ----------- sun -----------
    */
    // std::map<Direction, std::vector<int>> sunAnimationAtlas = {
    //     {LEFT,  { 1, 9, 17, 25 }},
    //     {RIGHT, { 0, 8, 16, 24 }},
    // };

    // //     Entity(Vector2 position, Vector2 scale, const char *textureFilepath, EntityType entityType);
    mGameState.sun = new Entity(
        {mOrigin.x, mOrigin.y}, // position
        {100.0f, 100.0f},                        // scale
        "assets/game/toppng.com-sun-transparent-background-451x476 (1).png",                 // texture file address
        NPC                                      // entity type
    );

    mGameState.sun->setAIType(LERPER);
    mGameState.sun->setAIState(IDLE);

    mGameState.sun->setLerpFactor(0.2);

    mGameState.sun->setColliderDimensions({
        mGameState.sun->getScale().x / 1.1f,
        mGameState.sun->getScale().y / 1.1f
    });
    
}

void LevelA::update(float deltaTime)
{
    

    mGameState.xochitl->update(
        deltaTime,      // delta time / fixed timestep
        mGameState.xochitl,        // player
        mGameState.map, // map
        nullptr,        // collidable entities
        0               // col. entity count
    );

    mGameState.sun->update(deltaTime, mGameState.xochitl, nullptr,
            nullptr, 0);

    if (mGameState.xochitl->getPosition().y > END_GAME_THRESHOLD) mGameState.nextSceneID = 1;

    if (mGameState.sun->checkCollision(mGameState.xochitl)) {
        lives--;

        if (lives == 0) mGameState.nextSceneID=3;
        else mGameState.nextSceneID=0; 
    }
}

void LevelA::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.xochitl->render();
    
    mGameState.sun->render();


    mGameState.map->render();
    
}

void LevelA::shutdown()
{
    delete mGameState.xochitl;
    mGameState.xochitl = nullptr;
    delete mGameState.map;
    mGameState.map = nullptr;

    delete mGameState.sun;
    mGameState.sun = nullptr;


    UnloadSound(mGameState.jumpSound);
    UnloadSound(mGameState.buzzing);
    mGameState.jumpSound = {};
}
