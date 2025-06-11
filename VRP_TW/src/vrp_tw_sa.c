#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "../include/vrp_tw_sa.h"
#include "../include/estructuras.h"
#include "../include/lista_flota.h"
#include "../include/lista_ruta.h"
#include "../include/control_memoria.h"
#include "../include/salida_datos.h"
#include "../include/movimientos_sa.h"

struct hormiga *copiar_hormiga_sa(struct hormiga *hormiga_original)
{

    struct hormiga *hormiga_copia = asignar_memoria_hormigas(1);

    hormiga_copia->id_hormiga = hormiga_original->id_hormiga;
    hormiga_copia->fitness_global = hormiga_original->fitness_global;
    hormiga_copia->vehiculos_maximos = hormiga_original->vehiculos_maximos;
    hormiga_copia->vehiculos_necesarios = hormiga_original->vehiculos_necesarios;
    hormiga_copia->flota = copiar_lista_vehiculos(hormiga_original->flota);

    return hormiga_copia;
}

void calculamosVentanasCapacidad(struct lista_vehiculos *flota, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    struct nodo_vehiculo *nodoVehiculo = flota->cabeza;

    while (nodoVehiculo != NULL)
    {
        struct vehiculo *vehiculo = nodoVehiculo->vehiculo;
        struct nodo_ruta *clienteActual = vehiculo->ruta->cabeza;
        struct nodo_ruta *clienteAnterior = NULL;

        double tiempo = 0.0;
        double capacidad = 0.0;
        double inicio = 0.0;
        double fin = 0.0;

        if (vehiculo->clientes_contados > 0)
        {
            int clientesReales = 0;

            struct nodo_ruta *temp = vehiculo->ruta->cabeza;
            while (temp != NULL)
            {
                if (temp->cliente != 0)
                {
                    clientesReales++;
                }
                temp = temp->siguiente;
            }

            if (clientesReales > 0)
            {
                while (clienteActual != NULL)
                {
                    int id_actual = clienteActual->cliente;

                    if (clienteAnterior == NULL)
                    {
                        if (id_actual != 0)
                        {
                            tiempo += instancia_distancias[0][id_actual] / vehiculo->velocidad;

                            if (tiempo < vrp->clientes[id_actual].vt_inicial)
                            {
                                tiempo = vrp->clientes[id_actual].vt_inicial;
                            }

                            inicio = tiempo;
                            capacidad += vrp->clientes[id_actual].demanda_capacidad;

                            if (tiempo > vrp->clientes[id_actual].vt_final)
                            {
                                printf("Violación de ventana de tiempo: Cliente %d, llegada %.2f > ventana final %.2f\n",
                                       id_actual, tiempo, vrp->clientes[id_actual].vt_final);
                            }
                        }
                    }
                    else
                    {
                        if (clienteAnterior->cliente != 0)
                        {
                            tiempo += vrp->clientes[clienteAnterior->cliente].tiempo_servicio;
                        }

                        tiempo += instancia_distancias[clienteAnterior->cliente][id_actual] / vehiculo->velocidad;

                        if (tiempo < vrp->clientes[id_actual].vt_inicial)
                        {
                            tiempo = vrp->clientes[id_actual].vt_inicial;
                        }

                        if (id_actual != 0)
                        {
                            capacidad += vrp->clientes[id_actual].demanda_capacidad;

                            if (tiempo > vrp->clientes[id_actual].vt_final)
                            {
                                printf("Violación de ventana de tiempo: Cliente %d, llegada %.2f > ventana final %.2f\n",
                                       id_actual, tiempo, vrp->clientes[id_actual].vt_final);
                            }
                        }
                    }

                    if (clienteActual->siguiente == NULL || clienteActual->siguiente->cliente == 0)
                    {
                        if (id_actual != 0)
                        {
                            tiempo += vrp->clientes[id_actual].tiempo_servicio;
                        }

                        tiempo += instancia_distancias[id_actual][0] / vehiculo->velocidad;
                        fin = tiempo;

                        if (clienteActual->siguiente != NULL && clienteActual->siguiente->cliente == 0)
                        {
                            clienteActual = clienteActual->siguiente;
                        }
                    }

                    clienteAnterior = clienteActual;
                    clienteActual = clienteActual->siguiente;
                }
            }
        }

        vehiculo->capacidad_acumulada = capacidad;
        vehiculo->tiempo_salida_vehiculo = inicio;
        vehiculo->tiempo_llegada_vehiculo = fin;

        // Verificación de restricción de capacidad
        if (capacidad > vehiculo->capacidad_maxima)
        {
            printf("Violación de capacidad en Vehículo ID %d: %.2f > %.2f\n",
                   vehiculo->id_vehiculo, capacidad, vehiculo->capacidad_maxima);
        }

        nodoVehiculo = nodoVehiculo->siguiente;
    }
}

void evaluaFO_SA(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    hormiga->fitness_global = 0.0;

    struct lista_ruta *ruta;
    nodo_ruta *nodo_actual;
    double fitness_vehiculo;
    int cliente_actual, cliente_siguiente;
    struct nodo_vehiculo *vehiculo_actual = hormiga->flota->cabeza;

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
        hormiga->fitness_global += fitness_vehiculo;
        vehiculo_actual = vehiculo_actual->siguiente;
    }
}

// Algoritmo de Recocido Simulado (SA)
void sa(struct vrp_configuracion *vrp, struct hormiga *hormiga_solucion_vecina,
        struct hormiga *hormiga_solucion_actual, struct hormiga *hormiga_mejor_solucion,
        struct individuo *ind, double **instancia_distancias)
{
    double temperatura = ind->temperatura_inicial;
    double delta, prob;
    bool aceptado = false;

    // Ciclo de enfriamiento
    while (temperatura > ind->temperatura_final)
    {
        // printf("Temp: %.4f | Fitness actual: %.4f | Mejor: %.4f\n",
        //        temperatura,
        //        hormiga_solucion_actual->fitness_global,
        //        hormiga_mejor_solucion->fitness_global);

        for (int i = 0; i < ind->numIteracionesSA; i++)
        {
            // 1. Copiar solución actual a vecina para modificar
            if (hormiga_solucion_vecina->flota)
                liberar_lista_vehiculos(hormiga_solucion_vecina->flota);

            hormiga_solucion_vecina->fitness_global = hormiga_solucion_actual->fitness_global;
            hormiga_solucion_vecina->vehiculos_necesarios = hormiga_solucion_actual->vehiculos_necesarios;
            hormiga_solucion_vecina->flota = copiar_lista_vehiculos(hormiga_solucion_actual->flota);

            // 2. Generar vecino aplicando movimiento
            prob = (double)rand() / RAND_MAX;
            double temp_invertida = temperatura / ind->temperatura_inicial;
            aceptado = false;

            if (vrp->num_clientes == 26)
            {

                if (hormiga_solucion_vecina->vehiculos_necesarios > 1)
                {
                    // temp_invertida va de 0 a 1, así que 1-temp_invertida favorece exploración al inicio
                    double temp_explo = 1.0 - temp_invertida; // va de 1 a 0

                    double umbral1 = 0.40 * temp_invertida;           // 2-opt (más importante)
                    double umbral2 = umbral1 + 0.25 * temp_invertida; // reinserción
                    double umbral3 = umbral2 + 0.20 * temp_invertida; // 2.5-opt
                    double umbral4 = umbral3 + 0.10 * temp_explo;     // swap_intra (opcional)
                    // resto para 2.5-opt

                    if (prob < umbral1)
                        aceptado = swap_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral2)
                        aceptado = swap_intra(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral3)
                        aceptado = reinsercion_intra_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral4)
                        aceptado = opt_2(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else
                        aceptado = opt_2_5(hormiga_solucion_vecina, vrp, instancia_distancias);
                }
                else
                {

                    // temp_invertida va de 0 a 1, así que 1-temp_invertida favorece exploración al inicio
                    double temp_explo = 1.0 - temp_invertida; // va de 1 a 0

                    double umbral1 = 0.30 * temp_invertida;           // 2-opt
                    double umbral2 = umbral1 + 0.25 * temp_invertida; // 2.5-opt
                    double umbral3 = umbral2 + 0.20 * temp_explo;     // reinserción
                    double umbral4 = umbral3 + 0.15 * temp_explo;     // or-opt
                    // resto para swap_intra
                    if (prob < umbral1)
                        aceptado = opt_2(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral2)
                        aceptado = opt_2_5(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral3)
                        aceptado = reinsercion_intra_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral4)
                        aceptado = or_opt(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else
                        aceptado = swap_intra(hormiga_solucion_vecina, vrp, instancia_distancias);
                }
            }

            if (vrp->num_clientes == 51)
            {

                if (hormiga_solucion_vecina->vehiculos_necesarios > 1)
                {
                    // temp_invertida va de 0 a 1, así que 1-temp_invertida favorece exploración al inicio
                    double temp_explo = 1.0 - temp_invertida; // va de 1 a 0

                    double umbral1 = 0.30 * temp_invertida;           // 2-opt
                    double umbral2 = umbral1 + 0.25 * temp_explo;     // swap_inter (exploración)
                    double umbral3 = umbral2 + 0.20 * temp_invertida; // reinserción
                    double umbral4 = umbral3 + 0.15 * temp_invertida; // or-opt
                                                                      // resto para 2.5-opt
                    // resto para opt_2_5

                    if (prob < umbral1)
                        aceptado = opt_2(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral2)
                        aceptado = swap_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral3)
                        aceptado = reinsercion_intra_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral4)
                        aceptado = or_opt(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else
                        aceptado = opt_2_5(hormiga_solucion_vecina, vrp, instancia_distancias);
                }
                else // Un solo vehículo
                {
                    double temp_explo = 1.0 - temp_invertida; // va de 1 a 0

                    double umbral1 = 0.35 * temp_invertida;           // 2-opt (fundamental en TSP)
                    double umbral2 = umbral1 + 0.25 * temp_invertida; // 2.5-opt
                    double umbral3 = umbral2 + 0.20 * temp_invertida; // or-opt
                    double umbral4 = umbral3 + 0.15 * temp_explo;     // reinserción
                    // resto para swap_intra

                    if (prob < umbral1)
                        aceptado = opt_2(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral2)
                        aceptado = opt_2_5(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral3)
                        aceptado = or_opt(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral4)
                        aceptado = reinsercion_intra_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else
                        aceptado = swap_intra(hormiga_solucion_vecina, vrp, instancia_distancias);
                }
            }

            if (vrp->num_clientes == 101)
            {

                if (hormiga_solucion_vecina->vehiculos_necesarios > 1)
                {
                    // temp_invertida va de 0 a 1, así que 1-temp_invertida favorece exploración al inicio
                    double temp_explo = 1.0 - temp_invertida; // va de 1 a 0

                    double umbral1 = 0.25 * temp_explo;               // swap_inter (exploración fuerte)
                    double umbral2 = umbral1 + 0.20 * temp_invertida; // 2-opt
                    double umbral3 = umbral2 + 0.18 * temp_invertida; // cross-exchange
                    double umbral4 = umbral3 + 0.15 * temp_invertida; // or-opt
                    double umbral5 = umbral4 + 0.12 * temp_invertida; // relocate-chain
                    double umbral6 = umbral5 + 0.08 * temp_invertida; // reinserción
                    // resto para opt_2_5

                    if (prob < umbral1)
                        aceptado = swap_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral2)
                        aceptado = opt_2(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral3)
                        aceptado = cross_exchange(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral4)
                        aceptado = or_opt(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral5)
                        aceptado = relocate_chain(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral6)
                        aceptado = reinsercion_intra_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else
                        aceptado = opt_2_5(hormiga_solucion_vecina, vrp, instancia_distancias);
                }
                else // Un solo vehículo
                {
                    double temp_explo = 1.0 - temp_invertida; // va de 1 a 0

                    double umbral1 = 0.30 * temp_invertida;           // 2-opt (crítico)
                    double umbral2 = umbral1 + 0.25 * temp_invertida; // 2.5-opt
                    double umbral3 = umbral2 + 0.20 * temp_invertida; // or-opt
                    double umbral4 = umbral3 + 0.15 * temp_explo;     // relocate-chain
                    double umbral5 = umbral4 + 0.08 * temp_invertida; // reinserción
                    // resto para swap_intra

                    if (prob < umbral1)
                        aceptado = opt_2(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral2)
                        aceptado = opt_2_5(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral3)
                        aceptado = or_opt(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral4)
                        aceptado = relocate_chain(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else if (prob < umbral5)
                        aceptado = reinsercion_intra_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                    else
                        aceptado = swap_intra(hormiga_solucion_vecina, vrp, instancia_distancias);
                }
            }

            if (!aceptado)
                continue;

            // 3. Evaluar solución vecina
            evaluaFO_SA(hormiga_solucion_vecina, vrp, instancia_distancias);
            delta = hormiga_solucion_vecina->fitness_global - hormiga_solucion_actual->fitness_global;

            // 4. Criterio de aceptación
            bool aceptar = false;
            if (delta < 0)
            {
                // Mejora: siempre acepta
                aceptar = true;
            }
            else
            {
                // Empeora: acepta con probabilidad
                double prob_aceptacion = exp(-delta / temperatura);
                if ((double)rand() / RAND_MAX < prob_aceptacion)
                    aceptar = true;
            }

            if (aceptar)
            {
                // 5. Actualizar solución actual
                if (hormiga_solucion_actual->flota)
                    liberar_lista_vehiculos(hormiga_solucion_actual->flota);

                hormiga_solucion_actual->fitness_global = hormiga_solucion_vecina->fitness_global;
                hormiga_solucion_actual->vehiculos_necesarios = hormiga_solucion_vecina->vehiculos_necesarios;
                hormiga_solucion_actual->flota = copiar_lista_vehiculos(hormiga_solucion_vecina->flota);

                // 6. Actualizar mejor solución si es necesario
                if (hormiga_solucion_vecina->fitness_global < hormiga_mejor_solucion->fitness_global)
                {
                    if (hormiga_mejor_solucion->flota)
                        liberar_lista_vehiculos(hormiga_mejor_solucion->flota);

                    hormiga_mejor_solucion->fitness_global = hormiga_solucion_vecina->fitness_global;
                    hormiga_mejor_solucion->vehiculos_necesarios = hormiga_solucion_vecina->vehiculos_necesarios;
                    hormiga_mejor_solucion->flota = copiar_lista_vehiculos(hormiga_solucion_vecina->flota);
                }
            }
        }

        temperatura *= ind->factor_enfriamiento; // Disminuye temperatura
    }
}
// Función principal que ejecuta SA sobre un individuo del VRP
void vrp_tw_sa(struct vrp_configuracion *vrp, struct hormiga *hormiga_original, struct individuo *ind, double **instancia_distancias)
{
    struct hormiga *hormiga_solucion_vecina = asignar_memoria_hormigas(1);
    struct hormiga *hormiga_solucion_actual = asignar_memoria_hormigas(1);
    struct hormiga *hormiga_mejor_solucion = asignar_memoria_hormigas(1);
    struct hormiga *hormiga_solucion_inicial = asignar_memoria_hormigas(1);

    hormiga_solucion_inicial = copiar_hormiga_sa(hormiga_original);
    hormiga_solucion_vecina = copiar_hormiga_sa(hormiga_solucion_inicial);
    hormiga_solucion_actual = copiar_hormiga_sa(hormiga_solucion_inicial);
    hormiga_mejor_solucion = copiar_hormiga_sa(hormiga_solucion_inicial);

    sa(vrp, hormiga_solucion_vecina, hormiga_solucion_actual, hormiga_mejor_solucion, ind, instancia_distancias);

    // Si se mejora la solución global, se guarda
    if (hormiga_mejor_solucion->fitness_global < hormiga_original->fitness_global)
    {
        // printf("\nMejoro de %.2lf a %.2lf", hormiga_original->fitness_global, hormiga_mejor_solucion->fitness_global);

        hormiga_original->fitness_global = hormiga_mejor_solucion->fitness_global;
        hormiga_original->vehiculos_necesarios = hormiga_mejor_solucion->vehiculos_necesarios;

        if (hormiga_original->flota)
            liberar_lista_vehiculos(hormiga_original->flota);

        hormiga_original->flota = copiar_lista_vehiculos(hormiga_mejor_solucion->flota);
    }
    // else
    // {
    //     printf("\nNo mejora %.2lf", hormiga_original->fitness_global);
    // }

    liberar_memoria_hormiga(hormiga_solucion_inicial, 1);
    liberar_memoria_hormiga(hormiga_solucion_vecina, 1);
    liberar_memoria_hormiga(hormiga_solucion_actual, 1);
    liberar_memoria_hormiga(hormiga_mejor_solucion, 1);
}