#include <SDL2/SDL.h>
#include <cmath>
#include <vector>
#include <iostream>

const int MAP_WIDTH = 20;
const int MAP_HEIGHT = 20;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int worldMap[MAP_WIDTH][MAP_HEIGHT] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,1,1,0,0,1,0,0,0,0,0,1,1,1,0,0,0,1},
    {1,0,0,1,1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1},
    {1,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,1,1,0,0,1,1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

struct Player {
    double pos_x, pos_y;
    double dir_x, dir_y;
    double plane_x, plane_y;
};

bool init(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return false;
    }

    *window = SDL_CreateWindow(
        "raycaster",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (*window == NULL) {
        SDL_Quit();
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == NULL) {
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return false;
    }

    return true;
}

void handle_input(const Uint8* keys, Player& player, double move_speed, double rot_speed) {
    if (keys[SDL_SCANCODE_UP]) {
        if (worldMap[int(player.pos_x + player.dir_x * move_speed)][int(player.pos_y)] == 0) 
            player.pos_x += player.dir_x * move_speed;
        if (worldMap[int(player.pos_x)][int(player.pos_y + player.dir_y * move_speed)] == 0) 
            player.pos_y += player.dir_y * move_speed;
    }
    if (keys[SDL_SCANCODE_DOWN]) {
        if (worldMap[int(player.pos_x - player.dir_x * move_speed)][int(player.pos_y)] == 0) 
            player.pos_x -= player.dir_x * move_speed;
        if (worldMap[int(player.pos_x)][int(player.pos_y - player.dir_y * move_speed)] == 0) 
            player.pos_y -= player.dir_y * move_speed;
    }
    if (keys[SDL_SCANCODE_RIGHT]) {
        double old_dir_x = player.dir_x;
        player.dir_x = player.dir_x * cos(rot_speed) - player.dir_y * sin(rot_speed);
        player.dir_y = old_dir_x * sin(rot_speed) + player.dir_y * cos(rot_speed);
        double old_plane_x = player.plane_x;
        player.plane_x = player.plane_x * cos(rot_speed) - player.plane_y * sin(rot_speed);
        player.plane_y = old_plane_x * sin(rot_speed) + player.plane_y * cos(rot_speed);
    }
    if (keys[SDL_SCANCODE_LEFT]) {
        double old_dir_x = player.dir_x;
        player.dir_x = player.dir_x * cos(-rot_speed) - player.dir_y * sin(-rot_speed);
        player.dir_y = old_dir_x * sin(-rot_speed) + player.dir_y * cos(-rot_speed);
        double old_plane_x = player.plane_x;
        player.plane_x = player.plane_x * cos(-rot_speed) - player.plane_y * sin(-rot_speed);
        player.plane_y = old_plane_x * sin(-rot_speed) + player.plane_y * cos(-rot_speed);
    }
}

void cast_ray(SDL_Renderer* renderer, const Player& player, int x) {
    double camera_x = 2 * x / double(SCREEN_WIDTH) - 1;
    double ray_dir_x = player.dir_x + player.plane_x * camera_x;
    double ray_dir_y = player.dir_y + player.plane_y * camera_x;

    int map_x = int(player.pos_x);
    int map_y = int(player.pos_y);

    if (map_x < 0 || map_x >= MAP_WIDTH || map_y < 0 || map_y >= MAP_HEIGHT) {
        return;
    }

    double delta_dist_x = (ray_dir_x == 0) ? 1e30 : std::abs(1 / ray_dir_x);
    double delta_dist_y = (ray_dir_y == 0) ? 1e30 : std::abs(1 / ray_dir_y);
    double side_dist_x, side_dist_y;
    int step_x, step_y;
    int hit = 0;
    int side;

    if (ray_dir_x < 0) {
        step_x = -1;
        side_dist_x = (player.pos_x - map_x) * delta_dist_x;
    } else {
        step_x = 1;
        side_dist_x = (map_x + 1.0 - player.pos_x) * delta_dist_x;
    }
    if (ray_dir_y < 0) {
        step_y = -1;
        side_dist_y = (player.pos_y - map_y) * delta_dist_y;
    } else {
        step_y = 1;
        side_dist_y = (map_y + 1.0 - player.pos_y) * delta_dist_y;
    }

    while (hit == 0) {
        if (side_dist_x < side_dist_y) {
            side_dist_x += delta_dist_x;
            map_x += step_x;
            side = 0;
        } else {
            side_dist_y += delta_dist_y;
            map_y += step_y;
            side = 1;
        }
        
        if (map_x < 0 || map_x >= MAP_WIDTH || map_y < 0 || map_y >= MAP_HEIGHT) {
            break;
        }
        
        if (worldMap[map_x][map_y] > 0) hit = 1;
    }

    if (hit == 1) {
        double perp_wall_dist;
        if (side == 0) perp_wall_dist = (map_x - player.pos_x + (1 - step_x) / 2) / ray_dir_x;
        else perp_wall_dist = (map_y - player.pos_y + (1 - step_y) / 2) / ray_dir_y;

        int line_height = (int)(SCREEN_HEIGHT / perp_wall_dist);
        int draw_start = -line_height / 2 + SCREEN_HEIGHT / 2;
        if(draw_start < 0) draw_start = 0;
        int draw_end = line_height / 2 + SCREEN_HEIGHT / 2;
        if(draw_end >= SCREEN_HEIGHT) draw_end = SCREEN_HEIGHT - 1;

        // draw ceiling
        SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
        SDL_RenderDrawLine(renderer, x, 0, x, draw_start);
        
        // draw floor
        SDL_SetRenderDrawColor(renderer, 25, 25, 112, 255);
        SDL_RenderDrawLine(renderer, x, draw_end, x, SCREEN_HEIGHT - 1);

        // draw walls
        SDL_SetRenderDrawColor(renderer, 
            side == 1 ? 255 : 0,        // r
            side == 1 ? 0 : 255,        // g
            0,                          // b
            255);                       // a

        SDL_RenderDrawLine(renderer, x, draw_start, x, draw_end);
    }
}

int main(int argc, char* argv[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    
    if (!init(&window, &renderer)) {
        return 1;
    }

    Player player = {1.5, 1.5, 1.0, 0.0, 0.0, 0.66};
    Uint64 last_time = SDL_GetTicks();

    const int FPS = 60;
    const int FRAME_DELAY = 1000 / FPS;

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }        
        }

        Uint64 current_time = SDL_GetTicks64();
        double frame_time = (current_time - last_time) / 1000.0;
        last_time = current_time;

        double move_speed = frame_time * 5.0;
        double rot_speed = frame_time * 3.0;

        const Uint8* keys = SDL_GetKeyboardState(NULL);
        handle_input(keys, player, move_speed, rot_speed);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for(int x = 0; x < SCREEN_WIDTH; x++) {
            cast_ray(renderer, player, x);
        }

        SDL_RenderPresent(renderer);

        // limit framerate
        Uint64 frame_time_ms = SDL_GetTicks64() - current_time;
        if (frame_time_ms < FRAME_DELAY) {
            //std::cout << frame_time_ms << "|" << FRAME_DELAY << std::endl;
            SDL_Delay(FRAME_DELAY - frame_time_ms);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
