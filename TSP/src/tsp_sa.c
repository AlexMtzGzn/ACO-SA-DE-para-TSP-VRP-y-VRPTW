#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

// Intercambia dos clientes distintos en la solución vecina
void intercambiarClientes(struct individuo *ind, struct tsp_configuracion *tsp)
{
    int cliente1, cliente2;

    // Selección aleatoria de dos clientes distintos (sin incluir el depósito 0)
    do
    {
        cliente1 = (rand() % tsp->num_clientes) + 1; 
        cliente2 = (rand() % tsp->num_clientes) + 1;
    } while (cliente1 == cliente2);

    nodo_ruta *nodo_cliente1 = NULL;
    nodo_ruta *nodo_cliente2 = NULL;

    // Recorre la lista para encontrar los nodos que contienen a cliente1 y cliente2
    struct lista_ruta *ruta = ind->metal->solucion_vecina;
    struct nodo_ruta *actual = ruta->cabeza;

    while (actual != NULL)
    {
        // Ignora el depósito (0), no debe ser intercambiado
        if (actual->cliente == 0)
        {
            actual = actual->siguiente;
            continue;
        }

        if (actual->cliente == cliente1)
            nodo_cliente1 = actual;
        else if (actual->cliente == cliente2)
            nodo_cliente2 = actual;

        // Sale del ciclo si encontró ambos
        if (nodo_cliente1 && nodo_cliente2)
            break;

        actual = actual->siguiente;
    }

    // Intercambia los valores de cliente si ambos fueron encontrados
    if (nodo_cliente1 && nodo_cliente2)
    {
        int temp = nodo_cliente1->cliente;
        nodo_cliente1->cliente = nodo_cliente2->cliente;
        nodo_cliente2->cliente = temp;
    }
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
    double temperatura = ind->temperatura_inicial;
    double delta;

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
            generar_vecino(ind);               // Genera una nueva solución vecina
            intercambiarClientes(ind, tsp);    // Intercambia dos clientes en la ruta (sin tocar el depósito)
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
    ind->metal = (struct metal *)malloc(sizeof(struct metal));
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
