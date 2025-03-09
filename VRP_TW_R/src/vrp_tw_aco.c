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
        printf("  Vehículos contados: %d/%d\n", hormigas[i].vehiculos_contados, hormigas[i].vehiculos_maximos);
        printf("  Fitness global: %.2f\n", hormigas[i].fitness_global);

        // Imprimir tabu
        imprimir_tabu(hormigas[i].tabu, vrp->num_clientes);

        // Imprimir flota de vehículos
        imprimir_flota(hormigas[i].flota);

        printf("-------------------------------------------------\n");
    }

    printf("=================================================\n");
}

void inicializar_hormiga(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga)
{
    for (int i = 0; i < ind->numHormigas; i++)
    {
        hormiga[i].id_hormiga = i;                                        // Le asiganamos el id de la hormiga que es i
        do{
            hormiga[i].tabu = asignar_memoria_arreglo_int(vrp->num_clientes); // Generamos memoria para el tabu que es el numero de clientes que tenemos

        }while(hormiga[i].tabu == NULL);

        hormiga[i].tabu_contador = 0;
        do{
            hormiga[i].probabilidades = asignar_memoria_arreglo_double(vrp->num_clientes); // Asignamos memoria para las probablidadades el cual es el numero de clientes que hay

        }while(hormiga[i].probabilidades == NULL);
        
        hormiga[i].vehiculos_contados = 0;                                             // Inicializamos el numero de vehiculos contados en 0
        hormiga[i].vehiculos_maximos = vrp->num_vehiculos;                             // Inicializamos el nuemro de vehiculos maximos con vrp->num_vehiculos
        do
            hormiga[i].flota = asignar_memoria_lista_vehiculos();
        while (hormiga[i].flota == NULL); // Asignamos memoria para la flota de la hormiga

        inserta_vehiculo_flota(&hormiga[i], vrp); // Insertamos el vehiculo y sus datos
    }
}

double calcular_tiempo_viaje(double distancia)
{
    int velocidad = 1.0;
    return distancia / velocidad;
}

bool calcular_ruta(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, struct vehiculo *vehiculo, double **instancia_visiblidad, double **instancia_feromona)
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
            double distancia_viaje = calcular_distancia(vrp, origen, destino);
            double tiempo_viaje = calcular_tiempo_viaje(distancia_viaje);
            double distancia_viaje_deposito = calcular_distancia(vrp, destino, 0);
            double tiempo_viaje_deposito = calcular_tiempo_viaje(distancia_viaje_deposito);

            // Verificación de ventanas de tiempo y restricciones
            if (vehiculo->tiempo_consumido + tiempo_viaje >= vrp->clientes[i].tiempo_inicial &&
                vehiculo->tiempo_consumido + tiempo_viaje <= vrp->clientes[i].tiempo_final &&
                vehiculo->capacidad_restante >= vrp->clientes[i].demanda &&
                vehiculo->tiempo_consumido + tiempo_viaje + vrp->clientes[i].servicio + tiempo_viaje_deposito <= vehiculo->tiempo_maximo)
            {
                double valuacion_tiempo = (vehiculo->tiempo_maximo > 0) ? (1.0 / vehiculo->tiempo_maximo) : 0.0;
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
            if (hormiga->tabu[j] == 0 && hormiga->probabilidades[j] > 0.0) // Verificamos que sea un cliente factible
            {
                prob_acumulada += hormiga->probabilidades[j] / hormiga->suma_probabilidades;
                if (aleatorio <= prob_acumulada)
                {
                    insertar_cliente_ruta(hormiga, vehiculo, &(vrp->clientes[j]));

                    // Actualizamos la capacidad restante (restando la demanda)
                    vehiculo->capacidad_restante -= vrp->clientes[j].demanda;

                    // Actualizamos el tiempo consumido
                    double distancia_viaje = calcular_distancia(vrp, origen, j);
                    double tiempo_viaje = calcular_tiempo_viaje(distancia_viaje);
                    vehiculo->tiempo_consumido += tiempo_viaje + vrp->clientes[j].servicio;


                    hormiga->tabu[j] = 1;
                    hormiga->tabu_contador++;
                    
                    respuesta_agregado = true;
                    break; // Salimos del bucle una vez que hemos agregado un cliente
                }
            }
        }
    }

    return respuesta_agregado; // Devolvemos verdadero si se agregó un cliente, falso en caso contrario
}

bool necesita_nuevo_vehiculo(struct vrp_configuracion *vrp, struct hormiga *hormiga, struct vehiculo *vehiculo)
{
    bool respuesta_vehiculo_nuevo = false;
    
    // Si no hay clientes visitados, no necesitamos un nuevo vehículo
    if (hormiga->tabu_contador == vrp->num_clientes)
        return false;  // Ya visitamos todos los clientes
        
    // Verificar si hay capacidad mínima
    double capacidad_minima_requerida = 999999.0;
    bool cliente_no_visitado = false;
    
    // Buscar el cliente con menor demanda entre los no visitados
    for (int i = 0; i < vrp->num_clientes; i++)
    {
        if (hormiga->tabu[i] == 0) {
            cliente_no_visitado = true;
            if (vrp->clientes[i].demanda < capacidad_minima_requerida)
                capacidad_minima_requerida = vrp->clientes[i].demanda;
        }
    }
    
    // Si no hay más clientes sin visitar, no necesitamos un nuevo vehículo
    if (!cliente_no_visitado)
        return false;
        
    // Si no hay capacidad para el cliente con menor demanda, necesitamos un nuevo vehículo    
    if (vehiculo->capacidad_restante < capacidad_minima_requerida)
        return true;

    // Comprobar si hay algún cliente factible para este vehículo
    int origen = vehiculo->ruta->cola->cliente;
    bool hay_cliente_factible = false;

    for (int i = 0; i < vrp->num_clientes; i++)
    {
        if (hormiga->tabu[i] == 0)
        {
            double distancia_viaje = calcular_distancia(vrp, origen, i);
            double tiempo_viaje = calcular_tiempo_viaje(distancia_viaje);
            double distancia_regreso = calcular_distancia(vrp, i, 0); // Distancia de vuelta al depósito
            double tiempo_regreso = calcular_tiempo_viaje(distancia_regreso);

            // Verificar ventana de tiempo, capacidad y tiempo máximo
            if (vehiculo->tiempo_consumido + tiempo_viaje >= vrp->clientes[i].tiempo_inicial &&
                vehiculo->tiempo_consumido + tiempo_viaje <= vrp->clientes[i].tiempo_final &&
                vehiculo->capacidad_restante >= vrp->clientes[i].demanda &&
                vehiculo->tiempo_consumido + tiempo_viaje + vrp->clientes[i].servicio + tiempo_regreso <= vehiculo->tiempo_maximo)
            {
                hay_cliente_factible = true;
                break;
            }
        }
    }

    // Si no hay ningún cliente factible para el vehículo actual
    if (!hay_cliente_factible)
        respuesta_vehiculo_nuevo = true;

    return respuesta_vehiculo_nuevo;
}

void aco(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, double **instancia_visiblidad, double **instancia_feromona)
{
    int max_intentos = 20; // Número máximo de intentos para agregar un cliente
    int intentos = 0;      // Contador de intentos
    bool cliente_agregado; // Bandera para saber si se agrego el cliente correctamente

    // Inicializar tabu_contador a 0
    hormiga->tabu_contador = 0;
    
    // Inicializar array tabu a 0 (todos los clientes no visitados)
    for (int i = 0; i < vrp->num_clientes; i++) {
        hormiga->tabu[i] = 0;
    }

    while (hormiga->tabu_contador < vrp->num_clientes)
    {
        // Intentar agregar un cliente
        struct nodo_vehiculo *vehiculo_actual = hormiga->flota->cola;
        cliente_agregado = calcular_ruta(vrp, ind, hormiga, vehiculo_actual->vehiculo, instancia_visiblidad, instancia_feromona);
        
        if (cliente_agregado)
        {
            // Reiniciar contador de intentos si se agregó un cliente exitosamente
            intentos = 0;
            
            // Imprimir información para depuración
            printf("\nSe agregó un cliente. Tabu_contador: %d/%d", 
                   hormiga->tabu_contador, vrp->num_clientes);
        }
        else
        {
            intentos++;
            
            // Verificar si se han asignado todos los clientes
            if (hormiga->tabu_contador == vrp->num_clientes) {
                printf("\nTodos los clientes han sido asignados correctamente");
                // Calcular fitness global (suma de distancias o tiempo, según objetivo)
                hormiga->fitness_global = 0.0; // Implemente su cálculo de fitness aquí
                break;
            }
            
            // Si después de varios intentos no se puede agregar un cliente,
            // verificar si necesitamos un nuevo vehículo
            if (intentos >= max_intentos || necesita_nuevo_vehiculo(vrp, hormiga, vehiculo_actual->vehiculo))
            {
                printf("\nSe necesita un nuevo vehículo (intentos = %d)", intentos);
                
                // Verificar si hay vehículos disponibles
                if (hormiga->vehiculos_contados >= hormiga->vehiculos_maximos)
                {
                    // No hay más vehículos disponibles, no podemos completar la solución
                    printf("\nNo hay más vehículos disponibles (%d/%d). Solución infactible.", 
                          hormiga->vehiculos_contados, hormiga->vehiculos_maximos);
                    hormiga->fitness_global = 999999.0; // Un valor muy alto como penalización
                    break;
                }

                // Agregar un nuevo vehículo
                inserta_vehiculo_flota(hormiga, vrp);
                printf("\nSe ha insertado un nuevo vehículo (%d/%d)", 
                       hormiga->vehiculos_contados, hormiga->vehiculos_maximos);
                intentos = 0; // Reiniciar contador de intentos
            }
        }
    }
    
    // Calcular fitness final si la solución es factible
    if (hormiga->fitness_global != 999999.0) {
        // Implementar cálculo de fitness basado en objetivos (distancia total, tiempo, etc.)
        double fitness_total = 0.0;
        struct nodo_vehiculo *actual = hormiga->flota->cabeza;
        
        while (actual != NULL) {
            fitness_total += actual->vehiculo->fitness_vehiculo;
            actual = actual->siguiente;
        }
        
        hormiga->fitness_global = fitness_total;
        printf("\nFitness global calculado: %.2f", hormiga->fitness_global);
    }
}

void vrp_tw_aco(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_visiblidad, double **instancia_feromona)
{
    ind->numHormigas = 1;
    struct hormiga *hormiga = malloc(sizeof(struct hormiga) * ind->numHormigas);

    inicializar_hormiga(vrp, ind, hormiga);

    for (int i = 0; i < ind->numHormigas; i++)
    {
        aco(vrp, ind, &hormiga[i], instancia_visiblidad, instancia_feromona);
    }

    printf("\n\n\n");
    imprimir_hormigas(hormiga, vrp, ind->numHormigas);
}
