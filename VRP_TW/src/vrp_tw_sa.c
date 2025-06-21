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

// Función que copia una hormiga (solución) para usar en SA
struct hormiga *copiar_hormiga_sa(struct hormiga *hormiga_original)
{
    // Asigna memoria para una hormiga (1)
    struct hormiga *hormiga_copia = asignar_memoria_hormigas(1);

    // Copia atributos simples
    hormiga_copia->id_hormiga = hormiga_original->id_hormiga;
    hormiga_copia->fitness_global = hormiga_original->fitness_global;
    hormiga_copia->vehiculos_maximos = hormiga_original->vehiculos_maximos;
    hormiga_copia->vehiculos_necesarios = hormiga_original->vehiculos_necesarios;

    // Copia profunda de la lista de vehículos (flota)
    hormiga_copia->flota = copiar_lista_vehiculos(hormiga_original->flota);

    return hormiga_copia;
}

// Calcula ventanas de tiempo y capacidad acumulada para cada vehículo de la flota
void calculamosVentanasCapacidad(struct lista_vehiculos *flota, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    struct nodo_vehiculo *nodoVehiculo = flota->cabeza;

    // Itera sobre cada vehículo en la flota
    while (nodoVehiculo != NULL)
    {
        struct vehiculo *vehiculo = nodoVehiculo->vehiculo;
        struct nodo_ruta *clienteActual = vehiculo->ruta->cabeza;
        struct nodo_ruta *clienteAnterior = NULL;

        double tiempo = 0.0;    // Tiempo acumulado para la ruta
        double capacidad = 0.0; // Capacidad acumulada
        double inicio = 0.0;    // Tiempo de inicio para el vehículo
        double fin = 0.0;       // Tiempo de llegada al final de la ruta

        if (vehiculo->clientes_contados > 0)
        {
            int clientesReales = 0;

            // Cuenta clientes diferentes del depósito (cliente 0)
            struct nodo_ruta *temp = vehiculo->ruta->cabeza;
            while (temp != NULL)
            {
                if (temp->cliente != 0)
                {
                    clientesReales++;
                }
                temp = temp->siguiente;
            }

            // Si hay clientes reales, calcula tiempos y capacidades
            if (clientesReales > 0)
            {
                while (clienteActual != NULL)
                {
                    int id_actual = clienteActual->cliente;

                    if (clienteAnterior == NULL) // Primer cliente
                    {
                        if (id_actual != 0)
                        {
                            // Tiempo desde depósito hasta el primer cliente dividido por velocidad
                            tiempo += instancia_distancias[0][id_actual] / vehiculo->velocidad;

                            // Si se llega antes de la ventana, se espera
                            if (tiempo < vrp->clientes[id_actual].vt_inicial)
                            {
                                tiempo = vrp->clientes[id_actual].vt_inicial;
                            }

                            inicio = tiempo;                                         // Tiempo inicio ruta
                            capacidad += vrp->clientes[id_actual].demanda_capacidad; // Suma demanda

                            // Verifica violación ventana tiempo
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
                            // Se añade el tiempo de servicio del cliente anterior
                            tiempo += vrp->clientes[clienteAnterior->cliente].tiempo_servicio;
                        }

                        // Tiempo entre cliente anterior y actual dividido por velocidad
                        tiempo += instancia_distancias[clienteAnterior->cliente][id_actual] / vehiculo->velocidad;

                        // Espera si llega antes de la ventana inicial
                        if (tiempo < vrp->clientes[id_actual].vt_inicial)
                        {
                            tiempo = vrp->clientes[id_actual].vt_inicial;
                        }

                        if (id_actual != 0)
                        {
                            capacidad += vrp->clientes[id_actual].demanda_capacidad;

                            // Verifica violación ventana tiempo
                            if (tiempo > vrp->clientes[id_actual].vt_final)
                            {
                                printf("Violación de ventana de tiempo: Cliente %d, llegada %.2f > ventana final %.2f\n",
                                       id_actual, tiempo, vrp->clientes[id_actual].vt_final);
                            }
                        }
                    }

                    // Si es el último cliente o el siguiente es depósito (fin ruta)
                    if (clienteActual->siguiente == NULL || clienteActual->siguiente->cliente == 0)
                    {
                        if (id_actual != 0)
                        {
                            tiempo += vrp->clientes[id_actual].tiempo_servicio;
                        }

                        // Tiempo regreso a depósito
                        tiempo += instancia_distancias[id_actual][0] / vehiculo->velocidad;
                        fin = tiempo;

                        // Si el siguiente es depósito, avanzamos para terminar
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

        // Guardamos resultados en el vehículo
        vehiculo->capacidad_acumulada = capacidad;
        vehiculo->tiempo_salida_vehiculo = inicio;
        vehiculo->tiempo_llegada_vehiculo = fin;

        // Verifica restricción de capacidad
        if (capacidad > vehiculo->capacidad_maxima)
        {
            printf("Violación de capacidad en Vehículo ID %d: %.2f > %.2f\n",
                   vehiculo->id_vehiculo, capacidad, vehiculo->capacidad_maxima);
        }

        nodoVehiculo = nodoVehiculo->siguiente;
    }
}

// Evalúa la función objetivo (fitness) para la hormiga en base a la distancia total de la solución
void evaluaFO_SA(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    hormiga->fitness_global = 0.0;

    struct lista_ruta *ruta;
    nodo_ruta *nodo_actual;
    double fitness_vehiculo;
    int cliente_actual, cliente_siguiente;
    struct nodo_vehiculo *vehiculo_actual = hormiga->flota->cabeza;

    // Para cada vehículo en la flota
    while (vehiculo_actual != NULL)
    {
        fitness_vehiculo = 0.0;
        ruta = vehiculo_actual->vehiculo->ruta;
        nodo_actual = ruta->cabeza;

        // Suma la distancia entre clientes consecutivos en la ruta
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

// Función principal de recocido simulado (Simulated Annealing) optimizado para el VRPTW
void sa(struct vrp_configuracion *vrp, struct hormiga *hormiga_solucion_vecina,
        struct hormiga *hormiga_solucion_actual, struct hormiga *hormiga_mejor_solucion,
        struct individuo *ind, double **instancia_distancias)
{
    double temperatura = ind->temperatura_inicial; // Temperatura inicial
    double delta, prob;
    bool aceptado = false;
    int operador_usado = -1; // Para identificar qué operador se utilizó

    // Ciclo de enfriamiento: mientras la temperatura sea mayor que la temperatura final
    while (temperatura > ind->temperatura_final)
    {
        // Por cada iteración en el ciclo interno
        for (int i = 0; i < ind->numIteracionesSA; i++)
        {
            // 1. Preparar solución vecina: liberar flota antigua y copiar solución actual
            if (hormiga_solucion_vecina->flota)
                liberar_lista_vehiculos(hormiga_solucion_vecina->flota);

            hormiga_solucion_vecina->fitness_global = hormiga_solucion_actual->fitness_global;
            hormiga_solucion_vecina->vehiculos_necesarios = hormiga_solucion_actual->vehiculos_necesarios;
            hormiga_solucion_vecina->flota = copiar_lista_vehiculos(hormiga_solucion_actual->flota);

            // 2. Selección aleatoria de operador para generar vecino
            prob = (double)rand() / RAND_MAX;
            aceptado = false;
            operador_usado = -1;

            // --- INSTANCIAS PEQUEÑAS ---
            if (vrp->num_clientes <= 26)
            {
                if (hormiga_solucion_vecina->vehiculos_necesarios > 1)
                {
                    // Configuración agresiva para varias rutas
                    double umbral1 = 0.25;           // Cross-exchange
                    double umbral2 = umbral1 + 0.20; // Relocate-chain
                    double umbral3 = umbral2 + 0.20; // Swap inter
                    double umbral4 = umbral3 + 0.15; // Reinserción intra/inter
                    double umbral5 = umbral4 + 0.10; // 2.5-opt
                    double umbral6 = umbral5 + 0.05; // 2-opt
                    // Restante para or-opt y swap intra

                    if (prob < umbral1)
                    {
                        aceptado = cross_exchange(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 7;
                    }
                    else if (prob < umbral2)
                    {
                        aceptado = relocate_chain(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 8;
                    }
                    else if (prob < umbral3)
                    {
                        aceptado = swap_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 4;
                    }
                    else if (prob < umbral4)
                    {
                        aceptado = reinsercion_intra_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 5;
                    }
                    else if (prob < umbral5)
                    {
                        aceptado = opt_2_5(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 6;
                    }
                    else if (prob < umbral6)
                    {
                        aceptado = opt_2(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 1;
                    }
                    else if (prob < umbral6 + 0.03)
                    {
                        aceptado = or_opt(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 2;
                    }
                    else
                    {
                        aceptado = swap_intra(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 3;
                    }
                }
                else
                {
                    double umbral1 = 0.30;           // Relocate-chain
                    double umbral2 = umbral1 + 0.25; // 2.5-opt
                    double umbral3 = umbral2 + 0.20; // Reinserción
                    double umbral4 = umbral3 + 0.15; // 2-opt
                    // Restante para or-opt

                    if (prob < umbral1)
                    {
                        aceptado = relocate_chain(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 8;
                    }
                    else if (prob < umbral2)
                    {
                        aceptado = opt_2_5(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 6;
                    }
                    else if (prob < umbral3)
                    {
                        aceptado = reinsercion_intra_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 5;
                    }
                    else if (prob < umbral4)
                    {
                        aceptado = opt_2(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 1;
                    }
                    else
                    {
                        aceptado = or_opt(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 2;
                    }
                }
            }

            // --- INSTANCIAS MEDIANAS ---
            if (vrp->num_clientes > 26 && vrp->num_clientes <= 51)
            {
                if (hormiga_solucion_vecina->vehiculos_necesarios > 1)
                {
                    double umbral1 = 0.30;
                    double umbral2 = umbral1 + 0.25;
                    double umbral3 = umbral2 + 0.20;
                    double umbral4 = umbral3 + 0.12;
                    double umbral5 = umbral4 + 0.08;
                    double umbral6 = umbral5 + 0.03;

                    if (prob < umbral1)
                    {
                        aceptado = cross_exchange(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 7;
                    }
                    else if (prob < umbral2)
                    {
                        aceptado = relocate_chain(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 8;
                    }
                    else if (prob < umbral3)
                    {
                        aceptado = swap_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 4;
                    }
                    else if (prob < umbral4)
                    {
                        aceptado = reinsercion_intra_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 5;
                    }
                    else if (prob < umbral5)
                    {
                        aceptado = opt_2_5(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 6;
                    }
                    else if (prob < umbral6)
                    {
                        aceptado = opt_2(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 1;
                    }
                    else if (prob < umbral6 + 0.01)
                    {
                        aceptado = or_opt(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 2;
                    }
                    else
                    {
                        aceptado = swap_intra(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 3;
                    }
                }
                else // Un solo vehículo
                {
                    double umbral1 = 0.35;
                    double umbral2 = umbral1 + 0.30;
                    double umbral3 = umbral2 + 0.20;
                    double umbral4 = umbral3 + 0.10;

                    if (prob < umbral1)
                    {
                        aceptado = relocate_chain(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 8;
                    }
                    else if (prob < umbral2)
                    {
                        aceptado = opt_2_5(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 6;
                    }
                    else if (prob < umbral3)
                    {
                        aceptado = reinsercion_intra_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 5;
                    }
                    else if (prob < umbral4)
                    {
                        aceptado = opt_2(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 1;
                    }
                    else
                    {
                        aceptado = or_opt(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 2;
                    }
                }
            }

            // --- INSTANCIAS GRANDES ---
            if (vrp->num_clientes > 51 && vrp->num_clientes <= 101)
            {
                if (hormiga_solucion_vecina->vehiculos_necesarios > 1)
                {
                    double umbral1 = 0.35;
                    double umbral2 = umbral1 + 0.30;
                    double umbral3 = umbral2 + 0.20;
                    double umbral4 = umbral3 + 0.10;
                    double umbral5 = umbral4 + 0.03;
                    double umbral6 = umbral5 + 0.01;

                    if (prob < umbral1)
                    {
                        aceptado = relocate_chain(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 8;
                    }
                    else if (prob < umbral2)
                    {
                        aceptado = cross_exchange(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 7;
                    }
                    else if (prob < umbral3)
                    {
                        aceptado = swap_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 4;
                    }
                    else if (prob < umbral4)
                    {
                        aceptado = reinsercion_intra_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 5;
                    }
                    else if (prob < umbral5)
                    {
                        aceptado = opt_2_5(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 6;
                    }
                    else if (prob < umbral6)
                    {
                        aceptado = opt_2(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 1;
                    }
                    else if (prob < umbral6 + 0.005)
                    {
                        aceptado = or_opt(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 2;
                    }
                    else
                    {
                        aceptado = swap_intra(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 3;
                    }
                }
                else // Un solo vehículo
                {
                    double umbral1 = 0.40;
                    double umbral2 = umbral1 + 0.35;
                    double umbral3 = umbral2 + 0.20;
                    double umbral4 = umbral3 + 0.03;

                    if (prob < umbral1)
                    {
                        aceptado = relocate_chain(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 8;
                    }
                    else if (prob < umbral2)
                    {
                        aceptado = opt_2_5(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 6;
                    }
                    else if (prob < umbral3)
                    {
                        aceptado = reinsercion_intra_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 5;
                    }
                    else if (prob < umbral4)
                    {
                        aceptado = opt_2(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 1;
                    }
                    else
                    {
                        aceptado = or_opt(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 2;
                    }
                }
            }

            // Si no se aceptó el movimiento, pasa a siguiente iteración
            if (!aceptado)
                continue;

            // Limpieza de vehículos vacíos tras ciertos operadores para evitar rutas vacías inútiles
            bool necesita_limpieza = false;
            if (hormiga_solucion_vecina->vehiculos_necesarios > 1)
            {
                // Operadores que pueden dejar vehículos vacíos:
                // 4 = swap_inter, 5 = reinsercion, 7 = cross_exchange, 8 = relocate_chain
                if (operador_usado == 4 || operador_usado == 5 ||
                    operador_usado == 7 || operador_usado == 8)
                    necesita_limpieza = true;

                // Limpieza periódica cada 15 iteraciones como backup
                if (i % 15 == 0)
                    necesita_limpieza = true;
            }

            if (necesita_limpieza)
                hormiga_solucion_vecina->vehiculos_necesarios = eliminar_vehiculos_vacios(hormiga_solucion_vecina->flota);

            // 3. Evalúa la función objetivo para la solución vecina
            evaluaFO_SA(hormiga_solucion_vecina, vrp, instancia_distancias);
            delta = hormiga_solucion_vecina->fitness_global - hormiga_solucion_actual->fitness_global;

            // 4. Criterio de aceptación
            bool aceptar = false;
            if (delta < 0)
            {
                // Mejoró la solución, aceptar siempre
                aceptar = true;
            }
            else
            {
                // Empeoró, aceptar con cierta probabilidad según temperatura y delta
                double prob_aceptacion = exp(-delta / temperatura);
                if ((double)rand() / RAND_MAX < prob_aceptacion)
                    aceptar = true;
            }

            // Si se acepta la solución vecina
            if (aceptar)
            {
                // Libera memoria de la solución actual para no perder memoria
                if (hormiga_solucion_actual->flota)
                    liberar_lista_vehiculos(hormiga_solucion_actual->flota);

                // Actualiza solución actual con la vecina
                hormiga_solucion_actual->fitness_global = hormiga_solucion_vecina->fitness_global;
                hormiga_solucion_actual->vehiculos_necesarios = hormiga_solucion_vecina->vehiculos_necesarios;
                hormiga_solucion_actual->flota = copiar_lista_vehiculos(hormiga_solucion_vecina->flota);

                // Si mejora la mejor solución, actualizarla también
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

        // Reduce temperatura para próxima iteración del enfriamiento
        temperatura *= ind->factor_enfriamiento;
    }

    // Al final, limpiar vehículos vacíos de la mejor solución
    hormiga_mejor_solucion->vehiculos_necesarios = eliminar_vehiculos_vacios(hormiga_mejor_solucion->flota);
}

// Función principal que ejecuta SA sobre un individuo del VRP_TW con ventanas de tiempo
void vrp_tw_sa(struct vrp_configuracion *vrp, struct hormiga *hormiga_original, struct individuo *ind, double **instancia_distancias)
{
    // Asignación de memoria para soluciones vecinas, actuales y mejores
    struct hormiga *hormiga_solucion_vecina = asignar_memoria_hormigas(1);
    struct hormiga *hormiga_solucion_actual = asignar_memoria_hormigas(1);
    struct hormiga *hormiga_mejor_solucion = asignar_memoria_hormigas(1);
    struct hormiga *hormiga_solucion_inicial = asignar_memoria_hormigas(1);

    // Copia la solución original para iniciar el proceso de SA
    hormiga_solucion_inicial = copiar_hormiga_sa(hormiga_original);
    hormiga_solucion_vecina = copiar_hormiga_sa(hormiga_solucion_inicial);
    hormiga_solucion_actual = copiar_hormiga_sa(hormiga_solucion_inicial);
    hormiga_mejor_solucion = copiar_hormiga_sa(hormiga_solucion_inicial);

    // Ejecuta el algoritmo de recocido simulado
    sa(vrp, hormiga_solucion_vecina, hormiga_solucion_actual, hormiga_mejor_solucion, ind, instancia_distancias);

    // Si la mejor solución obtenida es mejor que la original, actualiza la solución original
    if (hormiga_mejor_solucion->fitness_global < hormiga_original->fitness_global)
    {
        hormiga_original->fitness_global = hormiga_mejor_solucion->fitness_global;
        hormiga_original->vehiculos_necesarios = hormiga_mejor_solucion->vehiculos_necesarios;

        // Libera la memoria de la flota original antes de copiar la nueva solución
        if (hormiga_original->flota)
            liberar_lista_vehiculos(hormiga_original->flota);

        hormiga_original->flota = copiar_lista_vehiculos(hormiga_mejor_solucion->flota);
    }

    // Libera la memoria de las soluciones temporales usadas en SA
    liberar_memoria_hormiga(hormiga_solucion_inicial, 1);
    liberar_memoria_hormiga(hormiga_solucion_vecina, 1);
    liberar_memoria_hormiga(hormiga_solucion_actual, 1);
    liberar_memoria_hormiga(hormiga_mejor_solucion, 1);
}
