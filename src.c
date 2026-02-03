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
#define WINDOW_WIDTH_PX (1000)
#define WINDOW_HEIGHT_PX (600)
#define WALL_WIDTH_PX (1000)
#define WALL_HEIGHT_PX (600)
#define WALL_BORDER_PX (1000)
#define FOV_MAX_Z (10)
#define FOV_MIN_Z (1)
#define NUM_OF_WALLS (20)
#define PLAYER_SPEED_Z (1.5)
#define PLAYER_SPEED_XY (400)
#define PLAYER_SIZE_PX (100)
#define MAX_PLAYER_POS_X ((WALL_WIDTH_PX - PLAYER_SIZE_PX) / 2)
#define MAX_PLAYER_POS_Y ((WALL_HEIGHT_PX - PLAYER_SIZE_PX) / 2)

typedef struct
{
    float x;
    float y;
    float z;
} Vector3D;

typedef struct
{
    float w;
    float h;
} Size2D;

typedef struct
{
    Vector3D position;
    Size2D size;
} Rect;

typedef struct
{
    Rect world;
    Rect proj;
    int brightness;
    float border_width;
} Wall;

typedef struct
{
    Vector3D position;
    bool alive;
    bool animated;
} Entity;

typedef struct
{
    int window_height_px;
    int window_width_px;
    int fov_max_z;
    int fov_min_z;
    int num_of_walls;
} EngineParams;

typedef struct
{
    int ticks;
    int inc_x;
    int inc_y;
} PathElement;

float g_dt                 = 0;
int g_keys_pressed         = 0;
Wall g_walls[NUM_OF_WALLS] = {0};
Entity g_player            = (Entity){(Vector3D){.x = 0, .y = 0, .z = 0}, .alive = TRUE, .animated = TRUE};
const PathElement g_path[] = {
    {10, 0, 0},
    {20, 10, 0},
    {20, 0, 10},
    {20, -10, -10},
    {10, 0, 0},
};

void jsLogVector3D(Vector3D);
void jsLogCStr(char*);
void jsLogInt(int);
void jsLogFloat(float);
float jsGetDt(void);
void jsSetEngineParams(EngineParams);
void jsUpdateWallRect(int, Rect, int, float);

void engine_init(void)
{
    jsLogCStr("Init game\0");
    for (int i = 0; i < NUM_OF_WALLS; i++)
    {
        float position_z = (float)i * (float)(FOV_MAX_Z - FOV_MIN_Z) / (float)NUM_OF_WALLS + (float)FOV_MIN_Z;
        g_walls[i]       = (Wall){
                  .world = (Rect){
                      .position = (Vector3D){
                          .x = 0,
                          .y = 0,
                          .z = position_z,
                },
                      .size = (Size2D){
                          .w = WALL_WIDTH_PX,
                          .h = WALL_HEIGHT_PX,
                },
            },
                  .brightness   = 0,
                  .border_width = WALL_BORDER_PX,
        };
    }
    jsSetEngineParams((EngineParams){
        .window_height_px = WINDOW_HEIGHT_PX,
        .window_width_px  = WINDOW_WIDTH_PX,
        .fov_max_z        = FOV_MAX_Z,
        .fov_min_z        = FOV_MIN_Z,
        .num_of_walls     = NUM_OF_WALLS,
    });
}

void engine_key_down(int key_code)
{
    if (key_code < KEY_BASE || key_code > KEY_MAX)
    {
        return;
    }
    g_keys_pressed |= 1 << (key_code - KEY_BASE);
    jsLogCStr("pressed\0");
    jsLogInt(g_keys_pressed);
}

void engine_key_up(int key_code)
{
    if (key_code < KEY_BASE || key_code > KEY_MAX)
    {
        return;
    }
    g_keys_pressed &= ~(1 << (key_code - KEY_BASE));
    jsLogCStr("released\0");
    jsLogInt(g_keys_pressed);
}

void __update_player(void)
{
    if (g_keys_pressed & KEY_W_MASK)
    {
        g_player.position.y += PLAYER_SPEED_XY * g_dt;
    }
    if (g_keys_pressed & KEY_S_MASK)
    {
        g_player.position.y -= PLAYER_SPEED_XY * g_dt;
    }
    if (g_keys_pressed & KEY_D_MASK)
    {
        g_player.position.x += PLAYER_SPEED_XY * g_dt;
    }
    if (g_keys_pressed & KEY_A_MASK)
    {
        g_player.position.x -= PLAYER_SPEED_XY * g_dt;
    }
    if (g_player.position.x > MAX_PLAYER_POS_X)
    {
        g_player.position.x = MAX_PLAYER_POS_X;
    }
    if (g_player.position.x < -MAX_PLAYER_POS_X)
    {
        g_player.position.x = -MAX_PLAYER_POS_X;
    }
    if (g_player.position.y > MAX_PLAYER_POS_Y)
    {
        g_player.position.y = MAX_PLAYER_POS_Y;
    }
    if (g_player.position.y < -MAX_PLAYER_POS_Y)
    {
        g_player.position.y = -MAX_PLAYER_POS_Y;
    }
}

void __evolve_wall(int wall_index)
{
    static int tick               = 0;
    static float position_x       = 0.0;
    static float position_y       = 0.0;
    static int path_element_index = 0;
    static int tick_threshold     = 10;
    if (tick == tick_threshold)
    {
        tick               = 0;
        path_element_index = (path_element_index + 1) % (sizeof(g_path) / sizeof(g_path[0]));
        tick_threshold     = g_path[path_element_index].ticks;
    }

    Wall* wall_p = &g_walls[wall_index];
    wall_p->world.position.z -= PLAYER_SPEED_Z * g_dt;
    if (wall_p->world.position.z <= FOV_MIN_Z)
    {
        position_x += g_path[path_element_index].inc_x;
        position_y += g_path[path_element_index].inc_y;
        tick++;
        jsLogInt(path_element_index);
        jsLogFloat(position_x);
        jsLogFloat(position_y);
        wall_p->world.position.z += (float)(FOV_MAX_Z - FOV_MIN_Z);
        wall_p->world.position.x = position_x;
        wall_p->world.position.y = position_y;
    }
    wall_p->proj.position.x =                                                                        //
        (wall_p->world.position.x - wall_p->world.size.w / 2 - WALL_BORDER_PX - g_player.position.x) //
            / wall_p->world.position.z
        + WINDOW_WIDTH_PX / 2;
    wall_p->proj.position.y =                                                                        //
        (wall_p->world.position.y - wall_p->world.size.h / 2 - WALL_BORDER_PX - g_player.position.y) //
            / wall_p->world.position.z
        + WINDOW_HEIGHT_PX / 2;
    wall_p->proj.position.z = wall_p->world.position.z;
    wall_p->proj.size.w     = wall_p->world.size.w / wall_p->world.position.z;
    wall_p->proj.size.h     = wall_p->world.size.h / wall_p->world.position.z;
    wall_p->brightness      = 255 * (1 - (wall_p->world.position.z - FOV_MIN_Z) / (FOV_MAX_Z - FOV_MIN_Z));
    wall_p->border_width    = WALL_BORDER_PX / wall_p->world.position.z;
}

void engine_update(void)
{
    g_dt = jsGetDt();
    __update_player();
    for (int i = 0; i < NUM_OF_WALLS; i++)
    {
        __evolve_wall(i);
        jsUpdateWallRect(i, g_walls[i].proj, g_walls[i].brightness, g_walls[i].border_width);
    }
}
