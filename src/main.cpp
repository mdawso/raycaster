#include <SDL2/SDL.h>
#include <cmath>
#include <algorithm>
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int RAY_EVERY_N_PIXELS = 2;

const int MAP_WIDTH = 20;
const int MAP_HEIGHT = 20;
const int worldMap[MAP_WIDTH][MAP_HEIGHT] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

double pos_x = 1.5, pos_y = 1.5;
double dir_x = 1.0, dir_y = 0.0;
double plane_x = 0.0, plane_y = 1.0;

void look(double rot_speed) {
    double old_dir_x = dir_x;
    dir_x = dir_x * cos(-rot_speed) - dir_y * sin(-rot_speed);
    dir_y = old_dir_x * sin(-rot_speed) + dir_y * cos(-rot_speed);
    double old_plane_x = plane_x;
    plane_x = plane_x * cos(-rot_speed) - plane_y * sin(-rot_speed);
    plane_y = old_plane_x * sin(-rot_speed) + plane_y * cos(-rot_speed);
}

void strafe(double strafe_x, double strafe_y, double move_speed) {
    if (worldMap[int(pos_x + strafe_x * move_speed)][int(pos_y)] == 0) 
        pos_x += strafe_x * move_speed;
    if (worldMap[int(pos_x)][int(pos_y + strafe_y * move_speed)] == 0) 
        pos_y += strafe_y * move_speed;
}

void handle_input(const Uint8* keys, double move_speed, double rot_speed) {
    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) {
        strafe(dir_x, dir_y, move_speed);
    }
    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) {
        strafe(-dir_x, -dir_y, move_speed);
    }
    if (keys[SDL_SCANCODE_A]) {
        strafe(dir_y, -dir_x, move_speed);
    }
    if (keys[SDL_SCANCODE_D]){
        strafe(-dir_y, dir_x, move_speed);
    }
    if (keys[SDL_SCANCODE_RIGHT]) {
        look(-rot_speed);
    }
    if (keys[SDL_SCANCODE_LEFT]) {
        look(rot_speed);
    }
}

void cast_ray(SDL_Renderer* renderer, int x) {
    double camera_x = 2 * x / double(SCREEN_WIDTH) - 1;
    double ray_dir_x = dir_x + plane_x * camera_x;
    double ray_dir_y = dir_y + plane_y * camera_x;

    int map_x = int(pos_x);
    int map_y = int(pos_y);

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
        side_dist_x = (pos_x - map_x) * delta_dist_x;
    } else {
        step_x = 1;
        side_dist_x = (map_x + 1.0 - pos_x) * delta_dist_x;
    }
    if (ray_dir_y < 0) {
        step_y = -1;
        side_dist_y = (pos_y - map_y) * delta_dist_y;
    } else {
        step_y = 1;
        side_dist_y = (map_y + 1.0 - pos_y) * delta_dist_y;
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
        if (side == 0) perp_wall_dist = (map_x - pos_x + (1 - step_x) / 2) / ray_dir_x;
        else perp_wall_dist = (map_y - pos_y + (1 - step_y) / 2) / ray_dir_y;

        int line_height = (int)(SCREEN_HEIGHT / perp_wall_dist);
        int draw_start = -line_height / 2 + SCREEN_HEIGHT / 2;
        if(draw_start < 0) draw_start = 0;
        int draw_end = line_height / 2 + SCREEN_HEIGHT / 2;
        if(draw_end >= SCREEN_HEIGHT) draw_end = SCREEN_HEIGHT - 1;

        float r = side == 1 ? 1.0f : 0.6f;
        float g = side == 1 ? 0.0f : 0.0f;
        float b = 0;

        float brightness = 1.0f;

        float min_dist = 0.0f;
        float max_dist = 30.0f;
        float dist_b = (perp_wall_dist - min_dist) / (max_dist - min_dist);
        dist_b = 1.0f - std::sqrt(std::clamp(dist_b, 0.0f, 1.0f));
        brightness *= dist_b;

        r *= brightness; g *= brightness; b *= brightness;

        SDL_SetRenderDrawColor(renderer, 
            uint8_t(r * 255.0f),
            uint8_t(g * 255.0f),
            uint8_t(b * 255.0f),
            255);

        if (RAY_EVERY_N_PIXELS >= 2) {
            SDL_Rect current_rect = {x - RAY_EVERY_N_PIXELS / 2, draw_start, RAY_EVERY_N_PIXELS, draw_end - draw_start};
            SDL_RenderFillRect(renderer, &current_rect);
        } else {
            SDL_RenderDrawLine(renderer, x, draw_start, x, draw_end);
        }
    }
}

void draw_ceiling_and_floor(SDL_Renderer* renderer) {
    // draw ceiling with gradient
    for (int y = 0; y < SCREEN_HEIGHT / 2; ++y) {
        float t = float(y) / (SCREEN_HEIGHT / 2);
        uint8_t colour = uint8_t(50 * (1 - t));
        SDL_SetRenderDrawColor(renderer, colour, colour, colour, 255);
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }

    // draw floor with gradient
    for (int y = SCREEN_HEIGHT / 2; y < SCREEN_HEIGHT; ++y) {
        float t = float(y - SCREEN_HEIGHT / 2) / (SCREEN_HEIGHT / 2);
        uint8_t colour = uint8_t(150 * t);
        SDL_SetRenderDrawColor(renderer, colour, colour, colour, 255);
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }
}

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

int main(int argc, char* argv[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    
    if (!init(&window, &renderer)) {
        return 1;
    }

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
        
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        double move_speed = frame_time * 5.0;
        double rot_speed = frame_time * 3.0;
        handle_input(keys, move_speed, rot_speed);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        draw_ceiling_and_floor(renderer);
        for(int x = 0; x < SCREEN_WIDTH + RAY_EVERY_N_PIXELS; x = x + RAY_EVERY_N_PIXELS) {
            // we draw an extra ray here (SCREEN_WIDTH + RAY_EVERY_N_PIXELS) to prevent a gap on the right side of the screen
            cast_ray(renderer, x);
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
