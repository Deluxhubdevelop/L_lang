local grafics = import("lgrafics")

print> "Criando janela..."
window = grafics.create_window(800, 600, "Teste LGraphics")

print> "Desenhando..."
grafics.clear(window, grafics.COLOR_BLACK)

// Retângulo vermelho
grafics.draw_rect(window, 100, 100, 200, 150, grafics.COLOR_RED)

// Retângulo arredondado azul
grafics.draw_rounded_rect(window, 350, 100, 200, 150, 20, grafics.COLOR_BLUE)

// Círculo amarelo
grafics.draw_circle(window, 200, 350, 80, grafics.COLOR_YELLOW)

// Linha verde
grafics.draw_line(window, 400, 300, 600, 450, grafics.COLOR_GREEN)

// Texto branco
grafics.draw_text(window, 100, 500, "Olá, L Lang!", grafics.COLOR_WHITE, 32)
grafics.draw_text(window, 100, 540, "Isso é um texto na tela", {200, 200, 200}, 20)

grafics.render(window)

print> "Janela aberta. Clique no X para fechar."

while true then
    if grafics.poll_events() is 1 then
        break
    end
    grafics.delay(16)
end

grafics.quit(window)
print> "Fechou!"