#include<iostream>
#include<fstream>
#include<string>

using namespace std;

// Estrutura para armazenar os dados do registro
struct dados {
    int chave;           // Chave de identificação do registro
    char nome[30];       // Nome associado ao registro
};

// União que representa tanto o cabeçalho quanto os registros da lista
union celula {
    struct {
        int quant;       // Quantidade de registros ativos
        int first;       // Posição do primeiro registro válido
        int last;        // Posição do último registro válido
        int free;        // Início da lista de registros livres
        int tam;         // Tamanho total da estrutura (capacidade máxima)
    } cabecalho;
    struct {
        int next;        // Próximo registro na lista ativa
        int prev;        // Registro anterior na lista ativa
        dados reg;       // Dados do registro
    } lista;
};

// Inicializa o arquivo e cria a lista de registros livres encadeados
void inicializar(fstream &arq, int n) {
    cout << "\nInicializando a estrutura de registros\n";

    celula cab, l;
    arq.seekp(0, arq.beg);  // Posiciona ponteiro de escrita no início do arquivo

    cab.cabecalho.quant = 0;
    cab.cabecalho.first = -1;  // Lista ativa vazia
    cab.cabecalho.last = -1;
    cab.cabecalho.free = 1;    // Primeira posição livre é a 1
    cab.cabecalho.tam = n;

    arq.write((char*)&cab, sizeof(cab));  // Escreve o cabeçalho no arquivo

    // Cria a lista de registros livres
    for (int i = 1; i <= n; i++) {
        l.lista.next = (i == n) ? -1 : i + 1;  // Encadeia os livres
        arq.write((char*)&l, sizeof(l));
    }
}

// Imprime toda a estrutura do arquivo, incluindo cabeçalho e registros
void imprimirEstrutura(fstream &arq) {
    celula cab, l;
    cout << "\nImprimindo estrutura\n";
    arq.seekg(0, arq.beg);  // Posiciona no início do arquivo
    arq.read((char*)&cab, sizeof(cab));

    // Imprime os dados do cabeçalho
    cout << "Quant: " << cab.cabecalho.quant << ", First: " << cab.cabecalho.first
         << ", Last: " << cab.cabecalho.last << ", Free: " << cab.cabecalho.free
         << ", Tam: " << cab.cabecalho.tam << "\n";

    // Imprime todos os registros (ocupados ou livres)
    for (int i = 1; i <= cab.cabecalho.tam; i++) {
        cout << "Posicao " << i << ": ";
        arq.read((char*)&l, sizeof(l));
        cout << "Chave: " << l.lista.reg.chave << ", Nome: " << l.lista.reg.nome
             << ", Next: " << l.lista.next << ", Prev: " << l.lista.prev << "\n";
    }
}

// Imprime apenas os registros ativos da lista encadeada
void imprimirLista(fstream &arq) {
    celula cab, l;
    arq.seekg(0, arq.beg);
    arq.read((char*)&cab, sizeof(cab));

    cout << "\nRegistros validos:\n";
    int atual = cab.cabecalho.first;

    while (atual != -1) {
        arq.seekg(atual * sizeof(celula), arq.beg);
        arq.read((char*)&l, sizeof(l));

        cout << "Chave: " << l.lista.reg.chave << ", Nome: " << l.lista.reg.nome << "\n";
        atual = l.lista.next;
    }
}

// Imprime a lista de registros livres encadeados
void imprimirFree(fstream &arq) {
    celula cab, l;
    arq.seekg(0, arq.beg);
    arq.read((char*)&cab, sizeof(cab));

    cout << "\nRegistros livres:\n";
    int livre = cab.cabecalho.free;

    while (livre != -1) {
        arq.seekg(livre * sizeof(celula), arq.beg);
        arq.read((char*)&l, sizeof(l));

        cout << "Posicao: " << livre << ", Prox Livre: " << l.lista.next << "\n";
        livre = l.lista.next;
    }
}

// Pesquisa um registro pela chave e retorna verdadeiro se encontrado
bool pesquisa(fstream &arq, dados &d) {
    celula cab, l;
    arq.seekg(0, arq.beg);
    arq.read((char*)&cab, sizeof(cab));

    int atual = cab.cabecalho.first;

    while (atual != -1) {
        arq.seekg(atual * sizeof(celula), arq.beg);
        arq.read((char*)&l, sizeof(l));

        if (l.lista.reg.chave == d.chave) {
            d = l.lista.reg;  // Retorna os dados encontrados
            return true;
        }
        atual = l.lista.next;
    }
    return false;
}

// Insere um registro no final da lista ativa
void inserir(fstream &arq, dados d) {
    celula cab, l, anterior;
    arq.seekg(0, arq.beg);
    arq.read((char*)&cab, sizeof(cab));

    if (cab.cabecalho.free == -1) {
        cout << "Sem espaco livre.\n";
        return;
    }

    int posLivre = cab.cabecalho.free;
    arq.seekg(posLivre * sizeof(celula), arq.beg);
    arq.read((char*)&l, sizeof(l));

    l.lista.reg = d;
    l.lista.next = -1;
    l.lista.prev = cab.cabecalho.last;

    // Atualiza ponteiros do último registro caso a lista não esteja vazia
    if (cab.cabecalho.last != -1) {
        arq.seekg(cab.cabecalho.last * sizeof(celula), arq.beg);
        arq.read((char*)&anterior, sizeof(anterior));
        anterior.lista.next = posLivre;
        arq.seekp(cab.cabecalho.last * sizeof(celula), arq.beg);
        arq.write((char*)&anterior, sizeof(anterior));
    } else {
        cab.cabecalho.first = posLivre;
    }

    cab.cabecalho.last = posLivre;
    cab.cabecalho.free = l.lista.next;
    cab.cabecalho.quant++;

    arq.seekp(posLivre * sizeof(celula), arq.beg);
    arq.write((char*)&l, sizeof(l));

    arq.seekp(0, arq.beg);
    arq.write((char*)&cab, sizeof(cab));
}

// Insere um registro ordenadamente de acordo com a chave
void inserirOrdenado(fstream &arq, dados d) {
    celula cab, l, novo, anterior, proximo;
    arq.seekg(0, arq.beg);
    arq.read((char*)&cab, sizeof(cab));

    if (cab.cabecalho.free == -1) {
        cout << "Sem espaco livre.\n";
        return;
    }

    int posLivre = cab.cabecalho.free;
    arq.seekg(posLivre * sizeof(celula), arq.beg);
    arq.read((char*)&novo, sizeof(novo));

    int proximoLivre = novo.lista.next;
    novo.lista.reg = d;

    int atual = cab.cabecalho.first;
    int anteriorPos = -1;

    // Busca posição de inserção comparando as chaves
    while (atual != -1) {
        arq.seekg(atual * sizeof(celula), arq.beg);
        arq.read((char*)&l, sizeof(l));

        if (d.chave < l.lista.reg.chave)
            break;

        anteriorPos = atual;
        atual = l.lista.next;
    }

    novo.lista.next = atual;
    novo.lista.prev = anteriorPos;

    // Atualiza ponteiros do anterior se não for inserido no início
    if (anteriorPos != -1) {
        arq.seekg(anteriorPos * sizeof(celula), arq.beg);
        arq.read((char*)&anterior, sizeof(anterior));
        anterior.lista.next = posLivre;
        arq.seekp(anteriorPos * sizeof(celula), arq.beg);
        arq.write((char*)&anterior, sizeof(anterior));
    } else {
        cab.cabecalho.first = posLivre;
    }

    // Atualiza ponteiros do próximo se não for inserido no final
    if (atual != -1) {
        arq.seekg(atual * sizeof(celula), arq.beg);
        arq.read((char*)&proximo, sizeof(proximo));
        proximo.lista.prev = posLivre;
        arq.seekp(atual * sizeof(celula), arq.beg);
        arq.write((char*)&proximo, sizeof(proximo));
    } else {
        cab.cabecalho.last = posLivre;
    }

    cab.cabecalho.free = proximoLivre;
    cab.cabecalho.quant++;

    arq.seekp(posLivre * sizeof(celula), arq.beg);
    arq.write((char*)&novo, sizeof(novo));

    arq.seekp(0, arq.beg);
    arq.write((char*)&cab, sizeof(cab));
}

// Remove um registro da lista ativa e o insere na lista de livres
bool remover(fstream &arq, dados &d) {
    celula cab, l, anterior, proximo;
    arq.seekg(0, arq.beg);
    arq.read((char*)&cab, sizeof(cab));

    int atual = cab.cabecalho.first;

    while (atual != -1) {
        arq.seekg(atual * sizeof(celula), arq.beg);
        arq.read((char*)&l, sizeof(l));

        if (l.lista.reg.chave == d.chave) {
            d = l.lista.reg;  // Retorna os dados antes da remoção

            // Atualiza ponteiros do anterior
            if (l.lista.prev != -1) {
                arq.seekg(l.lista.prev * sizeof(celula), arq.beg);
                arq.read((char*)&anterior, sizeof(anterior));
                anterior.lista.next = l.lista.next;
                arq.seekp(l.lista.prev * sizeof(celula), arq.beg);
                arq.write((char*)&anterior, sizeof(anterior));
            } else {
                cab.cabecalho.first = l.lista.next;
            }

            // Atualiza ponteiros do próximo
            if (l.lista.next != -1) {
                arq.seekg(l.lista.next * sizeof(celula), arq.beg);
                arq.read((char*)&proximo, sizeof(proximo));
                proximo.lista.prev = l.lista.prev;
                arq.seekp(l.lista.next * sizeof(celula), arq.beg);
                arq.write((char*)&proximo, sizeof(proximo));
            } else {
                cab.cabecalho.last = l.lista.prev;
            }

            // Devolve o espaço para a lista de livres
            l.lista.next = cab.cabecalho.free;
            cab.cabecalho.free = atual;
            cab.cabecalho.quant--;

            arq.seekp(atual * sizeof(celula), arq.beg);
            arq.write((char*)&l, sizeof(l));

            arq.seekp(0, arq.beg);
            arq.write((char*)&cab, sizeof(cab));

            return true;
        }
        atual = l.lista.next;
    }
    return false;
}

// Função principal: Exibe menu e gerencia operações
int main() {
    fstream arq;
    dados d;
    int n, op = -1;

    // Tenta abrir o arquivo, caso não exista cria um novo
    arq.open("pagina.dat", ios::binary | fstream::in | fstream::out);
    if (!arq.is_open()) {
        cout << "Criando arquivo pagina.dat\n";
        arq.open("pagina.dat", ios::binary | fstream::in | fstream::out | fstream::trunc);
        cout << "Digite o numero maximo de registros: ";
        cin >> n;
        inicializar(arq, n);
    }

    // Loop do menu de operações
    while (op != 0) {
        cout << "\nMenu:\n1 - Insere\n2 - Insere ordenado\n3 - Remove\n4 - Pesquisa\n5 - Imprime Lista\n6 - Imprime Estrutura\n7 - Imprime Livres\n0 - Sair\nOpcao: ";
        cin >> op;

        switch (op) {
            case 1:
                cout << "Chave: "; cin >> d.chave;
                cout << "Nome: "; cin >> d.nome;
                inserir(arq, d);
                break;
            case 2:
                cout << "Chave: "; cin >> d.chave;
                cout << "Nome: "; cin >> d.nome;
                inserirOrdenado(arq, d);
                break;
            case 3:
                cout << "Chave a remover: "; cin >> d.chave;
                if (remover(arq, d)) cout << "Removido: " << d.nome << "\n";
                else cout << "Nao encontrado\n";
                break;
            case 4:
                cout << "Chave a pesquisar: "; cin >> d.chave;
                if (pesquisa(arq, d)) cout << "Encontrado: " << d.nome << "\n";
                else cout << "Nao encontrado\n";
                break;
            case 5:
                imprimirLista(arq);
                break;
            case 6:
                imprimirEstrutura(arq);
                break;
            case 7:
                imprimirFree(arq);
                break;
            case 0:
                cout << "Finalizando\n";
                break;
            default:
                cout << "Opcao invalida\n";
        }
    }
    return 0;
}
