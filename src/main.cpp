#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <algorithm>
#include <cmath>

constexpr int MAP_WIDTH = 20;
constexpr int MAP_HEIGHT = 20;
constexpr int worldMap[MAP_WIDTH][MAP_HEIGHT] = {
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

int main(int argc, char* argv[]) {
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;
    const int RAY_EVERY_N_PIXELS = 2;

    if (!SDL_Init(SDL_INIT_VIDEO)) return -1;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    SDL_CreateWindowAndRenderer("raycaster", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer);

    double pos_x = 1.5, pos_y = 1.5;
    double dir_x = 1.0, dir_y = 0.0;
    double plane_x = 0.0, plane_y = 0.66;
    double base_move_speed = 5.0, base_rot_speed = 3.0;

    bool running = true;
    Uint64 last_time = SDL_GetTicks();

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        Uint64 current_time = SDL_GetTicks();
        double dt = (current_time - last_time) / 1000.0;
        last_time = current_time;

        const bool* keys = SDL_GetKeyboardState(NULL);

        auto move = [&](double dx, double dy) {
            double next_x = pos_x + dx * base_move_speed * dt;
            double next_y = pos_y + dy * base_move_speed * dt;
            if (worldMap[static_cast<int>(next_x)][static_cast<int>(pos_y)] == 0) pos_x = next_x;
            if (worldMap[static_cast<int>(pos_x)][static_cast<int>(next_y)] == 0) pos_y = next_y;
        };

            if (keys[SDL_SCANCODE_W]) move(dir_x, dir_y);
            if (keys[SDL_SCANCODE_S]) move(-dir_x, -dir_y);
            if (keys[SDL_SCANCODE_A]) move(-plane_x, -plane_y);
            if (keys[SDL_SCANCODE_D]) move(plane_x, plane_y);

            double rot = 0.0;
        if (keys[SDL_SCANCODE_LEFT]) rot = -base_rot_speed * dt;
        if (keys[SDL_SCANCODE_RIGHT]) rot = base_rot_speed * dt;

        if (std::abs(rot) > 0.0) {
            double old_dir_x = dir_x;
            dir_x = dir_x * std::cos(rot) - dir_y * std::sin(rot);
            dir_y = old_dir_x * std::sin(rot) + dir_y * std::cos(rot);
            double old_plane_x = plane_x;
            plane_x = plane_x * std::cos(rot) - plane_y * std::sin(rot);
            plane_y = old_plane_x * std::sin(rot) + plane_y * std::cos(rot);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int y = 0; y < SCREEN_HEIGHT / 2; ++y) {
            float t = static_cast<float>(y) / (static_cast<float>(SCREEN_HEIGHT) / 2.0f);
            uint8_t color = static_cast<uint8_t>(50.0f * (1.0f - t));
            SDL_SetRenderDrawColor(renderer, color, color, color, 255);
            SDL_RenderLine(renderer, 0, y, SCREEN_WIDTH, y);
        }

        for (int y = SCREEN_HEIGHT / 2; y < SCREEN_HEIGHT; ++y) {
            float t = (static_cast<float>(y) - static_cast<float>(SCREEN_HEIGHT) / 2.0f) / (static_cast<float>(SCREEN_HEIGHT) / 2.0f);
            uint8_t color = static_cast<uint8_t>(150.0f * t);
            SDL_SetRenderDrawColor(renderer, color, color, color, 255);
            SDL_RenderLine(renderer, 0, y, SCREEN_WIDTH, y);
        }

        // Raycasting
        for (int x = 0; x < SCREEN_WIDTH; x += RAY_EVERY_N_PIXELS) {
            double camera_x = 2.0 * x / static_cast<double>(SCREEN_WIDTH) - 1.0;
            double ray_dir_x = dir_x + plane_x * camera_x;
            double ray_dir_y = dir_y + plane_y * camera_x;

            int map_x = static_cast<int>(pos_x);
            int map_y = static_cast<int>(pos_y);

            double delta_dist_x = (ray_dir_x == 0) ? 1e30 : std::abs(1.0 / ray_dir_x);
            double delta_dist_y = (ray_dir_y == 0) ? 1e30 : std::abs(1.0 / ray_dir_y);
            double side_dist_x, side_dist_y;
            int step_x, step_y;
            int hit = 0, side = 0;

            if (ray_dir_x < 0) { step_x = -1; side_dist_x = (pos_x - map_x) * delta_dist_x; }
            else { step_x = 1; side_dist_x = (map_x + 1.0 - pos_x) * delta_dist_x; }
            if (ray_dir_y < 0) { step_y = -1; side_dist_y = (pos_y - map_y) * delta_dist_y; }
            else { step_y = 1; side_dist_y = (map_y + 1.0 - pos_y) * delta_dist_y; }

            while (hit == 0) {
                if (side_dist_x < side_dist_y) { side_dist_x += delta_dist_x; map_x += step_x; side = 0; }
                else { side_dist_y += delta_dist_y; map_y += step_y; side = 1; }
                if (map_x >= 0 && map_x < MAP_WIDTH && map_y >= 0 && map_y < MAP_HEIGHT) {
                    if (worldMap[map_x][map_y] > 0) hit = 1;
                } else break;
            }

            if (hit) {
                double perp_wall_dist = (side == 0) ? (side_dist_x - delta_dist_x) : (side_dist_y - delta_dist_y);
                int line_h = static_cast<int>(SCREEN_HEIGHT / perp_wall_dist);
                int d_start = std::max(0, -line_h / 2 + SCREEN_HEIGHT / 2);
                int d_end = std::min(SCREEN_HEIGHT - 1, line_h / 2 + SCREEN_HEIGHT / 2);

                float brightness = std::clamp(1.0f - static_cast<float>(perp_wall_dist / 20.0), 0.1f, 1.0f);
                float color_scale = (side == 1) ? 0.7f : 1.0f;
                uint8_t r = static_cast<uint8_t>(255 * brightness * color_scale);

                SDL_SetRenderDrawColor(renderer, r, 0, 0, 255);
                SDL_FRect wall_rect = { static_cast<float>(x), static_cast<float>(d_start),
                    static_cast<float>(RAY_EVERY_N_PIXELS), static_cast<float>(d_end - d_start) };
                    SDL_RenderFillRect(renderer, &wall_rect);
            }
        }
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
