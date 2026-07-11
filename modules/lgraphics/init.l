// ============================================
// Módulo lgrafics - Biblioteca Gráfica
// ============================================

print> "🎨 Carregando lgrafics..."

// ============================================
// CONSTANTES
// ============================================

COLOR_BLACK = {0, 0, 0}
COLOR_WHITE = {255, 255, 255}
COLOR_RED = {255, 0, 0}
COLOR_GREEN = {0, 255, 0}
COLOR_BLUE = {0, 0, 255}
COLOR_YELLOW = {255, 255, 0}
COLOR_CYAN = {0, 255, 255}
COLOR_MAGENTA = {255, 0, 255}

// ============================================
// FUNÇÕES WRAPPER
// ============================================

function create_window(width, height, title)
    if title is nil then
        title = "L Lang Graphics"
    end
    return native_create_window(width, height, title)
end

function draw_rect(window, x, y, w, h, color)
    if color is nil then
        color = COLOR_WHITE
    end
    return native_draw_rect(window, x, y, w, h, color[0], color[1], color[2])
end

function draw_rounded_rect(window, x, y, w, h, radius, color)
    if color is nil then
        color = COLOR_WHITE
    end
    if radius is nil then
        radius = 10
    end
    return native_draw_rounded_rect(window, x, y, w, h, radius, color[0], color[1], color[2])
end

function draw_rect_border(window, x, y, w, h, color, thickness)
    if color is nil then
        color = COLOR_WHITE
    end
    if thickness is nil then
        thickness = 1
    end
    return native_draw_rect_border(window, x, y, w, h, color[0], color[1], color[2], thickness)
end

function draw_circle(window, center_x, center_y, radius, color)
    if color is nil then
        color = COLOR_WHITE
    end
    return native_draw_circle(window, center_x, center_y, radius, color[0], color[1], color[2])
end

function draw_line(window, x1, y1, x2, y2, color)
    if color is nil then
        color = COLOR_WHITE
    end
    return native_draw_line(window, x1, y1, x2, y2, color[0], color[1], color[2])
end

function set_pixel(window, x, y, color)
    if color is nil then
        color = COLOR_WHITE
    end
    return native_set_pixel(window, x, y, color[0], color[1], color[2])
end

function draw_text(window, x, y, text, color, size)
    if color is nil then
        color = COLOR_WHITE
    end
    if size is nil then
        size = 24
    end
    return native_draw_text(window, x, y, text, color[0], color[1], color[2], size)
end

function load_image(path)
    result = native_load_image(path)
    // result é {id, width, height}
    return {
        id = result[0],
        w = result[1],
        h = result[2]
    }
end

function draw_image(window, image, x, y, w, h)
    if w is nil or h is nil then
        w = image.w
        h = image.h
    end
    return native_draw_image(image.id, x, y, w, h)
end

function clear(window, color)
    if color is nil then
        color = COLOR_BLACK
    end
    return native_clear(window, color[0], color[1], color[2])
end

function render(window)
    return native_render(window)
end

function poll_events()
    return native_poll_events()
end

function quit(window)
    return native_quit(window)
end

function delay(ms)
    return native_delay(ms)
end

function get_window_width(window)
    return native_get_window_width(window)
end

function get_window_height(window)
    return native_get_window_height(window)
end

// ============================================
// EXPORTA FUNÇÕES NATIVAS
// ============================================

native_create_window = nil
native_draw_rect = nil
native_draw_rounded_rect = nil
native_draw_rect_border = nil
native_draw_circle = nil
native_draw_line = nil
native_set_pixel = nil
native_draw_text = nil
native_load_image = nil
native_draw_image = nil
native_clear = nil
native_render = nil
native_poll_events = nil
native_quit = nil
native_delay = nil
native_get_window_width = nil
native_get_window_height = nil

print> "✅ lgrafics carregado com sucesso!"
print> "   Use grafics.create_window() para criar uma janela"