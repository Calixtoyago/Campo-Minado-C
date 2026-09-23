# Campo Minado

Implementação do clássico Campo Minado em C, com interface gráfica feita com a biblioteca [raylib](https://www.raylib.com/). A cada partida o tabuleiro é gerado aleatoriamente, as áreas sem minas por perto se abrem em cascata e o jogador pode marcar bandeiras, com um contador no topo da tela. Ao fim da partida, o tabuleiro inteiro é revelado.

## Como jogar

| Ação | Controle |
| --- | --- |
| Abrir uma célula | Clique esquerdo |
| Marcar ou desmarcar uma bandeira | Clique direito |
| Jogar de novo (após o fim da partida) | ENTER |
| Fechar o jogo | ESC ou o botão de fechar da janela |

Cada número indica quantas minas existem nas oito células ao redor. A partida termina em vitória quando todas as células sem mina são abertas, e em derrota quando uma mina é aberta. As bandeiras servem só para marcação: não é preciso marcar as minas para vencer.

## Requisitos

O projeto foi desenvolvido e testado no Windows. Para compilar, são necessários o GCC (MinGW-w64, aqui pelo w64devkit) e a raylib. O instalador oficial da raylib para Windows já traz os dois.

## Instalação no Windows

1. Baixe o instalador da raylib para Windows no [site oficial](https://www.raylib.com/).
2. Instale no caminho padrão, `C:\raylib`. O comando de compilação do projeto usa esse caminho.
3. Adicione `C:\raylib\w64devkit\bin` à variável de ambiente `PATH` para usar o `gcc` em qualquer terminal. Se preferir não mexer no `PATH`, abra o terminal pelo `C:\raylib\w64devkit\w64devkit.exe`, que já vem configurado.

## Compilar e executar

Na pasta do projeto, rode:

```shell
gcc campo-minado.c -o campo-minado.exe -IC:/raylib/raylib/src -LC:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows
```

Depois é só abrir o `campo-minado.exe` gerado, pelo terminal ou com dois cliques.

O que cada parte do comando faz:

- `-I` e `-L` apontam para os headers e a biblioteca da raylib. Se ela estiver instalada em outro lugar, ajuste esses caminhos.
- `-lraylib` liga o programa à raylib, e `-lopengl32 -lgdi32 -lwinmm` são bibliotecas do Windows que ela usa.
- `-mwindows` abre o jogo sem a janela do console. Remova essa opção se quiser ver as mensagens de depuração que o programa imprime.

### Linux e macOS (não testado)

O código usa só a raylib e a biblioteca padrão do C, então deve compilar em outros sistemas. Instale a raylib seguindo a [wiki oficial](https://github.com/raysan5/raylib/wiki) (no macOS, `brew install raylib pkg-config`) e compile sem as opções do Windows:

```shell
# Linux
gcc campo-minado.c -o campo-minado -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# macOS
gcc campo-minado.c -o campo-minado $(pkg-config --cflags --libs raylib)
```

## Configurar a dificuldade

A dificuldade é definida pelos `#define` no início do `campo-minado.c`: `COL` é o número de colunas (a largura do tabuleiro), `ROW` é o número de linhas (a altura) e `MINAS_MAX` é a quantidade de minas. Para trocar, altere os valores conforme a tabela e compile de novo. O padrão é o Normal.

| `#define` | Fácil | Normal | Especialista |
| --- | --- | --- | --- |
| `COL` | 9 | 16 | 30 |
| `ROW` | 9 | 16 | 16 |
| `MINAS_MAX` | 10 | 40 | 99 |

O total de casas (`CONTADOR_CAMPO`), o tamanho da janela e o tamanho dos textos são calculados a partir desses valores.

## Como funciona

O estado da partida fica na struct `CampoMinado`: as duas matrizes do tabuleiro, o resultado do jogo (`perdeu` e `ganhou`), o contador de bandeiras exibido no topo da tela e o contador de casas ainda fechadas, usado para detectar a vitória. A matriz `campo` é a verdade do tabuleiro: cada célula guarda quantas minas tem ao redor (0 a 8) ou o valor 9, que representa uma mina. A matriz `mascara` é o que o jogador vê: `*` para célula fechada, `#` para bandeira e o próprio dígito para célula aberta.

### Linhas, colunas e pixels

As matrizes seguem a convenção `campo[linha][coluna]`. A linha anda na vertical e vai de 0 a `ROW - 1`; a coluna anda na horizontal e vai de 0 a `COL - 1`. Na tela, a coluna corresponde ao `x` e a linha ao `y`, e a conversão entre os dois acontece só em dois pontos: no desenho (índice para pixel) e nos cliques (pixel para índice).

| | Índice → pixel (desenho) | Pixel → índice (clique) |
| --- | --- | --- |
| Horizontal | `x = coluna * PASSO` | `coluna = x / PASSO` |
| Vertical | `y = linha * PASSO + PASSO_ALTURA` | `linha = (y - PASSO_ALTURA) / PASSO` |

`PASSO` é a distância em pixels entre o início de uma célula e o da próxima, e `PASSO_ALTURA` é a altura da faixa no topo da tela, onde ficam o contador e as mensagens.

### Funções

| Função | O que faz |
| --- | --- |
| `gerar_mapa` | Zera o tabuleiro, fecha todas as células, sorteia as minas e calcula os números |
| `contar_minas` | Para cada mina, incrementa o número das células vizinhas |
| `abrir_mapa` | Abre uma célula e, se ela não tiver minas ao redor, abre as vizinhas recursivamente, sem passar por bandeiras |
| `draw_superior` | Desenha o contador de bandeiras no topo da tela |
| `draw_mapa` | Desenha o tabuleiro, tanto durante a partida quanto revelado na tela final |
| `draw_final` | Desenha a mensagem de vitória ou derrota junto com o tabuleiro revelado |
| `fontsize` | Calcula o tamanho da fonte proporcional à largura da janela |

O `main` segue o padrão de game loop da raylib: a cada quadro, lê o mouse e o teclado, atualiza o estado do jogo e redesenha a tela.