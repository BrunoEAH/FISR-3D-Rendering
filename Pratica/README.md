# opengl-quake

Um visualizador simples de mapas do Quake em OpenGL, originalmente criado por Johan Gardhage e posteriormente aprimorado por Bruno Hayek.

O principal objetivo deste fork é renderizar o visualizador de mapas do Quake utilizando o Fast Inverse Square Root, um método de cálculo tradicional ou uma aproximação por série de Taylor. Mais detalhes estão disponíveis no seguinte repositório: [https://github.com/BrunoEAH/FISR-3D-Rendering](https://github.com/BrunoEAH/FISR-3D-Rendering). Também aprimoramos o arquivo de build para que ele gere um executável para Windows. Por fim, adaptamos o código para C++20 e adicionamos uma janela para visualizar o benchmark de cada método de cálculo.


![Screenshot](screenshots/quake.png "quake")

![GIF](screenshots/lv_0_20250524193135.gif "quake")


## Pré-requisitos

Para compilar o opengl-quake, você deve primeiro instalar as seguintes ferramentas:

- [SDL 2.0.4](https://www.libsdl.org/download-2.0.php) (or later)
- [GLU](https://en.wikipedia.org/wiki/OpenGL_Utility_Library)
- [Ninja 1.5](https://ninja-build.org/) (or later)
- C++20

Também utilizamos o ImGUI ([https://github.com/ocornut/imgui](https://github.com/ocornut/imgui)), mas seus arquivos estão localizados na pasta third\_party.


### Instalar dependências

#### openSUSE

`$ sudo zypper install ninja gcc-c++ libSDL2-devel glu-devel`

#### Ubuntu

`$ sudo apt install ninja-build g++ libsdl2-dev libglu1-mesa-dev`

## Instruções de compilação

Para compilar o programa de demonstração do Quake, execute:

`$ ninja`

Um diretório `build` será criado, contendo o programa de demonstração.

## Uso

```
Uso: quake [OPÇÃO]...

Opções:
 -h, --help         Exibe este texto e sai
 -w, --window       Renderiza em uma janela
     --fullwindow   Renderiza em uma janela em tela cheia
 -f, --fullscreen   Renderiza em tela cheia
 -c, --showcursor   Mostra o cursor do mouse
     --nocursor     Esconde o cursor do mouse
Pressione F para ativar o algoritmo FISR
Pressione T para ativar o algoritmo tradicional
```


## Licença

Licenciado sob a licença MIT. Veja [LICENSE](LICENSE) para mais informações.

## Autores

* Johan Gardhage
* Bruno Hayek
* Gustavo Ciola
* Caio Alexandre de Andrade
* Nicolas Melnik
