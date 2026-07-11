// Módulo lutils - Utilitários

// Exporta funções
function table_contains(tabela, valor)
    for i = 0, #tabela - 1, 1 then
        if tabela[i] is valor then
            return true
        end
    end
    return false
end

function table_join(tabela, separador)
    if separador is nil then
        separador = ", "
    end
    resultado = ""
    for i = 0, #tabela - 1, 1 then
        if i > 0 then
            resultado = resultado .. separador
        end
        resultado = resultado .. tabela[i]
    end
    return resultado
end

function map(tabela, func)
    resultado = {}
    for i = 0, #tabela - 1, 1 then
        resultado[i] = func(tabela[i])
    end
    return resultado
end

function filter(tabela, func)
    resultado = {}
    idx = 0
    for i = 0, #tabela - 1, 1 then
        if func(tabela[i]) then
            resultado[idx] = tabela[i]
            idx = idx + 1
        end
    end
    return resultado
end

function reduce(tabela, func, inicial)
    acumulador = inicial
    for i = 0, #tabela - 1, 1 then
        acumulador = func(acumulador, tabela[i])
    end
    return acumulador
end

// Funções matemáticas nativas (implementadas em C++)
// random(min, max)
// sleep(ms)

print> "lutils carregado com sucesso!"