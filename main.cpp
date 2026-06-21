#include "CS3113/ShaderProgram.h"
#include "CS3113/LevelC.h" // needed to include wouldnt see it for some reason 
#include "CS3113/SceneMenu.h" 
#include "CS3113/SceneGameOver.h" 
#include "CS3113/SceneWin.h" 

/**
* Author: Atiya Thounaojam
* Assignment: Rise of the AI
* Date due: 2026-06-20, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/



// Global Constants
constexpr int SCREEN_WIDTH     = 1400,
              SCREEN_HEIGHT    = 900,
              FPS              = 120,
              NUMBER_OF_LEVELS = 3;// changed

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Camera2D gCamera = { 0 };

Scene *gCurrentScene = nullptr;
std::vector<Scene*> gLevels = {};

LevelA *gLevelA = nullptr;
LevelB *gLevelB = nullptr;

//inherited scenes added 
LevelC *gLevelC = nullptr; // third level 
SceneGameOver *gGameOver = nullptr;  //game over scene
SceneWin *gWin = nullptr ; // win scene 
SceneMenu *gMenu = nullptr; // menu scene 


int currLevel = 0;

Effects *gEffects = nullptr;

ShaderProgram gShader;

// added for player lives 

int lives = 3; 

// for consistent music

Music bgm = {} ;

// playing buzzing?
bool buzzing = false;







// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene *scene) // used chat to debug seg fault 
{   
    if (scene == nullptr) return; 


    gCurrentScene = scene;
    gCurrentScene->initialise();

    if (gCurrentScene!=nullptr && gCurrentScene->getState().xochitl != nullptr) { 
        StopSound(gCurrentScene->getState().buzzing); // used chat to figure out why buzzing wasnt looping
        buzzing = false;
    }

    if (gCurrentScene->getState().xochitl != nullptr) gCamera.target = gCurrentScene->getState().xochitl->getPosition();
    else gCamera.target = ORIGIN;
}


void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Effects & Shaders");
    InitAudioDevice();

    //coppied from level or scene i forgot 
    bgm = LoadMusicStream("assets/game/jumping.mp3");
    SetMusicVolume(bgm, 0.13f);
    PlayMusicStream(bgm);



    gShader.load("shaders/vertex.glsl", "shaders/fragment.glsl");

    gLevelA = new LevelA(ORIGIN, "#C0897E");
    gLevelB = new LevelB(ORIGIN, "#ADD8E6");

    gLevelC = new LevelC(ORIGIN, "#ADD8E6");

    gGameOver = new SceneGameOver(ORIGIN, "#FFFFFF");
    gWin = new SceneWin(ORIGIN, "#FFFFFF");
    gMenu = new SceneMenu(ORIGIN, "#FFFFFF");

// _____________________________________________________

    gLevels.push_back(gLevelA);
    gLevels.push_back(gLevelB);

    gLevels.push_back(gLevelC);

    gLevels.push_back(gGameOver);
    gLevels.push_back(gWin);
    gLevels.push_back(nullptr);
    gLevels.push_back(gMenu);

    currLevel = 6;
    switchToScene(gLevels[currLevel]);

    gCamera.offset   = ORIGIN;
    gCamera.rotation = 0.0f;
    gCamera.zoom     = 1.0f;

    gEffects = new Effects(ORIGIN, (float) SCREEN_WIDTH * 1.5f, (float) SCREEN_HEIGHT * 1.5f);

    // gEffects->start(FADEIN);
    // gEffects->start(FADEOUT);
    // gEffects->start(SHRINK);
    // gEffects->start(GROW);



    SetTargetFPS(FPS);
}

void processInput()
{
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

    if (gCurrentScene->getState().xochitl == nullptr) return; // used chat to debug seg fault

    gCurrentScene->getState().xochitl->resetMovement();



    if      (IsKeyDown(KEY_A)) 
    {
        gCurrentScene->getState().xochitl->moveLeft();
        if (!buzzing) {
            PlaySound(gCurrentScene->getState().buzzing);
            buzzing = true;
        }
    }
    else if (IsKeyDown(KEY_D)) 
    {
        gCurrentScene->getState().xochitl->moveRight();
        if (!buzzing) {
            PlaySound(gCurrentScene->getState().buzzing);
            buzzing = true;
        }
    } else {
        StopSound(gCurrentScene->getState().buzzing); // used chat to figure out why buzzing wasnt looping
        buzzing = false;
    }

    if (IsKeyPressed(KEY_W) &&
        gCurrentScene->getState().xochitl->isCollidingBottom())
    {
        gCurrentScene->getState().xochitl->jump();
        PlaySound(gCurrentScene->getState().jumpSound);
    }

    if (GetLength(gCurrentScene->getState().xochitl->getMovement()) > 1.0f)
        gCurrentScene->getState().xochitl->normaliseMovement();

  

    if (IsKeyPressed(KEY_ONE) ) {

            currLevel = 0 ;
            switchToScene(gLevels[currLevel]); // to go back to first level "debug level"
    }

            // int id = gCurrentScene->getState().nextSceneID;
            // switchToScene(gLevels[id]);
    if (IsKeyPressed(KEY_TWO)) {

            currLevel = 1;
            switchToScene(gLevels[currLevel]);
    }

    if (IsKeyPressed(KEY_THREE)) {

            currLevel = 2;
            switchToScene(gLevels[currLevel]);
    }

}

void update()
{
    UpdateMusicStream(bgm);

    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);

        //used chat to figure out how to do the life implementation of knowing when to switch
        // have a placeholder scene and if its being used then start from whereever current scene is 
        int id = gCurrentScene->getState().nextSceneID;
        if (id != 5) {
            currLevel = id;
            switchToScene(gLevels[currLevel]);
        }

        if (gCurrentScene->getState().xochitl != nullptr) { //debug seg fault 
        Vector2 cameraTarget = { gCurrentScene->getState().xochitl->getPosition().x, ORIGIN.y };
        panCamera(&gCamera, &cameraTarget);
        gEffects->update(FIXED_TIMESTEP, &cameraTarget);

        
        }
        else { gEffects->update(FIXED_TIMESTEP, nullptr); }

        deltaTime -= FIXED_TIMESTEP;
    }
}

void render()
{
    BeginDrawing();
    BeginMode2D(gCamera);
    gShader.begin();

    gCurrentScene->render();

    gShader.end();
    gEffects->render();
    EndMode2D();

    //drawing lives proj 3 
    DrawText(TextFormat("Lives: %i", lives), 10, 10, 20, BLACK );

    EndDrawing();
}

void shutdown()
{
    delete gLevelA;
    delete gLevelB;
    delete gLevelC;

    for (int i = 0; i < NUMBER_OF_LEVELS; i++) gLevels[i] = nullptr;

    delete gEffects;
    gEffects = nullptr;

    gShader.unload();

    UnloadMusicStream(bgm);
    bgm = {}; 
    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();


        render();
    }

    shutdown();


    return 0;
}
