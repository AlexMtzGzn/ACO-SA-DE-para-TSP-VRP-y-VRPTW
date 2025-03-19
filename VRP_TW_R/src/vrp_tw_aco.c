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

/*// Función para imprimir la información de una ruta (lista de clientes)
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
}*/

void inicializar_hormiga(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga)
{
    for (int i = 0; i < ind->numHormigas; i++)
    {
        hormiga[i].id_hormiga = i + 1;                                                 // Le asiganamos el id de la hormiga que es i + 1
        hormiga[i].tabu = asignar_memoria_arreglo_int(vrp->num_clientes);              // Generamos memoria para el tabu que es el numero de clientes que tenemos
        hormiga[i].tabu_contador = 0;                                                  // Inicializmos en 0 el contador de tabu
        hormiga[i].posibles_clientes = asignar_memoria_arreglo_int(vrp->num_clientes); // Genemaors la mamoria para el arreglo deposibles clientes
        hormiga[i].posibles_clientes_contador = 0;                                     // Inicializamos en 0 los posibles clientes contador
        hormiga[i].probabilidades = asignar_memoria_arreglo_double(vrp->num_clientes); // Asignamos memoria para las probablidadades que se tiene al elegir el cliente
        hormiga[i].umbral = 0.5;                                                       // Definimos el umbral en 0.5
        hormiga[i].vehiculos_necesarios = 0;                                           // Inicializamos el numero de vehiculos contados en 0
        hormiga[i].vehiculos_maximos = vrp->num_vehiculos;                             // Inicializamos el numero de vehiculos maximos con vrp->num_vehiculos
        hormiga[i].flota = asignar_memoria_lista_vehiculos();                          // Asiganamos memoria para la flota de la homriga
        inserta_vehiculo_flota(&hormiga[i], vrp, hormiga->vehiculos_necesarios + 1);   // insertamos el primer vehiuculo de la hormiga
        hormiga[i].id_hormiga++;                                                       // Incrementamos el numero de vehiculos necesarios una vez que se agrego el vehiculo
    }
}

void calcular_posibles_clientes(int origen, struct vehiculo *vehiculo, struct vrp_configuracion *vrp, struct hormiga *hormiga, double **instancia_distancias)
{

    for (int i = 0; i < vrp->num_clientes; i++) // Recorremos todos los clientes
    {
        if (hormiga->tabu[i] == 0) // Verificar si el cliente no ha sido visitado
        {

            double distancia_viaje = instancia_distancias[origen][i];
            double tiempo_viaje = distancia_viaje / vehiculo->velocidad;
            double distancia_viaje_deposito = instancia_distancias[i][0];
            double tiempo_viaje_deposito = distancia_viaje_deposito / vehiculo->velocidad;

            if (vehiculo->vt_actual + tiempo_viaje >= vrp->clientes[i].vt_inicial &&
                vehiculo->vt_actual + tiempo_viaje <= vrp->clientes[i].vt_final)
            {
                // Si la capacidad_restante es espacio disponible, verifica si hay suficiente espacio
                if (vehiculo->capacidad_acumulada >= vrp->clientes[i].demanda_capacidad)
                {
                    if (vehiculo->vt_actual + tiempo_viaje + vrp->clientes[i].tiempo_servicio + tiempo_viaje_deposito <= vehiculo->vt_final)
                    {

                        hormiga->posibles_clientes[i] = 1;     // Marcar como posible cliente
                        hormiga->posibles_clientes_contador++; // Incrementar el contador
                    }
                }
            }
        }
    }
}
double calcular_probabilidad(int origen, int destino, struct individuo *ind, struct vrp_configuracion *vrp, struct hormiga *hormiga, double **instancia_feromona, double **instancia_visibilidad, double **instancia_ventanas_tiempo)
{
    double numerador = pow(instancia_feromona[origen][destino], ind->alpha) * pow(instancia_visibilidad[origen][destino], ind->beta) * pow(instancia_ventanas_tiempo[origen][destino], ind->gamma);
    hormiga->suma_probabilidades = 0.0;

    for (int i = 0; i < vrp->num_clientes; i++)
    {
        if (i != origen && hormiga->posibles_clientes[i] == 1)

            hormiga->suma_probabilidades += pow(instancia_feromona[origen][i], ind->alpha) *
                                            pow(instancia_visibilidad[origen][i], ind->beta) *
                                            pow(instancia_ventanas_tiempo[origen][i], ind->gamma);
    }
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

        for (int i = 0; i < vrp->num_vehiculos; i++) // Reseteamos el arreglo posibles clientes en 0.0 cada posicion
            hormiga->posibles_clientes[i] = 0;
        hormiga->posibles_clientes_contador = 0; // Reseteamos el contador del areglo del posibles clientes en 0.0

        while (hormiga->posibles_clientes_contador == 0 && vehiculo->vt_actual <= vehiculo->vt_final) // Este bucle es para verificar que posibles clientes no sea 0 y el timepo de vehiculo no haya excedido el tiempo
        {
            ruta = vehiculo->ruta;
            ultimo_cliente_ruta = ruta->cola;
            origen = ultimo_cliente_ruta->cliente; // Seleccionamos el último elemento de la ruta del vehiculo y lo asignamos como origen

            // Calculamos los posibles clientes
            calcular_posibles_clientes(origen, vehiculo, vrp, hormiga, instancia_distancias);
            if (hormiga->posibles_clientes_contador == 0) // Verificamos si el contador de posibles clientes es igual a 0.0
                vehiculo->vt_actual += 10;                // Si es asi entonces al tiempo del vehiuculo le sumamos 30 minutos al tiempo
        }

        if (hormiga->posibles_clientes_contador == 0) // Verificamos si contador de posibles clientes sea igual a 0.0
        {
            if (hormiga->vehiculos_necesarios + 1 <= hormiga->vehiculos_maximos) // Verificamos si al agregran un vehiuclo mas no violentamos el numero de vehiculos
            {
                if (ruta->cola->cliente != 0) // Verificamos que el vehiuclo que vamos a deja ya tenga el regreso al deposito
                    insertar_cliente_ruta(hormiga, vehiculo, &(vrp->clientes[0]));

                inserta_vehiculo_flota(hormiga, vrp, hormiga->vehiculos_necesarios + 1); // Agremos un nuevo vehiuclo a la flota
                hormiga->vehiculos_necesarios++;                                         // Incremetamos en 1 al numero de vehiculos
                flota_vehiculo = hormiga->flota->cabeza;
                vehiculo = flota_vehiculo->vehiculo; // Actualizmos el vehiculo al nuevo que agregamos
            }
            else
            {
                // Mensaje de que no se puede crear la ruta completa
                printf("\nNo se pudieron visitar a todos los clientes");
                break;
            }
        }
        else
        {
            int proximo_cliente = -1;                        // Definimos proximo cliente y le asiganmos -1 asegunaod que no hay cliente aun asiganad
            if ((double)rand() / RAND_MAX < hormiga->umbral) // Verificamos si el umbral es menos al numero aleatorio en tre 0 y 1
            {
                int contador_posible = 0;                // Declramos y definimos el contador_poisble en 0
                int posibles_indices[vrp->num_clientes]; // Declaramos y definimos el arreglo posibles_indices del tamaño del numero de clientes

                for (int i = 0; i < vrp->num_clientes; i++) // Recorremos el arreglo posibles clientes
                    if (hormiga->posibles_clientes[i] == 1) // Si la posisicon de arreglo posibles clientes es 1 lo agregamos al arreglo posibles_indices
                        posibles_indices[contador_posible++] = i;

                int random_idx = rand() % contador_posible;     // Numero aleatoiro entre los posibles numeros
                proximo_cliente = posibles_indices[random_idx]; // Asiganamos al proximo cliente de acuaerdo al numero aleatorio
            }
            else
            {

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
                        double probabilidad_acumulada = acumulador + hormiga->probabilidades[i];

                        // Verificamos si el número aleatorio cae dentro de las probabilidades acumuladas
                        if (aleatorio_seleccion <= probabilidad_acumulada && aleatorio_seleccion > acumulador)
                        {
                            proximo_cliente = i; // Seleccionamos este cliente
                            break;               // Terminamos el ciclo, ya que hemos seleccionado al cliente
                        }

                        // Actualizamos el acumulador para el siguiente cliente
                        acumulador = probabilidad_acumulada;
                    }
                }
            }

            insertar_cliente_ruta(hormiga, vehiculo, &(vrp->clientes[proximo_cliente]));                                                                   // Insertamos el cliente en la ruta del vehiculo
            vehiculo->vt_actual += (instancia_distancias[origen][proximo_cliente] / vehiculo->velocidad) + vrp->clientes[proximo_cliente].tiempo_servicio; // Sumamos el tiempo de consumo al vehiculo
            vehiculo->capacidad_acumulada += vrp->clientes[proximo_cliente].demanda_capacidad;                                                             // Restamos la demada al vehiculo
        }
    }
    if (ruta->cola->cliente != 0)
        insertar_cliente_ruta(hormiga, vehiculo, &(vrp->clientes[0]));
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

void vrp_tw_aco(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_visiblidad, double **instancia_distancias, double **instancia_feromona, double **instancia_ventanas_tiempo)
{
    struct hormiga *hormiga = malloc(sizeof(struct hormiga) * ind->numHormigas); // Asiganamos memoria para las hormigas

    inicializar_hormiga(vrp, ind, hormiga); // Inicializamos las hormigas con los datos que requiere

    // EL numero de iteracines
    for (int i = 0; i < 1 /*ind->numIteraciones*/; i++)
    { // El numero de hormigas
        for (int j = 0; j < ind->numHormigas; j++)
        {
            aco(vrp, ind, &hormiga[j], instancia_visiblidad, instancia_feromona, instancia_distancias, instancia_ventanas_tiempo);
            // calcular_fitness(&hormiga[j], instancia_distancias);
            // hormiga[j].umbral *= 0.95;
        }

        /*for (int j = 0; j < ind->numHormigas; j++)
            actualizar_feromona(ind, &hormiga[j], vrp, instancia_feromona);
        */
    }
    // imprimir_hormigas(hormiga, vrp, ind->numHormigas);
    //   liberar_memoria_hormiga(hormiga, ind);
}
