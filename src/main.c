#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"
#include "banco.h"

/* 
    JOGO           FACIL      NORMAL     ESPECIALISTA
     COL             09         16            30
     ROW             09         16            16
   MINAS_MAX         10         40            99
*/

/*
x da interface é a coluna da matriz
y da interface é a linha da matriz

// MATRIZ PARA INTERFACE
x = coluna * PASSO
y = linha * PASSO + PASSO_ALTURA

// INTERFACE PARA MATRIZ
coluna = x / PASSO
linha = (y - PASSO_ALTURA) / PASSO
*/

#define COL 16 // numero de colunas do campo minado
#define ROW 16// numero de linhas do campo minado
#define MINAS_MAX 40 // numero de minas
#define CONTADOR_CAMPO (COL * ROW) // total de casas no campo minado
#define TAMANHO 32
#define ESPACO 4
#define PASSO (TAMANHO + ESPACO) // PASSO = TAMANHO + ESPACO  (distancia do inicio de um quadrado do campo ate o proximo)
#define PASSO_ALTURA (PASSO * 3) // PASSO_ALTURA = PASSO * 3 (espaco acima do campo minado)
#define SCREEN_WIDTH (PASSO * COL) // SCREEN_WIDTH = PASSO * COL (largura da tela)
#define SCREEN_HEIGHT (PASSO * ROW + PASSO_ALTURA) // SCREEN_HEIGHT = PASSO * ROW + PASSO_ALTURA (altura da tela)

typedef struct campoMinado{
    int campo[ROW][COL];
    char mascara[ROW][COL];
    bool perdeu;
    bool ganhou;
    int contador_bandeiras;
    int contador_casas;
    double inicio;
    int segundos;
} CampoMinado;

void contar_minas(int campo[16][16]);
void gerar_mapa(CampoMinado *jogo);
void abrir_mapa(CampoMinado *jogo, int linha, int coluna);
void draw_superior(CampoMinado *jogo);
int fontsize(char *string, float proporcao);
void draw_final(char *s, Color color, CampoMinado *jogo);
int main(void);

void contar_minas(int campo[ROW][COL]) {
    int vizinhos[9][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1}, {0, 0}, {0, 1}, // {0, 0} e a bomba, os outros sao os indices dos campos vizinhos
        {1, -1}, {1, 0}, {1, 1}
    };
    
    for (int linha = 0; linha < ROW; linha++) {
        for (int coluna = 0; coluna < COL; coluna++) {
            if (campo[linha][coluna] == 9) {
                for (int v = 0; v < 9; v++) {
                    int n_linha = linha + vizinhos[v][0];
                    int n_coluna = coluna + vizinhos[v][1];

                    if ((n_linha >= 0 && n_linha < ROW) && (n_coluna >= 0 && n_coluna < COL)) {
                        if (campo[n_linha][n_coluna] != 9) {
                            campo[n_linha][n_coluna]++;
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
    for (int linha = 0; linha < ROW; linha++) {
        for (int coluna = 0; coluna < COL; coluna++) {
            jogo->campo[linha][coluna] = 0;
            jogo->mascara[linha][coluna] = '*';
        }
    }

    while (minas_contador < MINAS_MAX) {
        int linha = rand() % ROW; // pega o resto da divisão do intervalo dado e depois soma com o menor numero 
        int coluna = rand() % COL;

        
        if (jogo->campo[linha][coluna] == 0) {
            jogo->campo[linha][coluna] = 9;
            minas_contador++;
        }
    }

    return contar_minas(jogo->campo);
}

void abrir_mapa(CampoMinado *jogo, int linha, int coluna) {
    // verificar limites da matriz
    if ((linha < 0 || linha >= ROW) || (coluna < 0 || coluna >= COL)) {
        return;
    }

    // se for bandeira nao faz nada
    if (jogo->mascara[linha][coluna] == '#') {
        return;
    }

    // se for bomba nao faz nada
    if (jogo->campo[linha][coluna] == 9) {
        return;
    }

    if (jogo->mascara[linha][coluna] == jogo->campo[linha][coluna]+'0') { // verificar se a celula ja foi mostrada pra evitar loop
        return;
    } else {
        jogo->mascara[linha][coluna] = jogo->campo[linha][coluna] + '0'; // toda celula que chegar aqui nao sera 9 nem ja tera sido mostrada
        if (jogo->campo[linha][coluna] == 0) { 
            jogo->mascara[linha][coluna] = jogo->campo[linha][coluna] + '0';
            abrir_mapa(jogo, linha-1, coluna-1);
            abrir_mapa(jogo, linha-1, coluna);
            abrir_mapa(jogo, linha-1, coluna+1);
            abrir_mapa(jogo, linha, coluna-1);
            abrir_mapa(jogo, linha, coluna+1);
            abrir_mapa(jogo, linha+1, coluna-1);
            abrir_mapa(jogo, linha+1, coluna);
            abrir_mapa(jogo, linha+1, coluna+1);
        }
    }

    return;
}

void draw_superior(CampoMinado *jogo) {
    char bandeiras_string[4];
    snprintf(bandeiras_string, sizeof(bandeiras_string), "%d", jogo->contador_bandeiras);

    char segundos_string[4];
    snprintf(segundos_string, sizeof(segundos_string), "%d", jogo->segundos);
    
    DrawText(bandeiras_string, 36, 36, 50, RED);
    DrawText(segundos_string, SCREEN_WIDTH-75, 36, 50, RED);
}

void draw_mapa(CampoMinado *jogo) {
    Color color;
    int x, y;
    for (int linha = 0; linha < ROW; linha++) {
        for (int coluna = 0; coluna < COL; coluna++) {
            x = coluna * PASSO;
            y = linha * PASSO + PASSO_ALTURA;

            if (jogo->ganhou == false && jogo->perdeu == false) { // se o jogo ainda estiver rolando

                if (jogo->mascara[linha][coluna] != '*' && jogo->mascara[linha][coluna] != '#') {
                    color = WHITE;
                } else if (jogo->mascara[linha][coluna] == '#') {
                    color = RED;
                } else { // campo ainda encoberto
                    color = GRAY;
                }

            } else { // se o jogo ja terminou com ganhou ou perdeu

                if (jogo->campo[linha][coluna] == 9) {
                    color = RED;
                } else {
                    color = WHITE;
                }
                jogo->mascara[linha][coluna] = jogo->campo[linha][coluna]+'0';
                
            }

            DrawRectangle(
                x,
                y,
                32,
                32,
                color
            );

            char valor[2];
            valor[0] = jogo->mascara[linha][coluna];
            valor[1] = '\0';
    
            int valor_int = jogo->mascara[linha][coluna]-'0';
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
            
            DrawText(valor, x + 6, y + 2, 32, color);
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

void draw_final(char *s, Color color, CampoMinado *jogo) {
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
    char *caminho = "tempos.db";
    sqlite3 *db = banco_abrir(caminho);
    if (!db)
        return 1;

    Registro ultimo;
    Registro ranking[10];
    int total;

    CampoMinado jogo;
    jogo.perdeu = false;
    jogo.ganhou = false;
    jogo.contador_bandeiras = MINAS_MAX;
    jogo.contador_casas = CONTADOR_CAMPO;
    jogo.inicio = -1;
    jogo.segundos = 0;

    bool tempo_salvo = false;

    gerar_mapa(&jogo);

    Color cor_quadrado;
    Color cor_numero;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Campo Minado");
    
    SetTargetFPS(30);
    
    while (!WindowShouldClose()) {

        if ((jogo.inicio >= 0) && (jogo.perdeu == false) && (jogo.ganhou == false)) {
            jogo.segundos = (int)(GetTime() - jogo.inicio);
            if (jogo.segundos > 999) {
                jogo.segundos = 999;
            }
        }

        Vector2 mousePosition = GetMousePosition();
        
        // PROCESSO DO JOGO
        // clicar para abrir o mapa e verificador de vitoria e derrota
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !(jogo.ganhou == true || jogo.perdeu == true)) {
            if (jogo.inicio < 0) {
                jogo.inicio = GetTime();
            }

            int x = mousePosition.x;
            int y = mousePosition.y;

            if (y > PASSO_ALTURA) {
                int y = mousePosition.y - PASSO_ALTURA;
                int coluna = x / PASSO;
                int linha = y / PASSO;
    
                if (jogo.campo[linha][coluna] == 9 && jogo.mascara[linha][coluna] != '#') {
                    jogo.mascara[linha][coluna] = jogo.campo[linha][coluna]+'0';
                    jogo.perdeu = true;
                } else if (jogo.mascara[linha][coluna] == '#') {
                    ;
                } else {
                    jogo.contador_casas = CONTADOR_CAMPO;
                    abrir_mapa(&jogo, linha, coluna);
    
                    for (int linha = 0; linha < ROW; linha++) {
                        for (int coluna = 0; coluna < COL; coluna++) {
                            if (jogo.mascara[linha][coluna] != '*' && jogo.mascara[linha][coluna] != '#' ) {
                                jogo.contador_casas -= 1;
                            }
                        }
                    }
                    
                    if (jogo.contador_casas == MINAS_MAX) {
                        jogo.ganhou = true;
                    }
                }           
            }
        }

        // COLOCAR "BANDEIRA" ONDE ACHAR QUE É UMA MINA
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !(jogo.ganhou == true || jogo.perdeu == true)) {
            int x = mousePosition.x;
            int y = mousePosition.y;

            if (y > PASSO_ALTURA) {
                y = y - PASSO_ALTURA;
                int coluna = x / PASSO;
                int linha = y / PASSO;
                // printf("Bandeira - (%d, %d) - [%d][%d]\n", x, y, i, j);
    
                if (jogo.mascara[linha][coluna] == '*') {
                    jogo.mascara[linha][coluna] = '#';
                    jogo.contador_bandeiras--;
                    // VERIFICADOR PARA contador_bandeiras NAO PASSAR PARA NEGATIVO (opcional)
                    // if (contador_bandeiras <= 40 && contador_bandeiras > 0) {
                    //     contador_bandeiras--;
                    //     mascara[linha][coluna] = '#';
                    // }
                    
                } else if (jogo.mascara[linha][coluna] == '#') {
                    jogo.mascara[linha][coluna] = '*';
                    jogo.contador_bandeiras++;
                }
            } 
        }

        // REINICIAR O JOGO AO PRESSIONAR ENTER UMA VEZ
        if (IsKeyPressed(KEY_ENTER) && (jogo.ganhou == true || jogo.perdeu == true)) {
            jogo.ganhou = false;
            jogo.perdeu = false;
            jogo.contador_bandeiras = MINAS_MAX;
            jogo.inicio = -1;
            jogo.segundos = 0;

            tempo_salvo = false;

            gerar_mapa(&jogo);
        }

        BeginDrawing();

            ClearBackground(BLACK);
            
            // CRIAR O CAMPO MINADO NA INTERFACE
            if (jogo.perdeu == false && jogo.ganhou == false) {
                draw_superior(&jogo);

                draw_mapa(&jogo);

            } else if (jogo.perdeu == true) {
                draw_final("GAME OVER", RED, &jogo);
            } else if (jogo.ganhou == true) {
                draw_final("YOU WIN", GREEN, &jogo);
            }

            if ((jogo.ganhou || jogo.perdeu) && !tempo_salvo) {
                if (inserir_tempo(db, jogo.segundos, jogo.ganhou) == -1)
                    printf("ERRO - Nao inseriu no banco\n");
                tempo_salvo = true;

                total = buscar_tempos(db, ranking);
                printf("--- TOP 10 tempos ---\n");
                for (int i = 0; i < total; i++){
                    printf("%d. %ds - %s\n", i + 1, ranking[i].tempo, ranking[i].data);
                }

                if (mostrar_ultimo_tempo(db, &ultimo) == 1)
                    printf("\nJogo atual: %ds em %s\n", ultimo.tempo, ultimo.data);
                else
                    printf("Não foi possível buscar o jogo atual\n");
                }

        EndDrawing();
    }
    
    CloseWindow();
    
    banco_fechar(db);
    return 0;
}