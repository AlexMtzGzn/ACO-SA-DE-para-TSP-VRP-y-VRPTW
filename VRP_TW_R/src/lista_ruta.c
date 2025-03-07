#include <stdio.h>
#include <stdlib.h>
#include "lista_ruta.h"

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

void inserta_Principio_Lista(struct Lista *lista, int *elemento)
{

    struct Nodo *nodoNuevo = crearNodo(elemento);

    if (nodoNuevo != NULL)
    {
        if (es_Vacia_Lista(lista))
        
            lista->cabeza = lista->cola = nodoNuevo;
        
        else
        {
            nodoNuevo->siguiente = lista->cabeza;
            lista->cabeza = nodoNuevo;
        }
    }
    else
    
        printf("\nError al asignar memoria al nodo.");
    
}

void inserta_Final_Lista(struct Lista *lista, int *elemento)
{
    struct Nodo *nodoNuevo = crearNodo(elemento);

    if (nodoNuevo != NULL)
    {
        if (es_Vacia_Lista(lista))
        
            lista->cabeza = lista->cola = nodoNuevo;
        
        else
        {
            lista->cola->siguiente = nodoNuevo;
            lista->cola = nodoNuevo;
        }
    }
    else
    
        printf("\nError al asignar memoria al nodo.");
    
}

void inserta_Posicion_Lista(struct Lista *lista, int *elemento, int *posicion)
{
    struct Nodo *nodoNuevo = crearNodo(elemento);

    if (nodoNuevo != NULL)
    {
        if (es_Vacia_Lista(lista))
        
            lista->cabeza = lista->cola = nodoNuevo;
        
        else
        {
            struct Nodo *nodoActual = lista->cabeza, *nodoAnterior = NULL;
            int contador = 0;

            while (nodoActual != NULL && contador < *posicion)
            {
                nodoAnterior = nodoActual;
                nodoActual = nodoActual->siguiente;
                contador++;
            }

            if (contador == *posicion)
            {
                nodoNuevo->siguiente = nodoActual;

                if (nodoAnterior != NULL)
                
                    nodoAnterior->siguiente = nodoNuevo;
                
                else
                
                    lista->cabeza = nodoNuevo;
                

                if (nodoActual == NULL)
                
                    lista->cola = nodoNuevo;
                
            }
            else
            {
                printf("Posición fuera de rango. No se pudo insertar.\n");
                free(nodoNuevo);
            }
        }
    }
    else
        printf("Error al asignar memoria al nodo.\n");
}

void eliminar_Principio_Lista(struct Lista *lista)
{
    if (!es_Vacia_Lista(lista))
    {

        struct Nodo *nodoEliminar = lista->cabeza;
        lista->cabeza = nodoEliminar->siguiente;
        free(nodoEliminar);

        if(obtener_Longitud(lista) == 1)
            lista->cola = lista->cabeza;
        if(es_Vacia_Lista(lista))
            lista->cabeza = lista->cola = NULL;
    }
}

void eliminar_Final_Lista(struct Lista *lista)
{
    if (!es_Vacia_Lista(lista))
    {
        struct Nodo *nodoActual = lista->cabeza, *nodoAnterior = NULL;

        while (nodoActual->siguiente != NULL)
        {
            nodoAnterior = nodoActual;
            nodoActual = nodoActual->siguiente;
        }
        if (nodoAnterior == NULL)
        {
            lista->cabeza = lista->cola = NULL;
        }
        else
        {
            nodoAnterior->siguiente = NULL;
            lista->cola = nodoAnterior;
        }

        free(nodoActual);
    }
}

void eliminar_Posicion_Lista(struct Lista *lista, int *posicion)
{
    if (!es_Vacia_Lista(lista) && *posicion < obtener_Longitud(lista))
    {
        int contador = 0;
        struct Nodo *nodoActual = lista->cabeza, *nodoAnterior = NULL;

        while (nodoActual != NULL && contador != *posicion)
        {
            nodoAnterior = nodoActual;
            nodoActual = nodoActual->siguiente;
            contador++;
        }

        if (contador == *posicion)
        {
            if (nodoAnterior == NULL) 
            {
                lista->cabeza = nodoActual->siguiente;
                free(nodoActual);

                if (lista->cabeza == NULL) 
                    lista->cola = NULL;
            }
            else
            {
                nodoAnterior->siguiente = nodoActual->siguiente;
                free(nodoActual);

                if (nodoActual->siguiente == NULL)
                    lista->cola = nodoAnterior;
                
            }
        }
    }
}

void eliminar_valor_Lista(struct Lista *lista, int *elemento)
{
    if (!es_Vacia_Lista(lista))
    {
        struct Nodo *nodoActual = lista->cabeza, *nodoAnterior = NULL;

        while (nodoActual != NULL)
        {
            if (*elemento == nodoActual->dato)
            {
                if (nodoAnterior == NULL) 
                {
                    lista->cabeza = nodoActual->siguiente;
                    free(nodoActual);
                    nodoActual = lista->cabeza;
                    
                    if (lista->cabeza == NULL) 
                        lista->cola = NULL;
                }
                else
                {
                    nodoAnterior->siguiente = nodoActual->siguiente;
                    free(nodoActual);
                    nodoActual = nodoAnterior->siguiente;

                    if (nodoActual == NULL)
                        lista->cola = nodoAnterior;
                }
            }
            else
            {
                nodoAnterior = nodoActual;
                nodoActual = nodoActual->siguiente;
            }
        }
    }
}

int obtener_Elemento_En_Posicion(struct Lista *lista, int *elemento)
{
    if (es_Vacia_Lista(lista))
    
        return -1;
    
    else
    {
        struct Nodo *nodoActual = lista->cabeza;
        int posicion = 0;
        while (nodoActual != NULL)
        {
            if (nodoActual->dato == *elemento)
                return posicion;

            posicion++;
            nodoActual = nodoActual->siguiente;
        }
        return -1;
    }
}

int obtener_Posicion_Por_Elemento(struct Lista *lista, int *posicion)
{
    if (es_Vacia_Lista(lista) || *posicion < 0)
    
        return -1;
    
    else
    {
        struct Nodo *nodoActual = lista->cabeza;
        int contador = 0;

        while (nodoActual != NULL)
        {
            if (contador == *posicion)

                return nodoActual->dato;
            
            contador++;
            nodoActual = nodoActual->siguiente;
        }

        return -1;
    }
}

void imprimir_Lista(struct Lista *lista)
{
    if (es_Vacia_Lista(lista))
    
        printf("\nLa lista está vacía.\n");
    
    else
    {
        struct Nodo *nodoActual = lista->cabeza;

        printf("\nLista: ");

        while (nodoActual != NULL)
        {
            printf("%i", nodoActual->dato);

            if (nodoActual->siguiente != NULL)
                printf(" - ");

            nodoActual = nodoActual->siguiente;
        }

        printf("\n");
    }
}

void vaciar_Lista(struct Lista *lista)
{
    if (!es_Vacia_Lista(lista))
    {
        while (lista->cabeza != NULL)
            eliminar_Principio_Lista(lista);
        lista->cabeza = NULL;
    }
}

