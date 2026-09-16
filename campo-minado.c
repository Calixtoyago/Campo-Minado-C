// gcc campo-minado.c -o campo-minado.exe -IC:/raylib/raylib/src -LC:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"
#include <unistd.h>


void contar_minas(int minas_max, int campo[16][16]) {
    int vizinhos[9][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1}, {0, 0}, {0, 1}, // {0, 0} e a bomba, os outros sao os indices dos campos vizinhos
        {1, -1}, {1, 0}, {1, 1}
    };
    
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            if (campo[i][j] == 9) {
                for (int row = 0; row < 9; row++) {
                    int ni = i + vizinhos[row][0];
                    int nj = j + vizinhos[row][1];

                    if ((ni >= 0 && ni < 16) && (nj >= 0 && nj < 16)) {
                        if (campo[ni][nj] != 9) {
                            campo[ni][nj]++;
                        }
                    }
                }
            }
        }
    }
    return;
}

void gerar_mapa(int row, int col, int minas_max, int campo[16][16], char mascara[16][16]) {

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            campo[i][j] = 0;
        }
    }

    int minas_contador = 0;
    srand(time(NULL));

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            mascara[i][j] = '*';
        }
    }

    while (minas_contador < minas_max) {
        int row = rand() % 16; // pega o resto da divisão do intervalo dado e depois soma com o menor numero 
        int col = rand() % 16;

        
        if (campo[row][col] == 0) {
            campo[row][col] = 9;
            minas_contador++;
        }
    }

    return contar_minas(minas_max, campo);
}

// AS FUNCOES ABAIXO FORAM SUBSTITUIDAS PELA INTERFACE
// void mostrar_campo(int row, int col, int campo[16][16]) {
//     for (int i = 0; i < row; i++) {
//         for (int j = 0; j < col; j++) {
//             printf("%d ", campo[i][j]);
//         }
//         printf("\n");
//     }
// }

// void mostrar_mascara(int row, int col, char mascara[16][16]) {
//     for (int i = 0; i < row; i++) {
//         for (int j = 0; j < col; j++) {
//             printf("%c ", mascara[i][j]);
//         }
//         printf("\n");
//     }
// }

void abrir_mapa(int row, int col, int campo[16][16], char mascara[16][16], int x, int y) {
    // verificar limites da matriz
    if ((x < 0 || x >= row) || (y < 0 || y >= col)) {
        return;
    }

    // se for bomba nao faz nada
    if (campo[x][y] == 9) {
        return;
    }

    if (mascara[x][y] == campo[x][y]+'0') { // verificar se a celula ja foi mostrada pra evitar loop
        return;
    } else {
        mascara[x][y] = campo[x][y] + '0'; // toda celula que chegar aqui nao sera 9 nem ja tera sido mostrada
        if (campo[x][y] == 0) { 
            mascara[x][y] = campo[x][y] + '0';
            abrir_mapa(row, col, campo, mascara, x-1, y-1);
            abrir_mapa(row, col, campo, mascara, x-1, y);
            abrir_mapa(row, col, campo, mascara, x-1, y+1);
            abrir_mapa(row, col, campo, mascara, x, y-1);
            abrir_mapa(row, col, campo, mascara, x, y+1);
            abrir_mapa(row, col, campo, mascara, x+1, y-1);
            abrir_mapa(row, col, campo, mascara, x+1, y);
            abrir_mapa(row, col, campo, mascara, x+1, y+1);
        }
    }

    return;
}

void draw_final(char *s, int y, int fontsize, int screenWidth, Color color) {
    int largura_texto = MeasureText(s, fontsize);
    int x = (screenWidth - largura_texto) / 2;

    DrawText(s, x, y, fontsize, color);
    DrawText("Press ENTER", 65, 320, 65, ORANGE);
    DrawText("to play again", 83, 385, 65, ORANGE);
}

int main(void) {

    int minas_max = 40; // numero de minas

    int row = 16; // numero de linhas
    int col = 16; // numero de colunas

    int campo[16][16]; // campo inicializado na funcao gerar_mapa()

    // A GERACAO DA MASCARA ESTA DENTRO DA FUNCAO gerar_mapa()
    char mascara[16][16];

    gerar_mapa(row, col, minas_max, campo, mascara);
    
    const int TAMANHO = 32;
    const int ESPACO = 4;
    const int PASSO = TAMANHO + ESPACO;
    const int PASSO_ALTURA = PASSO * 2;
    const int screenWidth = PASSO * 16; // x
    const int screenHeight = PASSO * 16 + PASSO_ALTURA; // y
    bool perdeu = false;
    bool ganhou = false;
    int contador_bombas;
    Color cor_quadrado;
    Color cor_numero;
    
    InitWindow(screenWidth, screenHeight, "Campo Minado");
    
    SetTargetFPS(30);
    
    while (!WindowShouldClose()) {

        Vector2 mousePosition = GetMousePosition();
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !(ganhou == true || perdeu == true)) {
            int x = mousePosition.x;
            int y = mousePosition.y - PASSO_ALTURA;
            int i = x / PASSO;
            int j = y / PASSO;
            printf("Clique - (%d, %d) - [%d][%d]\n", x, y, i, j);

            if (campo[i][j] == 9 && mascara[i][j] != '#') {
                mascara[i][j] = campo[i][j]+'0';
                perdeu = true;
                sleep(2.5);
            } else if (mascara[i][j] == '#') {
                ;
            } else {
                contador_bombas = 256;
                abrir_mapa(row, col, campo, mascara, i, j);

                for (int i = 0; i < 16; i++) {
                    for (int j = 0; j < 16; j++) {
                        if (mascara[i][j] != '*' && mascara[i][j] != '#' ) {
                            contador_bombas -= 1;
                        }
                    }
                }
                
                if (contador_bombas == 40) {
                    ganhou = true;
                    sleep(2.5);
                }
            }           
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !(ganhou == true || perdeu == true)) {
            int x = mousePosition.x;
            int y = mousePosition.y - PASSO_ALTURA;
            int i = x / PASSO;
            int j = y / PASSO;
            printf("Bandeira - (%d, %d) - [%d][%d]\n", x, y, i, j);

            if (mascara[i][j] == '*') {
                mascara[i][j] = '#';
            } else if (mascara[i][j] == '#') {
                mascara[i][j] = '*';
            }
        }

        // REINICIAR O JOGO
        if (IsKeyPressed(KEY_ENTER) && (ganhou == true || perdeu == true)) {
            printf("------ FIM DE JOGO ---------\n");
            ganhou = false;
            printf("Mudou ganhou para false\n");
            perdeu = false;
            // perdeu_trigger = false;
            printf("Mudou perdeu para false\n");
            gerar_mapa(row, col, minas_max, campo, mascara);
            // ShowCursor();
            printf("-------- GEROU O MAPA ---------\n");
        }

        BeginDrawing();

            ClearBackground(BLACK); 

            if (perdeu == false && ganhou == false) {
                for (int i = 0; i < 16; i++) {
                    for (int j = 0; j < 16; j++) {
                        if (mascara[i][j] != '*' && mascara[i][j] != '#') {
                            cor_quadrado = WHITE;
                        } else if (mascara[i][j] == '#') {
                            cor_quadrado = RED;
                        } else {
                            cor_quadrado = GRAY;
                        }
                        DrawRectangle(
                            i * PASSO,
                            j * PASSO + PASSO_ALTURA,
                            TAMANHO,
                            TAMANHO,
                            cor_quadrado
                        );
                
                        if (mascara[i][j] != '*' && mascara[i][j] != '#') {
                            char valor[2];
                            valor[0] = mascara[i][j];
                            valor[1] = '\0';

                            int valor_int = mascara[i][j]-'0';
                            switch (valor_int)
                            {
                            case 0: cor_numero = BLACK; break;
                            case 1: cor_numero = BLUE; break;
                            case 2: cor_numero = GREEN; break;
                            case 3: cor_numero = RED; break;
                            case 4: cor_numero = DARKBLUE; break;
                            case 5: cor_numero = BROWN; break;
                            case 6: cor_numero = LIME; break;
                            case 7: cor_numero = PURPLE; break;
                            case 8: cor_numero = GRAY; break;
                            case 9: cor_numero = ORANGE; break;
                            }

                            DrawText(valor, i * PASSO + 6, j * PASSO + 2 + PASSO_ALTURA, 32, cor_numero);
                        }
                    }
                }
            } else if (perdeu == true) {
                draw_final("GAME OVER", 186, 75, screenWidth, RED);
            } else if (ganhou == true) {
                draw_final("YOU WIN", 186, 75, screenWidth, GREEN);
            }

        EndDrawing();
    }
    
    CloseWindow();
        
    return 0;
}
