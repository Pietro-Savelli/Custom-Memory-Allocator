#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>
#include <sys/mman.h>

// QUANDO SI ESEGUE VA INSERITO LA 1)DIMENSIONE DELL' HEAP E ALMENO IL 2)FILE DA CUI LEGGERE I DATI, in aggiunta si puo inserire il file in cui dover scrivere i dati ordinati per l'output


typedef struct malloc_2{
	size_t size;
	int free;
	struct malloc_2 *next;
}block;

block *inizioHeap;

void inizializza(size_t dimensione){
	inizioHeap->size = dimensione-sizeof(block);
	inizioHeap->free = 1;
	inizioHeap->next = NULL;
}


void *alloca(block* libero, size_t dimensione){

	if(libero->size>dimensione+sizeof(block)+1){
		block *new = (block*)((char*)libero + sizeof(block) + dimensione);
		// inizializzo il nuovo
		new->next = libero->next;
		new->free = 1;
		new->size = libero->size-sizeof(block)-dimensione;

		//aggiorno libero
		libero->next = new;
		libero->free = 0;
		libero->size = dimensione;
	}
	else{
		libero->free = 0;
	}

	return (char*)libero+sizeof(block); // rivedi cosa torna
}


void *my_malloc(size_t dimensione){
	block *attuale = inizioHeap;
	while(attuale!=NULL){
		if(attuale->free && attuale->size>=dimensione){
			return alloca(attuale, dimensione);
		}
		attuale = attuale->next;
	}
	return NULL;
}


void merge(block* primo, block* secondo){
	primo->size = primo->size+secondo->size+sizeof(block);
	primo->next = secondo->next;
}

void my_free(void* elimina){
	block *e = (block*)((char*)elimina - sizeof(block));
	e->free = 1;

	block *unisci = inizioHeap;
	while(unisci->next!=NULL){
		if(unisci->free && unisci->next->free){
			merge(unisci, unisci->next);
		}
		else
			 unisci = unisci->next;
	}
}

void stampa(){
	block *scorri = inizioHeap;
	for(; scorri!=NULL; scorri=scorri->next){
		printf("%zu|%d|%p|%p\n", scorri->size, scorri->free, scorri, scorri->next);
		printf("=========================================\n");
	}
}


int main(int argc, char const *argv[]){

	if(argc<3){
		fprintf(stderr, "non hai inserito abbastanza dati");
		exit(1);
	}

	// controllo se la grandezza dell heap va bene almeno nel caso limite
	size_t dimensione = atoi(argv[1]);
	if(dimensione<=sizeof(block)){
		fprintf(stderr, "la dimensione %d e' troppo piccola per iniziazlizare la memoria", dimensione);
		exit(1);
	}

	// inizializzo la memoria
	void *mem = mmap(NULL, dimensione, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	inizioHeap = (block*)mem;
	inizializza(dimensione);

	//leggo il file input e le sue informazioni
	int fd_in = open(argv[2], O_RDONLY);
	if(fd_in==-1){
		fprintf(stderr, "errore in apertura del file input");
		exit(1);
	}

	struct stat statistiche;
	
	if(fstat(fd_in, &statistiche) == -1){
		fprintf(stderr, "errore in esecuzione di fstat");
		exit(1);
	}

	// alloco spazio a sufficenza per poter scrivere tutto 
	char *datiFile = my_malloc(statistiche.st_size);
	if(datiFile == NULL){
		fprintf(stderr, "errore my_malloc, spazio non sufficiente");
		exit(1);
	}

	if(read(fd_in, datiFile, statistiche.st_size) == -1){
		fprintf(stderr, "errore in esecuzione di fstat");
		exit(1);
	}
	close(fd_in);

	//leggo il file input e le sue informazioni
	int fd_out = open(argv[2], O_RDWR|O_TRUNC|O_CREAT, S_IRWXU);
	if(fd_out==-1){
		fprintf(stderr, "errore in apertura del file out");
		exit(1);
	}

	//selezione file
	char *scorri = datiFile;
	while (scorri < datiFile + statistiche.st_size){
		char *inizio = scorri;

		int validita = *(int*)scorri;
		scorri+=sizeof(int);

		size_t lunghezza = *(size_t*)scorri;
		scorri+=sizeof(size_t);

		if(validita%2==0 && lunghezza>200){
			write(fd_out, inizio, sizeof(int)+sizeof(size_t)+lunghezza);
		}
		scorri += lunghezza;
	}

	close(fd_out);
	my_free(datiFile);
	munmap(mem, dimensione);
}