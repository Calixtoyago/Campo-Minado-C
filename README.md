# Campo Minado

Implementação do clássico Campo Minado em C, com interface gráfica feita com a biblioteca [raylib](https://www.raylib.com/). A cada partida, o tabuleiro de 16x16 com 40 minas é gerado aleatoriamente, as áreas sem minas por perto se abrem em cascata e o jogador pode marcar bandeiras. No topo da tela ficam o contador de bandeiras e o cronômetro. Ao fim da partida, o tabuleiro inteiro é revelado, o tempo é salvo em um banco de dados [SQLite](https://www.sqlite.org/) e o jogo mostra uma tela com o ranking das vitórias mais rápidas.

## Como jogar

| Ação | Controle |
| --- | --- |
| Abrir uma célula | Clique esquerdo |
| Marcar ou desmarcar uma bandeira | Clique direito |
| Ver o ranking (após o fim da partida) | ENTER |
| Jogar de novo (na tela do ranking) | ENTER |
| Fechar o jogo | ESC ou o botão de fechar da janela |

Cada número indica quantas minas existem nas oito células ao redor. A partida termina em vitória quando todas as células sem mina são abertas, e em derrota quando uma mina é aberta. As bandeiras servem só para marcação: não é preciso marcar as minas para vencer.

O cronômetro começa no primeiro clique esquerdo, para quando a partida termina e vai até 999 segundos, como no clássico.

O tabuleiro tem tamanho fixo de 16 colunas por 16 linhas, com 40 minas, o equivalente ao nível intermediário do clássico.

## Ranking e tempos salvos

Ao fim de cada partida, vencida ou perdida, o jogo grava no arquivo `tempos.db` o tempo em segundos, o resultado e a data e hora. O arquivo é criado automaticamente na primeira execução, na pasta de onde o jogo foi aberto, e não vai para o repositório: cada jogador tem o seu.

Na tela final da partida, o ENTER abre a tela do ranking, que mostra:

- o **top 10** dos menores tempos entre as partidas vencidas, com a posição, o tempo e a data de cada uma;
- o **tempo da partida que acabou de terminar**, vencida ou perdida.

Um novo ENTER, na tela do ranking, começa a próxima partida.

Para consultar o histórico completo, incluindo as derrotas, dá para abrir o `tempos.db` com qualquer programa que leia bancos SQLite, como o [DB Browser for SQLite](https://sqlitebrowser.org/).

A tabela `tempos` tem esta estrutura:

| Coluna | Tipo | Conteúdo |
| --- | --- | --- |
| `id` | `INTEGER` | Identificador da partida, gerado automaticamente |
| `tempo` | `INTEGER` | Duração da partida em segundos |
| `concluido` | `INTEGER` | `1` para vitória, `0` para derrota |
| `data` | `TEXT` | Data e hora do fim da partida, no horário local |

## Estrutura do projeto

```
Campo-Minado-C/
├── lib/
│   └── sqlite3/
│       ├── sqlite3.c   # SQLite (amalgamation), incluído no repositório
│       └── sqlite3.h
├── src/
│   ├── main.c          # jogo: lógica, desenho e game loop
│   ├── banco.c         # funções de acesso ao banco de dados
│   └── banco.h         # declarações das funções e da struct Registro
├── .gitignore
└── README.md
```

O código do jogo fica em `src/`, e as bibliotecas de terceiros, em `lib/`. O SQLite está incluído no repositório na forma de *amalgamation*, um único arquivo `.c` com a biblioteca inteira, então não é preciso instalá-lo. A versão usada aparece no início do `lib/sqlite3/sqlite3.h`, em `SQLITE_VERSION`.

## Requisitos

O projeto foi desenvolvido e testado no Windows. Para compilar, são necessários o GCC (MinGW-w64, aqui pelo w64devkit) e a raylib. O instalador oficial da raylib para Windows já traz os dois.

## Instalação no Windows

1. Baixe o instalador da raylib para Windows no [site oficial](https://www.raylib.com/).
2. Instale no caminho padrão, `C:\raylib`. O comando de compilação do projeto usa esse caminho.
3. Adicione `C:\raylib\w64devkit\bin` à variável de ambiente `PATH` para usar o `gcc` em qualquer terminal. Se preferir não mexer no `PATH`, abra o terminal pelo `C:\raylib\w64devkit\w64devkit.exe`, que já vem configurado.

## Compilar e executar

A compilação tem duas etapas, e os comandos devem ser rodados na raiz do projeto, porque os caminhos são relativos a ela. Primeiro, compile o SQLite uma única vez:

```shell
gcc -c lib/sqlite3/sqlite3.c -o lib/sqlite3/sqlite3.o -O2
```

Isso leva alguns segundos, porque o `sqlite3.c` é grande. O resultado é o `sqlite3.o`, um arquivo objeto com a biblioteca já compilada. Só é preciso repetir essa etapa se o SQLite for atualizado ou se o `.o` for apagado. O `.o` não vai para o repositório, então quem clonar o projeto precisa rodar essa etapa uma vez.

Depois, compile o jogo:

```shell
gcc src/main.c src/banco.c lib/sqlite3/sqlite3.o -o campo-minado.exe -Ilib/sqlite3 -IC:/raylib/raylib/src -LC:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows
```

Essa é a etapa que se repete a cada alteração no código. Ela é rápida porque só compila os arquivos do jogo e aproveita o SQLite já compilado.

Depois é só abrir o `campo-minado.exe` gerado, com dois cliques ou pelo terminal. Feche o jogo antes de compilar de novo: no Windows, um executável aberto não pode ser sobrescrito, e a compilação falha.

O que cada parte do comando faz:

- `src/main.c`, `src/banco.c` e `lib/sqlite3/sqlite3.o` são compilados e juntados em um único executável, o `campo-minado.exe`, criado na raiz do projeto.
- `-Ilib/sqlite3` indica onde está o `sqlite3.h`.
- `-I` e `-L` com `C:/raylib/raylib/src` apontam para os headers e a biblioteca da raylib. Se ela estiver instalada em outro lugar, ajuste esses caminhos.
- `-lraylib` liga o programa à raylib, e `-lopengl32 -lgdi32 -lwinmm` são bibliotecas do Windows que ela usa.
- `-mwindows` abre o jogo sem a janela do console. As mensagens de erro do banco de dados são impressas no console, então remova essa opção e execute o jogo pelo terminal quando precisar investigar um problema.

### Linux e macOS (não testado)

O código usa só a raylib, o SQLite incluído e a biblioteca padrão do C, então deve compilar em outros sistemas. Instale a raylib seguindo a [wiki oficial](https://github.com/raysan5/raylib/wiki) (no macOS, `brew install raylib pkg-config`), compile o SQLite com o mesmo comando da primeira etapa e depois compile o jogo sem as opções do Windows:

```shell
# Linux
gcc src/main.c src/banco.c lib/sqlite3/sqlite3.o -o campo-minado -Ilib/sqlite3 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# macOS
gcc src/main.c src/banco.c lib/sqlite3/sqlite3.o -o campo-minado -Ilib/sqlite3 $(pkg-config --cflags --libs raylib)
```

## Como funciona

O estado da partida fica na struct `CampoMinado`: as duas matrizes do tabuleiro, o resultado do jogo (`perdeu` e `ganhou`), o contador de bandeiras, o contador de casas ainda fechadas, usado para detectar a vitória, e os dados do cronômetro (`inicio` e `segundos`). A matriz `campo` é a verdade do tabuleiro: cada célula guarda quantas minas tem ao redor (0 a 8) ou o valor 9, que representa uma mina. A matriz `mascara` é o que o jogador vê: `*` para célula fechada, `#` para bandeira e o próprio dígito para célula aberta.

O tamanho do tabuleiro e a quantidade de minas são definidos pelos `#define` `COL`, `ROW` e `MINAS_MAX` no início do `src/main.c`. O total de casas (`CONTADOR_CAMPO`) e o tamanho da janela são calculados a partir deles.

### Linhas, colunas e pixels

As matrizes seguem a convenção `campo[linha][coluna]`. A linha anda na vertical e vai de 0 a `ROW - 1`; a coluna anda na horizontal e vai de 0 a `COL - 1`. Na tela, a coluna corresponde ao `x` e a linha ao `y`, e a conversão entre os dois acontece só em dois pontos: no desenho (índice para pixel) e nos cliques (pixel para índice).

| | Índice → pixel (desenho) | Pixel → índice (clique) |
| --- | --- | --- |
| Horizontal | `x = coluna * PASSO` | `coluna = x / PASSO` |
| Vertical | `y = linha * PASSO + PASSO_ALTURA` | `linha = (y - PASSO_ALTURA) / PASSO` |

`PASSO` é a distância em pixels entre o início de uma célula e o da próxima, e `PASSO_ALTURA` é a altura da faixa no topo da tela, onde ficam o contador, o cronômetro e as mensagens.

### Cronômetro

O cronômetro usa o `GetTime()` da raylib, que devolve os segundos desde a abertura da janela. No primeiro clique esquerdo, esse valor é guardado em `inicio`; antes disso, `inicio` vale `-1`, que indica que o tempo ainda não começou. A cada quadro, enquanto a partida está em andamento, `segundos` recebe a diferença entre o tempo atual e `inicio`, limitada a 999. Como o cálculo só acontece durante a partida, o tempo congela sozinho ao ganhar ou perder, e o reinício volta os dois campos ao estado inicial.

### Telas

O `main` alterna entre três telas, controladas pelos campos `ganhou` e `perdeu` do jogo e pela variável `tela_ranking`:

1. **Partida**, enquanto `ganhou` e `perdeu` são `false`: o placar e o tabuleiro.
2. **Fim de partida**, quando um dos dois vira `true`: a mensagem de vitória ou derrota com o tabuleiro revelado.
3. **Ranking**, quando `tela_ranking` é `true`: o top 10 e o tempo da partida atual.

O ENTER só funciona depois que a partida termina. Na tela de fim de partida, ele muda `tela_ranking` para `true`; na tela do ranking, volta `tela_ranking` para `false`, reinicia o estado do jogo e gera um novo tabuleiro.

### Banco de dados

O acesso ao banco fica isolado no módulo `banco.c`, e o `main.c` só conhece as funções declaradas no `banco.h`, sem chamar o SQLite diretamente. A conexão é aberta uma vez no início do `main`, antes da janela, e fechada no final, depois do `CloseWindow()`.

As consultas guardam os resultados na struct `Registro`, declarada no `banco.h`, com o tempo e a data de uma partida. O ranking é um array `Registro ranking[10]`, e a partida atual é um único `Registro ultimo`. Os dois ficam declarados no `main`, e as funções de busca recebem o endereço deles para preenchê-los.

A lógica do banco e o desenho ficam separados. Quando a partida termina, um bloco que roda **uma única vez** salva o tempo, busca o ranking e a partida atual e guarda os resultados nessas variáveis. A tela do ranking é desenhada a cada quadro a partir delas, sem voltar ao banco. Para que esse bloco rode uma vez só, o `main` usa a variável `tempo_salvo`: quando a partida termina e ela ainda é `false`, o bloco executa e ela passa a ser `true`. O reinício da partida volta a variável para `false`. Sem esse controle, o game loop tentaria salvar o mesmo tempo e consultar o banco a cada quadro.

### Funções

| Função | Arquivo | O que faz |
| --- | --- | --- |
| `gerar_mapa` | `main.c` | Zera o tabuleiro, fecha todas as células, sorteia as minas e calcula os números |
| `contar_minas` | `main.c` | Para cada mina, incrementa o número das células vizinhas |
| `abrir_mapa` | `main.c` | Abre uma célula e, se ela não tiver minas ao redor, abre as vizinhas recursivamente, sem passar por bandeiras |
| `draw_superior` | `main.c` | Desenha o contador de bandeiras e o cronômetro no topo da tela |
| `draw_mapa` | `main.c` | Desenha o tabuleiro, tanto durante a partida quanto revelado na tela final |
| `draw_final` | `main.c` | Desenha a mensagem de vitória ou derrota junto com o tabuleiro revelado |
| `draw_ranking` | `main.c` | Desenha a tela do ranking a partir dos registros já buscados, sem acessar o banco |
| `fontsize` | `main.c` | Calcula o tamanho da fonte proporcional à largura da janela |
| `banco_abrir` | `banco.c` | Abre ou cria o `tempos.db` e cria a tabela `tempos` se ela não existir. Retorna `NULL` em caso de erro |
| `inserir_tempo` | `banco.c` | Grava o tempo e o resultado de uma partida. Retorna o `id` da linha criada, ou `-1` em caso de erro |
| `buscar_tempos` | `banco.c` | Preenche o array recebido com as 10 vitórias mais rápidas. Retorna quantas encontrou, ou `-1` em caso de erro |
| `mostrar_ultimo_tempo` | `banco.c` | Preenche o `Registro` recebido com a última partida salva. Retorna `1` se encontrou, `0` se não há partidas salvas, ou `-1` em caso de erro |
| `banco_fechar` | `banco.c` | Fecha a conexão com o banco |

O `main` segue o padrão de game loop da raylib: a cada quadro, atualiza o cronômetro, lê o mouse e o teclado, atualiza o estado do jogo, salva o tempo e busca o ranking quando a partida termina e desenha a tela atual.