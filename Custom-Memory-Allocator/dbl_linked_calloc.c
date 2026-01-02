#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

typedef struct nodo{
    char free;
    size_t size;
    struct nodo *next;
    struct nodo *prev;
}nodo;

nodo* inizio;

void inizziallizza(nodo *n, size_t size){
    n->free = 1;
    n->next = NULL;
    n->size = size-sizeof(nodo);
    n->prev = NULL;
}

void nuovo(nodo* new, nodo* attuale, int dim){
    // inizializzo il nodo nuovo (libero)
    new->free = 1;
    new->size = attuale->size - dim - sizeof(nodo);
    new->next = attuale->next;
    new->prev = attuale;

}

void *nuovoNodo(nodo* attuale, size_t s, int n){
    char *base = (char *)attuale;

    // se entra un nodo dopo la nuova allocazione
    if(attuale->size >= n*s+sizeof(nodo)){

        nodo *new = (nodo *)(base + sizeof(nodo) + n * s);// il nuovo indirizzo è attuale + la dimensioen totrale dell array piu la sizeof del nodo per la gestione
        nodo* suc = attuale->next;   // salva il vecchio next

        nuovo(new, attuale, s*n)

        //aggiorno i valori di attuale
        if (attuale->next != NULL)
            suc->prev = new;

        attuale->size = s*n;
        attuale->free = 0;
        attuale->next = new;

    }
    else{
        attuale->free = 0;
    }


    return (void*)((char*)attuale + sizeof(nodo));
}

// [0]->[]->[]
//   <-  <-
/*
0 --> next =1, prev=NULL;
1 --> next = 2, prev=0;
2--> next = NULL, prev=1;

aggiunta nodo in mezzo

0 --> next =1, prev=NULL;
1 --> next = 2, prev=0; -------> agggrionamento next = new
new-> next = 1->next, prev = 2->prev;
2--> next = NULL, prev=1; -----> aggiornamento prev = new

*/

void *my_Calloc(int n, size_t s){
    nodo* attuale = inizio;

    for(;attuale != NULL; attuale=attuale->next){
        if(attuale->free && attuale->size >= n*s){
            void* dati = nuovoNodo(attuale, s, n);
            memset(dati, 0, n*s); // azzerare tutti i byte
            return dati;
        }
    }
    return NULL;
}


void unisci(nodo *corrente, nodo *successivo){
    corrente->size = corrente->size + sizeof(nodo) + successivo->size;
    corrente->next = successivo->next;

    if (successivo->next != NULL)
        successivo->next->prev = corrente;
}

void my_free(void* indirizzo){
    nodo *elimina = (nodo*)((char*)indirizzo - sizeof(nodo));
    elimina->free = 1;

    // 1. Fusione con il successivo, se possibile
    if (elimina->next != NULL && elimina->next->free == 1) {
        unisci(elimina, elimina->next);
    }

    // 2. Fusione con il precedente, se possibile
    if (elimina->prev != NULL && elimina->prev->free == 1) {
        unisci(elimina->prev, elimina);
    }
}

// TEST
// Funzione per stampare lo stato dell'heap
void stampaHeap() {

    nodo *curr = inizio;
    int i = 0;

    printf("\n--- STATO HEAP ---\n");

    while(curr != NULL){
        printf("Blocco %d -> size: %zu | free: %d | next: %p | addr: %p | array: %p\n",
               i, curr->size, curr->free, 
               (void*)curr->next, 
               (void*)curr,
               curr->array);

        curr = curr->next;
        i++;
    }

    printf("-------------------\n");
}


int main(int argc, char** argv){ 

    if (argc != 2) {
        printf("Uso: %s <dimensione_heap>\n", argv[0]);
        return 1;
    }

    size_t size = atoi(argv[1]);
    if(size<sizeof(nodo))
        fprintf("dim troppo piccola");


    void* memoria = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (memoria == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // inizializzo
    inizio = (nodo*)memoria;
    inizziallizza(inizio, size);

    printf("Heap appena inizializzato:\n");
    stampaHeap();

    // allocazione: 5 interi
    printf("\nChiamo my_Calloc(5, sizeof(int))...\n");
    int *a = my_Calloc(5, sizeof(int));

    printf("\nHeap dopo my_Calloc:\n");
    stampaHeap();

    // controllo sul contenuto
    if (a) {
        printf("\nContenuto array allocato:\n");
        for (int i = 0; i < 5; i++)
            printf("%d ", a[i]);  // deve stampare cinque 0
        printf("\n");
    }
    int *b = my_Calloc(17, sizeof(int));
    int *d = my_Calloc(17, sizeof(int));
    stampaHeap();

    my_free(b);
    stampaHeap();
    int *h = my_Calloc(2, sizeof(int));
    stampaHeap();
    my_free(a);
    my_free(b);
    my_free(c);
    stampaHeap();

    return 0;
}
