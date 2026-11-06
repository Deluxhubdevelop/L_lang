# L_lang 🚀

![Status](https://img.shields.io/badge/Status-Em%20Desenvolvimento-orange)
![Linguagem](https://img.shields.io/badge/Backend-C%2B%2B-blue)
![Sintaxe](https://img.shields.io/badge/Sintaxe-Lua%20%2B%20C%2B%2B-purple)
![Plataforma](https://img.shields.io/badge/Plataforma-Linux%20(Atual)-lightgrey)

**L_lang** é uma linguagem de programação de baixo nível, escrita inteiramente em C++, que combina a legibilidade da sintaxe do Lua com o poder e o controle de memória do C++. O objetivo final é criar uma linguagem capaz de rodar em sistemas embarcados e, futuramente, servir como base para o desenvolvimento de um sistema operacional (OS).

> **Nota de Desenvolvimento:** Este projeto está em construção ativa e foi auxiliado pela IA DeepSeek. Algumas funcionalidades podem estar incompletas ou em fase de testes.

## 🛠️ Arquitetura da Toolchain

A L_lang não é apenas um interpretador; o objetivo é construir um ecossistema completo de compilação:

*   **`.l` (Fonte):** Código escrito na sintaxe da L_lang (Lua-like + C++).
*   **`.xar` (Xess Assembly Rang):** Linguagem intermediária (IR) gerada pelo compilador, legível por humanos.
*   **`.xb` (Xess Binary):** Binário final (Flat Binary) pronto para ser executado. O Header mágico contém a string: `ilove xess, its my fav medal` (porque sim).
*   **`.xl` (Xess Library):** Bibliotecas do sistema.
    *   `nomono.xl`: Biblioteca **estática** (runtime embutido no executável).
    *   `il0l.xl`: Biblioteca **dinâmica** (carregada em tempo de execução).

## 📝 Exemplo de Sintaxe (WIP)

```lua
-- Alocação estática na stack
local x = 10

-- Ponteiro (C++-like)
local p = ptr x

-- Escrevendo na memória
p^ = 20

-- Struct (Futuro)
struct Player do
    local hp = 100
    function take_damage(amount)
        self.hp = self.hp - amount
    end
end
