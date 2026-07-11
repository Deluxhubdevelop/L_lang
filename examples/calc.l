// ============================================
// CALCULADORA L LANG
// ============================================

print> "======================================"
print> "   🧮 CALCULADORA L LANG"
print> "======================================"
print> ""

// ============================================
// FUNÇÕES
// ============================================

function somar(a, b)
    return a + b
end

function subtrair(a, b)
    return a - b
end

function multiplicar(a, b)
    return a * b
end

function dividir(a, b)
    if b is 0 then
        print> "ERRO: Divisão por zero!"
        return 0
    end
    return a / b
end

function potencia(a, b)
    resultado = 1
    for i = 0, b, 1 then
        resultado = resultado * a
    end
    return resultado
end

function fatorial(n)
    if n is 0 then
        return 1
    end
    if n < 0 then
        print> "ERRO: Fatorial de negativo!"
        return 0
    end
    resultado = 1
    for i = 1, n, 1 then
        resultado = resultado * i
    end
    return resultado
end

// ============================================
// MAIN
// ============================================

print> "Digite o primeiro número: "
input> a
a = tonumber(a)

print> "Digite o segundo número: "
input> b
b = tonumber(b)

print> "Soma: " .. somar(a, b)
print> "Subtração: " .. subtrair(a, b)
print> "Multiplicação: " .. multiplicar(a, b)

if b is 0 then
    print> "Divisão: ERRO (divisão por zero)"
end
if b is not 0 then
    print> "Divisão: " .. dividir(a, b)
end