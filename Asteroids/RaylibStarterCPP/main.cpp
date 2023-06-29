
#include "raylib.h"
#include "iostream"
#include "string"
#include "random"
#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
int screenWidth = 1280;
int screenHeight = 720;


//structure of spaceship
struct SpaceShip
{
   float posx, posy;
    float speedx, speedy;
    float rotation;
    float acceleration;
    bool alive;
    float ShipHitbox;
};
//structure of shooting
struct Shooting
{
    float posx, posy;
    float speedx, speedy;
    float size;
    float rotation;
    bool alive;
    int range;
};
//structure of the asteroids;
struct Asteroids
{
    float posx, posy;
    float speedx, speedy;
    float size;
    bool alive;
};

//variable for the structure of spaceship
SpaceShip spaceship;
//variable for the structure of bullets
const int bulletMax = 5;
Shooting bullets[bulletMax];
int AsteroidsMax = 5;//amount of Big asteroids
int AsteroidsMaxMed = 10;//value should be double the amount of big asteroids as each of the big asteroids will split into two
int AsteroidsMaxSmall = 20;//value should be double the amount of medium asteroids as they each split into two

//dynamic array is used for the asteroid structure as the asteroids add one more after finishing a round
Asteroids* asteroidsBig = new Asteroids[AsteroidsMax];
Asteroids* asteroidsMed = new Asteroids[AsteroidsMaxMed];
Asteroids* asteroidsSmall = new Asteroids[AsteroidsMaxSmall];
int AsteroidsDestrtoyed;
bool winner = false;
bool retry = false;
int lives = 3;
int Round = 1;

//initializes the game
void GameInitialize()
{

    

    
    //initializes the player
    spaceship.posx = screenWidth / 2;
    spaceship.posy = screenHeight / 2;
    spaceship.speedx = 0;
    spaceship.speedy = 0;
    spaceship.rotation = 0;
    spaceship.acceleration = 8;
    spaceship.alive = true;
    spaceship.ShipHitbox = 10;

    //initializes the big asteroids
    for (int i = 0; i < AsteroidsMax; i++) 
    {
        //this random function is to make sure the asteroids are only positioned on the left side to make sure they dont draw inside the player causing an unfair loss
        std::random_device rd;
        std::uniform_int_distribution<int> distributionx(0, screenWidth - (screenWidth / 1.5));
        std::uniform_int_distribution<int> distributiony(0, screenHeight);

        asteroidsBig[i].posx = distributionx(rd);
        asteroidsBig[i].posy = distributiony(rd);
        asteroidsBig[i].speedx = rand() % 3 + 1;
        asteroidsBig[i].speedy = rand() % 3 + 1;
        asteroidsBig[i].size = 50;
        asteroidsBig[i].alive = true;

        std::cout << asteroidsBig[i].posy << std::endl;
    }

    //initializes the medium asteroids
    for (int i = 0; i < AsteroidsMaxMed; i++)
    {
        asteroidsMed[i].posx = 0;
        asteroidsMed[i].posy = 0;
        asteroidsMed[i].speedx = 0;
        asteroidsMed[i].speedy = 0;
        asteroidsMed[i].size = 30;
        asteroidsMed[i].alive = false;
    }

    //initializes the small asteroids
    for (int i = 0; i < AsteroidsMaxSmall; i++)
    {
        asteroidsSmall[i].posx = 0;
        asteroidsSmall[i].posy = 0;
        asteroidsSmall[i].speedx = 0;
        asteroidsSmall[i].speedy = 0;
        asteroidsSmall[i].size = 15;
        asteroidsSmall[i].alive = false;
    }

    //initializes the bullet
    for (int i = 0; i < bulletMax; i++)
    {
        bullets[i].posx = 0;
        bullets[i].posy = 0;
        bullets[i].speedx = 0;
        bullets[i].speedy = 0;
        bullets[i].size = 3;
        bullets[i].alive = false;
        bullets[i].rotation = 0;
        bullets[i].range = 300;
    }
    

}

//function to split the asteroids 
void AsteroidSplit(float &x, float &y, float &x2, float &y2, float &speedx, float &speedy, bool& alive)
{
        x = x2;
        y = y2;
        speedx = rand() % 3 + 1;
        speedy = rand() % 3 + 1;
        alive = true;
}

//function to wrap around screen
void WrapAroundScreen(float &x, float &y,float radius)
{

    if (x > (screenWidth + radius)) {
        x = -radius;
    }
    else if (x < ( -radius)) {
        x = screenWidth + radius;
    }

    if (y > (screenHeight + radius)) {
        y = -radius;
    }
    else if (y < ( -radius)) {
        y = screenHeight + radius;
    }
}


void UpdateGame()
{
    static int asteroidspawnSmall; //both declared as static it gets allocated for the whole round 
    static int asteroidspawnMed;

    if (!retry)
    {
        //sin and cos angle based on the spaceships rotation
        float sinAngle = sin(spaceship.rotation * DEG2RAD);
        float cosAngle = cos(spaceship.rotation * DEG2RAD);


        WrapAroundScreen(spaceship.posx, spaceship.posy, spaceship.ShipHitbox);

        //if the asteroid is alive it will constantly move around in a direction
        for (int i = 0; i < AsteroidsMax; i++)
        {
            if (asteroidsBig[i].alive)
            {
                asteroidsBig[i].posx += asteroidsBig[i].speedx;
                asteroidsBig[i].posy += asteroidsBig[i].speedy;
                WrapAroundScreen(asteroidsBig[i].posx, asteroidsBig[i].posy, asteroidsBig[i].size);
            }
        }

        for (int i = 0; i < AsteroidsMaxMed; i++)
        {
            if (asteroidsMed[i].alive)
            {
                asteroidsMed[i].posx += asteroidsMed[i].speedx;
                asteroidsMed[i].posy += asteroidsMed[i].speedy;
                WrapAroundScreen(asteroidsMed[i].posx, asteroidsMed[i].posy, asteroidsMed[i].size);
            }
        }


        for (int i = 0; i < AsteroidsMaxSmall; i++)
        {
            if (asteroidsSmall[i].alive)
            {
                asteroidsSmall[i].posx += asteroidsSmall[i].speedx;
                asteroidsSmall[i].posy += asteroidsSmall[i].speedy;
                WrapAroundScreen(asteroidsSmall[i].posx, asteroidsSmall[i].posy, asteroidsSmall[i].size);
            }
        }


        //player movement
        if (IsKeyDown(KEY_W))
        {
            spaceship.speedx += sinAngle * spaceship.acceleration * GetFrameTime();
            spaceship.speedy -= cosAngle * spaceship.acceleration * GetFrameTime();
        }

        //makes it so the player constantly moves as they are in space
        spaceship.posx += spaceship.speedx;
        spaceship.posy += spaceship.speedy;

        //player rotation
        if (IsKeyDown(KEY_A))
        {
            spaceship.rotation += -5;
        }

        if (IsKeyDown(KEY_D))
        {
            spaceship.rotation += 5;
        }

        //key to shoot
        if (IsKeyPressed(KEY_SPACE))
        {

            for (int i = 0; i < bulletMax; i++)
            {
                if (!bullets[i].alive)
                {
                    bullets[i].posx = spaceship.posx;
                    bullets[i].posy = spaceship.posy;
                    bullets[i].speedx = sinAngle * 10;
                    bullets[i].speedy = cosAngle * 10;
                    bullets[i].alive = true;
                    break;
                }

            }
        }

        if (spaceship.alive)
        {
            //collision detection for the player and asteroids
            for (int i = 0; i < AsteroidsMax; i++)
            {
                if (spaceship.alive && asteroidsBig[i].alive && CheckCollisionCircles({ spaceship.posx, spaceship.posy }, spaceship.ShipHitbox, { asteroidsBig[i].posx, asteroidsBig[i].posy }, asteroidsBig[i].size + 15))
                {
                    spaceship.alive = false;
                }
            }

            for (int i = 0; i < AsteroidsMaxMed; i++)
            {
                if (spaceship.alive && asteroidsMed[i].alive && CheckCollisionCircles({ spaceship.posx, spaceship.posy }, spaceship.ShipHitbox, { asteroidsMed[i].posx, asteroidsMed[i].posy }, asteroidsMed[i].size + 6))
                {
                    spaceship.alive = false;
                }
            }

            for (int i = 0; i < AsteroidsMaxSmall; i++)
            {
                if (spaceship.alive && asteroidsSmall[i].alive && CheckCollisionCircles({ spaceship.posx, spaceship.posy }, spaceship.ShipHitbox, { asteroidsSmall[i].posx, asteroidsSmall[i].posy }, asteroidsSmall[i].size + 2))
                {
                    spaceship.alive = false;
                }
            }

            for (int i = 0; i < bulletMax; i++)
            {
                if (bullets[i].alive)
                {
                    
                    bullets[i].posx += bullets[i].speedx;
                    bullets[i].posy -= bullets[i].speedy;

                    //give the bullet a set range and it would dissapear
                    float dx = bullets[i].posx - spaceship.posx;
                    float dy = bullets[i].posy - spaceship.posy;
                    if (sqrt(dx * dx + dy * dy) > bullets[i].range)
                    {
                        bullets[i].alive = false;
                    }


                    //collision detection for bullets and asteroids
                    for (int j = 0; j < AsteroidsMax; j++)
                    {
                        if (asteroidsBig[j].alive && CheckCollisionCircles({ asteroidsBig[j].posx, asteroidsBig[j].posy }, asteroidsBig[j].size, { bullets[i].posx, bullets[i].posy }, bullets[i].size))
                        {

                            
                            bullets[i].alive = false;
                            AsteroidsDestrtoyed++;


                            for (int a = 0; a < 2; a++)
                            {
                                AsteroidSplit(asteroidsMed[asteroidspawnMed].posx, asteroidsMed[asteroidspawnMed].posy, asteroidsBig[j].posx,
                                    asteroidsBig[j].posy, asteroidsMed[asteroidspawnMed].speedx, asteroidsMed[asteroidspawnMed].speedy, asteroidsMed[asteroidspawnMed].alive);
                                asteroidspawnMed++;
                            }

                            asteroidsBig[j].alive = false;

                        }
                    }

                    for (int j = 0; j < AsteroidsMaxMed; j++)
                    {
                        if (asteroidsMed[j].alive && CheckCollisionCircles({ asteroidsMed[j].posx, asteroidsMed[j].posy }, asteroidsMed[j].size, { bullets[i].posx, bullets[i].posy }, bullets[i].size))
                        {

                            
                            bullets[i].alive = false;
                            AsteroidsDestrtoyed++;

                            for (int a = 0; a < 2; a++)
                            {
                                AsteroidSplit(asteroidsSmall[asteroidspawnSmall].posx, asteroidsSmall[asteroidspawnSmall].posy, asteroidsMed[j].posx,
                                    asteroidsMed[j].posy, asteroidsSmall[asteroidspawnSmall].speedx, asteroidsSmall[asteroidspawnSmall].speedy, asteroidsSmall[asteroidspawnSmall].alive);
                                asteroidspawnSmall++;
                               
                            }
                            std::cout << asteroidspawnSmall;
                            asteroidsMed[j].alive = false;

                        }
                    }

                    for (int k = 0; k < AsteroidsMaxSmall; k++)
                    {
                        if (asteroidsSmall[k].alive && CheckCollisionCircles({ asteroidsSmall[k].posx, asteroidsSmall[k].posy }, asteroidsSmall[k].size, { bullets[i].posx, bullets[i].posy }, bullets[i].size))
                        {
                            AsteroidsDestrtoyed++;
                            bullets[i].alive = false;
                            asteroidsSmall[k].alive = false;
                        }
                    }
                }

            }

            

            
            




        }
        else
        {
            //if player is not active it loses a life and restart the round
            lives -= 1;
            retry = true;
            AsteroidsDestrtoyed = 0;
            asteroidspawnMed = 0;
            asteroidspawnSmall = 0;

            //if player loses all the lives it back to round one
            if (lives == 0)
            {
                AsteroidsMax = 5;
                AsteroidsMaxMed = 10;
                AsteroidsMaxSmall = 20;
                lives = 3;
                Round = 1;
            }
        }

        //if the asteroids are destroyed 
        if (AsteroidsDestrtoyed == AsteroidsMax + AsteroidsMaxMed + AsteroidsMaxSmall && spaceship.alive)
        {
            winner = true;
            retry = true;
            AsteroidsDestrtoyed = 0;
            asteroidspawnMed = 0;
            asteroidspawnSmall = 0;
            Round += 1;

            delete[] asteroidsBig;
            delete[] asteroidsMed;
            delete[] asteroidsSmall;

            AsteroidsMax += 1;
            AsteroidsMaxMed += 2;
            AsteroidsMaxSmall += 4;

            asteroidsBig = new Asteroids[AsteroidsMax];
            asteroidsMed = new Asteroids[AsteroidsMaxMed];
            asteroidsSmall = new Asteroids[AsteroidsMaxSmall];


            
        }

    }
    else
    {
        //if player presses R the game restarts or moves to next round
        if (IsKeyPressed(KEY_R))
        {
            GameInitialize();
            retry = false;
            winner = false;
        }
        
    }
    bool LeaveGame = false;
    if (IsKeyPressed(KEY_ESCAPE))
    {
        LeaveGame = true;
    }
    if (LeaveGame)
    {
        CloseWindow();
    }

}

void Draw()
{
    //same sin angle and cos angle but used for draw
    float sinAngle = sin(spaceship.rotation * DEG2RAD);
    float cosAngle = cos(spaceship.rotation * DEG2RAD);

    
    
    //draw bullets
    for (int i = 0; i < bulletMax; i++)
    {
        if (bullets[i].alive)
        {
            DrawCircle(bullets[i].posx, bullets[i].posy, bullets[i].size, BLUE);
        }
        else
        {
            DrawCircle(bullets[i].posx, bullets[i].posy, bullets[i].size, BLANK);
        }
    }
    
    //draw asteroids
    for (int i = 0; i < AsteroidsMax; i++)
    {
        if (asteroidsBig[i].alive)
        {
            DrawCircle(asteroidsBig[i].posx, asteroidsBig[i].posy, asteroidsBig[i].size, GRAY);
            DrawCircle(asteroidsBig[i].posx + 20, asteroidsBig[i].posy + 20, asteroidsBig[i].size / 1.2, GRAY);
            DrawCircle(asteroidsBig[i].posx + -20, asteroidsBig[i].posy + -20, asteroidsBig[i].size / 1.2, GRAY);
            DrawCircle(asteroidsBig[i].posx + -20, asteroidsBig[i].posy + 15, asteroidsBig[i].size / 1.1, GRAY);
        }
        else
        {
            DrawCircle(asteroidsBig[i].posx, asteroidsBig[i].posy, asteroidsBig[i].size, BLANK);
        }
    }

    for (int i = 0; i < AsteroidsMaxMed; i++)
    {
        if (asteroidsMed[i].alive)
        {
            DrawCircle(asteroidsMed[i].posx, asteroidsMed[i].posy, asteroidsMed[i].size, GRAY);
            DrawCircle(asteroidsMed[i].posx + 6, asteroidsMed[i].posy + -20, asteroidsMed[i].size / 1.04, GRAY);
            DrawCircle(asteroidsMed[i].posx + -10, asteroidsMed[i].posy + -20, asteroidsMed[i].size / 1.03, GRAY);
            DrawCircle(asteroidsMed[i].posx + -13, asteroidsMed[i].posy + -13, asteroidsMed[i].size / 1.1, GRAY);
        }
        else
        {
            DrawCircle(asteroidsMed[i].posx, asteroidsMed[i].posy, asteroidsMed[i].size, BLANK);
        }
    }
   
    for (int i = 0; i < AsteroidsMaxSmall; i++)
    {
        if (asteroidsSmall[i].alive)
        {
            DrawCircle(asteroidsSmall[i].posx, asteroidsSmall[i].posy, asteroidsSmall[i].size, GRAY);
            DrawCircle(asteroidsSmall[i].posx + 3, asteroidsSmall[i].posy + -10, asteroidsSmall[i].size / 1.04, GRAY);
            DrawCircle(asteroidsSmall[i].posx + -15, asteroidsSmall[i].posy + -10, asteroidsSmall[i].size / 1.03, GRAY);
            DrawCircle(asteroidsSmall[i].posx + -17, asteroidsSmall[i].posy + -9, asteroidsSmall[i].size / 1.1, GRAY);
        }
        else
        {
            DrawCircle(asteroidsSmall[i].posx, asteroidsSmall[i].posy, asteroidsSmall[i].size, BLANK);
        }
    }

    //draw spaceships
    if (spaceship.alive)
    {
        DrawTriangle(
            { spaceship.posx + 25 * sinAngle, spaceship.posy - 25 * cosAngle }
            , { spaceship.posx - 10 * cosAngle,spaceship.posy - 10 * sinAngle }
        , { spaceship.posx + 10 * cosAngle, spaceship.posy + 10 * sinAngle }, BLUE);
        if (IsKeyDown(KEY_W))
        {
            DrawTriangle(
                { spaceship.posx + 10 * -sinAngle, spaceship.posy - 10 * -cosAngle }
                , { spaceship.posx - 8 * -cosAngle,spaceship.posy - 8 * -sinAngle }
            , { spaceship.posx + 8 * -cosAngle, spaceship.posy + 8 * -sinAngle }, YELLOW);
        }
    }
    else
    {
        //if player dies you lose sign will appear
        DrawTriangle(
            { spaceship.posx + 25 * sinAngle, spaceship.posy - 25 * cosAngle }
            , { spaceship.posx - 10 * cosAngle,spaceship.posy - 10 * sinAngle }
        , { spaceship.posx + 10 * cosAngle, spaceship.posy + 10 * sinAngle }, BLANK);
        DrawText("YOU LOSE", 100, 150, 100, RED);
        DrawText("if you run out of lives the game will reset to round one", 50, 250, 25, RED);
    }

    //winner draw
    if (winner)
    {
        if (spaceship.alive)
        {
            DrawText("YOU WIN", 100, 150, 100, RED);
            DrawText("Click R for next round", 50, 250, 25, RED);
        }
    }

    //prints lives in the game
    if (lives > 0)
    {
        DrawText(std::to_string(lives).c_str(), screenWidth / 15, screenHeight / 10, 25, RED);
        DrawText("lives: ", screenWidth / 120, screenHeight / 10, 25, RED);
    }
    
    DrawText(std::to_string(Round).c_str(), screenWidth / 13, screenHeight / 7, 25, RED);
    DrawText("Round: ", screenWidth / 120, screenHeight / 7, 25, RED);

    DrawText("Close game: Esc", screenWidth / 120, screenHeight / 18, 25, RED);

}

int main(int argc, char* argv[])
{
   

    InitWindow(screenWidth, screenHeight, "Asteroid game");

    SetTargetFPS(60);
   
    GameInitialize();

    while (!WindowShouldClose())    
    {
       
        BeginDrawing();
        
        ClearBackground(BLACK);
        UpdateGame();
        Draw();
        

        EndDrawing();
        
    }

    
    CloseWindow();        
    return 0;
}