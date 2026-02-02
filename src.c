#define TRUE (1)
#define FALSE (0)
#define KEY_BASE (65)
#define KEY_MAX (90)
#define KEY_W (87)
#define KEY_A (65)
#define KEY_S (83)
#define KEY_D (68)
#define KEY_W_MASK (1 << (KEY_W - KEY_BASE))
#define KEY_A_MASK (1 << (KEY_A - KEY_BASE))
#define KEY_S_MASK (1 << (KEY_S - KEY_BASE))
#define KEY_D_MASK (1 << (KEY_D - KEY_BASE))

typedef int bool;
typedef struct
{
    float x;
    float y;
} Vector2D;

typedef struct
{
    Vector2D position;
    bool alive;
    bool animated;
} Entity;

void logVector2D(Vector2D);
void logCStr(char*);
void logValue(int);

int g_keys_pressed = 0;

Entity g_enemy = (Entity){
    (Vector2D){.x = 100, .y = 100},
    .alive    = TRUE,
    .animated = FALSE};
Entity g_player = (Entity){
    (Vector2D){.x = 100, .y = 100},
    .alive    = TRUE,
    .animated = TRUE};

void engine_init(void)
{
    logCStr("Init game\0");
}

void __update_player()
{
    if (g_keys_pressed & KEY_W_MASK)
    {
        g_player.position.x += 1;
    }
    if (g_keys_pressed & KEY_S_MASK)
    {
        g_player.position.x -= 1;
    }
    if (g_keys_pressed & KEY_A_MASK)
    {
        g_player.position.y -= 1;
    }
    if (g_keys_pressed & KEY_D_MASK)
    {
        g_player.position.y -= 1;
    }
    logVector2D(g_player.position);
}

void engine_key_down(int key_code)
{
    if (key_code < KEY_BASE || key_code > KEY_MAX)
    {
        return;
    }
    g_keys_pressed |= 1 << (key_code - KEY_BASE);
    logCStr("pressed\0");
    logValue(g_keys_pressed);
}

void engine_key_up(int key_code)
{
    if (key_code < KEY_BASE || key_code > KEY_MAX)
    {
        return;
    }
    g_keys_pressed &= ~(1 << (key_code - KEY_BASE));
    logCStr("released\0");
    logValue(g_keys_pressed);
}

void engine_update()
{
    __update_player();
    //    update_enemy();
    logVector2D(g_player.position);
}
