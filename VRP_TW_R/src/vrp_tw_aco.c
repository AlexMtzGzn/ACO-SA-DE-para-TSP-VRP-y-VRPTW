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
    printf("    - Capacidad restante: %.2f\n", vehiculo->capacidad_acumulada);
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
void actualizar_feromona(struct individuo *ind, struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_feromona, double delta)
{
    if (hormiga->id_hormiga == 1)
    {
        for (int i = 0; i < vrp->num_clientes; i++)
            for (int j = 0; j < vrp->num_clientes; j++)
                if (i != j)
                    instancia_feromona[i][j] *= (1 - ind->rho);
    }

    struct nodo_vehiculo *vehiculo_actual = hormiga->flota->cabeza;

    while (vehiculo_actual != NULL)
    {
        lista_ruta *ruta = vehiculo_actual->vehiculo->ruta;
        nodo_ruta *nodo_actual = ruta->cabeza;

        while (nodo_actual != NULL && nodo_actual->siguiente != NULL)
        {
            int cliente_actual = nodo_actual->cliente;
            int cliente_siguiente = nodo_actual->siguiente->cliente;

            if (cliente_actual != cliente_siguiente)
            {
                instancia_feromona[cliente_actual][cliente_siguiente] += delta;
                instancia_feromona[cliente_siguiente][cliente_actual] += delta;
            }

            else
            {
                instancia_feromona[cliente_actual][cliente_siguiente] = 0.0;
            }

            nodo_actual = nodo_actual->siguiente;
        }

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
        hormiga[i].id_hormiga = i + 1;                                                 // Le asiganamos el id de la hormiga que es i + 1
        hormiga[i].tabu = asignar_memoria_arreglo_int(vrp->num_clientes);              // Generamos memoria para el tabu que es el numero de clientes que tenemos
        hormiga[i].tabu_contador = 0;                                                  // Inicializmos en 0 el contador de tabu
        hormiga[i].posibles_clientes = asignar_memoria_arreglo_int(vrp->num_clientes); // Generamos la memoria para el arreglo de posibles clientes
        hormiga[i].posibles_clientes_contador = 0;                                     // Inicializamos en 0 los posibles clientes contador
        hormiga[i].probabilidades = asignar_memoria_arreglo_double(vrp->num_clientes); // Asignamos memoria para las probablidadades que se tiene al elegir el cliente
        hormiga[i].vehiculos_necesarios = 0;                                           // Inicializamos el numero de vehiculos contados en 0
        hormiga[i].vehiculos_maximos = vrp->num_vehiculos;                             // Inicializamos el numero de vehiculos maximos con vrp->num_vehiculos
        hormiga[i].flota = asignar_memoria_lista_vehiculos();                          // Asiganamos memoria para la flota de la homriga
        inserta_vehiculo_flota(&hormiga[i], vrp, hormiga->vehiculos_necesarios + 1);   // insertamos el primer vehiuculo de la hormiga
        hormiga[i].vehiculos_necesarios++;                                             // Incrementamos el numero de vehiculos necesarios una vez que se agrego el vehiculo
    }
}

void calcular_posibles_clientes(int origen, struct vehiculo *vehiculo, struct vrp_configuracion *vrp, struct hormiga *hormiga, double **instancia_distancias)
{
    // Iteramos sobre todos los clientes para verificar si pueden ser visitados
    for (int i = 1; i < vrp->num_clientes; i++) // Comenzamos en 1 porque el índice 0 es el deposito y ya esta agregado
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
                        hormiga->posibles_clientes[i] = 1;     // Se marca como posible cliente
                        hormiga->posibles_clientes_contador++; // Se incrementa el contador de posibles clientes
                    }
                }
            }
        }
    }
}

double calcular_probabilidad(int origen, int destino, struct individuo *ind, struct vrp_configuracion *vrp, struct hormiga *hormiga, double **instancia_feromona, double **instancia_visibilidad, double **instancia_ventanas_tiempo)
{
    // Calculamos el numerador de la fórmula de probabilidad
    // Se usa la feromona elevada a alpha, la visibilidad elevada a beta y las ventanas de tiempo elevadas a gamma
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

void aco(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, double **instancia_visiblidad, double **instancia_feromona, double **instancia_distancias, double **instancia_ventanas_tiempo)
{

    struct nodo_vehiculo *flota_vehiculo = hormiga->flota->cabeza; // Seleccionamos la flota de la hormiga
    struct vehiculo *vehiculo = flota_vehiculo->vehiculo;          // Seleccionamos la cabeza de la flota de la hormiga
    struct lista_ruta *ruta;                                       // Declaramos el apuntador ruta
    struct nodo_ruta *ultimo_cliente_ruta;                         // Declaramos el apuntador del el cliente origen o eltimo cliente de la ruta
    int origen;                                                    // Declarmos al origen para asignarle el ultimo cliente de la ruta
    while (hormiga->tabu_contador <= vrp->num_clientes)            // Revisamos si el contador del tabu es menor o igual al numero del clientes, verificamos si ya se lleno por completo el tabu
    {
        for (int i = 0; i < vrp->num_clientes; i++) // Reseteamos el arreglo posibles clientes en 0 cada posicion
            hormiga->posibles_clientes[i] = 0;
        hormiga->posibles_clientes_contador = 0; // Resetamos en 0 el contador de posibles clientes

        while (hormiga->posibles_clientes_contador == 0 && vehiculo->vt_actual <= vehiculo->vt_final) // Este bucle es para verificar que posibles clientes no sea 0 y el timepo de vehiculo no haya excedido el tiempo
        {
            ruta = vehiculo->ruta;                 // Asiganamos a ruta el apuntador de la ruta del vehiculo
            ultimo_cliente_ruta = ruta->cola;      // Asignamos el apuntador de la ruta
            origen = ultimo_cliente_ruta->cliente; // Seleccionamos el último elemento de la ruta del vehiculo y lo asignamos como origen

            // Calculamos los posibles clientes
            calcular_posibles_clientes(origen, vehiculo, vrp, hormiga, instancia_distancias);
            if (hormiga->posibles_clientes_contador == 0) // Verificamos si el contador de posibles clientes es igual a 0
                vehiculo->vt_actual += 1;                 // Si es asi entonces al tiempo del vehiuculo le sumamos 1 minutos a la ventana de de tiempo actual del vehiculo
        }

        if (hormiga->posibles_clientes_contador == 0) // Verificamos si contador de posibles clientes sea igual a 0
        {
            if (hormiga->vehiculos_necesarios + 1 <= hormiga->vehiculos_maximos) // Verificamos si al agregran un vehiuclo mas no violentamos el numero de vehiculos
            {
                if (ruta->cola->cliente != 0)                                      // Verificamos que el vehiculo que vamos a deja ya tenga el regreso al deposito
                    insertar_cliente_ruta(hormiga, vehiculo, &(vrp->clientes[0])); // Agregamos el deposito

                if (hormiga->tabu_contador < vrp->num_clientes) // Verificamos que el contador del tabu es menor al numero de clientes
                {
                    inserta_vehiculo_flota(hormiga, vrp, hormiga->vehiculos_necesarios + 1); // Agremos un nuevo vehiuclo a la flota
                    hormiga->vehiculos_necesarios++;                                         // Incremetamos en 1 al numero de vehiculos
                    flota_vehiculo = hormiga->flota->cola;
                    vehiculo = flota_vehiculo->vehiculo; // Actualizmos el vehiculo al nuevo que agregamos
                }
                else
                { // Si no rompemos el bucle
                    break;
                }
            }

            else
            {
                reiniciar_hormiga(hormiga, vrp); // Reiniciamos la hormiga si no se puedo generar una ruta
            }
        }
        else
        {

            int proximo_cliente = -1; // Definimos proximo cliente y le asiganmos -1 asegunaod que no hay cliente aun asiganad

            for (int i = 0; i < vrp->num_clientes; i++) // Reseteamos las probabilidades en 0.0 cada posicion de arreglo
                hormiga->probabilidades[i] = 0.0;

            for (int i = 0; i < vrp->num_clientes; i++) // Recorremos los posibles clientes
                if (hormiga->posibles_clientes[i] == 1) // Si encontramos un posible cliente lo mandamos a calculara la probabilidad de ser elegido
                    hormiga->probabilidades[i] = calcular_probabilidad(origen, i, ind, vrp, hormiga, instancia_feromona, instancia_visiblidad, instancia_ventanas_tiempo);

            double aleatorio_seleccion = ((double)rand() / RAND_MAX); // Número aleatorio entre 0 y 1
            double acumulador = 0.0;                                  // Acumulador para las probabilidades

            for (int i = 0; i < vrp->num_clientes; i++) // Recorremos todos los clientes
            {
                if (hormiga->posibles_clientes[i] == 1) // Verificamos si el cliente está disponible
                {
                    // Acumulamos la probabilidad para el cliente actual
                    acumulador += hormiga->probabilidades[i];

                    // Verificamos si el número aleatorio cae dentro de las probabilidades acumuladas
                    if (aleatorio_seleccion <= acumulador)
                    {
                        proximo_cliente = i; // Seleccionamos este cliente
                        break;               // Terminamos el ciclo, ya que hemos seleccionado al cliente
                    }
                }
            }

            if (proximo_cliente != -1)
            {
                insertar_cliente_ruta(hormiga, vehiculo, &(vrp->clientes[proximo_cliente]));                                                                   // Insertamos el cliente en la ruta del vehiculo
                vehiculo->vt_actual += (instancia_distancias[origen][proximo_cliente] / vehiculo->velocidad) + vrp->clientes[proximo_cliente].tiempo_servicio; // Sumamos el tiempo de consumo al vehiculo
                vehiculo->capacidad_acumulada += vrp->clientes[proximo_cliente].demanda_capacidad;
            }
        }
    }

    // Verificamos si ya se agrego el deposito al final de la ruta
    if (ruta->cola->cliente != 0)
        insertar_cliente_ruta(hormiga, vehiculo, &(vrp->clientes[0]));
}

void vrp_tw_aco(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_visiblidad, double **instancia_distancias, double **instancia_feromona, double **instancia_ventanas_tiempo)
{

    struct hormiga *hormiga = asignar_memoria_hormigas(ind); // Agregamos memoria para el numero de hormigas
    double delta;
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
            if (hormiga[j].fitness_global < delta)
                delta = hormiga[j].fitness_global;

        for (int j = 0; j < ind->numHormigas; j++)
            actualizar_feromona(ind, &hormiga[j], vrp, instancia_feromona, delta);

        if (i < ind->numIteraciones - 1)
            for (int j = 0; j < ind->numHormigas; j++)
                reiniciar_hormiga(&hormiga[j], vrp);
    }

    // Aqui debemos recuperar la mejor hormiga

    imprimir_hormigas(hormiga, vrp, ind->numHormigas);
    liberar_memoria_hormiga(hormiga, ind);
}
