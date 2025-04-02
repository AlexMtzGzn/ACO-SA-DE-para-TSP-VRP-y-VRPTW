#ifndef LISTA_FLOTA_H
#define LISTA_FLOTA_H

#include <stdbool.h>
#include "../includes/estructuras.h"

// Función para crear un nuevo nodo de vehículo en la flota de una hormiga
struct nodo_vehiculo *crearNodo(struct hormiga *hormiga, struct vrp_configuracion *vrp, int id);

// Función que verifica si una lista de vehículos está vacía
bool es_Vacia_Lista(struct lista_vehiculos *flota);

// Función para insertar un vehículo en la flota de una hormiga
void inserta_vehiculo_flota(struct hormiga *hormiga, struct vrp_configuracion *vrp, int id);

// Función para copiar una lista de vehículos (profundidad) de una flota a otra
struct lista_vehiculos* copiar_lista_vehiculos(struct lista_vehiculos *original);

// Función para liberar la memoria de un vehículo
void liberar_vehiculo(struct vehiculo *vehiculo);

// Función para liberar la memoria de una lista de vehículos
void liberar_lista_vehiculos(struct lista_vehiculos *lista);

#endif /*lista_flota.h*/
