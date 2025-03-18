#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include "aed.h"
#include "configuracion_vrp_tw.h"
#include "vrp_tw_aco.h"
#include "control_memoria.h"
#include "lista_flota.h"
#include "lista_ruta.h"

// Función para imprimir la información de una ruta (lista de clientes)
void imprimir_ruta(struct lista_ruta *ruta, int vehiculo_id)
{
    if (es_vacia_lista_ruta(ruta))
    {
        printf("    - Ruta del vehículo %d: VACÍA\n", vehiculo_id);
        return;
    }

    printf("    - Ruta del vehículo %d: ", vehiculo_id);

    struct nodo_ruta *actual = ruta->cabeza;
    printf("Depósito -> ");

    while (actual != NULL)
    {
        printf("Cliente %d", actual->cliente);
        if (actual->siguiente != NULL)
        {
            printf(" -> ");
        }
        actual = actual->siguiente;
    }

    printf(" -> Depósito\n");
}

// Función para imprimir la información de un vehículo
void imprimir_vehiculo(struct vehiculo *vehiculo)
{
    printf("  + Vehículo ID: %d\n", vehiculo->id_vehiculo);
    printf("    - Capacidad máxima: %.2f\n", vehiculo->capacidad_maxima);
    printf("    - Capacidad restante: %.2f\n", vehiculo->capacidad_restante);
    printf("    - Tiempo consumido: %.2f\n", vehiculo->tiempo_consumido);
    printf("    - Tiempo máximo: %.2f\n", vehiculo->tiempo_maximo);
    printf("    - Número de clientes: %d\n", vehiculo->clientes_contados);
    printf("    - Fitness del vehículo: %.2f\n", vehiculo->fitness_vehiculo);

    // Imprimir la ruta del vehículo
    imprimir_ruta(vehiculo->ruta, vehiculo->id_vehiculo);
}

// Función para imprimir la flota de vehículos de una hormiga
void imprimir_flota(struct lista_vehiculos *flota)
{
    if (es_Vacia_Lista(flota))
    {
        printf("  Flota: VACÍA\n");
        return;
    }

    printf("  Flota:\n");

    struct nodo_vehiculo *actual = flota->cabeza;
    int contador = 1;

    while (actual != NULL)
    {
        printf("  Vehículo #%d:\n", contador);
        imprimir_vehiculo(actual->vehiculo);
        actual = actual->siguiente;
        contador++;
    }
}

// Función para imprimir la información del array de tabu
void imprimir_tabu(int *tabu, int num_clientes)
{
    printf("  Tabu: [");
    for (int i = 0; i < num_clientes; i++)
    {
        printf("%d", tabu[i]);
        if (i < num_clientes - 1)
        {
            printf(", ");
        }
    }
    printf("]\n");
}

// Función principal para imprimir toda la información de las hormigas
void imprimir_hormigas(struct hormiga *hormigas, struct vrp_configuracion *vrp, int num_hormigas)
{
    printf("=================================================\n");
    printf("INFORMACIÓN DE HORMIGAS Y SUS RUTAS\n");
    printf("=================================================\n");

    for (int i = 0; i < num_hormigas; i++)
    {
        printf("\nHORMIGA #%d (ID: %d)\n", i + 1, hormigas[i].id_hormiga);
        // printf("  Vehículos contados: %d/%d\n", hormigas[i].vehiculos_contados, hormigas[i].vehiculos_maximos);
        printf("  Fitness global: %.2f\n", hormigas[i].fitness_global);

        // Imprimir tabu
        imprimir_tabu(hormigas[i].tabu, vrp->num_clientes);

        // Imprimir flota de vehículos
        imprimir_flota(hormigas[i].flota);

        printf("-------------------------------------------------\n");
    }

    printf("=================================================\n");
}
void actualizar_feromona(struct individuo *ind, struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_feromona)
{
    // Primero, aplicamos la evaporación de la feromona en cada ruta.
    for (int i = 0; i < vrp->num_clientes; i++)
    {
        for (int j = 0; j < vrp->num_clientes; j++)
        {
            // Evaporación: Reducimos la feromona en cada ruta por un factor de rho.
            if (i != j)
                instancia_feromona[i][j] *= (1 - ind->rho);
        }
    }

    // Luego, incrementamos la feromona en las rutas recorridas por las hormigas.
    struct nodo_vehiculo *vehiculo_actual = hormiga->flota->cabeza;

    while (vehiculo_actual != NULL)
    {
        // Accedemos a la ruta del vehículo actual
        lista_ruta *ruta = vehiculo_actual->vehiculo->ruta;
        nodo_ruta *nodo_actual = ruta->cabeza;

        // Recorremos la ruta y actualizamos la feromona de las rutas entre clientes consecutivos
        while (nodo_actual != NULL && nodo_actual->siguiente != NULL)
        {
            int cliente_actual = nodo_actual->cliente;
            int cliente_siguiente = nodo_actual->siguiente->cliente;

            // Aumentamos la feromona en la ruta entre cliente_actual y cliente_siguiente.
            // Usamos el fitness del vehículo (menor fitness = mayor cantidad de feromona).
            double incremento_feromona = 1.0 / vehiculo_actual->vehiculo->fitness_vehiculo;
            instancia_feromona[cliente_actual][cliente_siguiente] += incremento_feromona;

            // Avanzamos al siguiente nodo de la ruta
            nodo_actual = nodo_actual->siguiente;
        }

        // Pasamos al siguiente vehículo en la flota
        vehiculo_actual = vehiculo_actual->siguiente;
    }
}

void calcular_fitness(struct hormiga *hormiga, double **instancia_distancias)
{
    hormiga->fitness_global = 0.0;

    // Recorremos cada vehículo en la flota de la hormiga
    struct nodo_vehiculo *vehiculo_actual = hormiga->flota->cabeza;

    while (vehiculo_actual != NULL)
    {
        double fitness_vehiculo = 0.0;

        // Accedemos a la ruta del vehículo actual
        lista_ruta *ruta = vehiculo_actual->vehiculo->ruta; // Accedemos a la ruta del vehículo
        nodo_ruta *nodo_actual = ruta->cabeza;              // Empezamos desde el primer cliente de la ruta

        // Recorremos la ruta del vehículo sumando las distancias entre clientes consecutivos
        while (nodo_actual != NULL && nodo_actual->siguiente != NULL)
        {
            int cliente_actual = nodo_actual->cliente;
            int cliente_siguiente = nodo_actual->siguiente->cliente;

            // Sumamos la distancia entre el cliente actual y el siguiente
            fitness_vehiculo += instancia_distancias[cliente_actual][cliente_siguiente];

            // Avanzamos al siguiente nodo en la ruta
            nodo_actual = nodo_actual->siguiente;
        }

        // Actualizamos el fitness del vehículo (si tienes alguna fórmula adicional aquí, también se puede agregar)
        vehiculo_actual->vehiculo->fitness_vehiculo = fitness_vehiculo;

        // Sumamos el fitness del vehículo al fitness global de la hormiga
        hormiga->fitness_global += fitness_vehiculo;

        // Pasamos al siguiente vehículo en la flota
        vehiculo_actual = vehiculo_actual->siguiente;
    }
}

void inicializar_hormiga(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga)
{
    for (int i = 0; i < ind->numHormigas; i++)
    {
        hormiga[i].id_hormiga = i + 1;                                    // Le asiganamos el id de la hormiga que es i
        hormiga[i].tabu = asignar_memoria_arreglo_int(vrp->num_clientes); // Generamos memoria para el tabu que es el numero de clientes que tenemos
        hormiga[i].tabu_contador = 0;
        hormiga[i].posibles_clientes = asignar_memoria_arreglo_int(vrp->num_clientes);
        hormiga[i].probabilidades = asignar_memoria_arreglo_double(vrp->num_clientes); // Asignamos memoria para las probablidadades el cual es el numero de clientes que hay
        hormiga[i].vehiculos_necesarios = 0;                                           // Inicializamos el numero de vehiculos contados en 0
        hormiga[i].vehiculos_maximos = vrp->num_vehiculos;                             // Inicializamos el nuemro de vehiculos maximos con vrp->num_vehiculos
        hormiga[i].flota = asignar_memoria_lista_vehiculos();
        inserta_vehiculo_flota(&hormiga[i], vrp, hormiga->vehiculos_necesarios + 1);
        hormiga->vehiculos_necesarios++;
    }
}

double calcular_tiempo_viaje(double distancia)
{
    double velocidad = 1.0;
    return (double)distancia / velocidad;
}

void calcular_posibles_clientes(int origen, struct vehiculo *vehiculo, struct individuo *ind, struct vrp_configuracion *vrp, struct hormiga *hormiga, double **instancia_distancias)
{
    for (int i = 1; i < vrp->num_clientes; i++)
    {
        if (hormiga->tabu[i] == 0)
        {
            if (vehiculo->capacidad_restante + vrp->clientes[i].servicio <= vehiculo->capacidad_maxima)
            {
                double velocidad = 1; // Saber de donse sale
                double tiempo_viaje = instancia_distancias[origen][i] / velocidad;
                double tiempo_llegada = vehiculo->tiempo_consumido + tiempo_viaje;
                if (vrp->clientes[i].tiempo_inicial <= tiempo_llegada && tiempo_llegada + vrp->clientes[i].servicio <= vrp->clientes[i].tiempo_final)
                {
                    hormiga->posibles_clientes[i] = 1;
                    hormiga->posibles_cliente_contador++;
                }
            }
        }
    }
}
double calcular_probabilidad(int origen, int destino, struct vehiculo *vehiculo, struct individuo *ind, struct vrp_configuracion *vrp, struct hormiga *hormiga, double **instancia_feromona, double **instancia_visibilidad, double **instancia_distancias)
{
    double numerador = pow(instancia_feromona[origen][destino], ind->alpha) * pow(instancia_visibilidad[origen][destino], ind->beta) * pow((1.0 / vrp->clientes[destino].tiempo_final), ind->gamma);
    hormiga->suma_probabilidades = 0.0;

    for (int i = 0; i < vrp->num_clientes; i++)
    {
        if (i != origen && hormiga->posibles_clientes[i] == 1)
        {
            double valuacion_tiempo = 1.0 / vrp->clientes[i].tiempo_final;
            hormiga->suma_probabilidades += pow(instancia_feromona[origen][i], ind->alpha) *
                                            pow(instancia_visibilidad[origen][i], ind->beta) *
                                            pow(valuacion_tiempo, ind->gamma);
        }
    }
    return numerador / hormiga->suma_probabilidades;
}

/*bool calcular_ruta(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, struct vehiculo *vehiculo, double **instancia_visiblidad, double **instancias_distancias, double **instancia_feromona)
{
    bool cliente_agregado = false;
    struct lista_ruta *ruta = vehiculo->ruta;
    struct nodo_ruta *ultimo_cliente_ruta = ruta->cola;
    int origen = ultimo_cliente_ruta->cliente; // Seleccionamos el último elemento de la ruta del vehiculo y lo asignamos como origen

    for (int i = 0; i < vrp->num_clientes; i++)
        hormiga->probabilidades[i] = 0.0;

    for (int i = 0; i < vrp->num_clientes; i++)
    {
        if (hormiga->tabu[i] == 0)
            hormiga->probabilidades[i] = calcular_probabilidad(origen, i, vehiculo, ind, vrp, hormiga, instancia_feromona, instancia_visiblidad, instancias_distancias);
    }

    int nuevo_cliente = seleccinar_cliente(vrp, hormiga);
    if (nuevo_cliente != -1)
    {
        insertar_cliente_ruta(hormiga, vehiculo, &(vrp->clientes[nuevo_cliente]));

        double tiempo_viaje = calcular_tiempo_viaje(instancias_distancias[origen][nuevo_cliente]);
        vehiculo->tiempo_consumido += tiempo_viaje + vrp->clientes[nuevo_cliente].servicio;
        vehiculo->capacidad_restante -= vrp->clientes[nuevo_cliente].demanda;
        cliente_agregado = true;
    }

    return cliente_agregado; // Retorna true si un cliente fue agregado a la ruta, de lo contrario, false
}*/

void aco(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, double **instancia_visiblidad, double **instancia_feromona, double **instancia_distancias)
{

    struct nodo_vehiculo *flota_vehiculo = hormiga->flota->cabeza; // Seleccionamos la flota de la hormiga
    struct vehiculo *vehiculo = flota_vehiculo->vehiculo;          // Seleccionamos la cabeza de la flota de la hormiga
    struct lista_ruta *ruta;
    struct nodo_ruta *ultimo_cliente_ruta;
    int origen;

    while (hormiga->tabu_contador <= vrp->num_clientes)
    {

        while (vehiculo->tiempo_consumido <= vehiculo->tiempo_maximo) // Falta ajustra algo mas
        {
            ruta = vehiculo->ruta;
            ultimo_cliente_ruta = ruta->cola;
            origen = ultimo_cliente_ruta->cliente; // Seleccionamos el último elemento de la ruta del vehiculo y lo asignamos como origen
            for (int i = 0; i < vrp->num_clientes; i++)
                hormiga->posibles_clientes[i] = 0;
            hormiga->posibles_cliente_contador = 0;

            calcular_posibles_clientes(origen, vehiculo, ind, vrp, hormiga, instancia_distancias);

            if (hormiga->posibles_cliente_contador == 0)
                vehiculo->tiempo_consumido += 30;
        }

        if (hormiga->posibles_cliente_contador == 0)
        {
            if (hormiga->vehiculos_necesarios + 1 < hormiga->vehiculos_maximos)
            {
                // Aqui agregamos al deposito al final   Hay que revisar
                // Falta n fi para verifiar que se haya agregado el depostio de regreso en la ruta
                inserta_vehiculo_flota(hormiga, vrp, hormiga->vehiculos_necesarios + 1);
                hormiga->vehiculos_necesarios++;
                flota_vehiculo = hormiga->flota->cabeza;
                vehiculo = flota_vehiculo->vehiculo;
            }
            else
            {
                // Mensaje de que no se puede crear la ruta
                break;
            }
        }
        else
        {
            // Hay que rescatar al origen para aca
            int proximo_cliente;
            double umbral = 0; // hay que ajustalro
            if ((double)rand() / RAND_MAX < umbral)
            {
                int count_possible = 0;
                int possible_indices[vrp->num_clientes];

                for (int i = 0; i < vrp->num_clientes; i++)
                {
                    if (hormiga->posibles_clientes[i] == 1)
                    {
                        possible_indices[count_possible++] = i;
                    }
                }

                if (count_possible > 0)
                {
                    int random_idx = rand() % count_possible;
                    proximo_cliente = possible_indices[random_idx];
                }
                else
                {
                    proximo_cliente = -1;
                }
            }
            else
            {

                for (int i = 0; i < vrp->num_clientes; i++)
                {
                    if (hormiga->posibles_clientes[i] == 1)
                        hormiga->probabilidades[i] = calcular_probabilidad(origen, i, vehiculo, ind, vrp, hormiga, instancia_feromona, instancia_visiblidad, instancia_distancias);
                    else
                        hormiga->probabilidades[i] = 0.0;
                }
                double r = ((double)rand() / RAND_MAX);
                double acumulado = 0.0;
                int proximo_cliente = -1;

                for (int i = 0; i < vrp->num_clientes; i++)
                {
                    if (hormiga->posibles_clientes[i] == 1)
                    {
                        acumulado += hormiga->probabilidades[i];
                        if (r <= acumulado && proximo_cliente == -1)
                        {
                            proximo_cliente = i;
                            break;
                        }
                    }
                }
            }
        }
    }
}
/*void liberar_memoria_hormiga(struct hormiga * hormiga, struct individuo * ind)
{
    // Liberar las estructuras dinámicas dentro de cada hormiga
    for (int i = 0; i < ind->numHormigas; i++)
    {
        // Liberar la memoria de la tabla tabu
        free(hormiga[i].tabu);
        // Liberar las probabilidades si están asignadas dinámicamente
        free(hormiga[i].probabilidades);

        // Liberar la flota (si está asignada dinámicamente)
        struct nodo_vehiculo *vehiculo_actual = hormiga[i].flota->cabeza;
        while (vehiculo_actual != NULL)
        {
            // Liberar las rutas de cada vehículo
            struct nodo_ruta *nodo_actual = vehiculo_actual->vehiculo->ruta->cabeza;
            while (nodo_actual != NULL)
            {
                struct nodo_ruta *temp = nodo_actual;
                nodo_actual = nodo_actual->siguiente;
                free(temp);
            }
            free(vehiculo_actual->vehiculo->ruta);
            vehiculo_actual = vehiculo_actual->siguiente;
        }
        free(hormiga[i].flota);
    }
    // Finalmente, liberar la memoria de las hormigas
    free(hormiga);
}*/

void vrp_tw_aco(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_visiblidad, double **instancia_distancias, double **instancia_feromona)
{
    struct hormiga *hormiga = malloc(sizeof(struct hormiga) * ind->numHormigas);

    inicializar_hormiga(vrp, ind, hormiga);

    // imprimir_hormigas(hormiga, vrp, ind->numHormigas);
    for (int i = 0; i < 1 /*ind->numIteraciones*/; i++)
    { // Aqui dedemos itererar
        for (int j = 0; j < ind->numHormigas; j++)
            aco(vrp, ind, &hormiga[j], instancia_visiblidad, instancia_feromona, instancia_distancias);
        /*for (int j = 0; j < ind->numHormigas; j++)
            actualizar_feromona(ind, &hormiga[j], vrp, instancia_feromona);
        */
    }
    imprimir_hormigas(hormiga, vrp, ind->numHormigas);
    liberar_memoria_hormiga(hormiga, ind);
}
