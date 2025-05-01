#include <stdlib.h>
#include "../include/estructuras.h"
#include "../include/control_memoria.h"
#include "../include/lista_flota.h"
#include "../include/lista_ruta.h"
#include "../include/salida_datos.h"

// Función para crear un nuevo nodo de vehículo
struct nodo_vehiculo *crearNodo(struct hormiga *hormiga, struct vrp_configuracion *vrp, int id)
{
    // Asignar memoria para el nodo del vehículo
    struct nodo_vehiculo *vehiculo_nuevo = asignar_memoria_nodo_vehiculo();

    // Asignar memoria para el vehículo y configurar sus parámetros
    vehiculo_nuevo->vehiculo = asignar_memoria_vehiculo();
    vehiculo_nuevo->vehiculo->id_vehiculo = id;
    vehiculo_nuevo->vehiculo->capacidad_maxima = vrp->num_capacidad;
    vehiculo_nuevo->vehiculo->capacidad_acumulada = 0.0;
    vehiculo_nuevo->vehiculo->clientes_contados = 0;
    vehiculo_nuevo->vehiculo->fitness_vehiculo = 0.0;
    
    // Asignar memoria para la lista de rutas y añadir el primer cliente
    vehiculo_nuevo->vehiculo->ruta = asignar_memoria_lista_ruta();
    vehiculo_nuevo->siguiente = NULL;
    insertar_cliente_ruta(hormiga, vehiculo_nuevo->vehiculo, &(vrp->clientes[0]));

    return vehiculo_nuevo;
}

// Función que verifica si la lista de vehículos está vacía
bool es_Vacia_Lista(struct lista_vehiculos *flota)
{
    return flota->cabeza == NULL;
}

// Función para insertar un vehículo en la flota de hormiga
void inserta_vehiculo_flota(struct hormiga *hormiga, struct vrp_configuracion *vrp, int id)
{
    // Crear un nuevo nodo de vehículo
    struct nodo_vehiculo *vehiculo_nuevo = crearNodo(hormiga, vrp, id);

    if (vehiculo_nuevo != NULL)
    {
        // Si la flota está vacía, establecer el nuevo vehículo como cabeza y cola
        if (es_Vacia_Lista(hormiga->flota))
        {
            hormiga->flota->cabeza = hormiga->flota->cola = vehiculo_nuevo;
        }
        else
        {
            // Si la flota no está vacía, agregar el vehículo al final
            hormiga->flota->cola->siguiente = vehiculo_nuevo;
            hormiga->flota->cola = vehiculo_nuevo;
        }
    }
    else
        // Si hubo un error al asignar memoria, mostrar mensaje de error
        imprimir_mensaje("Error al asignar memoria al nodo del vehiculo.");
}

// Función para copiar un vehículo original y crear una copia
struct vehiculo *copiar_vehiculo(struct vehiculo *original)
{
    // Asignar memoria para el nuevo vehículo
    struct vehiculo *nuevo_vehiculo = asignar_memoria_vehiculo();

    // Copiar los atributos del vehículo original
    nuevo_vehiculo->id_vehiculo = original->id_vehiculo;
    nuevo_vehiculo->capacidad_maxima = original->capacidad_maxima;
    nuevo_vehiculo->capacidad_acumulada = original->capacidad_acumulada;
    nuevo_vehiculo->clientes_contados = original->clientes_contados;
    nuevo_vehiculo->fitness_vehiculo = original->fitness_vehiculo;

    // Copiar la ruta asociada al vehículo original
    nuevo_vehiculo->ruta = copiar_ruta(original);

    return nuevo_vehiculo;
}

// Función para copiar una lista de vehículos
struct lista_vehiculos *copiar_lista_vehiculos(struct lista_vehiculos *original)
{
    // Asignar memoria para la nueva lista de vehículos
    struct lista_vehiculos *nueva_lista = asignar_memoria_lista_vehiculos();
    nueva_lista->cabeza = NULL;
    nueva_lista->cola = NULL;

    // Recorrer la lista original de vehículos
    struct nodo_vehiculo *actual = original->cabeza;
    while (actual != NULL)
    {
        // Crear un nuevo nodo y copiar los datos del nodo actual
        struct nodo_vehiculo *nuevo_nodo = asignar_memoria_nodo_vehiculo();
        nuevo_nodo->vehiculo = copiar_vehiculo(actual->vehiculo);
        nuevo_nodo->siguiente = NULL;

        // Si la lista está vacía, el nuevo nodo será la cabeza y la cola
        if (nueva_lista->cabeza == NULL)
        {
            nueva_lista->cabeza = nuevo_nodo;
            nueva_lista->cola = nuevo_nodo;
        }
        else
        {
            // Si la lista no está vacía, agregar el nuevo nodo al final
            nueva_lista->cola->siguiente = nuevo_nodo;
            nueva_lista->cola = nuevo_nodo;
        }

        actual = actual->siguiente;
    }

    return nueva_lista;
}

// Función para liberar la memoria de un vehículo
void liberar_vehiculo(struct vehiculo *vehiculo)
{
    // Liberar la memoria asociada a la ruta del vehículo
    liberar_ruta(vehiculo->ruta);
    // Liberar la memoria del vehículo
    free(vehiculo);
}

// Función para vaciar la memoria de la lista de vehículos
void vaciar_lista_vehiculos(struct lista_vehiculos *flota)
{
    struct nodo_vehiculo *vehiculo_actual = flota->cabeza;
    while (vehiculo_actual)
    {
        // Guardar el siguiente nodo para evitar perderlo
        struct nodo_vehiculo *vehiculo_temp = vehiculo_actual;
        vehiculo_actual = vehiculo_actual->siguiente;

        // Liberar la memoria del vehículo asociado a este nodo
        liberar_vehiculo(vehiculo_temp->vehiculo);

        // Liberar la memoria del nodo actual
        free(vehiculo_temp);
    }

    // Establecer la cabeza y cola de la flota a NULL
    flota->cabeza = NULL;
    flota->cola = NULL;
}

// Función para liberar la memoria de la lista de vehículos
void liberar_lista_vehiculos(struct lista_vehiculos *flota)
{
    struct nodo_vehiculo *vehiculo_actual = flota->cabeza;
    while (vehiculo_actual)
    {
        // Guardar el siguiente nodo para evitar perderlo
        struct nodo_vehiculo *vehiculo_temp = vehiculo_actual;
        vehiculo_actual = vehiculo_actual->siguiente;

        // Liberar la memoria del vehículo asociado a este nodo
        liberar_vehiculo(vehiculo_temp->vehiculo);

        // Liberar la memoria del nodo actual
        free(vehiculo_temp);
    }

    free(flota);
}
