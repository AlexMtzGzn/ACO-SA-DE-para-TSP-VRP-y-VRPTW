#ifndef LISTA_FLOTA_H
#define LISTA_FLOTA_H

#include <stdbool.h>
#include "vrp_tw_aco.h"
#include "configuracion_vrp_tw.h"

typedef struct nodo_vehiculo {
    struct vehiculo *vehiculo;  
    struct nodo_vehiculo *siguiente;
} nodo_vehiculo;

typedef struct lista_vehiculos {
    nodo_vehiculo *cabeza;
    nodo_vehiculo *cola;
} lista_vehiculos;

bool es_Vacia_Lista(struct lista_vehiculos *flota);
struct nodo_vehiculo *crearNodo(struct hormiga *hormiga, struct vrp_configuracion *vrp);
void inserta_lista_flota(struct hormiga *hormiga, struct vrp_configuracion *vrp);
#endif /*lista_flota.h*/