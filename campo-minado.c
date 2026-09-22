#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"

/*
    JOGO           FACIL      NORMAL      
     ROW             09         16
     COL             09         16
   MINAS_MAX         10         40
CONTADOR_BOMBAS      81         256
*/

#define ROW 16 // numero de linhas do campo minado
#define COL 16// numero de colunas do campo minado
#define MINAS_MAX 40 // numero de minas
#define CONTADOR_BOMBAS (ROW * COL)
#define TAMANHO 32
#define ESPACO 4
#define PASSO (TAMANHO + ESPACO) // PASSO = TAMANHO + ESPACO  (distancia do inicio de um quadrado do campo ate o proximo)
#define PASSO_ALTURA (PASSO * 3) // PASSO_ALTURA = PASSO * 3 (espaco acima do campo minado)
#define SCREEN_WIDTH (PASSO * ROW) // SCREEN_WIDTH = PASSO * ROW (largura da tela)
#define SCREEN_HEIGHT (PASSO * COL + PASSO_ALTURA) // SCREEN_HEIGHT = PASSO * COL + PASSO_ALTURA (altura da tela)

typedef struct campoMinado{
    int campo[ROW][COL];
    char mascara[ROW][COL];
    bool perdeu;
    bool ganhou;
} CampoMinado;

void contar_minas(int campo[ROW][COL]) {
    int vizinhos[9][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1}, {0, 0}, {0, 1}, // {0, 0} e a bomba, os outros sao os indices dos campos vizinhos
        {1, -1}, {1, 0}, {1, 1}
    };
    
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if (campo[i][j] == 9) {
                for (int row = 0; row < 9; row++) {
                    int ni = i + vizinhos[row][0];
                    int nj = j + vizinhos[row][1];

                    if ((ni >= 0 && ni < ROW) && (nj >= 0 && nj < COL)) {
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

void gerar_mapa(CampoMinado *jogo) {
    int minas_contador = 0;
    srand(time(NULL));
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            jogo->campo[i][j] = 0;
            jogo->mascara[i][j] = '*';
        }
    }

    while (minas_contador < MINAS_MAX) {
        int row = rand() % ROW; // pega o resto da divisão do intervalo dado e depois soma com o menor numero 
        int col = rand() % COL;

        
        if (jogo->campo[row][col] == 0) {
            jogo->campo[row][col] = 9;
            minas_contador++;
        }
    }

    return contar_minas(jogo->campo);
}

void abrir_mapa(CampoMinado *jogo, int x, int y) {
    // verificar limites da matriz
    if ((x < 0 || x >= ROW) || (y < 0 || y >= COL)) {
        return;
    }

    // se for bandeira nao faz nada
    if (jogo->mascara[x][y] == '#') {
        return;
    }

    // se for bomba nao faz nada
    if (jogo->campo[x][y] == 9) {
        return;
    }

    if (jogo->mascara[x][y] == jogo->campo[x][y]+'0') { // verificar se a celula ja foi mostrada pra evitar loop
        return;
    } else {
        jogo->mascara[x][y] = jogo->campo[x][y] + '0'; // toda celula que chegar aqui nao sera 9 nem ja tera sido mostrada
        if (jogo->campo[x][y] == 0) { 
            jogo->mascara[x][y] = jogo->campo[x][y] + '0';
            abrir_mapa(jogo, x-1, y-1);
            abrir_mapa(jogo, x-1, y);
            abrir_mapa(jogo, x-1, y+1);
            abrir_mapa(jogo, x, y-1);
            abrir_mapa(jogo, x, y+1);
            abrir_mapa(jogo, x+1, y-1);
            abrir_mapa(jogo, x+1, y);
            abrir_mapa(jogo, x+1, y+1);
        }
    }

    return;
}


void draw_mapa(CampoMinado *jogo) {
    Color color;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if (jogo->ganhou == false && jogo->perdeu == false) { // se o jogo ainda estiver rolando

                if (jogo->mascara[i][j] != '*' && jogo->mascara[i][j] != '#') {
                    color = WHITE;
                } else if (jogo->mascara[i][j] == '#') {
                    color = RED;
                } else { // campo ainda encoberto
                    color = GRAY;
                }

            } else { // se o jogo ja terminou com ganhou ou perdeu

                if (jogo->campo[i][j] == 9) {
                    color = RED;
                } else {
                    color = WHITE;
                }
                jogo->mascara[i][j] = jogo->campo[i][j]+'0';
                
            }

            DrawRectangle(
                i * PASSO,
                j * PASSO + PASSO_ALTURA,
                32,
                32,
                color
            );

            char valor[2];
            valor[0] = jogo->mascara[i][j];
            valor[1] = '\0';
    
            int valor_int = jogo->mascara[i][j]-'0';
            switch (valor_int)
            {
            case 0: color = BLACK; break;
            case 1: color = BLUE; break;
            case 2: color = GREEN; break;
            case 3: color = RED; break;
            case 4: color = DARKBLUE; break;
            case 5: color = BROWN; break;
            case 6: color = LIME; break;
            case 7: color = PURPLE; break;
            case 8: color = GRAY; break;
            case 9: color = RED; break;
            }
    
            DrawText(valor, i * PASSO + 6, j * PASSO + 2 + PASSO_ALTURA, 32, color);
        }
    }
}

int fontsize(char *string, float proporcao) {
    int fontsize = 10;
    while (MeasureText(string, fontsize) < SCREEN_WIDTH * proporcao) {
        fontsize++;
    }
    fontsize--;

    return fontsize;
}

void draw_final(char *s, int y, Color color, CampoMinado *jogo) {
    int fontsize_gameOver = fontsize("GAME OVER", 0.5);
    int fontisize_pressEnter = fontsize("Press ENTER to play again", 0.75);

    int largura_texto_gameOver = MeasureText(s, fontsize_gameOver);
    int x_gameOver = (SCREEN_WIDTH - largura_texto_gameOver) / 2;

    int largura_texto_pressEnter = MeasureText("Press ENTER to play again", fontisize_pressEnter);
    int x_pressEnter = (SCREEN_WIDTH - largura_texto_pressEnter) / 2;

    DrawText(s, x_gameOver, 14, fontsize_gameOver, color);
    DrawText("Press ENTER to play again", x_pressEnter, 70, fontisize_pressEnter, ORANGE);

    draw_mapa(jogo);
}

int main(void) {
    CampoMinado jogo;
    jogo.perdeu = false;
    jogo.ganhou = false;

    gerar_mapa(&jogo);

    int contador_bandeiras = MINAS_MAX;
    int contador_bombas;
    Color cor_quadrado;
    Color cor_numero;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Campo Minado");
    
    SetTargetFPS(30);
    
    while (!WindowShouldClose()) {

        Vector2 mousePosition = GetMousePosition();
        
        // PROCESSO DO JOGO
        // clicar para abrir o mapa e verificador de vitoria e derrota
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !(jogo.ganhou == true || jogo.perdeu == true)) {
            int x = mousePosition.x;
            int y = mousePosition.y - PASSO_ALTURA;
            int i = x / PASSO;
            int j = y / PASSO;
            printf("Clique - (%d, %d) - [%d][%d]\n", x, y, i, j);

            if (jogo.campo[i][j] == 9 && jogo.mascara[i][j] != '#') {
                jogo.mascara[i][j] = jogo.campo[i][j]+'0';
                jogo.perdeu = true;
            } else if (jogo.mascara[i][j] == '#') {
                ;
            } else {
                contador_bombas = CONTADOR_BOMBAS;
                abrir_mapa(&jogo, i, j);

                for (int i = 0; i < ROW; i++) {
                    for (int j = 0; j < COL; j++) {
                        if (jogo.mascara[i][j] != '*' && jogo.mascara[i][j] != '#' ) {
                            contador_bombas -= 1;
                        }
                    }
                }
                
                if (contador_bombas == MINAS_MAX) {
                    jogo.ganhou = true;
                }
            }           
        }

        // COLOCAR "BANDEIRA" ONDE ACHAR QUE É UMA MINA
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !(jogo.ganhou == true || jogo.perdeu == true)) {
            int x = mousePosition.x;
            int y = mousePosition.y;

            if (y > PASSO_ALTURA) {
                y = y - PASSO_ALTURA;
                int i = x / PASSO;
                int j = y / PASSO;
                printf("Bandeira - (%d, %d) - [%d][%d]\n", x, y, i, j);
    
                if (jogo.mascara[i][j] == '*') {
                    jogo.mascara[i][j] = '#';
                    contador_bandeiras--;
                    // VERIFICADOR PARA contador_bandeiras NAO PASSAR PARA NEGATIVO (opcional)
                    // if (contador_bandeiras <= 40 && contador_bandeiras > 0) {
                    //     contador_bandeiras--;
                    //     mascara[i][j] = '#';
                    // }
                    
                } else if (jogo.mascara[i][j] == '#') {
                    jogo.mascara[i][j] = '*';
                    contador_bandeiras++;
                }
            } 
        }

        // REINICIAR O JOGO AO PRESSIONAR ENTER UMA VEZ
        if (IsKeyPressed(KEY_ENTER) && (jogo.ganhou == true || jogo.perdeu == true)) {
            jogo.ganhou = false;
            jogo.perdeu = false;
            contador_bandeiras = MINAS_MAX;

            gerar_mapa(&jogo);
        }

        BeginDrawing();

            ClearBackground(BLACK); 
            
            // CRIAR O CAMPO MINADO NA INTERFACE
            if (jogo.perdeu == false && jogo.ganhou == false) {
                char bandeiras_string[2];
                snprintf(bandeiras_string, 6, "%d", contador_bandeiras);
                
                DrawText(bandeiras_string, 36, 36, 50, RED);
                cor_quadrado = RED;

                draw_mapa(&jogo);

            } else if (jogo.perdeu == true) {
                draw_final("GAME OVER", 186, RED, &jogo);
            } else if (jogo.ganhou == true) {
                draw_final("YOU WIN", 186, GREEN, &jogo);
            }

        EndDrawing();
    }
    
    CloseWindow();
        
    return 0;
}