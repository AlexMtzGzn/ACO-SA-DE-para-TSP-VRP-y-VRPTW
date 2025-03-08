#ifndef LISTA_RUTA_H
#define LISTA_RUTA_H

#include <stdbool.h>

// struct hormiga;
// struct vrp_configuracion;
struct vehiculo;
typedef struct nodo_ruta {
    int cliente;
    struct nodo_ruta *siguiente;
} nodo_ruta;

typedef struct lista_ruta {
    nodo_ruta *cabeza;
    nodo_ruta *cola;
} lista_ruta;

struct nodo_ruta *crear_nodo_ruta(struct hormiga * hormiga ,struct cliente *cliente);
bool es_vacia_lista_ruta(struct lista_ruta *ruta);
void insertar_cliente_ruta(struct hormiga * hormiga,struct vehiculo *vehiculo, struct cliente *cliente);

#endif /*lista_ruta.h*/