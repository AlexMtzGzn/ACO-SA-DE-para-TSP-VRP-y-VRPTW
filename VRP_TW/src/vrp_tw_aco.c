#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include "../includes/estructuras.h"
#include "../includes/vrp_tw_aco.h"
#include "../includes/lista_flota.h"
#include "../includes/lista_ruta.h"
#include "../includes/control_memoria.h"

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
    printf("    - Capacidad acumulada: %.2f\n", vehiculo->capacidad_acumulada);
    printf("    - Tiempo consumido: %.2f\n", vehiculo->vt_actual);
    printf("    - Tiempo máximo: %.2f\n", vehiculo->vt_final);
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

void refuerzo_feromona_mejor_ruta(struct hormiga *hormiga, double **instancia_feromona, double delta)
{
    struct nodo_vehiculo *vehiculo_actual = hormiga->flota->cabeza;

    // Recorremos cada vehículo de la flota de la hormiga
    while (vehiculo_actual != NULL)
    {
        lista_ruta *ruta = vehiculo_actual->vehiculo->ruta; // Obtenemos la ruta del vehículo
        nodo_ruta *nodo_actual = ruta->cabeza;

        // Recorremos la ruta del vehículo
        while (nodo_actual != NULL && nodo_actual->siguiente != NULL)
        {
            int cliente_actual = nodo_actual->cliente;               // Cliente actual en la ruta
            int cliente_siguiente = nodo_actual->siguiente->cliente; // Cliente siguiente en la ruta

            // Si el cliente actual no es el mismo que el siguiente, actualizamos la feromona
            if (cliente_actual != cliente_siguiente)
            {
                instancia_feromona[cliente_actual][cliente_siguiente] += delta; // Aumentamos la feromona en esta arista
                instancia_feromona[cliente_siguiente][cliente_actual] += delta; // Aumentamos la feromona en la arista inversa
            }
            else // Si son el mismo cliente, no debe haber feromona (se debe eliminar la feromona)
            {
                instancia_feromona[cliente_actual][cliente_siguiente] = 0.0; // Se elimina la feromona
            }

            nodo_actual = nodo_actual->siguiente; // Avanzamos al siguiente nodo de la ruta
        }

        vehiculo_actual = vehiculo_actual->siguiente; // Avanzamos al siguiente vehículo en la flota de la hormiga
    }
}

void actualizar_feromona(struct individuo *ind, struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_feromona, double delta)
{
    // Primero, reducimos la cantidad de feromona en todas las aristas de la matriz de feromona
    for (int i = 0; i < vrp->num_clientes; i++)
        for (int j = 0; j < vrp->num_clientes; j++)
            if (i != j)                                     // Evitamos la diagonal (i == j) que representaría un cliente consigo mismo
                instancia_feromona[i][j] *= (1 - ind->rho); // Reducimos la feromona por el factor rho

    // Iteramos sobre todas las hormigas para actualizar las feromonas de acuerdo a sus rutas
    for (int i = 0; i < ind->numHormigas; i++)
    {
        struct nodo_vehiculo *vehiculo_actual = hormiga[i].flota->cabeza;

        // Recorremos cada vehículo de la flota de la hormiga
        while (vehiculo_actual != NULL)
        {
            lista_ruta *ruta = vehiculo_actual->vehiculo->ruta; // Obtenemos la ruta del vehículo
            nodo_ruta *nodo_actual = ruta->cabeza;

            // Recorremos la ruta del vehículo
            while (nodo_actual != NULL && nodo_actual->siguiente != NULL)
            {
                int cliente_actual = nodo_actual->cliente;               // Cliente actual en la ruta
                int cliente_siguiente = nodo_actual->siguiente->cliente; // Cliente siguiente en la ruta

                // Si el cliente actual no es el mismo que el siguiente, actualizamos la feromona
                if (cliente_actual != cliente_siguiente)
                {
                    instancia_feromona[cliente_actual][cliente_siguiente] += delta; // Aumentamos la feromona en esta arista
                    instancia_feromona[cliente_siguiente][cliente_actual] += delta; // Aumentamos la feromona en la arista inversa
                }
                else // Si son el mismo cliente, no debe haber feromona (se debe eliminar la feromona)
                {
                    instancia_feromona[cliente_actual][cliente_siguiente] = 0.0; // Se elimina la feromona
                }

                nodo_actual = nodo_actual->siguiente; // Avanzamos al siguiente nodo de la ruta
            }

            vehiculo_actual = vehiculo_actual->siguiente; // Avanzamos al siguiente vehículo en la flota de la hormiga
        }
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
        hormiga[i].id_hormiga = i + 1;                                                 // Asignamos el ID a la hormiga, que es i + 1
        hormiga[i].tabu = asignar_memoria_arreglo_int(vrp->num_clientes);              // Asignamos memoria para el arreglo tabu con el tamaño de clientes
        hormiga[i].tabu_contador = 0;                                                  // Inicializamos el contador de tabu en 0
        hormiga[i].posibles_clientes = asignar_memoria_arreglo_int(vrp->num_clientes); // Asignamos memoria para los posibles clientes
        hormiga[i].posibles_clientes_contador = 0;                                     // Inicializamos el contador de posibles clientes en 0
        hormiga[i].probabilidades = asignar_memoria_arreglo_double(vrp->num_clientes); // Asignamos memoria para las probabilidades de los clientes
        hormiga[i].vehiculos_necesarios = 0;                                           // Inicializamos el número de vehículos necesarios en 0
        hormiga[i].vehiculos_maximos = vrp->num_vehiculos;                             // Asignamos el número máximo de vehículos
        hormiga[i].flota = asignar_memoria_lista_vehiculos();                          // Asignamos memoria para la flota de vehículos
        inserta_vehiculo_flota(&hormiga[i], vrp, hormiga->vehiculos_necesarios + 1);   // Insertamos el primer vehículo de la hormiga
        hormiga[i].vehiculos_necesarios++;                                             // Incrementamos el número de vehículos necesarios tras agregar el primer vehículo
    }
}

void calcular_posibles_clientes(int origen, struct vehiculo *vehiculo, struct vrp_configuracion *vrp, struct hormiga *hormiga, double **instancia_distancias)
{
    // Iteramos sobre todos los clientes para verificar si pueden ser visitados
    for (int i = 1; i < vrp->num_clientes; i++) // Comenzamos en 1 porque el índice 0 es el depósito, que ya está agregado
    {
        if (hormiga->tabu[i] == 0) // Verificamos si el cliente aún no ha sido visitado
        {
            // Calculamos la distancia y tiempo de viaje desde el origen al cliente actual
            double distancia_viaje = instancia_distancias[origen][i];
            double tiempo_viaje = distancia_viaje / vehiculo->velocidad;

            // Calculamos la distancia y tiempo de regreso al depósito desde el cliente actual
            double distancia_viaje_deposito = instancia_distancias[i][0];
            double tiempo_viaje_deposito = distancia_viaje_deposito / vehiculo->velocidad;

            // Verificamos si el vehículo llega dentro de la ventana de tiempo del cliente
            if (vehiculo->vt_actual + tiempo_viaje >= vrp->clientes[i].vt_inicial &&
                vehiculo->vt_actual + tiempo_viaje <= vrp->clientes[i].vt_final)
            {
                // Verificamos si el vehículo tiene suficiente capacidad para atender al cliente
                if (vehiculo->capacidad_acumulada + vrp->clientes[i].demanda_capacidad <= vehiculo->capacidad_maxima)
                {
                    // Verificamos si el vehículo puede visitar al cliente y luego regresar al depósito antes de su límite de tiempo
                    if (vehiculo->vt_actual + tiempo_viaje + vrp->clientes[i].tiempo_servicio + tiempo_viaje_deposito <= vehiculo->vt_final)
                    {
                        // Si todas las condiciones se cumplen, marcamos al cliente como posible
                        hormiga->posibles_clientes[i] = 1;     // Marcamos al cliente como posible
                        hormiga->posibles_clientes_contador++; // Incrementamos el contador de posibles clientes
                    }
                }
            }
        }
    }
}

double calcular_probabilidad(int origen, int destino, struct individuo *ind, struct vrp_configuracion *vrp, struct hormiga *hormiga, double **instancia_feromona, double **instancia_visibilidad, double **instancia_ventanas_tiempo)
{
    // Calculamos el numerador de la fórmula de probabilidad
    // Utilizamos la feromona elevada a alpha, la visibilidad elevada a beta y las ventanas de tiempo elevadas a gamma
    double numerador = pow(instancia_feromona[origen][destino], ind->alpha) *
                       pow(instancia_visibilidad[origen][destino], ind->beta) *
                       pow(instancia_ventanas_tiempo[origen][destino], ind->gamma);

    // Inicializamos la suma de probabilidades a 0 antes de calcular el denominador
    hormiga->suma_probabilidades = 0.0;

    // Calculamos la suma de probabilidades (denominador)
    for (int i = 0; i < vrp->num_clientes; i++)
    {
        // Verificamos que el cliente no sea el origen y que sea un cliente posible
        if (i != origen && hormiga->posibles_clientes[i] == 1)
        {
            // Acumulamos la probabilidad de cada cliente posible
            hormiga->suma_probabilidades += pow(instancia_feromona[origen][i], ind->alpha) *
                                            pow(instancia_visibilidad[origen][i], ind->beta) *
                                            pow(instancia_ventanas_tiempo[origen][i], ind->gamma);
        }
    }

    // Retornamos la probabilidad de elegir el cliente destino dado el origen
    return numerador / hormiga->suma_probabilidades;
}

void aco(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, double **instancia_visibilidad, double **instancia_feromona, double **instancia_distancias, double **instancia_ventanas_tiempo)
{
    struct nodo_vehiculo *flota_vehiculo = hormiga->flota->cabeza; // Seleccionamos la flota de la hormiga
    struct vehiculo *vehiculo = flota_vehiculo->vehiculo;          // Seleccionamos la cabeza de la flota de la hormiga
    struct lista_ruta *ruta = vehiculo->ruta;                      // Declaramos el apuntador ruta
    struct nodo_ruta *ultimo_cliente_ruta;                         // Apuntador del último cliente en la ruta
    int origen;                                                    // Origen para asignar el último cliente de la ruta

    // Bucle principal: continua hasta que la hormiga haya recorrido todos los clientes
    while (hormiga->tabu_contador <= vrp->num_clientes)
    {
        // Reseteamos las probabilidades de los clientes
        for (int i = 0; i < vrp->num_clientes; i++)
            hormiga->posibles_clientes[i] = 0;
        hormiga->posibles_clientes_contador = 0; // Resetamos el contador de posibles clientes

        // Mientras no haya posibles clientes y el vehículo esté dentro de su ventana de tiempo
        while (hormiga->posibles_clientes_contador == 0 && vehiculo->vt_actual <= vehiculo->vt_final)
        {
            ruta = vehiculo->ruta;                 // Asignamos la ruta del vehículo
            ultimo_cliente_ruta = ruta->cola;      // Asignamos el último cliente en la ruta
            origen = ultimo_cliente_ruta->cliente; // Asignamos el cliente origen

            // Calculamos los posibles clientes que pueden ser visitados
            calcular_posibles_clientes(origen, vehiculo, vrp, hormiga, instancia_distancias);
            if (hormiga->posibles_clientes_contador == 0) // Si no hay clientes posibles
                vehiculo->vt_actual += 1;                 // Aumentamos el tiempo del vehículo en un minuto
        }

        if (hormiga->posibles_clientes_contador == 0)
        {
            // Si no hay clientes posibles, verificamos si podemos agregar un vehículo a la flota
            if (hormiga->vehiculos_necesarios + 1 <= hormiga->vehiculos_maximos)
            {
                if (ruta->cola->cliente != 0)                                      // Verificamos si el vehículo actual ya ha regresado al depósito
                    insertar_cliente_ruta(hormiga, vehiculo, &(vrp->clientes[0])); // Agregamos el depósito

                // Si aún no hemos alcanzado el número de vehículos necesarios, agregamos uno más
                if (hormiga->tabu_contador < vrp->num_clientes)
                {
                    inserta_vehiculo_flota(hormiga, vrp, hormiga->vehiculos_necesarios + 1); // Añadimos un nuevo vehículo a la flota
                    hormiga->vehiculos_necesarios++;                                         // Incrementamos el número de vehículos necesarios
                    flota_vehiculo = hormiga->flota->cola;                                   // Actualizamos la flota de vehículos
                    vehiculo = flota_vehiculo->vehiculo;                                     // Asignamos el nuevo vehículo
                }
                else
                {
                    break; // Si no se puede agregar un vehículo, rompemos el ciclo
                }
            }
            else
            {
                reiniciar_hormiga(hormiga, vrp); // Reiniciamos la hormiga si no se pudo generar una ruta válida
            }
        }
        else
        {
            int proximo_cliente = -1; // Inicializamos el cliente como no seleccionado

            // Reseteamos las probabilidades de los clientes
            for (int i = 0; i < vrp->num_clientes; i++)
                hormiga->probabilidades[i] = 0.0;

            // Calculamos las probabilidades de cada cliente posible
            for (int i = 0; i < vrp->num_clientes; i++)
                if (hormiga->posibles_clientes[i] == 1)
                    hormiga->probabilidades[i] = calcular_probabilidad(origen, i, ind, vrp, hormiga, instancia_feromona, instancia_visibilidad, instancia_ventanas_tiempo);

            double aleatorio_seleccion = ((double)rand() / RAND_MAX); // Número aleatorio entre 0 y 1
            double acumulador = 0.0;                                  // Acumulador de probabilidades

            // Seleccionamos el próximo cliente basado en las probabilidades acumuladas
            for (int i = 0; i < vrp->num_clientes; i++)
            {
                if (hormiga->posibles_clientes[i] == 1) // Si el cliente está disponible
                {
                    acumulador += hormiga->probabilidades[i]; // Acumulamos la probabilidad

                    if (aleatorio_seleccion <= acumulador) // Si el número aleatorio está dentro de la probabilidad acumulada
                    {
                        proximo_cliente = i; // Seleccionamos al cliente
                        break;
                    }
                }
            }

            // Si se seleccionó un cliente válido, lo agregamos a la ruta
            if (proximo_cliente != -1)
            {
                insertar_cliente_ruta(hormiga, vehiculo, &(vrp->clientes[proximo_cliente]));                                                                   // Insertamos el cliente
                vehiculo->vt_actual += (instancia_distancias[origen][proximo_cliente] / vehiculo->velocidad) + vrp->clientes[proximo_cliente].tiempo_servicio; // Actualizamos el tiempo del vehículo
                vehiculo->capacidad_acumulada += vrp->clientes[proximo_cliente].demanda_capacidad;                                                             // Actualizamos la capacidad acumulada
            }
        }
    }

    // Verificamos si el depósito fue agregado al final de la ruta
    if (ruta->cola->cliente != 0)
        insertar_cliente_ruta(hormiga, vehiculo, &(vrp->clientes[0])); // Insertamos el depósito al final
}

void vrp_tw_aco(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_visiblidad, double **instancia_distancias, double **instancia_feromona, double **instancia_ventanas_tiempo)
{

    struct hormiga *hormiga = asignar_memoria_hormigas(ind); // Agregamos memoria para el numero de hormigas
    // double delta; Vamos hacer la seunda version de la feromona y ver cual es mejor
    double delta;
    int indice = -1;

    inicializar_hormiga(vrp, ind, hormiga); // Inicializamos las hormigas

    for (int i = 0; i < ind->numIteraciones; i++)
    {
        for (int j = 0; j < ind->numHormigas; j++)
        {
            aco(vrp, ind, &hormiga[j], instancia_visiblidad, instancia_feromona, instancia_distancias, instancia_ventanas_tiempo);
            calcular_fitness(&hormiga[j], instancia_distancias);
        }

        delta = INFINITY;
        for (int j = 0; j < ind->numHormigas; j++)
            if (hormiga[j].fitness_global < delta){
                delta = hormiga[j].fitness_global;
                indice = j;
            }
                

        actualizar_feromona(ind, hormiga, vrp, instancia_feromona, delta);
        refuerzo_feromona_mejor_ruta(&hormiga[indice],instancia_feromona,delta);

        if (i < ind->numIteraciones - 1)
            for (int j = 0; j < ind->numHormigas; j++)
                reiniciar_hormiga(&hormiga[j], vrp);
    }

    // Aqui debemos recuperar la mejor hormiga
    imprimir_hormigas(hormiga, vrp, ind->numHormigas);
    liberar_memoria_hormiga(hormiga, ind);
}
