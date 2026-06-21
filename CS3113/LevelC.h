#ifndef LevelC_H
#define LevelC_H

#include "LevelA.h"

class LevelC : public Scene
{
private:
    unsigned int mLevelData[LEVEL_WIDTH * LEVEL_HEIGHT] = {
        4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
        4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
        4, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 4,
        4, 0, 0, 0, 0, 2, 2, 0, 3, 0, 0, 2, 0, 4,
        4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 4,
        4, 2, 2, 3, 2, 3, 2, 2, 3, 3, 3, 3, 0, 4,
        4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 4,
        4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 4
    };

public:
    static constexpr float TILE_DIMENSION          = 75.0f,
                           ACCELERATION_OF_GRAVITY = 981.0f,
                           END_GAME_THRESHOLD      = 800.0f;

    LevelC();
    LevelC(Vector2 origin, const char *bgHexCode);
    ~LevelC();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // LevelC_H
