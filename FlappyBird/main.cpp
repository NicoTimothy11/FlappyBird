#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>

using namespace std;

typedef enum GameScreen { TITLE, GAMEPLAY, ENDING } GameScreen;

class Obstacle {
    public:
        float x, y, width, gapHeight;

        void Update(float speed) {
            x -= speed;
        }

        void Render() {
            DrawRectangle(x, 0, width, y, GREEN);
            DrawRectangle(x, y  + gapHeight, width, GetScreenHeight() - (y + gapHeight), GREEN);
        }

        bool checkCollision(float birdX, float birdY, float birdSize){
            if(birdX + birdSize > x && birdX - birdSize < x + width) {
                if(birdY < y || birdY + birdSize > y + gapHeight) {
                    return true;
                }
            }
            return false;
        }

};

class FlappyBird {
    public:
        Vector2 position;
        float gravity;
        float velocity;
        float flapForce;
        float maxFallSpeed;
        float size;
        bool canFlap;

        void render() {
            DrawRectangle(position.x, position.y, size, size, PINK);
        }

        void update() {
            velocity += gravity;
            position.y += velocity;
            if(position.y < size) position.y;
            if(position.y > GetScreenHeight() - size) position.y = GetScreenHeight();
        }

        void ClampToEdges() {
            position.y = Clamp(position.y, 0.0f, GetScreenHeight() - 40.0f);
        }
};

class Game {
    public:
        FlappyBird bird;
        std::vector<Obstacle> obstacles;
        float pipeSpeed;
        float pipeSpacing;
        float pipeWidth;
        float pipeHeight;
        float pipeGapHeight;

        bool GameOver = false;
        int FlappyScore = 0;

        void SetUp() {
            bird.position =  { 100, 50 };
            bird.gravity = 0.5f;
            bird.velocity = 0.0f;
            bird.flapForce = - 8.0f;
            bird.maxFallSpeed = 8.0f;
            bird.size = 40.0f;
            bird.canFlap = true;

            pipeSpeed = 2.0f;
            pipeSpacing = 200.0f;
            pipeWidth = 60.0f;
            pipeGapHeight = 200.0f;
        }

        void Update() {
            float deltaTime = GetFrameTime();
            bird.update();
            bird.ClampToEdges();

            for (auto& obs : obstacles) {
                obs.Update(pipeSpeed);
            }

            if (!obstacles.empty() && obstacles[0].x + pipeWidth < 0) {
                obstacles.erase(obstacles.begin());
            }

            if (obstacles.empty() || obstacles.back().x < GetScreenWidth() - pipeSpacing) {

                float gapY = rand() % (GetScreenHeight() - (int)pipeGapHeight);
                Obstacle newObs;
                newObs.x = GetScreenWidth();
                newObs.y = gapY;
                newObs.width = pipeWidth;
                newObs.gapHeight = pipeGapHeight;
                obstacles.push_back(newObs);
            }

            for (auto& obs : obstacles) {
                if(obs.checkCollision(bird.position.x, bird.position.y , bird.size)) {
                    DrawText("Game Over!", GetScreenWidth() / 2 - 100, GetScreenHeight() / 2, 20, RED);
                    pipeSpeed = 0.0f;
                    bird.velocity = bird.position.y;
                    //bird.velocity = 0;
                    bird.canFlap = false;
                    GameOver = true;
                    break;
                }
            }
        }

        void Render() {
            DrawText("Flappy Score: ", 20, 20, 20, PINK);
            DrawText(TextFormat("%i", FlappyScore), 170, 20, 20, PINK);
            bird.render();
            for(auto& obs : obstacles) {
                obs.Render();
            }
        }

        void HandleInput(float deltaTime) {
            if(bird.canFlap) {
                if(IsKeyPressed(KEY_SPACE)) {
                    bird.velocity = bird.flapForce;
                }
                if(bird.velocity > bird.maxFallSpeed) {
                    bird.velocity = bird.maxFallSpeed;
                }
            }
        }

        void Score() {
            if(bird.canFlap) {
                if(IsKeyPressed(KEY_SPACE)) {
                    bird.velocity = bird.flapForce;
                    FlappyScore++;
                }
            }
        }

};

int main() {
    const int screenHeight = 500;
    const int screenWidth = 800;

    InitWindow(screenWidth, screenHeight, "Flappy Bird");
    SetTargetFPS(60);

    Game game;
    GameScreen currentScreen = TITLE;
    game.SetUp();
    
    while(!WindowShouldClose()) {

        float deltaTime = GetFrameTime();

        switch(currentScreen) {
            case TITLE:
            {
                if(IsKeyPressed(KEY_ENTER)) {
                    currentScreen = GAMEPLAY;
                }
            }break;

            case GAMEPLAY:
            {
                if(IsKeyPressed(KEY_ENTER)) {
                    currentScreen = ENDING;
                }
            }break;
            
            case ENDING:
            {
                if(IsKeyPressed(KEY_ENTER)) {
                    currentScreen = TITLE;
                }
            }break;

            default: break;
        }

        game.HandleInput(deltaTime);
        game.Update();

        BeginDrawing();
            switch(currentScreen) {
            case TITLE:
            {
                DrawRectangle(0, 0, screenWidth, screenHeight, GREEN);
                DrawText("FLAPPY BIRD", 250, 200, 40, DARKGREEN);
            }break;

            case GAMEPLAY:
            {
                ClearBackground(BLACK);
                game.Render();
                game.Score();
                if(game.GameOver == true) {
                    currentScreen = ENDING;
                }
                
            }break;

            case ENDING:
            {
                DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);
                DrawText("GAME OVER", 250, 50, 40, DARKBLUE);
                DrawText("YOUR SCORE", 250, 100, 40, DARKBLUE);
                DrawText(TextFormat("%i", game.FlappyScore), 380, 150, 40, DARKBLUE);

                if(IsKeyPressed(KEY_ENTER)){
                    currentScreen = TITLE;
                }
            }break;
            default: break;
        }
        EndDrawing();
    } 

    CloseWindow();
    return 0;
}
