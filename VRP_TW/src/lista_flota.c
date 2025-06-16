#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
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
    vehiculo_nuevo->vehiculo->vt_actual = 0.0;
    vehiculo_nuevo->vehiculo->vt_final = vrp->clientes[0].vt_final;
    vehiculo_nuevo->vehiculo->vt_inicial = vrp->clientes[0].vt_inicial;
    vehiculo_nuevo->vehiculo->clientes_contados = 0;
    vehiculo_nuevo->vehiculo->fitness_vehiculo = 0.0;
    vehiculo_nuevo->vehiculo->velocidad = 1.0;

    // Asignar memoria para la lista de rutas y añadir el primer cliente
    vehiculo_nuevo->vehiculo->ruta = asignar_memoria_lista_ruta();
    vehiculo_nuevo->vehiculo->datos_cliente = NULL;
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
        imprimir_mensaje("No se puedo asignar memoria al nodo del vehiculo.");
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
    nuevo_vehiculo->tiempo_llegada_vehiculo = original->tiempo_llegada_vehiculo;
    nuevo_vehiculo->tiempo_salida_vehiculo = original->tiempo_salida_vehiculo;
    nuevo_vehiculo->vt_actual = original->vt_actual;
    nuevo_vehiculo->vt_final = original->vt_final;
    nuevo_vehiculo->vt_inicial = original->vt_inicial;
    nuevo_vehiculo->velocidad = original->velocidad;
    nuevo_vehiculo->clientes_contados = original->clientes_contados;
    nuevo_vehiculo->fitness_vehiculo = original->fitness_vehiculo;

    // Copiar la ruta asociada al vehículo original
    nuevo_vehiculo->ruta = copiar_ruta(original);
    nuevo_vehiculo->datos_cliente = NULL;

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
    if (vehiculo->ruta)
        liberar_ruta(vehiculo->ruta);
    // Liberar memoria asociada a los datos del cliente
    if (vehiculo->datos_cliente)
        liberar_memoria_datos_cliente(vehiculo->datos_cliente);
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
        if (vehiculo_temp->vehiculo)
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
        // Guardar el siguiente nodo para evitar perdero
        struct nodo_vehiculo *vehiculo_temp = vehiculo_actual;
        vehiculo_actual = vehiculo_actual->siguiente;

        // Liberar la memoria del vehículo asociado a este nodo
        if (vehiculo_temp->vehiculo)
            liberar_vehiculo(vehiculo_temp->vehiculo);

        // // Liberar la memoria del nodo actual
        free(vehiculo_temp);
    }
    free(flota); // Liberar la memoria de la lista de vehículos
}

// Función que elimina de una lista de vehículos aquellos que no tienen clientes asignados (vacíos y retorna el total de vehículos restantes).
int eliminar_vehiculos_vacios(struct lista_vehiculos *lista)
{
    struct nodo_vehiculo *anterior = NULL;  // Puntero al nodo anterior para manejar la eliminación
    struct nodo_vehiculo *actual = lista->cabeza;  // Puntero al nodo actual que se va a revisar

    // Recorre toda la lista enlazada de vehículos
    while (actual != NULL)
    {
        // Si el vehículo actual no tiene clientes asignados
        if (actual->vehiculo->clientes_contados == 0)
        {
            // Guardamos el nodo a eliminar
            struct nodo_vehiculo *a_eliminar = actual;

            // Si el nodo a eliminar es el primero de la lista (sin nodo anterior)
            if (anterior == NULL)
            {
                // Ajustamos la cabeza de la lista al siguiente nodo
                lista->cabeza = actual->siguiente;
                actual = lista->cabeza;  // Continuamos desde la nueva cabeza
            }
            else
            {
                // Si no es el primero, enlazamos el nodo anterior con el siguiente del actual
                anterior->siguiente = actual->siguiente;
                actual = anterior->siguiente;  // Continuamos desde el siguiente del anterior
            }

            // Liberamos la memoria del vehículo y del nodo eliminado
            liberar_vehiculo(a_eliminar->vehiculo);
            free(a_eliminar);
        }
        else
        {
            // Si el vehículo sí tiene clientes, avanzamos al siguiente nodo
            anterior = actual;
            actual = actual->siguiente;
        }
    }

    // Después de eliminar los vacíos, reajustamos los IDs de vehículos empezando en 1
    int nuevo_id = 1;
    int total_vehiculos = 0;
    actual = lista->cabeza;

    // Recorremos nuevamente la lista para asignar IDs consecutivos y contar vehículos
    while (actual != NULL)
    {
        actual->vehiculo->id_vehiculo = nuevo_id++;  // Asignar nuevo ID
        total_vehiculos++;                           // Incrementar conteo
        actual = actual->siguiente;
    }

    return total_vehiculos;  // Retornamos la cantidad final de vehículos en la lista
}


// Función para seleccionar aleatoriamente un vehículo dentro de la flota de una hormiga y retorna el nodo del vehículo seleccionado o NULL si no se encuentra.
struct nodo_vehiculo *seleccionar_vehiculo_aleatorio(struct hormiga *hormiga)
{
    int intentos_maximos = 10;      // Número máximo de intentos para seleccionar un vehículo válido
    int vehiculo_aleatorio;         // ID aleatorio de vehículo seleccionado
    struct nodo_vehiculo *nodo_vehiculo_aleatorio = NULL;  // Puntero al nodo seleccionado

    // Intentamos hasta intentos_maximos veces
    for(int intento = 0; intento < intentos_maximos; intento++)
    {
        // Generamos un número aleatorio entre 1 y la cantidad de vehículos necesarios
        vehiculo_aleatorio = (rand() % hormiga->vehiculos_necesarios) + 1;

        // Recorremos la lista de vehículos para encontrar el nodo con ese ID
        nodo_vehiculo_aleatorio = hormiga->flota->cabeza;
        while (nodo_vehiculo_aleatorio != NULL)
        {
            // Si encontramos el vehículo con el ID aleatorio generado, lo retornamos
            if (nodo_vehiculo_aleatorio->vehiculo->id_vehiculo == vehiculo_aleatorio)
                return nodo_vehiculo_aleatorio; 

            nodo_vehiculo_aleatorio = nodo_vehiculo_aleatorio->siguiente;
        }
    }

    // Si no se encontró un vehículo válido en los intentos, retornamos NULL
    return NULL;
}
