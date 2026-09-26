#include "raylib.h"
#include <cmath>

#define SCREEN_W 960
#define SCREEN_H 540
#define WALL 40

// ---------- 玩家 ----------
#define PLAYER_RADIUS 12.0f
#define PLAYER_SPEED 220.0f
#define PLAYER_MAX_HP 6
#define IFRAME_TIME 1.0f

// ---------- 眼泪 ----------
#define MAX_TEARS 64
#define TEAR_RADIUS 5.0f
#define TEAR_SPEED 420.0f
#define TEAR_LIFETIME 1.1f
#define TEAR_DAMAGE 1
#define FIRE_COOLDOWN 0.26f

// ---------- 怪物 ----------
#define ENEMY_RADIUS 14.0f
#define ENEMY_SPEED 105.0f
#define ENEMY_MAX_HP 10
#define ENEMY_TOUCH_DAMAGE 1

typedef struct Player {
    Vector2 pos;
    int hp;
    float iframes;
    float fireTimer;
} Player;

typedef struct Tear {
    bool active;
    Vector2 pos;
    Vector2 vel;
    float life;
} Tear;

typedef struct Enemy {
    bool alive;
    Vector2 pos;
    int hp;
} Enemy;

typedef enum { ST_PLAYING, ST_DEAD } GameState;

static Player player;
static Tear tears[MAX_TEARS];
static Enemy enemy;
static GameState state;

// ---------- 房间几何 ----------

static bool InRoom(float x, float y, float r)
{
    return x - r >= WALL && x + r <= SCREEN_W - WALL
        && y - r >= WALL && y + r <= SCREEN_H - WALL;
}

static void ClampToRoom(Vector2 *pos, float r)
{
    if (pos->x - r < WALL) pos->x = WALL + r;
    if (pos->x + r > SCREEN_W - WALL) pos->x = SCREEN_W - WALL - r;
    if (pos->y - r < WALL) pos->y = WALL + r;
    if (pos->y + r > SCREEN_H - WALL) pos->y = SCREEN_H - WALL - r;
}

// ---------- 初始化 ----------

static void ResetGame(void)
{
    player.pos = Vector2{ SCREEN_W/2.0f, SCREEN_H*0.7f };
    player.hp = PLAYER_MAX_HP;
    player.iframes = 0.0f;
    player.fireTimer = 0.0f;

    for (int i = 0; i < MAX_TEARS; i++) tears[i].active = false;

    enemy.alive = true;
    enemy.pos = Vector2{ SCREEN_W/2.0f, SCREEN_H*0.3f };
    enemy.hp = ENEMY_MAX_HP;

    state = ST_PLAYING;
}

// ---------- 更新 ----------

static void SpawnTear(Vector2 pos, Vector2 dir)
{
    for (int i = 0; i < MAX_TEARS; i++)
    {
        if (!tears[i].active)
        {
            tears[i].active = true;
            tears[i].pos = pos;
            tears[i].vel = Vector2{ dir.x * TEAR_SPEED, dir.y * TEAR_SPEED };
            tears[i].life = TEAR_LIFETIME;
            return;
        }
    }
}

static void UpdatePlayer(float dt)
{
    Vector2 move = { 0 };
    if (IsKeyDown(KEY_W)) move.y -= 1.0f;
    if (IsKeyDown(KEY_S)) move.y += 1.0f;
    if (IsKeyDown(KEY_A)) move.x -= 1.0f;
    if (IsKeyDown(KEY_D)) move.x += 1.0f;

    if (move.x != 0.0f && move.y != 0.0f)
    {
        move.x *= 0.7071f;
        move.y *= 0.7071f;
    }

    player.pos.x += move.x * PLAYER_SPEED * dt;
    player.pos.y += move.y * PLAYER_SPEED * dt;
    ClampToRoom(&player.pos, PLAYER_RADIUS);

    if (player.iframes > 0.0f) player.iframes -= dt;
    if (player.fireTimer > 0.0f) player.fireTimer -= dt;

    Vector2 aim = { 0 };
    if (IsKeyDown(KEY_UP))    aim.y -= 1.0f;
    if (IsKeyDown(KEY_DOWN))  aim.y += 1.0f;
    if (IsKeyDown(KEY_LEFT))  aim.x -= 1.0f;
    if (IsKeyDown(KEY_RIGHT)) aim.x += 1.0f;

    if ((aim.x != 0.0f || aim.y != 0.0f) && player.fireTimer <= 0.0f)
    {
        float len = sqrtf(aim.x*aim.x + aim.y*aim.y);
        aim.x /= len;
        aim.y /= len;
        SpawnTear(player.pos, aim);
        player.fireTimer = FIRE_COOLDOWN;
    }
}

static void UpdateTears(float dt)
{
    for (int i = 0; i < MAX_TEARS; i++)
    {
        if (!tears[i].active) continue;

        tears[i].pos.x += tears[i].vel.x * dt;
        tears[i].pos.y += tears[i].vel.y * dt;
        tears[i].life -= dt;

        if (tears[i].life <= 0.0f || !InRoom(tears[i].pos.x, tears[i].pos.y, TEAR_RADIUS))
        {
            tears[i].active = false;
            continue;
        }

        float dx = tears[i].pos.x - enemy.pos.x;
        float dy = tears[i].pos.y - enemy.pos.y;
        if (enemy.alive && dx*dx + dy*dy < (TEAR_RADIUS + ENEMY_RADIUS)*(TEAR_RADIUS + ENEMY_RADIUS))
        {
            enemy.hp -= TEAR_DAMAGE;
            tears[i].active = false;
            if (enemy.hp <= 0) enemy.alive = false;
        }
    }
}

static void UpdateEnemy(float dt)
{
    if (!enemy.alive) return;

    Vector2 dir = { player.pos.x - enemy.pos.x, player.pos.y - enemy.pos.y };
    float len = sqrtf(dir.x*dir.x + dir.y*dir.y);
    if (len > 0.001f)
    {
        dir.x /= len;
        dir.y /= len;
        enemy.pos.x += dir.x * ENEMY_SPEED * dt;
        enemy.pos.y += dir.y * ENEMY_SPEED * dt;
    }

    float dx = enemy.pos.x - player.pos.x;
    float dy = enemy.pos.y - player.pos.y;
    if (player.iframes <= 0.0f
        && dx*dx + dy*dy < (ENEMY_RADIUS + PLAYER_RADIUS)*(ENEMY_RADIUS + PLAYER_RADIUS))
    {
        player.hp -= ENEMY_TOUCH_DAMAGE;
        player.iframes = IFRAME_TIME;
        if (player.hp <= 0) state = ST_DEAD;
    }
}

// ---------- 绘制 ----------

static void DrawHeart(Vector2 c, float s, int mode)
{
    Vector2 A = { c.x - s*0.55f, c.y - s*0.10f };
    Vector2 B = { c.x + s*0.55f, c.y - s*0.10f };
    Vector2 C = { c.x, c.y + s*0.50f };
    Vector2 M = { c.x, c.y - s*0.10f };

    if (mode == 0)
    {
        Color dim = Color{ 60, 40, 50, 255 };
        DrawCircleV(Vector2{ c.x - s*0.27f, c.y - s*0.18f }, s*0.28f, dim);
        DrawCircleV(Vector2{ c.x + s*0.27f, c.y - s*0.18f }, s*0.28f, dim);
        DrawTriangle(A, C, B, dim);
    }
    else
    {
        Color col = (mode == 1) ? Color{ 150, 30, 40, 255 } : Color{ 220, 40, 50, 255 };
        DrawCircleV(Vector2{ c.x - s*0.27f, c.y - s*0.18f }, s*0.28f, col);
        DrawTriangle(A, C, M, col);
        if (mode == 2)
        {
            DrawCircleV(Vector2{ c.x + s*0.27f, c.y - s*0.18f }, s*0.28f, col);
            DrawTriangle(M, C, B, col);
        }
    }
}

static void DrawHUD(void)
{
    for (int i = 0; i < PLAYER_MAX_HP / 2; i++)
    {
        int hpHere = player.hp - i*2;
        int mode = (hpHere >= 2) ? 2 : (hpHere == 1 ? 1 : 0);
        DrawHeart(Vector2{ 30.0f + i*26.0f, 20.0f }, 22.0f, mode);
    }

    if (!enemy.alive)
    {
        DrawText("ROOM CLEAR", SCREEN_W/2 - MeasureText("ROOM CLEAR", 24)/2, 50, 24, LIME);
    }
}

static void DrawPlayer(void)
{
    bool blink = player.iframes > 0.0f && ((int)(player.iframes * 12.0f) % 2 == 0);
    Color skin = blink ? Color{ 230, 210, 200, 128 } : RAYWHITE;

    DrawCircleV(player.pos, PLAYER_RADIUS, skin);

    Vector2 aim = { 0 };
    if (IsKeyDown(KEY_UP))    aim.y -= 1.0f;
    if (IsKeyDown(KEY_DOWN))  aim.y += 1.0f;
    if (IsKeyDown(KEY_LEFT))  aim.x -= 1.0f;
    if (IsKeyDown(KEY_RIGHT)) aim.x += 1.0f;

    Vector2 eyeOff = Vector2{ 4.0f + aim.x*2.0f, -2.0f + aim.y*2.0f };
    DrawCircleV(Vector2{ player.pos.x - eyeOff.x, player.pos.y + eyeOff.y }, 2.2f, BLACK);
    DrawCircleV(Vector2{ player.pos.x + eyeOff.x, player.pos.y + eyeOff.y }, 2.2f, BLACK);
}

static void DrawEnemy(void)
{
    if (!enemy.alive) return;

    DrawCircleV(enemy.pos, ENEMY_RADIUS, Color{ 190, 60, 70, 255 });

    float hpFrac = (float)enemy.hp / ENEMY_MAX_HP;
    Vector2 dir = { player.pos.x - enemy.pos.x, player.pos.y - enemy.pos.y };
    float len = sqrtf(dir.x*dir.x + dir.y*dir.y);
    if (len > 0.001f) { dir.x /= len; dir.y /= len; }

    DrawCircleV(Vector2{ enemy.pos.x + dir.x*5.0f - 3.0f, enemy.pos.y + dir.y*5.0f - 2.0f }, 2.4f, BLACK);
    DrawCircleV(Vector2{ enemy.pos.x + dir.x*5.0f + 3.0f, enemy.pos.y + dir.y*5.0f - 2.0f }, 2.4f, BLACK);

    DrawRectangle(WALL, SCREEN_H - WALL + 12, (int)((SCREEN_W - WALL*2) * hpFrac), 4, RED);
}

static void DrawTears(void)
{
    for (int i = 0; i < MAX_TEARS; i++)
    {
        if (!tears[i].active) continue;
        float shrink = tears[i].life < 0.25f ? tears[i].life / 0.25f : 1.0f;
        DrawCircleV(tears[i].pos, TEAR_RADIUS * shrink, Color{ 150, 200, 235, 255 });
    }
}

static void DrawRoom(void)
{
    ClearBackground(Color{ 24, 20, 37, 255 });
    DrawRectangle(0, 0, SCREEN_W, WALL, Color{ 90, 80, 100, 255 });
    DrawRectangle(0, SCREEN_H - WALL, SCREEN_W, WALL, Color{ 90, 80, 100, 255 });
    DrawRectangle(0, WALL, WALL, SCREEN_H - WALL*2, Color{ 90, 80, 100, 255 });
    DrawRectangle(SCREEN_W - WALL, WALL, WALL, SCREEN_H - WALL*2, Color{ 90, 80, 100, 255 });
}

// ---------- 主循环 ----------

int main(void)
{
    InitWindow(SCREEN_W, SCREEN_H, "C-Isaac [M1]");
    SetTargetFPS(60);
    ResetGame();

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        if (state == ST_PLAYING)
        {
            UpdatePlayer(dt);
            UpdateTears(dt);
            UpdateEnemy(dt);
        }
        else if (IsKeyPressed(KEY_R))
        {
            ResetGame();
        }

        BeginDrawing();
        DrawRoom();
        DrawTears();
        DrawEnemy();
        DrawPlayer();
        DrawHUD();
        DrawFPS(SCREEN_W - 80, SCREEN_H - 24);

        if (state == ST_DEAD)
        {
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Color{ 0, 0, 0, 160 });
            const char *msg = "YOU DIED";
            DrawText(msg, SCREEN_W/2 - MeasureText(msg, 48)/2, SCREEN_H/2 - 60, 48, RED);
            const char *hint = "press R to restart";
            DrawText(hint, SCREEN_W/2 - MeasureText(hint, 20)/2, SCREEN_H/2 + 10, 20, LIGHTGRAY);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
