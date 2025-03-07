#ifndef LISTA__RUTA_H
#define LISTA_RUTA_H

#include <stdbool.h>
typedef struct nodo_ruta {
    int cliente;
    struct nodo_ruta *siguiente;
} nodo_ruta;

typedef struct lista_ruta {
    nodo_ruta *cabeza;
    nodo_ruta *cola;
} lista_ruta;



#endif /*lista_ruta.h*/