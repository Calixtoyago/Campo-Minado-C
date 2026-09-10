#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"


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

void gerar_mapa(int minas_max, int campo[16][16]) {
    int minas_contador = 0;
    srand(time(NULL));

    while (minas_contador < minas_max) {
        int rol = rand() % 16; // pega o resto da divisão do intervalo dado e depois soma com o menor numero 
        int col = rand() % 16;
        
        if (campo[rol][col] == 0) {
            campo[rol][col] = 9;
            minas_contador++;
        }
    }
    return contar_minas(minas_max, campo);
}

void mostrar_campo(int row, int col, int campo[16][16]) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            printf("%d ", campo[i][j]);
        }
        printf("\n");
    }
}

void mostrar_mascara(int row, int col, char mascara[16][16]) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            printf("%c ", mascara[i][j]);
        }
        printf("\n");
    }
}

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

int main(void) {

    int minas_max = 40; // numero de minas

    int row = 16; // numero de linhas
    int col = 16; // numero de colunas

    int campo[16][16] = {0}; // tamanho do campo minado preenchido com 0

    char mascara[16][16];
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            mascara[i][j] = '*';
        }
    }

    gerar_mapa(minas_max, campo);

    // int x, y;
    // int contador = 0;
    // do {
    //     mostrar_mascara(row, col, mascara);
    //     printf("Valor de x: (linhas 0 - 15) -> ");
    //     scanf("%d", &x);
    //     printf("Valor de y: (colunas 0 - 15) -> ");
    //     scanf("%d", &y);

    //     mascara[16][16] = abrir_mapa(row, col, campo, mascara, x, y); // abrir o mapa para cada 0 encontrado em sequencia
        
    //     system("cls");
    // } while (campo[x][y] != 9);

    // if (contador == minas_max) {
    //     printf("Você venceu!!!\n");
    // } 
    // else {
    //     printf("Voce perdeu!!!\n");
    // }
    // mostrar_campo(row, col, campo);

    const int TAMANHO = 32;
    const int ESPACO = 4;
    const int PASSO = TAMANHO + ESPACO;
    const int screenWidth = PASSO * 16; // x
    const int screenHeight = PASSO * 16; // y
    bool perdeu = false;
    bool ganhou = false;
    int contador_bombas;
    Color cor_quadrado;
    
    InitWindow(screenWidth, screenHeight, "Campo Minado");

    SetTargetFPS(30);

    while (!WindowShouldClose()) {
        if (IsCursorHidden()) {
            ShowCursor();
        }

        Vector2 mousePosition = GetMousePosition();
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            int i = mousePosition.x / PASSO;
            int j = mousePosition.y / PASSO;

            if (campo[i][j] == 9 && mascara[i][j] != '#') {
                perdeu = true;
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
                }
            }           
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            int i = mousePosition.x / PASSO;
            int j = mousePosition.y / PASSO;

            if (mascara[i][j] == '*') {
                mascara[i][j] = '#';
            } else if (mascara[i][j] == '#') {
                mascara[i][j] = '*';
            }
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
                            j * PASSO,
                            TAMANHO,
                            TAMANHO,
                            cor_quadrado
                        );
                
                        if (mascara[i][j] != '*' && mascara[i][j] != '#') {
                            char valor[2];
                            valor[0] = mascara[i][j];
                            valor[1] = '\0';
                            DrawText(valor, i * PASSO + 6, j * PASSO + 2, 32, BLACK);
                        }
                    }
                }
            } else if (perdeu == true) {
                DrawText("GAME OVER", 65, 136, 75, RED);
            } else if (ganhou == true) {
                DrawText("YOU WIN", 65, 136, 75, GREEN);
            }

        EndDrawing();
    }
    
    CloseWindow();
        
    return 0;
}