#ifndef LISTA_RUTA_H
#define LISTA_RUTA_H

// Incluimos las bibliotecas necesarias
#include <stdbool.h>
#include "../include/estructuras.h"

// Función para crear un nuevo nodo en la ruta de una hormiga con un cliente específico
struct nodo_ruta *crear_nodo_ruta(struct hormiga *hormiga, struct cliente *cliente);

// Función que verifica si una lista de ruta está vacía
bool es_vacia_lista_ruta(struct lista_ruta *ruta);

// Función para insertar un cliente en la ruta de un vehículo en la flota de una hormiga
void insertar_cliente_ruta(struct hormiga *hormiga, struct vehiculo *vehiculo, struct cliente *cliente);

// Función para copiar una ruta de un vehículo original
struct lista_ruta *copiar_ruta(struct lista_ruta *ruta_original);

// Función para liberar la memoria ocupada por una lista de ruta
void liberar_ruta(struct lista_ruta *ruta);

#endif /*lista_ruta.h*/
