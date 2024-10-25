#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

struct Nodo *crearNodo(int *elemento)
{
    struct Nodo *nodoNuevo = malloc(sizeof(struct Nodo));
    if (nodoNuevo == NULL)
        return nodoNuevo;

    nodoNuevo->dato = *elemento;
    nodoNuevo->siguiente = NULL;

    return nodoNuevo;
}

bool es_Vacia_Lista(struct Lista *lista)
{
    return lista->cabeza == NULL;
}

int obtener_Longitud(struct Lista *lista)
{
    if (es_Vacia_Lista(lista))

        return -1;

    else
    {
        struct Nodo *nodoActual = lista->cabeza;
        int longitud = 0;

        while (nodoActual != NULL)
        {
            nodoActual = nodoActual->siguiente;
            longitud++;
        }

        return longitud;
    }
}

void inserta_lista_ordenada_entero(struct Lista *lista, int elemento)
{
    struct Nodo *nodoNuevo = crearNodo(elemento);

    if (nodoNuevo != NULL)
    {
        if (es_Vacia_Lista(lista))

            lista->cabeza = lista->cola = nodoNuevo;

        else
        {
            struct Nodo *nodoActual = lista->cabeza, *nodoAnterior = NULL;

            while (nodoActual != NULL && nodoActual < elemento)
            {
                nodoAnterior = nodoActual;
                nodoActual = nodoActual->siguiente;
            }
        }
    }
    else
    {

        printf("\nError al asignar memoria al nodo.");
    }
}

void eliminar_valor_Lista(struct Lista *lista, int *elemento) {
    if (!es_Vacia_Lista(lista)) {
        struct Nodo *nodoActual = lista->cabeza, *nodoAnterior = NULL;

        while (nodoActual != NULL) {
            if (*elemento == nodoActual->dato) {
                if (nodoAnterior == NULL) { 
                    lista->cabeza = nodoActual->siguiente;
                    free(nodoActual);

                    if (lista->cabeza == NULL)
                        lista->cola = NULL;
                } else { 
                    nodoAnterior->siguiente = nodoActual->siguiente;
                    free(nodoActual);

                    if (nodoAnterior->siguiente == NULL)
                        lista->cola = nodoAnterior;
                }
                break;
            } else {
                nodoAnterior = nodoActual;
                nodoActual = nodoActual->siguiente;
            }
        }
    }
}