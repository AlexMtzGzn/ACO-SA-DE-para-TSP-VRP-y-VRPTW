#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "../include/estructuras.h"
#include "../include/lista_ruta.h"
#include "../include/tsp_sa.h"
#include "../include/control_memoria.h"
#include "../include/salida_datos.h"

// Evalúa la función objetivo (FO) de la solución vecina generada en SA
void evaluaFO_SA(struct individuo *ind, struct tsp_configuracion *tsp, double **instancia_distancias)
{
    ind->metal->fitness_solucion_vecina = 0.0; // Inicializa el fitness

    // Recorre la lista de nodos de la solución vecina para sumar las distancias entre clientes consecutivos
    nodo_ruta *nodo_actual = ind->metal->solucion_vecina->cabeza;
    while (nodo_actual != NULL && nodo_actual->siguiente != NULL)
    {
        int cliente_actual = nodo_actual->cliente;
        int cliente_siguiente = nodo_actual->siguiente->cliente;

        // Suma la distancia entre cliente_actual y cliente_siguiente
        ind->metal->fitness_solucion_vecina += instancia_distancias[cliente_actual][cliente_siguiente];
        nodo_actual = nodo_actual->siguiente;
    }
}

// Invierte un segmento aleatorio de clientes en la ruta de la solución vecina
bool invertirSegmentoRuta(struct individuo *ind)
{
    struct lista_ruta *ruta = ind->metal->solucion_vecina;
    if (!ruta || !ruta->cabeza)
        return false;

    // Contar cuántos clientes (ignorando los depósitos con cliente = 0) hay en la ruta
    int num_clientes = 0;
    nodo_ruta *nodo_actual = ruta->cabeza;
    while (nodo_actual)
    {
        if (nodo_actual->cliente != 0)
            num_clientes++;
        nodo_actual = nodo_actual->siguiente;
    }

    if (num_clientes < 3)
        return false; // Se necesitan al menos 3 clientes para invertir un segmento

    // Elegir dos posiciones aleatorias distintas dentro de los clientes
    int inicio = rand() % num_clientes;
    int fin = rand() % num_clientes;
    while (inicio == fin)
        fin = rand() % num_clientes;

    if (inicio > fin)
    {
        int tmp = inicio;
        inicio = fin;
        fin = tmp;
    }

    // Crear un arreglo de punteros a los nodos de clientes (ignorando depósitos)
    nodo_ruta *clientes[num_clientes];
    nodo_actual = ruta->cabeza;
    int pos = 0;

    while (nodo_actual)
    {
        if (nodo_actual->cliente != 0)
        {
            clientes[pos] = nodo_actual;
            pos++;
        }
        nodo_actual = nodo_actual->siguiente;
    }

    // Invertir el segmento entre las posiciones 'inicio' y 'fin'
    while (inicio < fin)
    {
        int temp_cliente = clientes[inicio]->cliente;
        clientes[inicio]->cliente = clientes[fin]->cliente;
        clientes[fin]->cliente = temp_cliente;
        inicio++;
        fin--;
    }

    return true;
}

// Intercambia dos clientes aleatorios distintos en la solución vecina
bool intercambiarClientes(struct individuo *ind, struct tsp_configuracion *config)
{
    int intentos_maximos = 10;

    while (intentos_maximos--)
    {
        int cliente1 = (rand() % config->num_clientes) + 1; // Clientes válidos de 1 a n
        int cliente2 = (rand() % config->num_clientes) + 1;

        if (cliente1 == cliente2)
            continue;

        nodo_ruta *nodo1 = NULL;
        nodo_ruta *nodo2 = NULL;

        struct lista_ruta *ruta = ind->metal->solucion_vecina;
        if (!ruta || !ruta->cabeza)
            return false;

        nodo_ruta *nodo_actual = ruta->cabeza;

        // Buscar los nodos correspondientes a los clientes seleccionados
        while (nodo_actual)
        {
            if (nodo_actual->cliente == 0)
            {
                nodo_actual = nodo_actual->siguiente;
                continue; // Ignorar depósito
            }

            if (nodo_actual->cliente == cliente1)
                nodo1 = nodo_actual;
            else if (nodo_actual->cliente == cliente2)
                nodo2 = nodo_actual;

            if (nodo1 && nodo2)
                break;

            nodo_actual = nodo_actual->siguiente;
        }

        // Si ambos nodos fueron encontrados, intercambiar sus valores
        if (nodo1 && nodo2)
        {
            int temp = nodo1->cliente;
            nodo1->cliente = nodo2->cliente;
            nodo2->cliente = temp;
            return true;
        }
    }

    return false; // No se encontraron ambos clientes tras varios intentos
}

// Mueve un cliente de una posición a otra en la solución actual
bool moverClienteRuta(struct individuo *ind)
{
    if (!ind || !ind->metal || !ind->metal->solucion_actual)
        return false;

    lista_ruta *ruta = ind->metal->solucion_actual;
    if (!ruta->cabeza || !ruta->cabeza->siguiente)
        return false;

    // Contar el número total de clientes (nodos, incluyendo depósito inicial)
    int num_nodos = 0;
    nodo_ruta *nodo_actual = ruta->cabeza->siguiente;
    while (nodo_actual)
    {
        num_nodos++;
        nodo_actual = nodo_actual->siguiente;
    }

    if (num_nodos < 2)
        return false; // Se requiere al menos 2 nodos para mover uno

    int origen = rand() % num_nodos;
    int destino = rand() % num_nodos;

    while (origen == destino)
        destino = rand() % num_nodos;

    // Asegurar que origen < destino para simplificar lógica
    if (origen > destino)
    {
        int temp = origen;
        origen = destino;
        destino = temp;
    }

    // Buscar nodo anterior a la posición 'origen'
    nodo_ruta *nodo_prev_origen = ruta->cabeza;
    for (int i = 0; i < origen; i++)
        nodo_prev_origen = nodo_prev_origen->siguiente;

    nodo_ruta *nodo_mover = nodo_prev_origen->siguiente;
    if (!nodo_mover)
        return false;

    // Desconectar el nodo a mover
    nodo_prev_origen->siguiente = nodo_mover->siguiente;

    // Buscar nodo en la posición 'destino' para insertar después
    nodo_ruta *nodo_destino = ruta->cabeza->siguiente;
    for (int i = 0; i < destino - 1; i++)
        nodo_destino = nodo_destino->siguiente;

    // Insertar nodo_mover después de nodo_destino
    nodo_mover->siguiente = nodo_destino->siguiente;
    nodo_destino->siguiente = nodo_mover;

    return true;
}

// Genera una solución vecina a partir de la solución actual
void generar_vecino(struct individuo *ind)
{
    // Libera la solución vecina anterior si existe
    if (ind->metal->solucion_vecina)
        liberar_lista_ruta(ind->metal->solucion_vecina);

    // Crea una copia de la solución actual como base para la nueva vecina
    ind->metal->solucion_vecina = copiar_lista_ruta(ind->metal->solucion_actual);
}

// Algoritmo de Recocido Simulado para optimización de ruta en TSP
void sa(struct tsp_configuracion *tsp, struct individuo *ind, double **instancia_distancias)
{
    double temperatura = ind->temperatura_inicial, delta, prob, factor;
    bool aceptado = false;

    // Inicializa la mejor y actual solución con la solución inicial
    ind->metal->mejor_solucion = copiar_lista_ruta(ind->metal->solucion_inicial);
    ind->metal->fitness_mejor_solucion = ind->metal->fitness_solucion_inicial;

    ind->metal->solucion_actual = copiar_lista_ruta(ind->metal->solucion_inicial);
    ind->metal->fitness_solucion_actual = ind->metal->fitness_solucion_inicial;

    // Bucle principal de SA: se ejecuta mientras la temperatura no sea suficientemente baja
    while (temperatura > ind->temperatura_final)
    {
        // Por cada temperatura, se generan varias soluciones vecinas
        for (int i = 0; i < ind->numIteracionesSA; i++)
        {
            generar_vecino(ind); // Genera una nueva solución vecina

            prob = (double)rand() / RAND_MAX;
            factor = ind->factor_control * (1.0 - (temperatura / ind->temperatura_inicial));
            aceptado = false;

            if (prob < factor / 3.0)
                aceptado = invertirSegmentoRuta(ind);
            else if (prob < 2.0 * factor / 3.0)
                aceptado = intercambiarClientes(ind, tsp);
            else
                aceptado = moverClienteRuta(ind);

            if (!aceptado)
                continue;
            // Intercambia dos clientes en la ruta (sin tocar el depósito)
            evaluaFO_SA(ind, tsp, instancia_distancias); // Evalúa la calidad (fitness) de la solución vecina

            // Calcula la diferencia entre el fitness del vecino y el actual
            delta = ind->metal->fitness_solucion_vecina - ind->metal->fitness_solucion_actual;

            // Acepta la nueva solución si es mejor o con cierta probabilidad si es peor (criterio de Metropolis)
            if (delta < 0 || ((double)rand() / RAND_MAX < exp(-delta / temperatura)))
            {
                // Acepta la solución vecina como la actual
                liberar_lista_ruta(ind->metal->solucion_actual);
                ind->metal->solucion_actual = copiar_lista_ruta(ind->metal->solucion_vecina);
                ind->metal->fitness_solucion_actual = ind->metal->fitness_solucion_vecina;

                // Actualiza la mejor solución encontrada si es necesario
                if (ind->metal->fitness_solucion_actual < ind->metal->fitness_mejor_solucion)
                {
                    liberar_lista_ruta(ind->metal->mejor_solucion);
                    ind->metal->mejor_solucion = copiar_lista_ruta(ind->metal->solucion_actual);
                    ind->metal->fitness_mejor_solucion = ind->metal->fitness_solucion_actual;
                }
            }
        }

        // Reduce la temperatura (enfriamiento)
        temperatura *= ind->factor_enfriamiento;
    }

    // Libera la última solución actual al finalizar
    liberar_lista_ruta(ind->metal->solucion_actual);
}

// Inicializa la estructura 'metal' que contiene información del espacio de búsqueda para SA
void inicializar_metal(struct individuo *ind)
{
    ind->metal = asignar_memoria_metal();
    ind->metal->solucion_vecina = NULL;
    ind->metal->solucion_actual = NULL;
    ind->metal->mejor_solucion = NULL;
    ind->metal->fitness_solucion_actual = 0.0;
    ind->metal->fitness_solucion_vecina = 0.0;

    // Guarda la solución inicial (proveniente de ACO, por ejemplo)
    ind->metal->solucion_inicial = copiar_lista_ruta(ind->hormiga->ruta);
    ind->metal->fitness_solucion_inicial = ind->hormiga->fitness_global;
}

// Función principal que ejecuta el Recocido Simulado sobre el individuo
void tsp_sa(struct tsp_configuracion *tsp, struct individuo *ind, double **instancia_distancias)
{
    inicializar_metal(ind); // Inicializa estructuras auxiliares del SA

    sa(tsp, ind, instancia_distancias); // Ejecuta el algoritmo de Recocido Simulado

    // Si la mejor solución obtenida mejora la original de la hormiga, se actualiza
    if (ind->metal->fitness_mejor_solucion < ind->hormiga->fitness_global)
    {
        ind->fitness = ind->metal->fitness_mejor_solucion;
        ind->hormiga->fitness_global = ind->metal->fitness_mejor_solucion;

        // Libera la antigua ruta de la hormiga
        if (ind->hormiga->ruta)
            liberar_lista_ruta(ind->hormiga->ruta);

        // Asigna la nueva ruta mejorada como la nueva ruta de la hormiga
        ind->hormiga->ruta = copiar_lista_ruta(ind->metal->mejor_solucion);
    }

    liberar_memoria_metal(ind); // Libera toda la memoria utilizada por SA
}
