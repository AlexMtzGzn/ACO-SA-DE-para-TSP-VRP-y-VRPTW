#ifndef LISTA_FLOTA_H
#define LISTA_FLOTA_H

#include <stdbool.h>
#include "vrp_tw_aco.h"

typedef struct nodo_vehiculo {
    vehiculo vehiculo_data;  
    struct nodo_vehiculo *siguiente;
} nodo_vehiculo;

typedef struct lista_vehiculos {
    nodo_vehiculo *cabeza;
    nodo_vehiculo *cola;
} lista_vehiculos;


#endif /*lista_flota.h*/