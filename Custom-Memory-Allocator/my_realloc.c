#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef struct realloc{
	char free;
	size_t size;
	struct realloc *next;
}nodo;

nodo *inizoHeap;


void inizializza(size_t s){
	inizoHeap->free = 1;
	inizoHeap->size = s-sizeof(nodo);
	inizoHeap->next = NULL;
}

void *rialloca(void *posizione, size_t nuovaDim, void *puntatore){
	nodo *pos = (nodo*)posizione;
	nodo *new = (nodo*)((char*)posizione + sizeof(nodo) + nuovaDim);

	// inizializzo il new
	new->size = pos->size - nuovaDim - sizeof(nodo);
	new->free = 1;
	if(pos->next == NULL)
		new->next = NULL;
	else
		new->next = pos->next;

	//aggiorno il vecchio
	pos->size = nuovaDim;
	pos->free = 0;
	pos->next = new;

	return (void*)((char*)pos + sizeof(nodo));
}


void *myRealloc(void *puntatore, size_t nuovaDim){
	nodo* copia = inizoHeap;

	//Caso in cui si comporta come una malloc normale
	if(puntatore == NULL){
		for(;copia!=NULL; copia=copia->next){
			if(copia->free==1 && copia->size>=nuovaDim+sizeof(nodo)){
				return rialloca(copia, nuovaDim, puntatore);
			}
		}
	}

	else{
		nodo* attuale = (nodo*)((char*)puntatore-sizeof(nodo));

		//dimensione inserita piu piccola della precendete
		if(nuovaDim<attuale->size)
			return NULL;
		//stessa dimensione della precendete chiamata di allocazione
		if(nuovaDim==attuale->size)
			return puntatore;

		//dimensione accettabile
		attuale->free = 1;
		nodo *precendete = NULL;
		int temp = 0;
		while(copia!=NULL){
			if(copia->free==1 && copia->size>=nuovaDim+sizeof(nodo)){
				void *nuovo = rialloca(copia, nuovaDim, puntatore);
				memcpy(nuovo, puntatore, precendete->size);
				return nuovo;
			}
			
			precendete = copia;	
			copia = copia->next;
		}

	}	
	
	return NULL;
}

void myfree(void *puntatore){
	nodo *elimina = puntatore-sizeof(nodo);
	elimina->free = 1;
}

// TEST
// Funzione per stampare lo stato dell'heap
void stampaHeap() {
    nodo *curr = inizoHeap;
    int i = 0;
    printf("\n--- STATO HEAP ---\n");
    while(curr != NULL){
		printf("Blocco %d -> size: %zu | free: %d | next: %p | addr: %p\n",
       	i, curr->size, curr->free, (void*)curr->next, (void*)curr);
        curr = curr->next;
        i++;
    }
    printf("-------------------\n");
}

int main(int argc, char const *argv[]){

	size_t sizeHeap = atoi(argv[1]);
	if(sizeHeap<sizeof(nodo)){
		fprintf(stderr, "Errore: %d e' troppo piccola anche per ottinere il primo nodo", sizeHeap);
		exit(0);
	}

	void *memoria = malloc(sizeHeap);//mmap(NULL, sizeHeap, PROT_WRITE| PROT_READ, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0); non mi funziona su wind
	inizoHeap = (nodo*)memoria;

	printf("size: %d", inizoHeap->size);

	inizializza(sizeHeap);
	void *a = myRealloc(NULL, 7*sizeof(int));
	stampaHeap();
	a = myRealloc(a, 8*sizeof(int));
	void *b = myRealloc(NULL, 4*sizeof(int));
	void *c = myRealloc(NULL, 1);
	stampaHeap();
/*	a = myRealloc(a, 4*sizeof(int));
	if(a==NULL){
		fprintf(stderr, "Errore: Reallocazione di memoria piu piccola della precendete");
	}*/


	free(memoria);
}
