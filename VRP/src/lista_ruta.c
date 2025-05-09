#include <stdlib.h>
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
        hormiga->tabu[cliente->id_cliente] = 1;
        hormiga->tabu_contador++;
    }

    return nodo_nuevo; // Se retorna el nodo recién creado
}

// Función para verificar si una lista de ruta está vacía
bool es_vacia_lista_ruta(struct lista_ruta *ruta)
{
    return ruta == NULL || ruta->cabeza == NULL; // Devuelve true si la ruta es NULL o no tiene elementos
}

// Función para insertar un cliente en la ruta de un vehículo
void insertar_cliente_ruta(struct hormiga *hormiga, struct vehiculo *vehiculo, struct cliente *cliente)
{
    // Se crea un nuevo nodo de ruta para el cliente
    struct nodo_ruta *nodo_nuevo = crear_nodo_ruta(hormiga, cliente);

    // Si la lista de ruta está vacía, el nuevo nodo será cabeza y cola
    if (es_vacia_lista_ruta(vehiculo->ruta))
    {
        vehiculo->ruta->cabeza = vehiculo->ruta->cola = nodo_nuevo;
    }
    else // Si no está vacía, se añade al final de la lista
    {
        vehiculo->ruta->cola->siguiente = nodo_nuevo;
        vehiculo->ruta->cola = nodo_nuevo;
    }

    // Se incrementa el contador de clientes si el cliente no es el depósito (ID 0)
    if (cliente->id_cliente != 0)
        vehiculo->clientes_contados++;
}

// Función para copiar la ruta de un vehículo en una nueva estructura
struct lista_ruta *copiar_ruta(struct vehiculo *vehiculo_original)
{
    // Se asigna memoria para la nueva lista de ruta
    struct lista_ruta *ruta_nueva = malloc(sizeof(lista_ruta));
    ruta_nueva->cabeza = NULL;
    ruta_nueva->cola = NULL;

    // Se itera sobre la ruta original para copiar los nodos
    struct nodo_ruta *actual = vehiculo_original->ruta->cabeza;
    while (actual != NULL)
    {
        // Se asigna memoria para un nuevo nodo y se copian los datos
        struct nodo_ruta *nuevo_nodo = malloc(sizeof(struct nodo_ruta));
        nuevo_nodo->cliente = actual->cliente;
        nuevo_nodo->siguiente = NULL;

        // Si la nueva lista está vacía, el nodo es la cabeza y la cola
        if (ruta_nueva->cabeza == NULL)
        {
            ruta_nueva->cabeza = nuevo_nodo;
            ruta_nueva->cola = nuevo_nodo;
        }
        else // Si no, se añade al final de la lista
        {
            ruta_nueva->cola->siguiente = nuevo_nodo;
            ruta_nueva->cola = nuevo_nodo;
        }

        actual = actual->siguiente; // Se avanza al siguiente nodo en la ruta original
    }

    return ruta_nueva; // Se retorna la copia de la ruta
}

// Función para liberar la memoria de una lista de ruta
void liberar_ruta(struct lista_ruta *ruta)
{
    struct nodo_ruta *cliente_actual = ruta->cabeza;
    
    // Se libera cada nodo de la lista de ruta
    while (cliente_actual)
    {
        struct nodo_ruta *cliente_temp = cliente_actual;
        cliente_actual = cliente_actual->siguiente;
        free(cliente_temp);
    }
    free(ruta);
}

void eliminar_cliente_ruta(struct vehiculo *vehiculo, struct vrp_configuracion *vrp, int cliente, double **instancia_distancias) {
    struct nodo_ruta *anterior = vehiculo->ruta->cabeza;
    struct nodo_ruta *actual = anterior->siguiente;
    
    while (actual != NULL) {
        if (actual->cliente == cliente) {
            // Actualizar la capacidad acumulada del vehículo
            vehiculo->capacidad_acumulada -= vrp->clientes[cliente].demanda_capacidad;
            
            // Actualizar el enlace en la lista
            anterior->siguiente = actual->siguiente;
            
            // Actualizar la cola si estamos eliminando el último nodo
            if (actual == vehiculo->ruta->cola) {
                vehiculo->ruta->cola = anterior;
            }
            
            // Liberar memoria y actualizar contador
            free(actual);
            vehiculo->clientes_contados--;
            return;
        }
        anterior = actual;
        actual = actual->siguiente;
    }
}

bool insertarClienteEnPosicion(struct vehiculo *vehiculo, struct vrp_configuracion *vrp, int cliente, int posicion, double **instancia_distancias) {
    // Verificar capacidad antes de insertar
    double demanda = vrp->clientes[cliente].demanda_capacidad;
    if (vehiculo->capacidad_acumulada + demanda > vehiculo->capacidad_maxima) {
        return false;  // No hay capacidad suficiente
    }
    
    struct nodo_ruta *nuevo = asignar_memoria_nodo_ruta();
    if (!nuevo) return false;
    
    nuevo->cliente = cliente;
    nuevo->siguiente = NULL;
    
    struct nodo_ruta *actual = vehiculo->ruta->cabeza->siguiente;
    struct nodo_ruta *previo = vehiculo->ruta->cabeza;
    int pos_actual = 0;
    
    while (actual != NULL) {
        if (pos_actual == posicion) {
            nuevo->siguiente = actual;
            previo->siguiente = nuevo;
            
            // Actualizar la capacidad acumulada
            vehiculo->capacidad_acumulada += demanda;
            vehiculo->clientes_contados++;
            return true;
        }
        previo = actual;
        actual = actual->siguiente;
        pos_actual++;
    }
    
    // Si llegamos al final, insertar antes del depósito final o al final de la lista
    nuevo->siguiente = actual;
    previo->siguiente = nuevo;
    
    // Si estamos insertando en el último nodo, actualizar la cola
    if (actual == NULL) {
        vehiculo->ruta->cola = nuevo;
    }
    
    // Actualizar la capacidad acumulada
    vehiculo->capacidad_acumulada += demanda;
    vehiculo->clientes_contados++;
    return true;
}
