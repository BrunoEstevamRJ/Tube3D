# Tube3D

# Tube Renderer com Fonte de Luz (OpenGL)

Este projeto demonstra a renderização de um tubo 3D com OpenGL, utilizando shaders modernos (GLSL) e controles de câmera estilo FPS. Também exibe uma fonte de luz representada por um cubo, que ilumina o tubo através de cálculos de iluminação no shader.

## Funcionalidades

- Renderização de tubo 3D parametrizável (raio interno, externo, altura, segmentos)
- Cálculo de iluminação (ambient, difusa, especular)
- Visualização da fonte de luz através de um cubo branco
- Controle de câmera com teclado e mouse (WASD + mouse para mirar)
- Suporte a textura no tubo (exemplo: `wall.jpg`)
- Código estruturado para fácil expansão

## Controles

- **Mover**: W (frente), S (trás), A (esquerda), D (direita)
- **Subir/Descer**: Espaço (subir), Ctrl (descer)
- **Correr**: Shift
- **Mirar**: Mouse
- **Sair**: Esc

## Como usar

1. **Requisitos**
   - OpenGL 3.3+
   - GLFW
   - GLAD
   - GLM
   - stb_image.h
2. **Compilar**
   - Compile com os arquivos do projeto e as dependências.
   - Exemplo de compilação (Linux):
     ```
     g++ main.cpp -lglfw -ldl -lGL -o TubeRenderer
     ```
3. **Executar**
   - Execute o binário gerado:
     ```
     ./TubeRenderer
     ```
   - Certifique-se de ter a textura `wall.jpg` no mesmo diretório.

## Estrutura do Código

- **main.cpp**: Código principal, shaders, geração do tubo, controle de câmera, renderização.
- **stb_image.h**: Biblioteca para carregar texturas.
- **wall.jpg**: Exemplo de textura usada no tubo.

## Personalização

- Altere `lightPos` para mover a fonte de luz.
- Modifique parâmetros do tubo em `generateTube()`.
- Mude a textura alterando o nome do arquivo em `CreateTexture`.

## Créditos

- Autor: BrunoEstevamRJ
- Base: OpenGL + GLFW + GLAD + GLM + [stb_image](https://github.com/nothings/stb)

## Licença

Este projeto é apenas para fins educacionais.