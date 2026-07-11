// ============================================
// TESTE DO MÓDULO LUTILS
// ============================================

print> "=== TESTE LUTILS ==="
print> ""

utils = import "lutils"

numeros = {1, 2, 3, 4, 5}
print> "Números: " .. utils.table_join(numeros)

print> "Soma: " .. utils.sum(numeros)
print> "Média: " .. utils.average(numeros)
print> "Máximo: " .. utils.max(numeros)
print> "Mínimo: " .. utils.min(numeros)