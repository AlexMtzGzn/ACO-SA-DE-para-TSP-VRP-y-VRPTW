#ifndef LISTA_FLOTA_H
#define LISTA_FLOTA_H

#include <stdbool.h>
#include "../includes/estructuras.h"


struct nodo_vehiculo *crearNodo(struct hormiga *hormiga, struct vrp_configuracion *vrp, int id);
bool es_Vacia_Lista(struct lista_vehiculos *flota);
void inserta_vehiculo_flota(struct hormiga *hormiga, struct vrp_configuracion *vrp, int id);
struct lista_vehiculos* copiar_lista_vehiculos(struct lista_vehiculos *original);
void liberar_vehiculo(struct vehiculo *vehiculo);
void liberar_lista_vehiculos(struct lista_vehiculos *lista);

#endif /*lista_flota.h*/