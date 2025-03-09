#ifndef LISTA_FLOTA_H
#define LISTA_FLOTA_H

#include <stdbool.h>
#include "vrp_tw_aco.h"
#include "configuracion_vrp_tw.h"

struct hormiga;
struct vrp_configuracion;
struct vehiculo;

typedef struct nodo_vehiculo {
    struct vehiculo *vehiculo;  
    struct nodo_vehiculo *siguiente;
} nodo_vehiculo;

typedef struct lista_vehiculos {
    nodo_vehiculo *cabeza;
    nodo_vehiculo *cola;
} lista_vehiculos;


struct nodo_vehiculo *crearNodo(struct hormiga *hormiga, struct vrp_configuracion *vrp);
bool es_Vacia_Lista(struct lista_vehiculos *flota);
void inserta_vehiculo_flota(struct hormiga *hormiga, struct vrp_configuracion *vrp);
struct lista_vehiculo * recupera_vehiculo_cola(struct hormiga *hormiga);
#endif /*lista_flota.h*/