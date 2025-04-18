# Contador Decrescente

## 📚 Objetivo
Fazer um programa, em linguagem C, que implemente um contador decrescente controlado por interrupção, com o seguinte comportamento:
   -O contador decrescente reinicia em 9 e o valor da contagem é mostrado no display OLED.
   -O sistema entra em modo de contagem regressiva ativa, decrementando o contador de 1 em 1 a cada segundo até chegar em zero.
   -Durante essa contagem (ou seja, de 9 até 0), o programa deve registrar quantas vezes o Botão B (GPIO6) foi pressionado. 
   -Quando o contador atingir zero, o sistema congela e ignora temporariamente os cliques no Botão B (eles não devem ser acumulados fora do intervalo ativo).

## 📂 Estrutura do Repositório

```
/countdown_counter
│── /build         # Diretório de compilação (gerado pelo CMake)
│── /src           # Código-fonte principal (.c)
│── /include       # Cabeçalhos (.h, .hpp)
│── /lib           # Bibliotecas externas para o Display OLED
│── CMakeLists.txt # Configuração do projeto CMake
│── pico_sdk_import.cmake # Importação do SDK da Raspberry Pi
│── README.md      # Documentação do projeto
```
## 👤 Autor
**Gabriel Martins - Hbr Campinas**


