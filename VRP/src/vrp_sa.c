#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "../include/vrp_sa.h"
#include "../include/estructuras.h"
#include "../include/lista_flota.h"
#include "../include/lista_ruta.h"
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
bool mover_cliente_vehiculo(struct individuo *ind, struct vrp_configuracion *vrp)
{
    nodo_vehiculo *origen = NULL;
    nodo_vehiculo *destino = NULL;
    int intentos = 10;

    while (intentos--) {
        int id_origen = (rand() % ind->hormiga->vehiculos_necesarios) + 1;
        int id_destino = (rand() % ind->hormiga->vehiculos_necesarios) + 1;

        // Buscar vehículos por ID
        origen = ind->metal->solucion_vecina->cabeza;
        while (origen && origen->vehiculo->id_vehiculo != id_origen)
            origen = origen->siguiente;

        destino = ind->metal->solucion_vecina->cabeza;
        while (destino && destino->vehiculo->id_vehiculo != id_destino)
            destino = destino->siguiente;

        if (!origen || !destino || origen->vehiculo->clientes_contados <= 2)
            continue;

        int pos_cliente = rand() % (origen->vehiculo->clientes_contados - 2) + 1;

        nodo_ruta *anterior = origen->vehiculo->ruta->cabeza;
        nodo_ruta *actual = anterior->siguiente;
        for (int i = 1; i < pos_cliente && actual && actual->siguiente; i++) {
            anterior = actual;
            actual = actual->siguiente;
        }

        if (!actual || actual->cliente <= 0 || actual->cliente >= vrp->num_clientes)
            continue;

        double demanda = vrp->clientes[actual->cliente].demanda_capacidad;

        if (destino->vehiculo->capacidad_acumulada + demanda > destino->vehiculo->capacidad_maxima)
            continue;

        // Eliminar cliente de vehículo origen
        anterior->siguiente = actual->siguiente;
        if (actual == origen->vehiculo->ruta->cola)
            origen->vehiculo->ruta->cola = anterior;
        origen->vehiculo->capacidad_acumulada -= demanda;
        origen->vehiculo->clientes_contados--;

        // Insertar cliente antes del depósito final en destino
        nodo_ruta *penultimo = destino->vehiculo->ruta->cabeza;
        while (penultimo->siguiente != destino->vehiculo->ruta->cola)
            penultimo = penultimo->siguiente;

        actual->siguiente = destino->vehiculo->ruta->cola;
        penultimo->siguiente = actual;
        destino->vehiculo->capacidad_acumulada += demanda;
        destino->vehiculo->clientes_contados++;

        return true;
    }

    return false;
}

bool invertir_segmento_ruta(struct individuo *ind) {
    nodo_vehiculo *vehiculo = NULL;
    int intentos = 10;

    for (int i = 0; i < intentos; i++) {
        vehiculo = seleccionar_vehiculo_aleatorio(ind);
        if (vehiculo && vehiculo->vehiculo->clientes_contados >= 3)
            break;
    }

    if (!vehiculo || vehiculo->vehiculo->clientes_contados < 3)
        return false;

    for (int i = 0; i < intentos; i++) {
        int total = vehiculo->vehiculo->clientes_contados;
        int inicio = rand() % total;
        int fin = rand() % total;

        while (inicio == fin)
            fin = rand() % total;

        if (inicio > fin) {
            int tmp = inicio;
            inicio = fin;
            fin = tmp;
        }

        int longitud = fin - inicio + 1;
        int *segmento = asignar_memoria_arreglo_int(longitud);
        if (!segmento) return false;

        nodo_ruta *nodo = vehiculo->vehiculo->ruta->cabeza->siguiente;
        for (int j = 0; j < inicio; j++)
            nodo = nodo->siguiente;

        nodo_ruta *iter = nodo;
        for (int j = 0; j < longitud; j++) {
            segmento[j] = iter->cliente;
            iter = iter->siguiente;
        }

        for (int j = 0; j < longitud / 2; j++) {
            int temp = segmento[j];
            segmento[j] = segmento[longitud - 1 - j];
            segmento[longitud - 1 - j] = temp;
        }

        nodo = vehiculo->vehiculo->ruta->cabeza->siguiente;
        for (int j = 0; j < inicio; j++)
            nodo = nodo->siguiente;

        for (int j = 0; j < longitud; j++) {
            nodo->cliente = segmento[j];
            nodo = nodo->siguiente;
        }

        free(segmento);
        return true;
    }

    return false;
}


bool mover_dos_clientes_vehiculos(struct individuo *ind, struct vrp_configuracion *vrp, double **instancia_distancias) {
    nodo_vehiculo *veh1 = NULL, *veh2 = NULL;
    nodo_ruta *nodo1 = NULL, *nodo2 = NULL;
    int intentos = 10;

    while (intentos--) {
        veh1 = seleccionar_vehiculo_aleatorio(ind);
        veh2 = seleccionar_vehiculo_aleatorio(ind);

        if (!veh1 || !veh2 || veh1 == veh2)
            continue;

        if (veh1->vehiculo->clientes_contados < 3 || veh2->vehiculo->clientes_contados < 3)
            continue;

        int pos1 = rand() % (veh1->vehiculo->clientes_contados - 2) + 1;
        int pos2 = rand() % (veh2->vehiculo->clientes_contados - 2) + 1;

        nodo1 = veh1->vehiculo->ruta->cabeza->siguiente;
        for (int i = 1; i < pos1 && nodo1; i++)
            nodo1 = nodo1->siguiente;

        nodo2 = veh2->vehiculo->ruta->cabeza->siguiente;
        for (int i = 1; i < pos2 && nodo2; i++)
            nodo2 = nodo2->siguiente;

        if (!nodo1 || !nodo2 || nodo1->cliente == 0 || nodo2->cliente == 0)
            continue;

        int id1 = nodo1->cliente;
        int id2 = nodo2->cliente;

        if (id1 < 0 || id2 < 0 || id1 >= vrp->num_clientes || id2 >= vrp->num_clientes)
            continue;

        double d1 = vrp->clientes[id1].demanda_capacidad;
        double d2 = vrp->clientes[id2].demanda_capacidad;

        double nueva_cap1 = veh1->vehiculo->capacidad_acumulada - d1 + d2;
        double nueva_cap2 = veh2->vehiculo->capacidad_acumulada - d2 + d1;

        if (nueva_cap1 > veh1->vehiculo->capacidad_maxima || nueva_cap2 > veh2->vehiculo->capacidad_maxima)
            continue;

        // Intercambio válido
        nodo1->cliente = id2;
        nodo2->cliente = id1;
        veh1->vehiculo->capacidad_acumulada = nueva_cap1;
        veh2->vehiculo->capacidad_acumulada = nueva_cap2;

        return true;
    }

    return false;
}

// Copia la solución actual como base para generar un vecino
void generar_vecino(struct individuo *ind, struct vrp_configuracion *vrp)
{
    if (ind->metal->solucion_vecina)
        liberar_lista_vehiculos(ind->metal->solucion_vecina);

    ind->metal->solucion_vecina = copiar_lista_vehiculos(ind->metal->solucion_actual);
}

// Algoritmo de Recocido Simulado (SA)
void sa(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_distancias)
{
    double temperatura = ind->temperatura_inicial, delta, prob, factor;
    bool aceptado = false;

    // Inicialización de soluciones
    ind->metal->mejor_solucion = copiar_lista_vehiculos(ind->metal->solucion_inicial);
    ind->metal->fitness_mejor_solucion = ind->metal->fitness_solucion_inicial;
    ind->metal->solucion_actual = copiar_lista_vehiculos(ind->metal->solucion_inicial);
    ind->metal->fitness_solucion_actual = ind->metal->fitness_solucion_inicial;

    // Ciclo de enfriamiento
    while (temperatura > ind->temperatura_final)
    {
        for (int i = 0; i < ind->numIteracionesSA; i++)
        {
            generar_vecino(ind, vrp);

            prob = (double)rand() / RAND_MAX;
            factor = ind->factor_control * (1.0 - (temperatura / ind->temperatura_inicial));
            aceptado = false;

            // Se decide qué movimiento hacer según una probabilidad controlada por temperatura
            if (ind->hormiga->vehiculos_necesarios > 1)
            {

                if (prob < factor / 3.0)
                    aceptado = mover_cliente_vehiculo(ind, vrp);
                else if (prob < 2.0 * factor / 3.0)
                    aceptado = invertir_segmento_ruta(ind);
                else
                    aceptado = mover_dos_clientes_vehiculos(ind,vrp, instancia_distancias);
            }
            else
            {
                aceptado = false;
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
    ind->metal = asignar_memoria_metal(); // Asignación de memoria para la estructura metal
    ind->metal->solucion_vecina = NULL; // Inicializa la solución vecina
    ind->metal->solucion_actual = NULL; // Inicializa la solución actual
    ind->metal->mejor_solucion = NULL; // Inicializa la mejor solución
    ind->metal->fitness_solucion_actual = 0.0; // Inicializa el fitness de la solución actual
    ind->metal->fitness_solucion_vecina = 0.0; // Inicializa el fitness de la solución vecina
    ind->metal->fitness_mejor_solucion = 0.0; // Inicializa el fitness de la mejor solución
    ind->metal->solucion_inicial = copiar_lista_vehiculos(ind->hormiga->flota); // Copia la solución inicial
    ind->metal->fitness_solucion_inicial = ind->hormiga->fitness_global; // Guarda el fitness de la solución inicial
}

// Función principal que ejecuta SA sobre un individuo del VRP
void vrp_sa(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_distancias)
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
