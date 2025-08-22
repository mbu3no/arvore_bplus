# Implementação de Folha de Árvore B+ com Arquivos Binários

Este projeto implementa a estrutura de uma folha de árvore B+, utilizando uma lista de registros de tamanho fixo que é gerenciada diretamente em memória secundária (um arquivo binário). O objetivo é simular as operações de um nó folha sem carregar toda a estrutura para a memória principal (RAM).

O programa foi desenvolvido como parte de uma atividade acadêmica da disciplina de Estrutura de Dados, focando no manuseio de arquivos e na simulação de estruturas de dados em disco.

## 🧠 Conceito Principal: Gerenciamento em Disco

A principal característica deste projeto é que todas as operações (inserção, remoção, busca) são realizadas diretamente no arquivo `pagina.dat`. Isso significa que o programa lê e escreve registros individualmente, em vez de carregar o arquivo inteiro para um vetor em memória. Essa abordagem simula como um sistema de banco de dados interage com o disco para manipular as páginas de uma árvore.

A estrutura dentro do arquivo é organizada como duas listas encadeadas sobrepostas:

1.  **Lista de Registros Ativos:** Uma lista duplamente encadeada que conecta os registros válidos em ordem (ou na ordem de inserção). É gerenciada pelos ponteiros `first`, `last`, `next` e `prev`.
2.  **Lista de Registros Livres:** Uma lista simplesmente encadeada que conecta todos os espaços vazios disponíveis para futuras inserções. É gerenciada pelo ponteiro `free` no cabeçalho.

## ✨ Funcionalidades

O programa apresenta um menu interativo com as seguintes operações:

* **Inserir (`inserir`):** Adiciona um novo registro na primeira posição livre disponível.
* **Inserir Ordenado (`inserirOrdenado`):** Adiciona um novo registro mantendo a ordem lógica crescente das chaves, manipulando apenas os ponteiros `next` e `prev` sem deslocar fisicamente os registros.
* **Remoção (`remover`):** Remove um registro com base em sua chave e devolve o espaço para a lista de livres.
* **Pesquisa (`pesquisa`):** Busca um registro pela sua chave.
* **Imprimir Registros Inseridos (`imprimirLista`):** Exibe todos os registros ativos, seguindo a ordem lógica da lista (do `first` ao `last`).
* **Imprimir Registros Livres (`imprimirFree`):** Mostra todas as posições que estão disponíveis para inserção.
* **Imprimir Estrutura (`imprimirEstrutura`):** Exibe o conteúdo completo do arquivo, incluindo o cabeçalho e todos os registros (ocupados e livres) em suas posições físicas.

## 📂 Estrutura do Arquivo (`pagina.dat`)

O arquivo é organizado em blocos de tamanho fixo (`celula`). O primeiro bloco (posição 0) é sempre o cabeçalho, e os blocos seguintes são os registros.

* **Cabeçalho (Posição 0):**
    * `quant`: Quantidade de registros ativos.
    * `first`: Posição do primeiro registro na lista lógica.
    * `last`: Posição do último registro na lista lógica.
    * `free`: Posição do primeiro espaço livre.
    * `tam`: Capacidade máxima de registros no arquivo.

* **Registros (Posições > 0):**
    * `next`: Posição do próximo registro na lista.
    * `prev`: Posição do registro anterior na lista.
    * `reg`: Os dados do registro (`chave` e `nome`).

## ⚙️ Como Compilar e Executar

Para utilizar o programa, você precisará de um compilador C++ (como o g++).

1.  **Salve o código** em um arquivo chamado `binario.cpp`.
2.  **Abra um terminal** e navegue até a pasta onde o arquivo foi salvo.
3.  **Compile o código** com o seguinte comando:
    ```bash
    g++ binario.cpp -o programa
    ```
4.  **Execute o programa:**
    ```bash
    ./programa
    ```

**Observação:** Na primeira execução, se o arquivo `pagina.dat` não existir, o programa solicitará que você defina o tamanho máximo de registros para criar um novo arquivo.

## 💻 Exemplo de Uso

```console
Criando arquivo pagina.dat
Digite o numero maximo de registros: 5

Menu:
1 - Insere
2 - Insere ordenado
3 - Remove
4 - Pesquisa
5 - Imprime Lista
6 - Imprime Estrutura
7 - Imprime Livres
0 - Sair
Opcao: 2
Chave: 10
Nome: Ana

Menu:
Opcao: 2
Chave: 5
Nome: Carlos

Menu:
Opcao: 5

Registros validos:
Chave: 5, Nome: Carlos
Chave: 10, Nome: Ana

Menu:
Opcao: 6

Imprimindo estrutura
Quant: 2, First: 2, Last: 1, Free: 3, Tam: 5
Posicao 1: Chave: 10, Nome: Ana, Next: -1, Prev: 2
Posicao 2: Chave: 5, Nome: Carlos, Next: 1, Prev: -1
Posicao 3: Chave: 0, Nome: , Next: 4, Prev: 0
Posicao 4: Chave: 0, Nome: , Next: 5, Prev: 0
Posicao 5: Chave: 0, Nome: , Next: -1, Prev: 0

Menu:
Opcao: 0
Finalizando
