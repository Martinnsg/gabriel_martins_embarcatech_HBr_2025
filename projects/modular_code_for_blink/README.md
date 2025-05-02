# Modular Blink - Raspberry Pi Pico W

## 📚 Objetivo
Reestruturar o código blink seguindo a arquitetura modular recomendada.

## 📂 Estrutura do Repositório

```
/modular_code_for_blink
│── /build         # Diretório de compilação (gerado pelo CMake)
│── /app           # Código-fonte principal (.c)
│── /include       # Cabeçalhos (.h, .hpp)
│── /drivers       # Controle direto do Hardware    
│── /hal           # Abstração de hardware com funções simples e reutilizáveis.
│── CMakeLists.txt # Configuração do projeto CMake
│── pico_sdk_import.cmake # Importação do SDK da Raspberry Pi
│── README.md      # Documentação do projeto
```
## 👤 Autor
**Gabriel Martins - Hbr Campinas**


