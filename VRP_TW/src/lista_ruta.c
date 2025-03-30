#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../includes/estructuras.h"
#include "../includes/control_memoria.h"
#include "../includes/lista_ruta.h"

struct nodo_ruta *crear_nodo_ruta(struct hormiga *hormiga, struct cliente *cliente)
{
    if (!hormiga || !cliente)
    {
        fprintf(stderr, "Error: hormiga o cliente es NULL en crear_nodo_ruta\n");
        return NULL;
    }

    struct nodo_ruta *nodo_nuevo = malloc(sizeof(struct nodo_ruta));
    if (!nodo_nuevo)
    {
        fprintf(stderr, "Error: No se pudo asignar memoria para nodo_ruta\n");
        return NULL;
    }

    nodo_nuevo->cliente = cliente->id_cliente;
    nodo_nuevo->siguiente = NULL;

    if (hormiga->tabu[cliente->id_cliente] == 0)
    {
        hormiga->tabu[cliente->id_cliente] = 1;
        hormiga->tabu_contador++;
    }

    return nodo_nuevo;
}

bool es_vacia_lista_ruta(struct lista_ruta *ruta)
{
    return ruta == NULL || ruta->cabeza == NULL;
}

void insertar_cliente_ruta(struct hormiga *hormiga, struct vehiculo *vehiculo, struct cliente *cliente)
{
    if (!vehiculo || !vehiculo->ruta)
    {
        fprintf(stderr, "Error: vehiculo o ruta es NULL en insertar_cliente_ruta\n");
        return;
    }

    struct nodo_ruta *nodo_nuevo = crear_nodo_ruta(hormiga, cliente);
    if (!nodo_nuevo)
    {
        fprintf(stderr, "Error: No se pudo crear el nodo de la ruta\n");
        return;
    }

    if (es_vacia_lista_ruta(vehiculo->ruta))
    {
        vehiculo->ruta->cabeza = vehiculo->ruta->cola = nodo_nuevo;
    }
    else
    {
        vehiculo->ruta->cola->siguiente = nodo_nuevo;
        vehiculo->ruta->cola = nodo_nuevo;
    }

    if (cliente->id_cliente != 0)
        vehiculo->clientes_contados++;
}

struct lista_ruta *copiar_ruta(struct vehiculo *vehiculo_original)
{
    struct lista_ruta *ruta_nueva = malloc(sizeof(lista_ruta));
    ruta_nueva->cabeza = NULL;
    ruta_nueva->cola = NULL;

    struct nodo_ruta *actual = vehiculo_original->ruta->cabeza;
    while (actual != NULL)
    {
        struct nodo_ruta *nuevo_nodo = malloc(sizeof(struct nodo_ruta));
        nuevo_nodo->cliente = actual->cliente;
        nuevo_nodo->siguiente = NULL;

        if (ruta_nueva->cabeza == NULL)
        {
            ruta_nueva->cabeza = nuevo_nodo;
            ruta_nueva->cola = nuevo_nodo;
        }
        else
        {
            ruta_nueva->cola->siguiente = nuevo_nodo;
            ruta_nueva->cola = nuevo_nodo;
        }

        actual = actual->siguiente;
    }

    return ruta_nueva;
}

void liberar_ruta(struct lista_ruta *ruta)
{
    if (ruta == NULL)
        return;

    struct nodo_ruta *actual = ruta->cabeza;
    while (actual != NULL)
    {
        struct nodo_ruta *temp = actual;
        actual = actual->siguiente;
        free(temp);
    }
    
    free(ruta);
}