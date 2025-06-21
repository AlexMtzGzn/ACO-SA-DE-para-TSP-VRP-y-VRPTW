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
    struct lista_ruta *ruta_nueva = asignar_memoria_lista_ruta();
    ruta_nueva->cabeza = NULL;
    ruta_nueva->cola = NULL;

    // Se itera sobre la ruta original para copiar los nodos
    struct nodo_ruta *actual = vehiculo_original->ruta->cabeza;
    while (actual != NULL)
    {
        // Se asigna memoria para un nuevo nodo y se copian los datos
        struct nodo_ruta *nuevo_nodo = asignar_memoria_nodo_ruta();
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

bool verificar_restricciones(struct vehiculo *vehiculo, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    struct nodo_ruta *clienteActual = vehiculo->ruta->cabeza;
    struct nodo_ruta *clienteAnterior = NULL;

    double tiempo = 0.0;
    double capacidad = 0.0;
    double inicio = 0.0;
    double fin = 0.0;

    // Verificar los clientes de la ruta del vehículo
    while (clienteActual != NULL)
    {
        int id_actual = clienteActual->cliente;

        // Si es el primer cliente (después del depósito)
        if (clienteAnterior == NULL)
        {
            if (id_actual != 0)
            {
                tiempo += instancia_distancias[0][id_actual] / vehiculo->velocidad;

                // Asegurarse de no llegar antes del inicio de la ventana de tiempo
                if (tiempo < vrp->clientes[id_actual].vt_inicial)
                    tiempo = vrp->clientes[id_actual].vt_inicial;

                // Acumulando la demanda de capacidad
                capacidad += vrp->clientes[id_actual].demanda_capacidad;

                // Verificar violación de ventana de tiempo
                if (tiempo > vrp->clientes[id_actual].vt_final)
                {
                    // printf("Violación de ventana de tiempo: Cliente %d, llegada %.2f > ventana final %.2f\n",
                    // id_actual, tiempo, vrp->clientes[id_actual].vt_final);
                    return false; // Violación de ventana de tiempo, retorno falso
                }
            }
        }
        else
        {
            // Para los siguientes clientes, calcular tiempo de viaje y servicio
            if (clienteAnterior->cliente != 0)
                tiempo += vrp->clientes[clienteAnterior->cliente].tiempo_servicio;

            tiempo += instancia_distancias[clienteAnterior->cliente][id_actual] / vehiculo->velocidad;

            // Asegurarse de no llegar antes del inicio de la ventana de tiempo
            if (tiempo < vrp->clientes[id_actual].vt_inicial)
                tiempo = vrp->clientes[id_actual].vt_inicial;

            if (id_actual != 0)
            {
                capacidad += vrp->clientes[id_actual].demanda_capacidad;

                // Verificar violación de ventana de tiempo
                if (tiempo > vrp->clientes[id_actual].vt_final)
                {
                    // printf("Violación de ventana de tiempo: Cliente %d, llegada %.2f > ventana final %.2f\n",
                    //  id_actual, tiempo, vrp->clientes[id_actual].vt_final);
                    return false; // Violación de ventana de tiempo, retorno falso
                }
            }
        }

        clienteAnterior = clienteActual;
        clienteActual = clienteActual->siguiente;
    }

    // Después de la última entrega, volver al depósito
    if (clienteAnterior != NULL && clienteAnterior->cliente != 0)
        tiempo += vrp->clientes[clienteAnterior->cliente].tiempo_servicio;

    tiempo += instancia_distancias[clienteAnterior->cliente][0] / vehiculo->velocidad;
    fin = tiempo;

    // Asignamos el tiempo de salida y llegada del vehículo
    vehiculo->tiempo_salida_vehiculo = inicio;
    vehiculo->tiempo_llegada_vehiculo = fin;
    vehiculo->capacidad_acumulada = capacidad;

    // Verificación de capacidad total
    if (capacidad > vehiculo->capacidad_maxima)
    {
        // printf("Violación de capacidad en Vehículo ID %d: %.2f > %.2f\n",
        //  vehiculo->id_vehiculo, capacidad, vehiculo->capacidad_maxima);
        return false; // Violación de capacidad, retorno falso
    }

    return true; // Si no hubo violaciones, retorno verdadero
}

// Elimina un cliente específico de la ruta de un vehículo.
void eliminar_cliente_ruta(struct vehiculo *vehiculo, struct vrp_configuracion *vrp, int cliente, double **instancia_distancias)
{
    struct nodo_ruta *anterior = vehiculo->ruta->cabeza; // Nodo anterior comienza en la cabeza (depósito inicial)
    struct nodo_ruta *actual = anterior->siguiente;      // Nodo actual es el siguiente después de la cabeza

    // Recorremos la lista enlazada de nodos ruta buscando el cliente a eliminar
    while (actual != NULL)
    {
        if (actual->cliente == cliente) // Si encontramos el cliente buscado
        {
            anterior->siguiente = actual->siguiente; // Se salta el nodo actual en la lista
            liberar_memoria_nodo_ruta(actual);                            // Se libera la memoria del nodo eliminado
            vehiculo->clientes_contados--;           // Se decrementa el contador de clientes del vehículo
            return;                                  // Se termina la función ya que se eliminó el cliente
        }
        anterior = actual;          // Avanzamos el nodo anterior
        actual = actual->siguiente; // Avanzamos el nodo actual
    }
}

// Llena los datos temporales y capacidades de los clientes en cada vehículo, calculando tiempos de llegada, esperas, inicio y fin de servicio basado en las ventanas de tiempo y distancias.
void llenar_datos_clientes(struct lista_vehiculos *flota, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    struct nodo_vehiculo *nodoVehiculo = flota->cabeza; // Comenzamos con el primer vehículo en la flota

    // Iteramos sobre cada vehículo en la flota
    while (nodoVehiculo != NULL)
    {
        struct vehiculo *vehiculo = nodoVehiculo->vehiculo;
        struct nodo_ruta *clienteActual = vehiculo->ruta->cabeza; // Empezamos en el depósito inicial (cabeza)
        struct nodo_ruta *clienteAnterior = NULL;

        double tiempo = 0.0;    // Tiempo acumulado de ruta
        double capacidad = 0.0; // Capacidad acumulada de la carga del vehículo
        double inicio = 0.0;    // Tiempo de inicio del servicio (primer cliente)
        double fin = 0.0;       // Tiempo final al regresar al depósito

        // Si ya existía memoria para datos_cliente, la liberamos para evitar fugas
        if (vehiculo->datos_cliente != NULL)
            liberar_memoria_datos_cliente(vehiculo->datos_cliente);

        // Asignamos memoria para datos de clientes: clientes en ruta + 2 (depósito inicio y fin)
        vehiculo->datos_cliente = asignar_memoria_datos_clientes(vehiculo->clientes_contados + 2);

        int indice_cliente = 0;

        // ===== DEPÓSITO INICIAL =====
        vehiculo->datos_cliente[indice_cliente].cliente = 0;                               // ID del depósito inicial
        vehiculo->datos_cliente[indice_cliente].demanda_capacidad = 0;                     // No hay demanda en depósito
        vehiculo->datos_cliente[indice_cliente].ventana_inicial = 0;                       // Ventana inicial del depósito
        vehiculo->datos_cliente[indice_cliente].ventana_final = vrp->clientes[0].vt_final; // Ventana final del depósito
        vehiculo->datos_cliente[indice_cliente].tiempo_llegada = 0;
        vehiculo->datos_cliente[indice_cliente].tiempo_espera = 0;
        vehiculo->datos_cliente[indice_cliente].inicio_servicio = 0;
        vehiculo->datos_cliente[indice_cliente].duracion_servicio = 0;
        vehiculo->datos_cliente[indice_cliente].tiempo_salida = 0;
        indice_cliente++;

        // Recorremos cada cliente en la ruta (lista enlazada)
        while (clienteActual != NULL)
        {
            int id_actual = clienteActual->cliente; // ID del cliente actual en la ruta

            if (clienteAnterior == NULL) // Si es el primer cliente después del depósito
            {
                if (id_actual != 0) // Si no es el depósito
                {
                    // Calculamos tiempo de viaje desde depósito (0) hasta el cliente actual
                    double tiempo_viaje = instancia_distancias[0][id_actual] / vehiculo->velocidad;
                    tiempo += tiempo_viaje;

                    double tiempo_llegada = tiempo;
                    double tiempo_espera = 0.0;

                    // Si llegamos antes de la ventana de tiempo del cliente, esperamos
                    if (tiempo < vrp->clientes[id_actual].vt_inicial)
                    {
                        tiempo_espera = vrp->clientes[id_actual].vt_inicial - tiempo;
                        tiempo = vrp->clientes[id_actual].vt_inicial; // Avanzamos tiempo al inicio de ventana
                    }

                    if (indice_cliente == 1)
                        inicio = tiempo; // Guardamos tiempo de inicio servicio del primer cliente


                    capacidad += vrp->clientes[id_actual].demanda_capacidad; // Acumulamos demanda

                    // Guardamos los datos del cliente en el arreglo
                    vehiculo->datos_cliente[indice_cliente].cliente = id_actual;
                    vehiculo->datos_cliente[indice_cliente].demanda_capacidad = vrp->clientes[id_actual].demanda_capacidad;
                    vehiculo->datos_cliente[indice_cliente].ventana_inicial = vrp->clientes[id_actual].vt_inicial;
                    vehiculo->datos_cliente[indice_cliente].ventana_final = vrp->clientes[id_actual].vt_final;
                    vehiculo->datos_cliente[indice_cliente].tiempo_llegada = tiempo_llegada;
                    vehiculo->datos_cliente[indice_cliente].tiempo_espera = tiempo_espera;
                    vehiculo->datos_cliente[indice_cliente].inicio_servicio = tiempo;
                    vehiculo->datos_cliente[indice_cliente].duracion_servicio = vrp->clientes[id_actual].tiempo_servicio;
                    vehiculo->datos_cliente[indice_cliente].tiempo_salida = tiempo + vrp->clientes[id_actual].tiempo_servicio;

                    indice_cliente++;
                }
            }
            else
            {
                // Para clientes posteriores al primero
                if (clienteAnterior->cliente != 0) // Si cliente anterior no es depósito, sumar tiempo de servicio
                    tiempo += vrp->clientes[clienteAnterior->cliente].tiempo_servicio;

                // Sumamos tiempo de viaje entre cliente anterior y actual
                double tiempo_viaje = instancia_distancias[clienteAnterior->cliente][id_actual] / vehiculo->velocidad;
                tiempo += tiempo_viaje;

                if (id_actual != 0)
                {
                    double tiempo_llegada = tiempo;
                    double tiempo_espera = 0.0;

                    // Espera si llegamos antes del inicio de ventana del cliente
                    if (tiempo < vrp->clientes[id_actual].vt_inicial)
                    {
                        tiempo_espera = vrp->clientes[id_actual].vt_inicial - tiempo;
                        tiempo = vrp->clientes[id_actual].vt_inicial;
                    }

                    capacidad += vrp->clientes[id_actual].demanda_capacidad; // Acumulamos demanda

                    // Guardamos datos del cliente en la estructura
                    vehiculo->datos_cliente[indice_cliente].cliente = id_actual;
                    vehiculo->datos_cliente[indice_cliente].demanda_capacidad = vrp->clientes[id_actual].demanda_capacidad;
                    vehiculo->datos_cliente[indice_cliente].ventana_inicial = vrp->clientes[id_actual].vt_inicial;
                    vehiculo->datos_cliente[indice_cliente].ventana_final = vrp->clientes[id_actual].vt_final;
                    vehiculo->datos_cliente[indice_cliente].tiempo_llegada = tiempo_llegada;
                    vehiculo->datos_cliente[indice_cliente].tiempo_espera = tiempo_espera;
                    vehiculo->datos_cliente[indice_cliente].inicio_servicio = tiempo;
                    vehiculo->datos_cliente[indice_cliente].duracion_servicio = vrp->clientes[id_actual].tiempo_servicio;
                    vehiculo->datos_cliente[indice_cliente].tiempo_salida = tiempo + vrp->clientes[id_actual].tiempo_servicio;

                    indice_cliente++;
                }
            }

            // Si estamos en el último cliente o el siguiente es depósito, calculamos tiempo de regreso
            if (clienteActual->siguiente == NULL || clienteActual->siguiente->cliente == 0)
            {
                if (id_actual != 0)
                    tiempo += vrp->clientes[id_actual].tiempo_servicio;

                // Tiempo de viaje de regreso al depósito
                tiempo += instancia_distancias[id_actual][0] / vehiculo->velocidad;
                fin = tiempo; // Guardamos tiempo final

                // Avanzamos clienteActual si hay siguiente (depósito final)
                if (clienteActual->siguiente != NULL)
                    clienteActual = clienteActual->siguiente;
            }

            clienteAnterior = clienteActual;          // Actualizamos clienteAnterior para la siguiente iteración
            clienteActual = clienteActual->siguiente; // Avanzamos en la lista de ruta
        }

        // ===== DEPÓSITO FINAL =====
        vehiculo->datos_cliente[indice_cliente].cliente = 0;
        vehiculo->datos_cliente[indice_cliente].demanda_capacidad = 0;
        vehiculo->datos_cliente[indice_cliente].ventana_inicial = 0;
        vehiculo->datos_cliente[indice_cliente].ventana_final = vrp->clientes[0].vt_final;
        vehiculo->datos_cliente[indice_cliente].tiempo_llegada = fin;
        vehiculo->datos_cliente[indice_cliente].tiempo_espera = 0;
        vehiculo->datos_cliente[indice_cliente].inicio_servicio = fin;
        vehiculo->datos_cliente[indice_cliente].duracion_servicio = 0;
        vehiculo->datos_cliente[indice_cliente].tiempo_salida = fin;

        // Actualizamos propiedades acumuladas en vehículo
        vehiculo->capacidad_acumulada = capacidad;
        vehiculo->tiempo_salida_vehiculo = inicio;
        vehiculo->tiempo_llegada_vehiculo = fin;

        nodoVehiculo = nodoVehiculo->siguiente; // Pasamos al siguiente vehículo
    }
}

// Funcion que inserta un cliente en una posición específica dentro de la ruta de un vehículo.
bool inserta_cliente_en_posicion(struct vehiculo *vehiculo, struct vrp_configuracion *vrp, int cliente, int posicion, double **instancia_distancias)
{
    struct nodo_ruta *nuevo = asignar_memoria_nodo_ruta(); // Crear nuevo nodo para el cliente
    if (!nuevo)                                            // Si la asignación falla, retorna false
        return false;

    nuevo->cliente = cliente; // Asignar cliente al nuevo nodo
    nuevo->siguiente = NULL;  // Inicializar siguiente como NULL

    // Punteros para recorrer la lista
    struct nodo_ruta *actual = vehiculo->ruta->cabeza->siguiente; // Empieza después del depósito
    struct nodo_ruta *previo = vehiculo->ruta->cabeza;
    int pos_actual = 0;

    // Recorremos hasta encontrar la posición donde insertar
    while (actual != NULL)
    {
        if (pos_actual == posicion) // Si encontramos la posición deseada
        {
            nuevo->siguiente = actual;     // Insertamos el nodo nuevo antes del nodo actual
            previo->siguiente = nuevo;     // Enlazamos el nodo previo al nuevo nodo
            vehiculo->clientes_contados++; // Incrementamos contador de clientes
            return true;                   // Inserción exitosa
        }
        previo = actual; // Avanzamos punteros
        actual = actual->siguiente;
        pos_actual++;
    }

    // Si no se encontró la posición, intentamos insertar antes del depósito final (cliente 0)
    if (actual != NULL && actual->cliente == 0)
    {
        nuevo->siguiente = actual;
        previo->siguiente = nuevo;
        vehiculo->clientes_contados++;
        return true;
    }

    // Si no se pudo insertar, liberamos memoria y retornamos false
    liberar_memoria_nodo_ruta(nuevo);
    return false;
}
