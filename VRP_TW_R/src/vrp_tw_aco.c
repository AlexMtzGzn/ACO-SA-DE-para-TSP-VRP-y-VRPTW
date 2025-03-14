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

void inicializar_hormiga(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, int vehiculos_necesarios)
{
    for (int i = 0; i < ind->numHormigas; i++)
    {
        hormiga[i].id_hormiga = i + 1;                                    // Le asiganamos el id de la hormiga que es i
        hormiga[i].tabu = asignar_memoria_arreglo_int(vrp->num_clientes); // Generamos memoria para el tabu que es el numero de clientes que tenemos
        hormiga[i].tabu_contador = 0;
        hormiga[i].probabilidades = asignar_memoria_arreglo_double(vrp->num_clientes); // Asignamos memoria para las probablidadades el cual es el numero de clientes que hay
        hormiga[i].vehiculos_necesarios = vehiculos_necesarios;                        // Inicializamos el numero de vehiculos contados en 0
        hormiga[i].vehiculos_maximos = vrp->num_vehiculos;                             // Inicializamos el nuemro de vehiculos maximos con vrp->num_vehiculos
        hormiga[i].flota = asignar_memoria_lista_vehiculos();

        for (int j = 0; j < hormiga->vehiculos_necesarios; j++)
            inserta_vehiculo_flota(&hormiga[i], vrp, j + 1); // Insertamos el vehiculo y sus datos
    }
}

double calcular_tiempo_viaje(double distancia)
{
    double velocidad = 1.0;
    return (double)distancia / velocidad;
}

bool calcular_ruta(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, struct vehiculo *vehiculo, double **instancia_visiblidad, double **instancias_distancias, double **instancia_feromona)
{
    bool respuesta_agregado = false;
    struct lista_ruta *ruta = vehiculo->ruta;
    struct nodo_ruta *ultimo_cliente_ruta = ruta->cola;
    int origen = ultimo_cliente_ruta->cliente; // Seleccionamos el último elemento de la ruta del vehiculo y lo asignamos como origen

    for (int i = 0; i < vrp->num_clientes; i++)
        hormiga->probabilidades[i] = 0.0;

    hormiga->suma_probabilidades = 0.0; // Inicializamos en 0.0 la suma de probabilidades

    for (int i = 0; i < vrp->num_clientes; i++)
    {
        if (hormiga->tabu[i] == 0)
        {
            int destino = i; // El destino es el índice del cliente, no el valor en tabu
            double distancia_viaje = instancias_distancias[origen][destino];
            double tiempo_viaje = calcular_tiempo_viaje(distancia_viaje);
            double distancia_viaje_deposito = instancias_distancias[destino][0];
            double tiempo_viaje_deposito = calcular_tiempo_viaje(distancia_viaje_deposito);

            // Verificación de ventanas de tiempo y restricciones
            if (vehiculo->tiempo_consumido + tiempo_viaje >= vrp->clientes[destino].tiempo_inicial &&
                vehiculo->tiempo_consumido + tiempo_viaje <= vrp->clientes[destino].tiempo_final &&
                vehiculo->capacidad_restante >= vrp->clientes[destino].demanda && // Corrección aquí
                vehiculo->tiempo_consumido + tiempo_viaje + vrp->clientes[i].servicio + tiempo_viaje_deposito <= vehiculo->tiempo_maximo)
            {
                double valuacion_tiempo;

                if (vrp->clientes[destino].tiempo_final > 0)
                    valuacion_tiempo = 1.0 / vrp->clientes[destino].tiempo_final;
                else
                    valuacion_tiempo = 0.0;

                hormiga->probabilidades[i] = pow(instancia_feromona[origen][destino], ind->alpha) *
                                             pow(instancia_visiblidad[origen][destino], ind->beta) *
                                             pow(valuacion_tiempo, ind->gamma);

                hormiga->suma_probabilidades += hormiga->probabilidades[i];
            }
        }
    }
    if (hormiga->suma_probabilidades > 0.0)
    {
        double aleatorio = (double)rand() / RAND_MAX;
        double prob_acumulada = 0.0;
        for (int j = 0; j < vrp->num_clientes; j++)
        {
            if (hormiga->tabu[j] == 0) // Verificamos que sea un cliente factible
            {
                prob_acumulada += hormiga->probabilidades[j] / hormiga->suma_probabilidades;
                if (aleatorio < prob_acumulada)
                {
                    insertar_cliente_ruta(hormiga, vehiculo, &(vrp->clientes[j]));

                    // Actualizamos la capacidad restante (restando la demanda)
                    vehiculo->capacidad_restante -= vrp->clientes[j].demanda;

                    // Actualizamos el tiempo consumido
                    double distancia_viaje = instancias_distancias[origen][j];
                    double tiempo_viaje = calcular_tiempo_viaje(distancia_viaje);
                    vehiculo->tiempo_consumido += tiempo_viaje + vrp->clientes[j].servicio;

                    respuesta_agregado = true;
                    break; // Salimos del bucle una vez que hemos agregado un cliente
                }
            }
        }
    }

    return respuesta_agregado; // Devolvemos verdadero si se agregó un cliente, falso en caso contrario
}

nodo_vehiculo *seleccion_vehiculo_aleatorio(struct hormiga *hormiga, int vehiculo_aleatorio)
{

    struct nodo_vehiculo *vehiculo_aleatorio_flota = hormiga->flota->cabeza;

    while (vehiculo_aleatorio_flota != NULL)
    {

        if (vehiculo_aleatorio_flota->vehiculo->id_vehiculo == vehiculo_aleatorio)
            return vehiculo_aleatorio_flota;
        else
        {
            vehiculo_aleatorio_flota = vehiculo_aleatorio_flota->siguiente;
        }
    }
    return vehiculo_aleatorio_flota;
}

void aco(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, double **instancia_visiblidad, double **instancia_feromona, double **instancia_distancias)
{
    int max_intentos = 100; // Número máximo de intentos para agregar un cliente
    int intentos = 0;       // Contador de intentos
    bool cliente_agregado;  // Bandera para saber si se agrego el cliente correctamente

    while (hormiga->tabu_contador < vrp->num_clientes)
    {
        int vehiculo_aleatorio = (rand() % hormiga->vehiculos_necesarios) + 1; // Seleccionamos el vehiculo aleatoriamente de acuerdo al numero de vehiuclos necesarios

        // Intentar agregar un cliente
        struct nodo_vehiculo *vehiculo_actual = seleccion_vehiculo_aleatorio(hormiga, vehiculo_aleatorio);
        cliente_agregado = calcular_ruta(vrp, ind, hormiga, vehiculo_actual->vehiculo, instancia_visiblidad, instancia_distancias, instancia_feromona);

        if (cliente_agregado)
        {
            intentos = 0;
        }
        else
        {
            intentos++;

            // Verificar si se han asignado todos los clientes
            if (hormiga->tabu_contador == vrp->num_clientes)
            {
                printf("\nTodos los clientes han sido asignados correctamente");
                break;
            }

            // Si después de varios intentos no se puede agregar un cliente,

            if (intentos == max_intentos)
            {
                // Aqui no se puedo agregar mas clientes
               // break;
            }
        }
    }
}
void liberar_memoria_hormiga(struct hormiga *hormiga, struct individuo *ind)
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
}

void vrp_tw_aco(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_visiblidad, double **instancia_distancias, double **instancia_feromona)
{
    struct hormiga *hormiga = malloc(sizeof(struct hormiga) * ind->numHormigas);
    double suma_demanda = 0;
    for (int i = 0; i < vrp->num_clientes; i++)
    {
        suma_demanda += vrp->clientes[i].demanda;
    }
    int vehiculos_necesarios = (int)ceil(suma_demanda / vrp->num_capacidad);

    inicializar_hormiga(vrp, ind, hormiga, vehiculos_necesarios);

    for (int i = 0; i < ind->numIteraciones; i++)
    { // Aqui dedemos itererar
        for (int j = 0; j < ind->numHormigas; j++)
        {
            aco(vrp, ind, &hormiga[j], instancia_visiblidad, instancia_feromona, instancia_distancias);
        }
        for (int j = 0; j < ind->numHormigas; j++)
        {
            actualizar_feromona(ind, &hormiga[j], vrp, instancia_feromona);
        }
    }

    imprimir_hormigas(hormiga, vrp, ind->numHormigas);
}
