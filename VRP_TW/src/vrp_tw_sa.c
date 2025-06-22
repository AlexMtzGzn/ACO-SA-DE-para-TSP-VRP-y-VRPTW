#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
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
    struct nodo_ruta *nodo_actual;
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
    // Validaciones iniciales
    if (!vrp || !hormiga_solucion_vecina || !hormiga_solucion_actual ||
        !hormiga_mejor_solucion || !ind || !instancia_distancias)
    {
        return;
    }

    double temperatura = ind->temperatura_inicial;
    double delta, prob;
    bool aceptado = false;
    int operador_usado = -1;

    // Ciclo de enfriamiento
    while (temperatura > ind->temperatura_final)
    {
        for (int i = 0; i < ind->numIteracionesSA; i++)
        {
            // Preparar solución vecina
            if (hormiga_solucion_vecina->flota)
                liberar_lista_vehiculos(hormiga_solucion_vecina->flota);

            hormiga_solucion_vecina->fitness_global = hormiga_solucion_actual->fitness_global;
            hormiga_solucion_vecina->vehiculos_necesarios = hormiga_solucion_actual->vehiculos_necesarios;
            hormiga_solucion_vecina->flota = copiar_lista_vehiculos(hormiga_solucion_actual->flota);

            // Validar que la copia fue exitosa
            if (!hormiga_solucion_vecina->flota)
            {
                continue;
            }

            // Selección de operador basada en tamaño de instancia
            prob = (double)rand() / RAND_MAX;
            aceptado = false;
            operador_usado = -1;

            // Determinar configuración según tamaño de instancia
            if (vrp->num_clientes <= 26)
            {
                // INSTANCIAS PEQUEÑAS
                if (hormiga_solucion_vecina->vehiculos_necesarios > 1)
                {
                    // Múltiples vehículos - priorizar operadores inter-ruta
                    if (prob < 0.25)
                    {
                        aceptado = cross_exchange(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 7;
                    }
                    else if (prob < 0.45)
                    {
                        aceptado = relocate_chain(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 8;
                    }
                    else if (prob < 0.65)
                    {
                        aceptado = swap_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 4;
                    }
                    else if (prob < 0.8)
                    {
                        aceptado = reinsercion_intra_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 5;
                    }
                    else if (prob < 0.9)
                    {
                        aceptado = opt_2_5(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 6;
                    }
                    else if (prob < 0.95)
                    {
                        aceptado = opt_2(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 1;
                    }
                    else if (prob < 0.98)
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
                    // Un solo vehículo - priorizar operadores intra-ruta
                    if (prob < 0.30)
                    {
                        aceptado = relocate_chain(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 8;
                    }
                    else if (prob < 0.55)
                    {
                        aceptado = opt_2_5(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 6;
                    }
                    else if (prob < 0.75)
                    {
                        aceptado = reinsercion_intra_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 5;
                    }
                    else if (prob < 0.90)
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
            else if (vrp->num_clientes <= 51)
            {
                // INSTANCIAS MEDIANAS
                if (hormiga_solucion_vecina->vehiculos_necesarios > 1)
                {
                    if (prob < 0.30)
                    {
                        aceptado = cross_exchange(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 7;
                    }
                    else if (prob < 0.55)
                    {
                        aceptado = relocate_chain(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 8;
                    }
                    else if (prob < 0.75)
                    {
                        aceptado = swap_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 4;
                    }
                    else if (prob < 0.87)
                    {
                        aceptado = reinsercion_intra_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 5;
                    }
                    else if (prob < 0.95)
                    {
                        aceptado = opt_2_5(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 6;
                    }
                    else if (prob < 0.98)
                    {
                        aceptado = opt_2(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 1;
                    }
                    else if (prob < 0.99)
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
                    if (prob < 0.35)
                    {
                        aceptado = relocate_chain(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 8;
                    }
                    else if (prob < 0.65)
                    {
                        aceptado = opt_2_5(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 6;
                    }
                    else if (prob < 0.85)
                    {
                        aceptado = reinsercion_intra_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 5;
                    }
                    else if (prob < 0.95)
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
            else
            {
                // INSTANCIAS GRANDES (>51 clientes)
                if (hormiga_solucion_vecina->vehiculos_necesarios > 1)
                {
                    if (prob < 0.35)
                    {
                        aceptado = relocate_chain(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 8;
                    }
                    else if (prob < 0.65)
                    {
                        aceptado = cross_exchange(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 7;
                    }
                    else if (prob < 0.85)
                    {
                        aceptado = swap_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 4;
                    }
                    else if (prob < 0.95)
                    {
                        aceptado = reinsercion_intra_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 5;
                    }
                    else if (prob < 0.98)
                    {
                        aceptado = opt_2_5(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 6;
                    }
                    else if (prob < 0.99)
                    {
                        aceptado = opt_2(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 1;
                    }
                    else if (prob < 0.995)
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
                    if (prob < 0.40)
                    {
                        aceptado = relocate_chain(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 8;
                    }
                    else if (prob < 0.75)
                    {
                        aceptado = opt_2_5(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 6;
                    }
                    else if (prob < 0.95)
                    {
                        aceptado = reinsercion_intra_inter(hormiga_solucion_vecina, vrp, instancia_distancias);
                        operador_usado = 5;
                    }
                    else if (prob < 0.98)
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

            // Si no se aceptó el movimiento, continuar
            if (!aceptado)
            {
                continue;
            }

            // Limpieza de vehículos vacíos
            bool necesita_limpieza = false;
            if (hormiga_solucion_vecina->vehiculos_necesarios > 1)
            {
                // Operadores que pueden dejar vehículos vacíos
                if (operador_usado == 4 || operador_usado == 5 ||
                    operador_usado == 7 || operador_usado == 8)
                {
                    necesita_limpieza = true;
                }

                // Limpieza periódica
                if (i % 20 == 0)
                {
                    necesita_limpieza = true;
                }
            }

            if (necesita_limpieza)
            {
                int vehiculos_anteriores = hormiga_solucion_vecina->vehiculos_necesarios;
                hormiga_solucion_vecina->vehiculos_necesarios = eliminar_vehiculos_vacios(hormiga_solucion_vecina->flota);

                // Si se eliminaron vehículos, re-evaluar
                if (vehiculos_anteriores != hormiga_solucion_vecina->vehiculos_necesarios)
                {
                    evaluaFO_SA(hormiga_solucion_vecina, vrp, instancia_distancias);
                }
            }

            // Evaluar función objetivo
            if (!necesita_limpieza || hormiga_solucion_vecina->vehiculos_necesarios ==
                                          hormiga_solucion_actual->vehiculos_necesarios)
            {
                evaluaFO_SA(hormiga_solucion_vecina, vrp, instancia_distancias);
            }

            delta = hormiga_solucion_vecina->fitness_global - hormiga_solucion_actual->fitness_global;

            // 5. Criterio de aceptación
            bool aceptar = false;
            if (delta < -1e-6)
            { // Usar epsilon para comparación de flotantes
                aceptar = true;
            }
            else if (delta > 1e-6 && temperatura > 1e-10)
            {
                double prob_aceptacion = exp(-delta / temperatura);
                if ((double)rand() / RAND_MAX < prob_aceptacion)
                {
                    aceptar = true;
                }
            }

            //  Actualizar soluciones
            if (aceptar)
            {
                // Liberar memoria de la solución actual
                if (hormiga_solucion_actual->flota)
                {
                    liberar_lista_vehiculos(hormiga_solucion_actual->flota);
                }

                // Actualizar solución actual
                hormiga_solucion_actual->fitness_global = hormiga_solucion_vecina->fitness_global;
                hormiga_solucion_actual->vehiculos_necesarios = hormiga_solucion_vecina->vehiculos_necesarios;
                hormiga_solucion_actual->flota = copiar_lista_vehiculos(hormiga_solucion_vecina->flota);

                // Verificar si es mejor solución global
                if (hormiga_solucion_vecina->fitness_global < hormiga_mejor_solucion->fitness_global - 1e-6)
                {
                    if (hormiga_mejor_solucion->flota)
                    {
                        liberar_lista_vehiculos(hormiga_mejor_solucion->flota);
                    }

                    hormiga_mejor_solucion->fitness_global = hormiga_solucion_vecina->fitness_global;
                    hormiga_mejor_solucion->vehiculos_necesarios = hormiga_solucion_vecina->vehiculos_necesarios;
                    hormiga_mejor_solucion->flota = copiar_lista_vehiculos(hormiga_solucion_vecina->flota);
                }
            }
        }

        // Reducir temperatura
        temperatura *= ind->factor_enfriamiento;

        // Protección contra temperatura muy baja
        if (temperatura < 1e-10)
        {
            break;
        }
    }

    // Limpieza final
    if (hormiga_mejor_solucion->vehiculos_necesarios > 1)
    {
        int vehiculos_antes = hormiga_mejor_solucion->vehiculos_necesarios;
        hormiga_mejor_solucion->vehiculos_necesarios = eliminar_vehiculos_vacios(hormiga_mejor_solucion->flota);

        // Si se eliminaron vehículos, re-evaluar
        if (vehiculos_antes != hormiga_mejor_solucion->vehiculos_necesarios)
        {
            evaluaFO_SA(hormiga_mejor_solucion, vrp, instancia_distancias);
        }
    }
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
