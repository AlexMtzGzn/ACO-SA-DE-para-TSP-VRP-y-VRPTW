#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>

struct Nodo
{
    int dato_entero;
    double dato_doble;
    struct Nodo *siguiente;
};

struct Lista
{
    struct Nodo *cabeza, *cola;
};

//Crear Nodo Nuevo
struct Nodo* crearNodo_entero(int elemento);
struct Nodo* crearNodo_doble(double elemento);
//Operaciones Generales
bool es_Vacia_Lista(struct Lista *lista);
int obtener_Longitud(struct Lista *lista);

//Insertar
void inserta_Principio_Lista(struct Lista *lista,int * elemento);
void inserta_Final_Lista(struct Lista *lista,int * elemento);
void inserta_Posicion_Lista(struct Lista * lista, int *elemento, int *posicion);
void inserta_lista_ordenada_entero(struct Lista *lista, int elemento);
void inserta_lista_ordenada_doble(struct Lista *lista, double elemento);

//Eliminacion
void eliminar_Principio_Lista(struct Lista *lista);
void eliminar_Final_Lista(struct Lista *lista);
void eliminar_Posicion_Lista(struct Lista *lista, int * posicion);
void eliminar_valor_Lista(struct Lista *lista, int *elemento);

// Busqueda
int obtener_Elemento_En_Posicion(struct Lista *lista,int *elemento);
int obtener_Posicion_Por_Elemento(struct Lista *lista,int *posicion);
void imprimir_Lista(struct Lista *lista);
void vaciar_Lista(struct Lista *lista);


#endif /*lista.c*/