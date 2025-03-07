#include <stdio.h>
#include <stdlib.h>
#include "vrp_tw_aco.h"
#include "configuracion_vrp_tw.h"
#include "control_memoria.h"
#include "lista_ruta.h"


struct nodo_ruta *crear_nodo_ruta(struct cliente *cliente) {
    struct nodo_ruta *nodo_nuevo = malloc(sizeof(struct nodo_ruta));
    if (nodo_nuevo == NULL)
        return nodo_nuevo;
    
    nodo_nuevo->cliente = cliente;
    nodo_nuevo->siguiente = NULL;
    
    return nodo_nuevo;
}


bool es_vacia_lista_ruta(struct lista_ruta *ruta) {
    return ruta->cabeza == NULL;
}


void insertar_cliente_ruta(struct vehiculo *vehiculo, struct cliente *cliente) {
    struct nodo_ruta *nodo_nuevo = crear_nodo_ruta(cliente);
    
    if (nodo_nuevo != NULL) {
        if (es_vacia_lista_ruta(vehiculo->ruta)) {
            vehiculo->ruta->cabeza = vehiculo->ruta->cola = nodo_nuevo;
        } else {
            vehiculo->ruta->cola->siguiente = nodo_nuevo;
            vehiculo->ruta->cola = nodo_nuevo;
        }
        // Incrementar contador de clientes en el vehículo
        vehiculo->clientes_contados++;
    } else {
        printf("\nError al asignar memoria al nodo de la ruta.");
    }
}

