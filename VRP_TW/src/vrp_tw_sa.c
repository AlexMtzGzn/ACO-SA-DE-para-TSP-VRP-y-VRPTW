#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "../include/vrp_tw_sa.h"
#include "../include/estructuras.h"
#include "../include/lista_flota.h"
#include "../include/control_memoria.h"

// Evalúa la función objetivo para la solución vecina
void evaluaFO_SA(struct individuo *ind, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    ind->metal->fitness_solucion_vecina = 0.0;

    struct lista_ruta *ruta;
    nodo_ruta *nodo_actual;
    double fitness_vehiculo;
    int cliente_actual, cliente_siguiente;
    struct nodo_vehiculo *vehiculo_actual = ind->metal->solucion_vecina->cabeza;

    // Recorre todos los vehículos de la solución vecina
    while (vehiculo_actual != NULL)
    {
        fitness_vehiculo = 0.0;
        ruta = vehiculo_actual->vehiculo->ruta;
        nodo_actual = ruta->cabeza;

        // Suma la distancia entre pares consecutivos de clientes
        while (nodo_actual->siguiente != NULL)
        {
            cliente_actual = nodo_actual->cliente;
            cliente_siguiente = nodo_actual->siguiente->cliente;

            fitness_vehiculo += instancia_distancias[cliente_actual][cliente_siguiente];
            nodo_actual = nodo_actual->siguiente;
        }

        vehiculo_actual->vehiculo->fitness_vehiculo = fitness_vehiculo;
        ind->metal->fitness_solucion_vecina += fitness_vehiculo;
        vehiculo_actual = vehiculo_actual->siguiente;
    }
}

// Mueve un cliente aleatorio de un vehículo a otro (si es válido)
bool moverClienteVehiculo(struct individuo *ind, struct vrp_configuracion *vrp)
{
    nodo_vehiculo *vehiculo_origen = NULL;
    nodo_vehiculo *vehiculo_destino = NULL;
    int intentos = 10, id_vehiculo_origen, id_vehiculo_destino, pos_cliente, id_cliente;
    double demanda;
    nodo_ruta *actual = NULL, *prev = NULL, *penultimo = NULL;

    while (intentos--)
    {
        // Selecciona dos vehículos distintos aleatoriamente
        id_vehiculo_origen = (rand() % ind->hormiga->vehiculos_necesarios) + 1;
        do
            id_vehiculo_destino = (rand() % ind->hormiga->vehiculos_necesarios) + 1;
        while (id_vehiculo_origen == id_vehiculo_destino);

        // Busca el vehículo origen
        vehiculo_origen = ind->metal->solucion_vecina->cabeza;
        while (vehiculo_origen->vehiculo->id_vehiculo != id_vehiculo_origen)
            vehiculo_origen = vehiculo_origen->siguiente;

        // Busca el vehículo destino
        vehiculo_destino = ind->metal->solucion_vecina->cabeza;
        while (vehiculo_destino->vehiculo->id_vehiculo != id_vehiculo_destino)
            vehiculo_destino = vehiculo_destino->siguiente;

        if (vehiculo_origen->vehiculo->clientes_contados == 0)
            continue;

        // Escoge un cliente aleatorio del vehículo origen (no debe ser el depósito)
        pos_cliente = rand() % vehiculo_origen->vehiculo->clientes_contados;
        prev = vehiculo_origen->vehiculo->ruta->cabeza;
        actual = prev->siguiente;

        for (int i = 0; i < pos_cliente; i++)
        {
            prev = actual;
            actual = actual->siguiente;
        }

        if (actual->cliente == 0) // evita mover el depósito
            continue;

        id_cliente = actual->cliente;

        if (id_cliente < 0 || id_cliente >= vrp->num_clientes)
            continue;

        demanda = vrp->clientes[id_cliente].demanda_capacidad;

        // Verifica capacidad del vehículo destino
        if ((vehiculo_destino->vehiculo->capacidad_acumulada + demanda) > vehiculo_destino->vehiculo->capacidad_maxima)
            continue;

        // Elimina el cliente de la ruta del vehículo origen
        if (prev)
            prev->siguiente = actual->siguiente;
        else
            vehiculo_origen->vehiculo->ruta->cabeza = actual->siguiente;

        if (actual == vehiculo_origen->vehiculo->ruta->cola)
            vehiculo_origen->vehiculo->ruta->cola = prev;

        vehiculo_origen->vehiculo->capacidad_acumulada -= demanda;
        vehiculo_origen->vehiculo->clientes_contados--;

        // Inserta el cliente antes del depósito del vehículo destino
        penultimo = vehiculo_destino->vehiculo->ruta->cabeza;
        while (penultimo->siguiente != vehiculo_destino->vehiculo->ruta->cola)
            penultimo = penultimo->siguiente;

        actual->siguiente = vehiculo_destino->vehiculo->ruta->cola;
        penultimo->siguiente = actual;

        vehiculo_destino->vehiculo->capacidad_acumulada += demanda;
        vehiculo_destino->vehiculo->clientes_contados++;

        return true;
    }

    return false;
}

// Intercambia dos clientes aleatorios dentro de un mismo vehículo
bool intercambiarClientes(struct individuo *ind, struct vrp_configuracion *vrp)
{
    int vehiculo_aleatorio, intentos = 10, cliente1_idx, cliente2_idx, temp;
    nodo_vehiculo *vehiculo_actual = NULL;
    nodo_ruta *nodo1 = NULL, *nodo2 = NULL;

    while (intentos--)
    {
        vehiculo_aleatorio = (rand() % ind->hormiga->vehiculos_necesarios) + 1;

        vehiculo_actual = ind->metal->solucion_vecina->cabeza;
        while (vehiculo_actual != NULL)
        {
            if (vehiculo_actual->vehiculo->id_vehiculo == vehiculo_aleatorio)
                break;
            vehiculo_actual = vehiculo_actual->siguiente;
        }

        if (vehiculo_actual->vehiculo->clientes_contados < 2)
            continue;

        break;
    }

    if (vehiculo_actual->vehiculo->clientes_contados < 2)
        return false;

    // Selecciona dos índices distintos para intercambiar
    do
    {
        cliente1_idx = rand() % vehiculo_actual->vehiculo->clientes_contados;
        cliente2_idx = rand() % vehiculo_actual->vehiculo->clientes_contados;
    } while (cliente1_idx == cliente2_idx);

    nodo1 = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
    nodo2 = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;

    if (nodo1 == NULL || nodo2 == NULL)
        return false;
    

    for (int i = 0; i < cliente1_idx; i++) nodo1 = nodo1->siguiente;
    for (int i = 0; i < cliente2_idx; i++) nodo2 = nodo2->siguiente;

    // Intercambia los clientes
    temp = nodo1->cliente;
    nodo1->cliente = nodo2->cliente;
    nodo2->cliente = temp;

    return true;
}

// Copia la solución actual como base para generar un vecino
void generar_vecino(struct individuo *ind, struct vrp_configuracion *vrp)
{
    if (ind->metal->solucion_vecina)
        liberar_lista_vehiculos(ind->metal->solucion_vecina);

    ind->metal->solucion_vecina = copiar_lista_vehiculos(ind->metal->solucion_actual);
}

// Algoritmo de Recocido Simulado (SA)
// Algoritmo de Recocido Simulado (SA)
void sa(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_distancias)
{
    double temperatura = ind->temperatura_inicial;
    double delta;
    bool aceptado = false;

    // Inicialización de soluciones
    ind->metal->mejor_solucion = copiar_lista_vehiculos(ind->metal->solucion_inicial);
    ind->metal->fitness_mejor_solucion = ind->metal->fitness_solucion_inicial;
    ind->metal->solucion_actual = copiar_lista_vehiculos(ind->metal->solucion_inicial);
    ind->metal->fitness_solucion_actual = ind->metal->fitness_solucion_inicial;

    // Ciclo de enfriamiento
    while (temperatura > ind->temperatura_final)
    {
        // >>> Imprime la temperatura y los valores de fitness en cada paso del enfriamiento
        printf("Temp: %.4f | Fitness actual: %.4f | Mejor: %.4f\n",
               temperatura,
               ind->metal->fitness_solucion_actual,
               ind->metal->fitness_mejor_solucion);
        // <<<

        for (int i = 0; i < ind->numIteracionesSA; i++)
        {
            generar_vecino(ind, vrp);

            // Se decide qué movimiento hacer según una probabilidad controlada por temperatura
            if (ind->hormiga->vehiculos_necesarios > 1)
            {
                if ((double)rand() / RAND_MAX < ind->factor_control * (1.0 - (temperatura / ind->temperatura_inicial)))
                    aceptado = intercambiarClientes(ind, vrp);
                else
                    aceptado = moverClienteVehiculo(ind, vrp);
            }
            else
            {
                aceptado = intercambiarClientes(ind, vrp);
            }

            if (!aceptado)
                continue;

            evaluaFO_SA(ind, vrp, instancia_distancias);
            delta = ind->metal->fitness_solucion_vecina - ind->metal->fitness_solucion_actual;

            bool aceptar = false;
            if (delta < 0)
                aceptar = true;
            else if ((double)rand() / RAND_MAX < exp(-delta / temperatura))
                aceptar = true;

            if (aceptar)
            {
                liberar_lista_vehiculos(ind->metal->solucion_actual);
                ind->metal->solucion_actual = copiar_lista_vehiculos(ind->metal->solucion_vecina);
                ind->metal->fitness_solucion_actual = ind->metal->fitness_solucion_vecina;

                if (ind->metal->fitness_solucion_actual < ind->metal->fitness_mejor_solucion)
                {
                    liberar_lista_vehiculos(ind->metal->mejor_solucion);
                    ind->metal->mejor_solucion = copiar_lista_vehiculos(ind->metal->solucion_actual);
                    ind->metal->fitness_mejor_solucion = ind->metal->fitness_solucion_actual;
                }
            }
        }

        temperatura *= ind->factor_enfriamiento; // Disminuye temperatura
    }

    liberar_lista_vehiculos(ind->metal->solucion_actual);
}


// Inicializa la estructura metal de un individuo
void inicializar_metal(struct individuo *ind)
{
    ind->metal = asignar_memoria_metal();
    ind->metal->solucion_vecina = NULL;
    ind->metal->solucion_actual = NULL;
    ind->metal->mejor_solucion = NULL;
    ind->metal->fitness_solucion_actual = 0.0;
    ind->metal->fitness_solucion_vecina = 0.0;
    ind->metal->fitness_mejor_solucion = 0.0;
    ind->metal->solucion_inicial = copiar_lista_vehiculos(ind->hormiga->flota);
    ind->metal->fitness_solucion_inicial = ind->hormiga->fitness_global;
}

// Función principal que ejecuta SA sobre un individuo del VRP
void vrp_tw_sa(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_distancias)
{

    inicializar_metal(ind);
    sa(vrp, ind, instancia_distancias);

    // Si se mejora la solución global, se guarda
    if (ind->metal->fitness_mejor_solucion < ind->hormiga->fitness_global)
    {
        ind->fitness = ind->metal->fitness_mejor_solucion;
        ind->hormiga->fitness_global = ind->metal->fitness_mejor_solucion;

        if (ind->hormiga->flota)
            liberar_lista_vehiculos(ind->hormiga->flota);

        ind->hormiga->flota = copiar_lista_vehiculos(ind->metal->mejor_solucion);
    }

    liberar_memoria_metal(ind); // Limpieza final
}
