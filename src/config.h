#ifndef CONFIG_H
#define CONFIG_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Config {

    namespace Camera{
    constexpr unsigned int SCREEN_WIDTH  = 1280;
    constexpr unsigned int SCREEN_HEIGHT = 720;
    inline constexpr glm::vec3 CAMERA_POS   = {0.0f, 8.0f, 95.0f};
    inline constexpr glm::vec3 CAMERA_FRONT = {0.0f, -0.15f, -1.0f};
    inline constexpr glm::vec3 CAMERA_UP    = {0.0f, 1.0f, 0.0f};
    }

    namespace World {
    constexpr float MIN_X = -60.0f;
    constexpr float MAX_X = 60.0f;
    constexpr float MIN_Y = -40.0f;
    constexpr float MAX_Y = 20.0f;
    }

    namespace Bricks {
    constexpr int ROWS = 10;
    constexpr int COLS = 20;
    constexpr float SPACING_X = 3.0f;
    constexpr float SPACING_Y = 2.0f;
    constexpr float START_X = -13.5f;
    constexpr float START_Y = 2.0f;
    inline constexpr glm::vec3 SCALE = {2.5f, 1.8f, 2.0f};
    }

    namespace Paddle {
    inline constexpr glm::vec3 START_POS = {0.0f, -30.0f, 0.0f};
    inline constexpr glm::vec3 SCALE = {10.0f, 2.0f, 2.0f};
    constexpr float SPEED = 50.0f;
    }

    namespace Ball {
    inline constexpr glm::vec3 START_POS = {0.0f, -25.0f, 0.0f};
    inline constexpr glm::vec3 START_VEL = {10.0f, 16.0f, 0.0f};
    constexpr float RADIUS = 1.0f;
    constexpr float SPEEDUP_FACTOR = 1.15f; //  15%
    constexpr float MAX_SPEED = 40.0f;
    }
    namespace Stats {
    constexpr int INITIAL_LIVES = 3;
    constexpr int SCORE_PER_BRICK = 10;
    }
    namespace PowerUp {
    constexpr float FALL_SPEED = 15.0f;
    constexpr float DROP_CHANCE = 0.20f; // 20%
    constexpr float DURATION = 10.0f;
    }

}
#endif // CONFIG_H
