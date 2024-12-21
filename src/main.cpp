#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>
#include <iostream>

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

class Player {
    public:
    double pos_x = 1.5, pos_y = 1.5;
    double dir_x = 1, dir_y = 0;
    double plane_x = 0, plane_y = 1;
    int base_move_speed = 5, base_rot_speed = 3;

    void move(double dx, double dy, double dt) {
        if (worldMap[int(pos_x + dx * base_move_speed * dt)][int(pos_y)] == 0) pos_x += dx * base_move_speed * dt;
        if (worldMap[int(pos_x)][int(pos_y + dy * base_move_speed * dt)] == 0) pos_y += dy * base_move_speed * dt;
    }

    void turn(int direction, double dt) {
        double rot_speed = direction * base_rot_speed * dt;
        
        double old_dir_x = dir_x;
        dir_x = dir_x * cos(rot_speed) - dir_y * sin(rot_speed);
        dir_y = old_dir_x * sin(rot_speed) + dir_y * cos(rot_speed);
        double old_plane_x = plane_x;
        plane_x = plane_x * cos(rot_speed) - plane_y * sin(rot_speed);
        plane_y = old_plane_x * sin(rot_speed) + plane_y * cos(rot_speed);
    }
    
    void handle_input(const Uint8* keys, double dt) {
        if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) {
            move(dir_x, dir_y, dt);
        }
        if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) {
            move(-dir_x, -dir_y, dt);
        }
        if (keys[SDL_SCANCODE_A]) {
            move(dir_y, -dir_x, dt);
        }
        if (keys[SDL_SCANCODE_D]){
            move(-dir_y, dir_x, dt);
        }
        if (keys[SDL_SCANCODE_RIGHT]) {
            turn(1, dt);
        }
        if (keys[SDL_SCANCODE_LEFT]) {
            turn(-1, dt);
        }
    }
};

class Game {
    int SCREEN_WIDTH = 1280;
    int SCREEN_HEIGHT = 720;
    int RAY_EVERY_N_PIXELS = 4;
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    SDL_Event event;
    Player player;

    void draw_ceiling_and_floor() {
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

    void cast_ray(int x) {
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

    void draw_world(void) {
        draw_ceiling_and_floor();
        for(int x = 0; x < SCREEN_WIDTH + RAY_EVERY_N_PIXELS; x = x + RAY_EVERY_N_PIXELS) {
            // we draw an extra ray here (SCREEN_WIDTH + RAY_EVERY_N_PIXELS) to prevent a gap on the right side of the screen
            cast_ray(x);
        }
    }

    bool init_sdl(void) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            return false;
        }

        window = SDL_CreateWindow(
            "raycaster",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN
        );

        if (window == NULL) {
            SDL_Quit();
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == NULL) {
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }
        return true;
    }
    
    public:
    
    Game() {
        init_sdl();
    }

    ~Game() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void run(void) {
        Uint64 last_time = SDL_GetTicks64();
        running = true;
        while (running) {
            Uint64 current_time = SDL_GetTicks64();
            Uint64 dt = current_time - last_time;
            last_time = current_time;
            update(dt);
            draw();
        }
    }

    void draw(void) {
        SDL_RenderClear(renderer);
        draw_world();
        SDL_RenderPresent(renderer);
    }

    void update(Uint64 dt) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        player.handle_input(keys, dt / 1000.0);
    }
};

int main (void) {
    Game game;
    game.run();
}