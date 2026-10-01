# Campo Minado

Implementação do clássico Campo Minado em C, com interface gráfica feita com a biblioteca [raylib](https://www.raylib.com/). 

Fiz uma versão desse jogo em Python há um tempo, então já lembrava um pouco da lógica. Em C, a dificuldade foi a estrutura do código e a tipagem: cada variável precisa ter o tipo declarado, e ele não muda, ao contrário do Python.

Para jogar, baixe o executável na página de [Releases](https://github.com/Calixtoyago/Campo-Minado-C/releases).

## O que usei no projeto?

- **struct** - foi o que fez o código ficar limpo. Antes de usar a struct, as funções tinham de 3 a 6 parâmetros. Depois que refatorei o código para implementar o jogo com a struct CampoMinado, as funções passaram a ter de 1 a 3 parâmetros.
- **ponteiros** - o que na teoria parece simples (armazenar o endereço de uma variável), na prática pode ser um pouco confuso. Apesar disso, consegui entender a diferença dos seus usos e como passá-los como argumentos para as funções.
- **SQLite** - no Python é tão simples fazer `cursor.execute("SELECT...")`, porém no C eu tive que fazer toda a operação por trás do `execute()`. Demorei mais para implementar essas funções do que para fazer a lógica do jogo.
- **modularização** - separei o banco de dados em `banco.c` e `banco.h`, deixando o `main.c` só com a lógica do jogo e a interface. O `main.c` chama funções como `inserir_tempo()` sem precisar saber nada do SQLite.
- **raylib** - uma biblioteca simples voltada para desenvolvimento de jogos 2D e 3D. A interface do Campo Minado foi toda feita através dessa biblioteca. 

### O que faltou?

- **alocação dinâmica** - no momento só possui a dificuldade Intermediária (16x16 com 40 minas) e com alocação dinâmica é possível construir o corpo do jogo conforme a dificuldade escolhida pelo jogador.

## Como jogar

| Ação | Controle |
| --- | --- |
| Abrir uma célula | Clique esquerdo |
| Marcar ou desmarcar uma bandeira | Clique direito |
| Ver o ranking (após o fim da partida) | ENTER |
| Jogar de novo (na tela do ranking) | ENTER |
| Fechar o jogo | ESC ou o botão de fechar da janela |

Os tempos das vitórias ficam salvos no arquivo `tempos.db`, criado na pasta do jogo na primeira execução, e alimentam o ranking com as 10 vitórias mais rápidas.

## Como funciona

O estado da partida fica na struct `CampoMinado`: as duas matrizes do tabuleiro (`campo` e `mascara`), o resultado do jogo (`perdeu` e `ganhou`), o contador de bandeiras, o contador de casas ainda fechadas, usado para detectar a vitória, e os dados do cronômetro (`inicio` e `segundos`). 

A matriz `campo` é a verdade do tabuleiro: cada célula guarda quantas minas tem ao redor (0 a 8) ou o valor 9, que representa uma mina. A matriz `mascara` é o que o jogador vê: `*` para célula fechada, `#` para bandeira e o próprio dígito para célula aberta.

O tamanho do tabuleiro é fixo, definido por `#define` no início do `main.c`. Por isso, as matrizes têm tamanho fixo, e mudar a dificuldade durante o jogo exigiria alocação dinâmica.

## Requisitos para compilar

O projeto foi desenvolvido e testado no Windows. Para compilar, são necessários o GCC (MinGW-w64, aqui pelo w64devkit) e a raylib. O instalador oficial da raylib para Windows já traz os dois.

### Instalação no Windows

1. Baixe o instalador da raylib para Windows no [site oficial](https://www.raylib.com/).
2. Instale no caminho padrão, `C:\raylib`. O comando de compilação do projeto usa esse caminho.
3. Adicione `C:\raylib\w64devkit\bin` à variável de ambiente `PATH` para usar o `gcc` em qualquer terminal. Se preferir não mexer no `PATH`, abra o terminal pelo `C:\raylib\w64devkit\w64devkit.exe`, que já vem configurado.

### Compilar e executar

A compilação tem duas etapas, e os comandos devem ser rodados na raiz do projeto, porque os caminhos são relativos a ela. Primeiro, compile o SQLite:

```shell
gcc -c lib/sqlite3/sqlite3.c -o lib/sqlite3/sqlite3.o -O2
```
Essa etapa só é necessária uma vez. Depois de compilar o `sqlite3.c` ele vai gerar o arquivo `sqlite3.o` na mesma pasta.

Depois, compile o jogo:

```shell
gcc src/main.c src/banco.c lib/sqlite3/sqlite3.o -o campo-minado.exe -Ilib/sqlite3 -IC:/raylib/raylib/src -LC:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows
```

Pronto, é só abrir o `campo-minado.exe` gerado, com dois cliques ou pelo terminal. Feche o jogo antes de compilar de novo.

### Linux e macOS (não testado)
Instale a raylib seguindo a [wiki oficial](https://github.com/raysan5/raylib/wiki) (no macOS, `brew install raylib pkg-config`), compile o SQLite com o mesmo comando da primeira etapa e depois compile o jogo sem as opções do Windows:

```shell
# Linux
gcc src/main.c src/banco.c lib/sqlite3/sqlite3.o -o campo-minado -Ilib/sqlite3 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# macOS
gcc src/main.c src/banco.c lib/sqlite3/sqlite3.o -o campo-minado -Ilib/sqlite3 $(pkg-config --cflags --libs raylib)
```