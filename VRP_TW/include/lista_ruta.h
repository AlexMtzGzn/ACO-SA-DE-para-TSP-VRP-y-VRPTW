#ifndef LISTA_RUTA_H
#define LISTA_RUTA_H

#include <stdbool.h>
#include "../include/estructuras.h"

// Función para crear un nuevo nodo en la ruta de una hormiga con un cliente específico
struct nodo_ruta *crear_nodo_ruta(struct hormiga *hormiga, struct cliente *cliente);

// Función que verifica si una lista de ruta está vacía
bool es_vacia_lista_ruta(struct lista_ruta *ruta);

// Función para insertar un cliente en la ruta de un vehículo en la flota de una hormiga
void insertar_cliente_ruta(struct hormiga *hormiga, struct vehiculo *vehiculo, struct cliente *cliente);

// Función para copiar una ruta de un vehículo original
struct lista_ruta *copiar_ruta(struct vehiculo *vehiculo_original);

// Función para liberar la memoria ocupada por una lista de ruta
void liberar_ruta(struct lista_ruta *ruta);

bool verificarRestricciones(struct vehiculo *vehiculo, struct vrp_configuracion *vrp, double **instancia_distancias);
bool verificarRestriccionesFlota(struct lista_vehiculos *flota, struct vrp_configuracion *vrp, double **instancia_distancias);
void eliminar_cliente_ruta(struct vehiculo *vehiculo, struct vrp_configuracion *vrp, int cliente, double **instancia_distancias);
void llenar_datos_clientes(struct lista_vehiculos *flota, struct vrp_configuracion *vrp, double **instancia_distancias);

bool insertarClienteEnPosicion(struct vehiculo *vehiculo, struct vrp_configuracion *vrp, int cliente, int posicion, double **instancia_distancias);

#endif /*lista_ruta.h*/
