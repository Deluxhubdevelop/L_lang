#include "../../include/module.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <unordered_map>
#include <string>
#include <cmath>
#include <vector>

static std::unordered_map<int, SDL_Window*> windows;
static std::unordered_map<int, SDL_Renderer*> renderers;
static std::unordered_map<int, TTF_Font*> fonts;
static std::unordered_map<int, SDL_Texture*> textures;
static int window_counter = 0;
static int font_counter = 0;
static int texture_counter = 0;
static bool sdl_initialized = false;

// ============================================
// INICIALIZAÇÃO
// ============================================

void init_sdl() {
    if (!sdl_initialized) {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
        TTF_Init();
        IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
        sdl_initialized = true;
    }
}

void cleanup_sdl() {
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

// ============================================
// UTILITÁRIOS
// ============================================

// Desenha círculo na unha (pontos)
void draw_circle_points(SDL_Renderer* renderer, int cx, int cy, int radius, int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    
    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
            if (x*x + y*y <= radius*radius) {
                SDL_RenderDrawPoint(renderer, cx + x, cy + y);
            }
        }
    }
}

// Desenha retângulo arredondado
void draw_rounded_rect(SDL_Renderer* renderer, int x, int y, int w, int h, int radius, int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    
    // Desenha o retângulo central
    SDL_Rect rect = {x + radius, y, w - 2*radius, h};
    SDL_RenderFillRect(renderer, &rect);
    
    rect = {x, y + radius, w, h - 2*radius};
    SDL_RenderFillRect(renderer, &rect);
    
    // Desenha os 4 cantos arredondados
    for (int angle = 0; angle < 90; angle += 5) {
        double rad = angle * M_PI / 180.0;
        
        // Canto superior esquerdo
        int px = x + radius + (int)(radius * cos(rad + M_PI));
        int py = y + radius + (int)(radius * sin(rad + M_PI));
        SDL_RenderDrawPoint(renderer, px, py);
        
        // Canto superior direito
        px = x + w - radius + (int)(radius * cos(rad - M_PI/2));
        py = y + radius + (int)(radius * sin(rad - M_PI/2));
        SDL_RenderDrawPoint(renderer, px, py);
        
        // Canto inferior esquerdo
        px = x + radius + (int)(radius * cos(rad + M_PI/2));
        py = y + h - radius + (int)(radius * sin(rad + M_PI/2));
        SDL_RenderDrawPoint(renderer, px, py);
        
        // Canto inferior direito
        px = x + w - radius + (int)(radius * cos(rad));
        py = y + h - radius + (int)(radius * sin(rad));
        SDL_RenderDrawPoint(renderer, px, py);
    }
}

// ============================================
// FUNÇÕES DO MÓDULO
// ============================================

extern "C" Module init_module() {
    Module module;
    module.name = "lgrafics";
    module.type = ModuleType::NATIVE;
    
    init_sdl();
    
    // ========================================
    // CREATE WINDOW
    // ========================================
    module.functions["native_create_window"] = [](const std::vector<std::any>& args) -> std::any {
        int width = 800, height = 600;
        std::string title = "L Lang Graphics";
        
        if (args.size() > 0) width = (int)std::any_cast<double>(args[0]);
        if (args.size() > 1) height = (int)std::any_cast<double>(args[1]);
        if (args.size() > 2) title = std::any_cast<std::string>(args[2]);
        
        SDL_Window* window = SDL_CreateWindow(
            title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width, height,
            SDL_WINDOW_SHOWN
        );
        
        if (!window) {
            return -1.0;
        }
        
        SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            SDL_DestroyWindow(window);
            return -1.0;
        }
        
        int id = ++window_counter;
        windows[id] = window;
        renderers[id] = renderer;
        
        return (double)id;
    };
    
    // ========================================
    // DRAW RECT
    // ========================================
    module.functions["native_draw_rect"] = [](const std::vector<std::any>& args) -> std::any {
        if (args.size() < 5) return 0.0;
        
        int id = (int)std::any_cast<double>(args[0]);
        int x = (int)std::any_cast<double>(args[1]);
        int y = (int)std::any_cast<double>(args[2]);
        int w = (int)std::any_cast<double>(args[3]);
        int h = (int)std::any_cast<double>(args[4]);
        
        int r = 255, g = 255, b = 255;
        if (args.size() > 5) r = (int)std::any_cast<double>(args[5]);
        if (args.size() > 6) g = (int)std::any_cast<double>(args[6]);
        if (args.size() > 7) b = (int)std::any_cast<double>(args[7]);
        
        auto it = renderers.find(id);
        if (it == renderers.end()) return 0.0;
        
        SDL_Renderer* renderer = it->second;
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        
        SDL_Rect rect = {x, y, w, h};
        SDL_RenderFillRect(renderer, &rect);
        
        return 1.0;
    };
    
    // ========================================
    // DRAW ROUNDED RECT
    // ========================================
    module.functions["native_draw_rounded_rect"] = [](const std::vector<std::any>& args) -> std::any {
        if (args.size() < 6) return 0.0;
        
        int id = (int)std::any_cast<double>(args[0]);
        int x = (int)std::any_cast<double>(args[1]);
        int y = (int)std::any_cast<double>(args[2]);
        int w = (int)std::any_cast<double>(args[3]);
        int h = (int)std::any_cast<double>(args[4]);
        int radius = (int)std::any_cast<double>(args[5]);
        
        int r = 255, g = 255, b = 255;
        if (args.size() > 6) r = (int)std::any_cast<double>(args[6]);
        if (args.size() > 7) g = (int)std::any_cast<double>(args[7]);
        if (args.size() > 8) b = (int)std::any_cast<double>(args[8]);
        
        auto it = renderers.find(id);
        if (it == renderers.end()) return 0.0;
        
        SDL_Renderer* renderer = it->second;
        draw_rounded_rect(renderer, x, y, w, h, radius, r, g, b);
        
        return 1.0;
    };
    
    // ========================================
    // DRAW RECT BORDER
    // ========================================
    module.functions["native_draw_rect_border"] = [](const std::vector<std::any>& args) -> std::any {
        if (args.size() < 5) return 0.0;
        
        int id = (int)std::any_cast<double>(args[0]);
        int x = (int)std::any_cast<double>(args[1]);
        int y = (int)std::any_cast<double>(args[2]);
        int w = (int)std::any_cast<double>(args[3]);
        int h = (int)std::any_cast<double>(args[4]);
        
        int r = 255, g = 255, b = 255;
        int thickness = 1;
        if (args.size() > 5) r = (int)std::any_cast<double>(args[5]);
        if (args.size() > 6) g = (int)std::any_cast<double>(args[6]);
        if (args.size() > 7) b = (int)std::any_cast<double>(args[7]);
        if (args.size() > 8) thickness = (int)std::any_cast<double>(args[8]);
        
        auto it = renderers.find(id);
        if (it == renderers.end()) return 0.0;
        
        SDL_Renderer* renderer = it->second;
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        
        for (int i = 0; i < thickness; i++) {
            SDL_Rect rect = {x + i, y + i, w - 2*i, h - 2*i};
            SDL_RenderDrawRect(renderer, &rect);
        }
        
        return 1.0;
    };
    
    // ========================================
    // DRAW CIRCLE
    // ========================================
    module.functions["native_draw_circle"] = [](const std::vector<std::any>& args) -> std::any {
        if (args.size() < 4) return 0.0;
        
        int id = (int)std::any_cast<double>(args[0]);
        int cx = (int)std::any_cast<double>(args[1]);
        int cy = (int)std::any_cast<double>(args[2]);
        int radius = (int)std::any_cast<double>(args[3]);
        
        int r = 255, g = 255, b = 255;
        if (args.size() > 4) r = (int)std::any_cast<double>(args[4]);
        if (args.size() > 5) g = (int)std::any_cast<double>(args[5]);
        if (args.size() > 6) b = (int)std::any_cast<double>(args[6]);
        
        auto it = renderers.find(id);
        if (it == renderers.end()) return 0.0;
        
        SDL_Renderer* renderer = it->second;
        draw_circle_points(renderer, cx, cy, radius, r, g, b);
        
        return 1.0;
    };
    
    // ========================================
    // DRAW LINE
    // ========================================
    module.functions["native_draw_line"] = [](const std::vector<std::any>& args) -> std::any {
        if (args.size() < 5) return 0.0;
        
        int id = (int)std::any_cast<double>(args[0]);
        int x1 = (int)std::any_cast<double>(args[1]);
        int y1 = (int)std::any_cast<double>(args[2]);
        int x2 = (int)std::any_cast<double>(args[3]);
        int y2 = (int)std::any_cast<double>(args[4]);
        
        int r = 255, g = 255, b = 255;
        if (args.size() > 5) r = (int)std::any_cast<double>(args[5]);
        if (args.size() > 6) g = (int)std::any_cast<double>(args[6]);
        if (args.size() > 7) b = (int)std::any_cast<double>(args[7]);
        
        auto it = renderers.find(id);
        if (it == renderers.end()) return 0.0;
        
        SDL_Renderer* renderer = it->second;
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        
        return 1.0;
    };
    
    // ========================================
    // SET PIXEL
    // ========================================
    module.functions["native_set_pixel"] = [](const std::vector<std::any>& args) -> std::any {
        if (args.size() < 3) return 0.0;
        
        int id = (int)std::any_cast<double>(args[0]);
        int x = (int)std::any_cast<double>(args[1]);
        int y = (int)std::any_cast<double>(args[2]);
        
        int r = 255, g = 255, b = 255;
        if (args.size() > 3) r = (int)std::any_cast<double>(args[3]);
        if (args.size() > 4) g = (int)std::any_cast<double>(args[4]);
        if (args.size() > 5) b = (int)std::any_cast<double>(args[5]);
        
        auto it = renderers.find(id);
        if (it == renderers.end()) return 0.0;
        
        SDL_Renderer* renderer = it->second;
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawPoint(renderer, x, y);
        
        return 1.0;
    };
    
    // ========================================
    // DRAW TEXT
    // ========================================
    module.functions["native_draw_text"] = [](const std::vector<std::any>& args) -> std::any {
        if (args.size() < 4) return 0.0;
        
        int id = (int)std::any_cast<double>(args[0]);
        int x = (int)std::any_cast<double>(args[1]);
        int y = (int)std::any_cast<double>(args[2]);
        std::string text = std::any_cast<std::string>(args[3]);
        
        int r = 255, g = 255, b = 255;
        int font_size = 24;
        if (args.size() > 4) r = (int)std::any_cast<double>(args[4]);
        if (args.size() > 5) g = (int)std::any_cast<double>(args[5]);
        if (args.size() > 6) b = (int)std::any_cast<double>(args[6]);
        if (args.size() > 7) font_size = (int)std::any_cast<double>(args[7]);
        
        auto it = renderers.find(id);
        if (it == renderers.end()) return 0.0;
        
        SDL_Renderer* renderer = it->second;
        
        // Carrega fonte (cache)
        static TTF_Font* font = nullptr;
        if (!font) {
            // Tenta várias fontes comuns
            const char* font_paths[] = {
                "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                "/usr/share/fonts/truetype/ubuntu/Ubuntu-Regular.ttf",
                "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
                "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf"
            };
            
            for (const char* path : font_paths) {
                font = TTF_OpenFont(path, font_size);
                if (font) break;
            }
            
            if (!font) {
                // Fallback: cria fonte padrão
                font = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", font_size);
                if (!font) return 0.0;
            }
        }
        
        SDL_Color color = {r, g, b, 255};
        SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
        if (!surface) return 0.0;
        
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            SDL_FreeSurface(surface);
            return 0.0;
        }
        
        SDL_Rect dest = {x, y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &dest);
        
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        
        return 1.0;
    };
    
    // ========================================
    // DRAW IMAGE
    // ========================================
    module.functions["native_load_image"] = [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return -1.0;
        
        std::string path = std::any_cast<std::string>(args[0]);
        
        // Usa o primeiro renderer disponível
        if (renderers.empty()) return -1.0;
        SDL_Renderer* renderer = renderers.begin()->second;
        
        SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
        if (!texture) {
            return -1.0;
        }
        
        int id = ++texture_counter;
        textures[id] = texture;
        
        // Pega dimensões da textura
        int w, h;
        SDL_QueryTexture(texture, NULL, NULL, &w, &h);
        
        // Retorna um array com [id, w, h]
        std::vector<std::any> result;
        result.push_back((double)id);
        result.push_back((double)w);
        result.push_back((double)h);
        return result;
    };
    
    module.functions["native_draw_image"] = [](const std::vector<std::any>& args) -> std::any {
        if (args.size() < 3) return 0.0;
        
        int id = (int)std::any_cast<double>(args[0]);
        int x = (int)std::any_cast<double>(args[1]);
        int y = (int)std::any_cast<double>(args[2]);
        int w = 0, h = 0;
        
        // Pega dimensões opcionais
        if (args.size() > 3) w = (int)std::any_cast<double>(args[3]);
        if (args.size() > 4) h = (int)std::any_cast<double>(args[4]);
        
        auto it = textures.find(id);
        if (it == textures.end()) return 0.0;
        
        SDL_Texture* texture = it->second;
        if (!texture) return 0.0;
        
        // Pega dimensões originais se não foram passadas
        if (w == 0 || h == 0) {
            SDL_QueryTexture(texture, NULL, NULL, &w, &h);
        }
        
        SDL_Rect dest = {x, y, w, h};
        SDL_RenderCopy(renderers.begin()->second, texture, NULL, &dest);
        
        return 1.0;
    };
    
    // ========================================
    // CLEAR
    // ========================================
    module.functions["native_clear"] = [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return 0.0;
        
        int id = (int)std::any_cast<double>(args[0]);
        int r = 0, g = 0, b = 0;
        if (args.size() > 1) r = (int)std::any_cast<double>(args[1]);
        if (args.size() > 2) g = (int)std::any_cast<double>(args[2]);
        if (args.size() > 3) b = (int)std::any_cast<double>(args[3]);
        
        auto it = renderers.find(id);
        if (it == renderers.end()) return 0.0;
        
        SDL_Renderer* renderer = it->second;
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderClear(renderer);
        
        return 1.0;
    };
    
    // ========================================
    // RENDER
    // ========================================
    module.functions["native_render"] = [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return 0.0;
        
        int id = (int)std::any_cast<double>(args[0]);
        auto it = renderers.find(id);
        if (it == renderers.end()) return 0.0;
        
        SDL_Renderer* renderer = it->second;
        SDL_RenderPresent(renderer);
        
        return 1.0;
    };
    
    // ========================================
    // POLL EVENTS
    // ========================================
    module.functions["native_poll_events"] = [](const std::vector<std::any>& args) -> std::any {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return 1.0;
            }
        }
        return 0.0;
    };
    
    // ========================================
    // QUIT
    // ========================================
    module.functions["native_quit"] = [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return 0.0;
        
        int id = (int)std::any_cast<double>(args[0]);
        auto it = windows.find(id);
        if (it == windows.end()) return 0.0;
        
        SDL_DestroyRenderer(renderers[id]);
        SDL_DestroyWindow(windows[id]);
        windows.erase(id);
        renderers.erase(id);
        
        return 1.0;
    };
    
    // ========================================
    // DELAY
    // ========================================
    module.functions["native_delay"] = [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return 0.0;
        int ms = (int)std::any_cast<double>(args[0]);
        SDL_Delay(ms);
        return 0.0;
    };
    
    // ========================================
    // GET WINDOW WIDTH/HEIGHT
    // ========================================
    module.functions["native_get_window_width"] = [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return 0.0;
        int id = (int)std::any_cast<double>(args[0]);
        auto it = windows.find(id);
        if (it == windows.end()) return 0.0;
        
        int w, h;
        SDL_GetWindowSize(it->second, &w, &h);
        return (double)w;
    };
    
    module.functions["native_get_window_height"] = [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return 0.0;
        int id = (int)std::any_cast<double>(args[0]);
        auto it = windows.find(id);
        if (it == windows.end()) return 0.0;
        
        int w, h;
        SDL_GetWindowSize(it->second, &w, &h);
        return (double)h;
    };
    
    return module;
}