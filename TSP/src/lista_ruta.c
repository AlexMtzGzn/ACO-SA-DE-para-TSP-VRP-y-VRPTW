#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "../include/estructuras.h"
#include "../include/control_memoria.h"
#include "../include/lista_ruta.h"

// Función para crear un nuevo nodo de ruta con el cliente asignado a una hormiga
struct nodo_ruta *crear_nodo_ruta(struct hormiga *hormiga, struct cliente *cliente)
{
    // Se asigna memoria para un nuevo nodo de ruta
    struct nodo_ruta *nodo_nuevo = asignar_memoria_nodo_ruta();

    // Se almacena el ID del cliente en el nodo
    nodo_nuevo->cliente = cliente->id_cliente;
    nodo_nuevo->siguiente = NULL;

    // Se actualiza la lista tabú de la hormiga si el cliente aún no ha sido visitado
    if (hormiga->tabu[cliente->id_cliente] == 0)
    {
        hormiga->tabu[cliente->id_cliente] = 1; // Se marca al cliente como visitado
        hormiga->tabu_contador++; // Se incrementa el contador de clientes visitados
    }

    return nodo_nuevo; // Se retorna el nodo recién creado
}

// Función para verificar si una lista de ruta está vacía
bool es_vacia_lista_ruta(struct lista_ruta *ruta)
{
    return ruta == NULL || ruta->cabeza == NULL; // Devuelve true si la ruta es NULL o no tiene elementos
}

// Función para insertar un cliente en la ruta de un vehículo
void insertar_cliente_ruta(struct hormiga *hormiga, struct cliente *cliente)
{
    // Se crea un nuevo nodo de ruta para el cliente
    struct nodo_ruta *nodo_nuevo = crear_nodo_ruta(hormiga, cliente);
    struct lista_ruta *ruta = hormiga->ruta;

    // Si la lista de ruta está vacía, el nuevo nodo será cabeza y cola
    if (es_vacia_lista_ruta(ruta))
    {
        ruta->cabeza = ruta->cola = nodo_nuevo;
    }
    else // Si no está vacía, se añade al final de la lista
    {
        ruta->cola->siguiente = nodo_nuevo;
        ruta->cola = nodo_nuevo;
    }
}

// Función para copiar la ruta de un vehículo en una nueva estructura
struct lista_ruta *copiar_lista_ruta(struct lista_ruta *ruta_original)
{
    // Verificación de que la lista original no es nula
    if (ruta_original == NULL) {
        return NULL;
    }

    // Se asigna memoria para la nueva lista de ruta
    struct lista_ruta *ruta_nueva = asignar_memoria_lista_ruta();

    // Inicialización de la nueva lista como vacía
    ruta_nueva->cabeza = NULL;
    ruta_nueva->cola = NULL;

    // Se itera sobre la ruta original para copiar los nodos
    struct nodo_ruta *actual = ruta_original->cabeza;
    while (actual != NULL)
    {
        // Se crea un nuevo nodo con los mismos datos
        struct nodo_ruta *nuevo_nodo = asignar_memoria_nodo_ruta();
        nuevo_nodo->cliente = actual->cliente;
        nuevo_nodo->siguiente = NULL;

        // Añadir el nuevo nodo a la lista nueva
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

        // Avanzar al siguiente nodo de la ruta original
        actual = actual->siguiente;
    }

    return ruta_nueva; // Se retorna la copia de la ruta
}

// Función para liberar completamente una lista de ruta (nodos + estructura)
void liberar_lista_ruta(struct lista_ruta *ruta)
{
    if (ruta == NULL)
        return;

    struct nodo_ruta *cliente_actual = ruta->cabeza;

    // Liberamos todos los nodos de la lista
    while (cliente_actual)
    {
        struct nodo_ruta *cliente_temp = cliente_actual;
        cliente_actual = cliente_actual->siguiente;
        free(cliente_temp); // Liberamos cada nodo individualmente
    }

    free(ruta); // Finalmente, liberamos la estructura de la lista
}

// Función para vaciar una lista de ruta, pero manteniendo la estructura lista válida
void vaciar_lista_ruta(struct lista_ruta *ruta)
{
    if (ruta == NULL)
        return;

    struct nodo_ruta *cliente_actual = ruta->cabeza;

    // Liberamos todos los nodos de la lista
    while (cliente_actual)
    {
        struct nodo_ruta *cliente_temp = cliente_actual;
        cliente_actual = cliente_actual->siguiente;
        free(cliente_temp); // Liberamos cada nodo
    }

    // Reiniciamos la lista como vacía pero válida
    ruta->cabeza = NULL;
    ruta->cola = NULL;
}
