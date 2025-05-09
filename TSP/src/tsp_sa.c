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

bool invertirSegmentoRuta(struct individuo *ind)
{
    struct lista_ruta *ruta = ind->metal->solucion_vecina;
    if (!ruta || !ruta->cabeza)
        return false;

    // Contar clientes (ignorando el depósito 0)
    int total_clientes = 0;
    nodo_ruta *nodo = ruta->cabeza;
    while (nodo)
    {
        if (nodo->cliente != 0)
            total_clientes++;
        nodo = nodo->siguiente;
    }

    if (total_clientes < 3)
        return false;

    // Seleccionar dos índices aleatorios distintos
    int idx1 = rand() % total_clientes;
    int idx2 = rand() % total_clientes;
    while (idx1 == idx2)
        idx2 = rand() % total_clientes;

    if (idx1 > idx2)
    {
        int temp = idx1;
        idx1 = idx2;
        idx2 = temp;
    }

    // Recorrer la lista y ubicar los punteros a los nodos a invertir
    int pos = 0;
    nodo_ruta *nodo_actual = ruta->cabeza;
    nodo_ruta *segmento[total_clientes]; // Punteros a los nodos cliente

    while (nodo_actual)
    {
        if (nodo_actual->cliente != 0)
        {
            segmento[pos] = nodo_actual;
            pos++;
        }
        nodo_actual = nodo_actual->siguiente;
    }

    // Invertir el segmento entre idx1 y idx2
    while (idx1 < idx2)
    {
        int tmp = segmento[idx1]->cliente;
        segmento[idx1]->cliente = segmento[idx2]->cliente;
        segmento[idx2]->cliente = tmp;
        idx1++;
        idx2--;
    }

    return true;
}


// Intercambia dos clientes distintos en la solución vecina
bool intercambiarClientes(struct individuo *ind, struct tsp_configuracion *tsp)
{
    int intentos = 100;

    while (intentos--)
    {
        int cliente1 = (rand() % tsp->num_clientes) + 1; // Clientes 1..n
        int cliente2 = (rand() % tsp->num_clientes) + 1;

        if (cliente1 == cliente2)
            continue;

        nodo_ruta *nodo_cliente1 = NULL;
        nodo_ruta *nodo_cliente2 = NULL;

        struct lista_ruta *ruta = ind->metal->solucion_vecina;
        if (!ruta || !ruta->cabeza)
            return false;

        nodo_ruta *actual = ruta->cabeza;

        // Buscar los nodos correspondientes
        while (actual != NULL)
        {
            if (actual->cliente == 0)
            {
                actual = actual->siguiente;
                continue; // ignorar depósito
            }

            if (actual->cliente == cliente1)
                nodo_cliente1 = actual;
            else if (actual->cliente == cliente2)
                nodo_cliente2 = actual;

            if (nodo_cliente1 && nodo_cliente2)
                break;

            actual = actual->siguiente;
        }

        // Si ambos nodos fueron encontrados, intercambiarlos
        if (nodo_cliente1 && nodo_cliente2)
        {
            int temp = nodo_cliente1->cliente;
            nodo_cliente1->cliente = nodo_cliente2->cliente;
            nodo_cliente2->cliente = temp;
            return true;
        }
    }

    return false; // No se logró encontrar ambos clientes tras varios intentos
}

bool moverClienteRuta(struct individuo *ind)
{
    if (ind == NULL || ind->metal == NULL || ind->metal->solucion_actual == NULL) 
        return false; // Si el individuo o su solución actual son NULL, no se puede hacer nada

    lista_ruta *ruta = ind->metal->solucion_actual;
    
    // Verificar que haya al menos 2 clientes en la ruta
    if (ruta->cabeza == NULL || ruta->cabeza->siguiente == NULL)
        return false;

    // Seleccionar dos posiciones aleatorias dentro de la ruta
    int total_clientes = 0;
    nodo_ruta *nodo = ruta->cabeza->siguiente;
    while (nodo != NULL)
    {
        total_clientes++;
        nodo = nodo->siguiente;
    }

    if (total_clientes < 2)
        return false; // Si hay menos de 2 clientes, no podemos hacer el movimiento

    // Elegir dos índices aleatorios diferentes
    int idx1 = rand() % total_clientes;
    int idx2 = rand() % total_clientes;

    // Asegurarse de que los índices sean distintos
    while (idx1 == idx2)
        idx2 = rand() % total_clientes;

    if (idx1 > idx2)
    {
        int tmp = idx1;
        idx1 = idx2;
        idx2 = tmp;
    }

    // Obtener los nodos en las posiciones idx1 e idx2
    nodo_ruta *nodo_izq = ruta->cabeza->siguiente;
    nodo_ruta *nodo_der = ruta->cabeza->siguiente;

    for (int i = 0; i < idx1; i++)
        nodo_izq = nodo_izq->siguiente;

    for (int i = 0; i < idx2; i++)
        nodo_der = nodo_der->siguiente;

    // Verificar si los nodos seleccionados son válidos
    if (nodo_izq == NULL || nodo_der == NULL)
        return false;

    // Desconectar el nodo en idx1
    nodo_ruta *nodo_a_mover = nodo_izq;
    nodo_izq = nodo_izq->siguiente;
    nodo_a_mover->siguiente = NULL;  // Desconectar el nodo temporalmente

    // Conectar el nodo a la nueva posición (después de idx2)
    nodo_a_mover->siguiente = nodo_der->siguiente;
    nodo_der->siguiente = nodo_a_mover;

    // Opcional: Verificar que el movimiento sea válido (por ejemplo, capacidad de vehículos si es VRP)
    // Si tienes restricciones adicionales, puedes agregar validaciones aquí

    return true; // Movimiento exitoso
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
