#include "LevelB.h"

LevelB::LevelB()                                      : Scene{ {},     nullptr   } {}
LevelB::LevelB(Vector2 origin, const char *bgHexCode) : Scene{ origin, bgHexCode } {}

LevelB::~LevelB() { shutdown(); }

void LevelB::initialise()
{   

    mGameState.nextSceneID = 5;
    


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

    // fake bee


        /*
        ----------- fakeBee -----------
    */
    // std::map<Direction, std::vector<int>> fakeBeeAnimationAtlas = {
    //     {LEFT,  { 1, 9, 17, 25 }},
    //     {RIGHT, { 0, 8, 16, 24 }},
    // };

    // //     Entity(Vector2 position, Vector2 scale, const char *textureFilepath, EntityType entityType);

    std::map<Direction, std::vector<int>> fakeBeeAnimationAtlas = {
        { DOWN,  {  0,  1,  2,  3}},
        {UP,  {  0,  1,  2,  3} },
        {LEFT, {  0,  1,  2,  3} },
        {RIGHT, {  0,  1,  2,  3} }
    } ;

    mGameState.fakeBee = new Entity(
        {mOrigin.x + 100.0f, mOrigin.y }, // position
        {100.0f, 100.0f},              // scale
        "assets/game/knight.png",                      // texture file address
        ATLAS,                                       // single image or atlas?
        {8 ,8},                                    // atlas dimensions
        fakeBeeAnimationAtlas,                       // actual atlas
        NPC                                       // entity type
    );

    mGameState.fakeBee->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY }); 


    mGameState.fakeBee->setAIType(Patroller);
    mGameState.fakeBee->setAIState(IDLE);

    mGameState.fakeBee->setSpeed(75);

    mGameState.fakeBee->setColliderDimensions({
        mGameState.fakeBee->getScale().x /2,
        mGameState.fakeBee->getScale().y - 30
    });

    mGameState.fakeBee->moveLeft();

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

void LevelB::update(float deltaTime)
{

    mGameState.xochitl->update(
        deltaTime,      // delta time / fixed timestep
        nullptr,        // player
        mGameState.map, // map
        nullptr,        // collidable entities
        0               // col. entity count
    );

    mGameState.fakeBee->update(deltaTime, mGameState.xochitl, mGameState.map,
            nullptr, 0);
    
    mGameState.sun->update(deltaTime, mGameState.xochitl, nullptr,
            nullptr, 0);

    if (mGameState.fakeBee->checkCollision(mGameState.xochitl)) {
        lives--;
        if (lives == 0) mGameState.nextSceneID=3;
        else mGameState.nextSceneID=1; 
    }

    if (mGameState.sun->checkCollision(mGameState.xochitl)) {
        lives--;

        if (lives == 0) mGameState.nextSceneID=3;
        else mGameState.nextSceneID=1; 
    }

    if (mGameState.xochitl->getPosition().y > END_GAME_THRESHOLD) mGameState.nextSceneID = 2;

    
}

void LevelB::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.map->render();

    mGameState.xochitl->render();
    mGameState.fakeBee->render();
    mGameState.sun->render();
    
    

    
}

void LevelB::shutdown()

{
    delete mGameState.xochitl;
    mGameState.xochitl = nullptr;

    delete mGameState.fakeBee;
    mGameState.fakeBee = nullptr;

    delete mGameState.map;
    mGameState.map = nullptr;

    delete mGameState.sun;
    mGameState.sun = nullptr;



    UnloadSound(mGameState.jumpSound);
    UnloadSound(mGameState.buzzing);
    mGameState.jumpSound = {};
}
