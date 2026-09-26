#include "raylib.h"

#define SCREEN_W 960
#define SCREEN_H 540

int main(void)
{
    InitWindow(SCREEN_W, SCREEN_H, "C-Isaac [M0]");
    SetTargetFPS(60);

    Rectangle player = { SCREEN_W/2.0f - 12, SCREEN_H/2.0f - 12, 24, 24 };
    const float speed = 250.0f;

    while (!WindowShouldClose())
    {
        Vector2 move = { 0 };
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    move.y -= 1.0f;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  move.y += 1.0f;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  move.x -= 1.0f;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) move.x += 1.0f;

        if (move.x != 0.0f && move.y != 0.0f)
        {
            move.x *= 0.7071f;
            move.y *= 0.7071f;
        }

        player.x += move.x * speed * GetFrameTime();
        player.y += move.y * speed * GetFrameTime();

        if (player.x < 0.0f) player.x = 0.0f;
        if (player.y < 0.0f) player.y = 0.0f;
        if (player.x + player.width  > SCREEN_W) player.x = SCREEN_W - player.width;
        if (player.y + player.height > SCREEN_H) player.y = SCREEN_H - player.height;

        BeginDrawing();
        ClearBackground((Color){ 24, 20, 37, 255 });
        DrawRectangleRec(player, RAYWHITE);
        DrawText("M0: toolchain OK - WASD to move", 10, 10, 20, LIME);
        DrawFPS(SCREEN_W - 80, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
