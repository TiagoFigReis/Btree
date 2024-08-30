#include <stdio.h>
#include <stdlib.h>
#include "btree.h"

struct no {
    chave *chaves; // Array de chaves
    struct no **filhos;
    int n; // Número atual de chaves
    int folha; // 1 = nó é folha. 0 caso contrário.
    struct no *pai; // Pai do nó
};

struct Btree {
    struct no *raiz;
    int ordem;
    int numElementos;
};

no * alocaNo(int ordem) {
    no *novo_no = (no*)malloc(sizeof(no));

    // Erro de alocar nó
    if(!novo_no) {
        printf ("Erro ao alocar o no.\n");
        return NULL;
    }

    // Alocando espaço para as chaves --> Ímpar = ordem, par = ordem -1
    novo_no->chaves = (chave*)malloc(sizeof(chave) * (ordem - 1));

    // Erro de alocar chaves
    if(!novo_no->chaves) {
        printf("Erro ao alocar o vetor de chaves.\n");
        free(novo_no);
        return NULL;
    }

    // Alocando espaço para os filhos
    novo_no->filhos = (no**) malloc(sizeof(no*) * ordem);
    if(!novo_no->filhos) {
        printf ("Erro ao alocar os filhos do nó.\n");
        free(novo_no);
        return NULL;
    }

    // Inicializando nós filhos
    for(int i = 0; i < ordem; i++) {
        novo_no->filhos[i] = NULL;
    }

    novo_no->folha = 1;
    novo_no->n = 0;
    novo_no->pai = NULL;

    return novo_no;
}

void liberaNo (no *noRemovido){
    free(noRemovido->filhos);
    free(noRemovido->chaves);
    free(noRemovido);
    return;
}

Btree* criarArvore(int ordem) {
    Btree *arv = (Btree*)malloc(sizeof(Btree));

    if (!arv) {
        printf ("Erro ao alocar a árvore.\n");
        return NULL;
    }

    arv->ordem = ordem;
    arv->raiz = alocaNo(ordem);
    arv->numElementos = 0;

    return arv;
}

void addChave(no *aux, chave novaChave) {
    int indice = aux->n;
    // Procurando o local certo para inserir a chave
    while (indice != 0 && novaChave.valor < aux->chaves[indice - 1].valor) {
        aux->chaves[indice] = aux->chaves[indice-1]; // Dá um shift para a direita na chave
        aux->filhos[indice+1] = aux->filhos[indice]; // Dá um shift para a direita no filho
        indice--;
    }
    aux->filhos[indice+1] = aux->filhos[indice];
    aux->n++; // Aumenta o número de chaves
    aux->chaves[indice] = novaChave; // Insere a chave
}

no* split(Btree *arv, no *aux, chave chaveNova){
    // Alocando um novo nó para dividir o nó aux
    no *novoNo = alocaNo(arv->ordem); // Aloca espaço para um novo nó
    no *pai; // Prepara variável do pai
    int indice, i, tamanho = aux->n;
    chave meio;

    // Verifica se o nó foi alocado com sucesso
    if (!novoNo){
        return NULL;
    }

    novoNo->folha = aux->folha; // Define a mesma propriedade de folha no nó irmão
    indice = 0;
    for (i = (aux->n/2) +1; i < tamanho; i++){ // Copia a segunda metade do nó aux para o novo nó
        novoNo->chaves[indice] = aux->chaves[i]; // Copia a chave para o novo nó
        novoNo->filhos[indice] = aux->filhos[i]; // Copia o filho à esquerda da chave antiga pro novo nó
        novoNo->filhos[indice + 1] = aux->filhos[i + 1]; // Copia o filho à direita da chave antiga pro novo nó
        if (novoNo->filhos[indice]) { // Caso os filhos da chave que foi transferida exista
            novoNo->filhos[indice]->pai = novoNo; // Seta o pai do filho para o novo nó criado
            novoNo->filhos[indice + 1]->pai = novoNo; // Seta o pai do filho para o novo nó criado
        }
        novoNo->n++; // Aumenta o número de chaves no novo nó
        aux->n--; // Diminui o número de chaves no nó antigo
        indice ++; // Incrementa o índice
    }

    meio = aux->chaves[aux->n - 1];
    if (aux == arv->raiz){ // Se o nó que foi dividido é a raiz
        no *novaRaiz = alocaNo(arv->ordem); // Aloca uma nova raiz
        if (!novaRaiz){
            return NULL;
        }
        aux->n--; // Diminui o número de chaves no nó antigo
        novaRaiz->chaves[0] = meio; // A raiz recebe a chave do meio
        novaRaiz->n = 1; // Aumenta o número de chaves na nova raiz
        novaRaiz->filhos[0] = aux; // O filho da esquerda da nova raiz é o nó antigo
        novaRaiz->filhos[1] = novoNo; // O filho da direita da nova raiz é o novo nó
        novaRaiz->folha = 0; // A nova raiz não é folha
        aux->pai = novoNo->pai = novaRaiz; // Seta o pai dos nós
        arv->raiz = novaRaiz; // A nova raiz da árvore é a nova raiz criada
    } else {
        if (aux->pai->n == arv->ordem - 1){ // Se o pai do nó estiver cheio !!!!!!!!!!!!!
            pai = split(arv, aux->pai, meio); // Split  no pai
            if (meio.valor < pai->chaves[0].valor) { // Se a chave do meio for menor que a primeira chave do pai
                pai = aux->pai; // O pai passa a ser o pai do nó antigo
            }
            aux->n--; // Diminui o número de chaves no nó antigo
            indice = pai->n - 1; // Índice recebe o número de chaves do pai
            while (indice != 0 && meio.valor != pai->chaves[indice].valor) { // Encontra a posição correta para inserir a chave
                indice--; // Decrementa o índice
            }
            novoNo->pai = pai; // O pai do novo nó é o pai do nó antigo
            pai->filhos[indice + 1] = novoNo;
        } else { // Se o pai do nó não estiver cheio
            pai = aux->pai;
            indice = pai->n;
            while (indice != 0 && chaveNova.valor < pai->chaves[indice - 1].valor) { // Encontra a posição correta para inserir a chave
                pai->chaves[indice] = pai->chaves[indice - 1]; // Dá um shift para a direita na chave
                pai->filhos[indice + 1] = pai->filhos[indice]; // Dá um shift para a direita no filho
                indice--; // Decrementa o índice
            }
            pai->chaves[indice] = meio; // Insere o elemento do meio do aux no pai
            novoNo->pai = aux->pai; // O pai do novo nó é o pai do nó antigo
            pai->filhos[indice + 1] = novoNo; // O filho à direita da chave antiga é o novo nó
            pai->n++; // Aumenta o número de chaves no pai
            aux->n--; // Diminui o número de chaves no nó antigo
        }
    }

    if (meio.valor < chaveNova.valor){ // Se o valor inserido for maior que o meio do nó antigo
        aux = novoNo;  // Seta o aux para o novo nó
    }

    addChave(aux, chaveNova);
    return novoNo;
}



int insereChave(Btree *arv, chave novaChave) {
    no *aux = arv->raiz;
    int indice;

    // Achando o nó folha onde será inserido o valor
    while (!aux->folha) { // Enquanto não for folha
        indice = 0;
        while (indice < aux->n && novaChave.valor > aux->chaves[indice].valor) { // Encontra nó folha para inserir
            indice++;
        }
        aux = aux->filhos[indice];
    }

    // Verifica se o nó está cheio
    if (aux->n != arv->ordem - 1) { // Se não estiver cheio, chama a função addChave
        addChave(aux, novaChave);
    } else { // Se estiver cheio, chama a função split
        aux = split(arv, aux, novaChave);
    }
    if (!aux) {
        return 0;
    }

    arv->numElementos++;
    return 1;
}

int removeChave(Btree *arv, int chaveRemover){
    no *aux = arv->raiz;
    no *predecessor;
    int indice = 0;

    // Achando o nó onde está o valor a ser removido
    while (aux && aux->chaves[indice].valor != chaveRemover){ // Enquanto for diferente do valor a remover
        // Procura o elemento no nó ou o filho que terá que ir depois
        while (indice < aux->n && chaveRemover > aux->chaves[indice].valor) {
            indice++;
        }
        // Verifica se o elemento não foi encontrado ou se chegou no final do nó
        if (indice == aux->n || aux->chaves[indice].valor != chaveRemover) {
            aux = aux->filhos[indice];
            indice = 0;
        }
    }
    // Elemento não encontrado
    if (!aux){
        printf ("O elemento %d nao esta na arvore.\n", chaveRemover);
        return 0;
    }
    arv->numElementos--;
    // Se o elemento for folha
    if(aux->folha){
        // realiza um shift à esquerda das chaves a direita da chave a ser removida
        while (indice < aux->n - 1){
            aux->chaves[indice] = aux->chaves[indice + 1];
            indice++;
        }
        aux->n--;
    } else {
        // procura a chave predecessora
        predecessor = aux->filhos[indice];
        while (!predecessor->folha){
            predecessor = predecessor->filhos[predecessor->n];
        }
        //troca a chave predecessora com a chave a ser removida
        aux->chaves[indice] = predecessor->chaves[predecessor->n - 1];
        predecessor->n--;
        aux = predecessor;
    }
    if (aux != arv->raiz && aux->n < arv->ordem/2 - 1){
        balanceamento(arv, aux);
    }
    return 1;
}

int getNumElementos (Btree *arv){
    return arv->numElementos;
}

void rotacaodir(Btree *arv, no *noDesbal, int indiceNoDesbal){
    no *pai = noDesbal->pai, *irmao;
    irmao = pai->filhos[indiceNoDesbal-1]; // Pegando o irmão à esquerda
    addChave(noDesbal, pai->chaves[indiceNoDesbal-1]); // Adicionando a chave do pai no nó desbalanceado
    // Substituindo a chave do pai pela chave mais à direita do irmão à esquerda
    pai->chaves[indiceNoDesbal-1] = irmao->chaves[--irmao->n];
    // Colocando o filho mais à direita no irmão no filho mais a esquerda do nó Desbalanceado
    noDesbal->filhos[0] = irmao->filhos[irmao->n+1];
    if (irmao->filhos[irmao->n+1]) {
        irmao->filhos[irmao->n + 1]->pai = noDesbal;
    } 
    return;
}

void rotacaoesq(Btree *arv, no *noDesbal, int indiceNoDesbal){
    no *pai = noDesbal->pai, *irmao;
    int indice = 0;
    irmao = pai->filhos[indiceNoDesbal+1]; // Pegando o irmão à direita
    addChave(noDesbal, pai->chaves[indiceNoDesbal]); // Adicionando a chave do pai no nó desbalanceado
    pai->chaves[indiceNoDesbal] = irmao->chaves[0]; // Substituindo a primeira chave do irmão com a do pai
    // Colocando o filho mais à esquerda no irmão no filho mais à direita do nó Desbalanceado
    noDesbal->filhos[noDesbal->n] = irmao->filhos[0];
    if (irmao->filhos[0]) {
        irmao->filhos[0]->pai = noDesbal;
    }
    // Dando shift uma casa para esquerda em todas as chaves e filhos do irmão
    while (indice < irmao->n - 1){
        irmao->chaves[indice] = irmao->chaves[indice+1];
        irmao->filhos[indice] = irmao->filhos[indice+1];
        indice++;
    }
    irmao->filhos[indice] = irmao->filhos[indice+1];
    irmao->n--; // Decrementando a chave removida
    
    return;
}

no* merge(Btree *arv, no* noDesbal, int indiceNoDesbal){
    no *pai = noDesbal->pai, *irmao;
    int indiceIrmao = 0;
    // Caso o merge seja feito com o irmão à esquerda, transformar o nó desbalanceado no irmão e o irmão no nó desbalanceado
    if (indiceNoDesbal < pai->n){
        irmao = pai->filhos[indiceNoDesbal+1];
    } else {
        irmao = noDesbal;
        noDesbal = pai->filhos[--indiceNoDesbal];
    }

    //Descendo o pai para o no desbalanceado
    noDesbal->chaves[noDesbal->n++] = pai->chaves[indiceNoDesbal];

    //Passar os elementos do irmão para o no desbalanceado (merge)
    while (indiceIrmao < irmao->n){
        noDesbal->chaves[noDesbal->n] = irmao->chaves[indiceIrmao];//Passando a chaves do irmao para o no desbalanceado
        noDesbal->filhos[noDesbal->n] = irmao->filhos[indiceIrmao];//Passando o filho esquerdo da chave para o no desbalanceado
        noDesbal->filhos[noDesbal->n+1] = irmao->filhos[indiceIrmao+1];//Passando o filho direito da chave para o no desbalanceado
        if (irmao->filhos[indiceIrmao]){
            irmao->filhos[indiceIrmao]->pai = noDesbal;
            irmao->filhos[indiceIrmao+1]->pai = noDesbal;
        }
        noDesbal->n++;
        indiceIrmao++;
    }

    // Realizando shift à esquerda nos elementos do pai
    while (indiceNoDesbal < pai->n - 1){
        pai->chaves[indiceNoDesbal] = pai->chaves[indiceNoDesbal+1];
        pai->filhos[indiceNoDesbal+1] = pai->filhos[indiceNoDesbal+2];
        indiceNoDesbal++;
    }
    pai->n--;
    // Caso pai seja raiz e perdeu todos os elementos, atualizar raiz e liberar o espaço da raiz
    if (pai == arv->raiz && pai->n == 0){
        liberaNo(arv->raiz);
        arv->raiz = noDesbal;
        noDesbal->pai = NULL;
    }
    liberaNo (irmao);
    return noDesbal;
}



void balanceamento (Btree *arv, no *noDesbal){
    no* pai = noDesbal->pai;
    int indiceNoDesbal = 0;

    while (pai->filhos[indiceNoDesbal] != noDesbal){
        indiceNoDesbal++;
    }

    // Verificando se há irmão à esquerda e se remover uma chave dele não fará com que ele fique desbalanceado
    if ((indiceNoDesbal - 1 >= 0 && pai->filhos[indiceNoDesbal-1]->n - 1 >= arv->ordem/2 - 1)){

        rotacaodir(arv, noDesbal, indiceNoDesbal); // Realizando a rotação

    } else if ((indiceNoDesbal < pai->n && pai->filhos[indiceNoDesbal+1]->n - 1 >= arv->ordem/2 - 1)){
    // Verificando se há irmão à direita e se remover uma chave dele não fará com que ele fique desbalanceado
        rotacaoesq(arv, noDesbal, indiceNoDesbal); // Realizando a rotação
        
    } else {
    // A remoção de chave nos irmaos afeta o balanceamento deles, logo, realiza a operação de merge
        noDesbal = merge(arv, noDesbal, indiceNoDesbal);
        pai = noDesbal->pai;
        if (pai && pai != arv->raiz && pai->n < arv->ordem/2 - 1){
            balanceamento(arv, pai);
        }
    }
    return;
}

no* getRaiz (Btree *arv) {
    return arv->raiz;
}

void imprimirArvore(no *aux) {
    if (aux) {
        for (int i = 0; i < aux->n; i++) {
            printf("%d ", aux->chaves[i].valor);
        }
        printf ("- pai: ");
        if (aux->pai)
            for (int i = 0; i < aux->pai->n; i++) {
                printf("%d ", aux->pai->chaves[i].valor);
            }
        printf("\n");
        for(int i = 0; i<aux->n+1;i++){
            imprimirArvore(aux->filhos[i]);
        }
    }
    return;
}

int busca(no *aux, int nroMatricula) {
    int indice = 0;

    // Achando o nó onde está o valor a ser removido
    while (aux && aux->chaves[indice].valor != nroMatricula){ // Enquanto for diferente do valor a remover
        // Procura o elemento no nó ou o filho que terá que ir depois
        while (indice < aux->n && nroMatricula > aux->chaves[indice].valor) {
            indice++;
        }
        // Verifica se o elemento não foi encontrado ou se chegou no final do nó
        if (indice == aux->n || aux->chaves[indice].valor != nroMatricula) {
            aux = aux->filhos[indice];
            indice = 0;
        }
    }
    // Elemento não encontrado
    if (!aux){
        return 0;
    }
    return aux->chaves[indice].indice;

}
