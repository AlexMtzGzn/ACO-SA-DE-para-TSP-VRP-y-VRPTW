#include <stdio.h>
#include <stdlib.h>
#include "vrp_tw_aco.h"
#include "configuracion_vrp_tw.h"
#include "control_memoria.h"
#include "lista_ruta.h"


struct nodo_ruta *crear_nodo_ruta(struct hormiga *hormiga,struct cliente *cliente) {
    struct nodo_ruta *nodo_nuevo = malloc(sizeof(struct nodo_ruta));
    if (nodo_nuevo == NULL)
        return nodo_nuevo;
    
    nodo_nuevo->cliente = cliente->id_cliente;
    nodo_nuevo->siguiente = NULL;
    hormiga->tabu[cliente->id_cliente] = 1;
    
    return nodo_nuevo;
}


bool es_vacia_lista_ruta(struct lista_ruta *ruta) {
    return ruta->cabeza == NULL;
}


void insertar_cliente_ruta(struct hormiga * hormiga ,struct vehiculo *vehiculo, struct cliente *cliente) {
    struct nodo_ruta *nodo_nuevo = crear_nodo_ruta(hormiga,cliente);
    
    if (nodo_nuevo != NULL) {
        if (es_vacia_lista_ruta(vehiculo->ruta)) {
            vehiculo->ruta->cabeza = vehiculo->ruta->cola = nodo_nuevo;
        } else {
            vehiculo->ruta->cola->siguiente = nodo_nuevo;
            vehiculo->ruta->cola = nodo_nuevo;
        }
        vehiculo->clientes_contados++;  
    } else {
        printf("\nError al asignar memoria al nodo de la ruta.");
    }
}

