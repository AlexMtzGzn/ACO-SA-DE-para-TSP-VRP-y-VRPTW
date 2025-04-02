#ifndef LISTA_RUTA_H
#define LISTA_RUTA_H

#include <stdbool.h>
#include "../includes/estructuras.h"

struct nodo_ruta *crear_nodo_ruta(struct hormiga * hormiga ,struct cliente *cliente);
bool es_vacia_lista_ruta(struct lista_ruta *ruta);
void insertar_cliente_ruta(struct hormiga * hormiga,struct vehiculo *vehiculo, struct cliente *cliente);
struct lista_ruta *copiar_ruta(struct vehiculo *vehiculo_original);
struct lista_ruta *copiar_ruta(struct vehiculo *vehiculo_original);
void liberar_ruta(struct lista_ruta *ruta);
#endif /*lista_ruta.h*/