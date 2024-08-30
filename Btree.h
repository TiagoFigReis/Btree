#ifndef BTREE_BTREE_H
#define BTREE_BTREE_H

typedef struct chave{
    int valor;
    int indice;
}chave;

typedef struct no no;
typedef struct Btree Btree;

Btree* criarArvore(int ordem);
no* alocaNo(int ordem);
int insereChave(Btree *arv, chave valor);
no* split (Btree *arv, no* aux, chave valor);
int removeChave(Btree *arv, int chaveRemover);
void imprimirArvore (no *node);
no* getRaiz(Btree *arv);
int busca (no *aux, int nroMatricula);
void balanceamento (Btree *arv, no *noDesbal);
void rotacaodir (Btree *arv, no *noDesbal, int indiceNoDesbal);
void rotacaoesq (Btree *arv, no *noDesbal, int indiceNoDesbal);
no* merge(Btree *arv, no* noDesbal,int indiceNoDesbal);
void liberaNo (no *noRemovido);
int getNumElementos (Btree *arv);

#endif