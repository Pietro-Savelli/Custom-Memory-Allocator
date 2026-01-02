#  Custom Memory Allocator 

Questo repository contiene diverse implementazioni didattiche delle funzioni di gestione della memoria (`malloc`, `calloc`, `realloc`, `free`) scritte in C.


##  Contenuto

### 1. Allocatore con Lista Doppia (`dbl_linked_calloc.c`)
- **Struttura:** Doubly Linked List (puntatori `prev` e `next`).
- **Features:** Implementa `my_Calloc` e `my_free`.

### 2. Allocatore con mmap (`simple_malloc.c`)
- **Struttura:** Singly Linked List.
- **Tecnologia:** Usa la syscall `mmap` per richiedere pagine di memoria al sistema operativo.
- **Test:** Include un main che simula operazioni di lettura/scrittura su file usando la memoria allocata.

### 3. Realloc Semplice (`my_realloc.c`)
- Implementazione base di `realloc` e gestione dell'espansione dei blocchi.
- copre i casi standard ma non gestisce ancora tutti i casi.
