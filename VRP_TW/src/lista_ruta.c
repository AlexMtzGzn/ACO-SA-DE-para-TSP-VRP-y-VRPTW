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

    free(ruta); // Se libera la estructura de la lista de ruta
}

// // Función para verificar restricciones de un vehículo individual
// bool verificarRestricciones(struct vehiculo *vehiculo, struct vrp_configuracion *vrp, double **instancia_distancias) {
//     bool restricciones_ok = true;
    
//     struct lista_ruta *ruta = vehiculo->ruta;
//     struct nodo_ruta *clienteActual = ruta->cabeza;
//     struct nodo_ruta *clienteAnterior = NULL;

//     double tiempo = 0.0;         // Tiempo acumulado real (con servicio, espera, etc.)
//     double tiempo_llegada = 0.0; // Tiempo exacto de llegada antes de esperas
//     double capacidad = 0.0;      // Capacidad acumulada

//     // Si la ruta está vacía o solo tiene el depósito, no hay restricciones que violar
//     if (clienteActual == NULL || (clienteActual->cliente == 0 && clienteActual->siguiente == NULL)) {
       
//         return true;
//     }

//     // Recorremos la ruta cliente por cliente
//     while (clienteActual != NULL) {
//         int id = clienteActual->cliente;

//         // Acumular demanda si no es depósito (el depósito tiene demanda 0)
//         if (id != 0) {
//             capacidad += vrp->clientes[id].demanda_capacidad;
            
//             // Verificar si se excede la capacidad máxima
//             if (capacidad > vehiculo->capacidad_maxima) {
//                 restricciones_ok = false;
               
//                 break;
//             }
//         }

//         // Calcular tiempo de llegada
//         if (clienteAnterior != NULL) {
//             int prev_id = clienteAnterior->cliente;

//             // Solo sumamos tiempo de servicio si no es depósito
//             if (prev_id != 0) {
//                 tiempo += vrp->clientes[prev_id].tiempo_servicio;
//             }

//             // Tiempo de viaje desde cliente anterior al actual
//             tiempo += instancia_distancias[prev_id][id] / vehiculo->velocidad;
//         } else if (id != 0) {
//             // Primer movimiento desde depósito si el primer nodo no es el depósito
//             tiempo += instancia_distancias[0][id] / vehiculo->velocidad;
//         }
        
//         tiempo_llegada = tiempo; // Guardamos el tiempo de llegada exacto

//         // Verificar ventana temporal (solo para clientes, no para depósito)
//         if (id != 0) {
//             // Si llegamos demasiado tarde
//             if (tiempo_llegada > vrp->clientes[id].vt_final) {
//                 restricciones_ok = false;
                
//                 break;
//             }
            
//             // Si llegamos antes, actualizamos el tiempo para considerar la espera
//             if (tiempo < vrp->clientes[id].vt_inicial) {
//                 tiempo = vrp->clientes[id].vt_inicial;
//             }
//         }

//         clienteAnterior = clienteActual;
//         clienteActual = clienteActual->siguiente;
//     }

//     // Si todas las restricciones se cumplieron hasta ahora, verificar el retorno al depósito
//     if (restricciones_ok && clienteAnterior != NULL && clienteAnterior->cliente != 0) {
//         int last_id = clienteAnterior->cliente;
        
//         // Tiempo de servicio del último cliente
//         tiempo += vrp->clientes[last_id].tiempo_servicio;
        
//         // Tiempo de viaje de regreso al depósito
//         tiempo += instancia_distancias[last_id][0] / vehiculo->velocidad;

//         // Verificar si se llega tarde al depósito
//         if (tiempo > vrp->clientes[0].vt_final) {
//             restricciones_ok = false;
           
//         }
//     }

//     // Actualizar datos del vehículo si cumple restricciones
//     if (restricciones_ok) {
//         vehiculo->capacidad_acumulada = capacidad;
//         vehiculo->tiempo_llegada_vehiculo = tiempo;
        
//     }

//     return restricciones_ok;
// }

// bool verificarRestriccionesFlota(struct lista_vehiculos *flota, struct vrp_configuracion *vrp, double **instancia_distancias) {
//     bool flota_cumple_restricciones = true;
//     int vehiculos_con_violaciones = 0;
//     int total_vehiculos = 0;
    

    
//     struct nodo_vehiculo *nodoVehiculo = flota->cabeza;
    
//     // Recorremos todos los vehículos de la flota
//     while (nodoVehiculo != NULL) {
//         struct vehiculo *vehiculo = nodoVehiculo->vehiculo;
//         total_vehiculos++;
        
       
        
//         bool vehiculo_ok = verificarRestricciones(vehiculo, vrp, instancia_distancias);
        
//         if (!vehiculo_ok) {
//             flota_cumple_restricciones = false;
//             vehiculos_con_violaciones++;
//         }
        
//         nodoVehiculo = nodoVehiculo->siguiente;
//     }
    
//     // Resumen final
    

//     return flota_cumple_restricciones;
// }


bool verificarRestricciones(struct vehiculo *vehiculo, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    bool restricciones_ok = true;

    struct lista_ruta *ruta = vehiculo->ruta;
    struct nodo_ruta *clienteActual = ruta->cabeza;
    struct nodo_ruta *clienteAnterior = NULL;

    double tiempo = 0.0;         // Tiempo acumulado real (con servicio, espera, etc.)
    double tiempo_llegada = 0.0; // Tiempo exacto de llegada antes de esperas
    double capacidad = 0.0;      // Capacidad acumulada

    // Si la ruta está vacía o solo tiene el depósito, no hay restricciones que violar
    if (clienteActual == NULL || (clienteActual->cliente == 0 && clienteActual->siguiente == NULL))
    {

        return true;
    }

    // Recorremos la ruta cliente por cliente
    while (clienteActual != NULL)
    {
        int id = clienteActual->cliente;

        // Acumular demanda si no es depósito (el depósito tiene demanda 0)
        if (id != 0)
        {
            capacidad += vrp->clientes[id].demanda_capacidad;

            // Verificar si se excede la capacidad máxima
            if (capacidad > vehiculo->capacidad_maxima)
            {
                restricciones_ok = false;

                break;
            }
        }

        // Calcular tiempo de llegada
        if (clienteAnterior != NULL)
        {
            int prev_id = clienteAnterior->cliente;

            // Solo sumamos tiempo de servicio si no es depósito
            if (prev_id != 0)
            {
                tiempo += vrp->clientes[prev_id].tiempo_servicio;
            }

            // Tiempo de viaje desde cliente anterior al actual
            tiempo += instancia_distancias[prev_id][id] / vehiculo->velocidad;
        }
        else if (id != 0)
        {
            // Primer movimiento desde depósito si el primer nodo no es el depósito
            tiempo += instancia_distancias[0][id] / vehiculo->velocidad;
        }

        tiempo_llegada = tiempo; // Guardamos el tiempo de llegada exacto

        // Verificar ventana temporal (solo para clientes, no para depósito)
        if (id != 0)
        {
            // Si llegamos demasiado tarde
            if (tiempo_llegada > vrp->clientes[id].vt_final)
            {
                restricciones_ok = false;

                break;
            }

            // Si llegamos antes, actualizamos el tiempo para considerar la espera
            if (tiempo < vrp->clientes[id].vt_inicial)
            {
                tiempo = vrp->clientes[id].vt_inicial;
            }
        }

        clienteAnterior = clienteActual;
        clienteActual = clienteActual->siguiente;
    }

    // Si todas las restricciones se cumplieron hasta ahora, verificar el retorno al depósito
    if (restricciones_ok && clienteAnterior != NULL && clienteAnterior->cliente != 0)
    {
        int last_id = clienteAnterior->cliente;

        // Tiempo de servicio del último cliente
        tiempo += vrp->clientes[last_id].tiempo_servicio;

        // Tiempo de viaje de regreso al depósito
        tiempo += instancia_distancias[last_id][0] / vehiculo->velocidad;

        // Verificar si se llega tarde al depósito
        if (tiempo > vrp->clientes[0].vt_final)
        {
            restricciones_ok = false;
        }
    }

    return restricciones_ok;
}

void eliminar_cliente_ruta(struct vehiculo *vehiculo, struct vrp_configuracion *vrp, int cliente, double **instancia_distancias)
{
    struct nodo_ruta *anterior = vehiculo->ruta->cabeza;
    struct nodo_ruta *actual = anterior->siguiente;

    while (actual != NULL)
    {
        if (actual->cliente == cliente)
        {
            anterior->siguiente = actual->siguiente;
            free(actual);
            vehiculo->clientes_contados--;
            break;
        }
        anterior = actual;
        actual = actual->siguiente;
    }
}

bool insertarClienteEnPosicion(struct vehiculo *vehiculo, struct vrp_configuracion *vrp, int cliente, int posicion, double **instancia_distancias)
{
    struct nodo_ruta *nuevo = asignar_memoria_nodo_ruta();
    if (!nuevo)
        return false;

    nuevo->cliente = cliente;
    nuevo->siguiente = NULL;

    // Nos colocamos en la cabeza de la ruta (depósito)
    struct nodo_ruta *actual = vehiculo->ruta->cabeza;
    int pos_actual = -1; // Comenzamos en -1 porque la cabeza es el depósito (cliente 0)

    // Si la posición deseada es 0 (insertar justo después del depósito)
    if (posicion == 0)
    {
        // Insertamos justo después del depósito
        nuevo->siguiente = actual->siguiente;
        actual->siguiente = nuevo;
        vehiculo->clientes_contados++;
        return true;
    }

    // Recorremos la lista hasta encontrar la posición deseada, excluyendo los depósitos (cliente 0)
    while (actual->siguiente != NULL)
    {
        // Solo contamos clientes que no sean depósitos (cliente 0)
        if (actual->siguiente->cliente != 0)
        {
            pos_actual++;
            // Si hemos llegado a la posición deseada, insertamos el cliente
            if (pos_actual == posicion)
            {
                break;
            }
        }
        actual = actual->siguiente;
    }

    // Si llegamos a la posición correcta, insertamos el cliente
    if (pos_actual == posicion)
    {
        nuevo->siguiente = actual->siguiente;
        actual->siguiente = nuevo;
        vehiculo->clientes_contados++;
        return true;
    }

    // Si la posición deseada es más allá de los clientes contados, insertar antes del depósito
    if (actual->siguiente != NULL && actual->siguiente->cliente == 0)
    {
        // Insertamos antes del depósito
        nuevo->siguiente = actual->siguiente;
        actual->siguiente = nuevo;
        vehiculo->clientes_contados++;
        return true;
    }
    return false; // Si no se encontró la posición deseada
}
