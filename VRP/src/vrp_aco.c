#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include "../include/estructuras.h"
#include "../include/vrp_aco.h"
#include "../include/vrp_sa.h"
#include "../include/lista_flota.h"
#include "../include/lista_ruta.h"
#include "../include/control_memoria.h"
#include "../include/salida_datos.h"

void recuperamos_mejor_hormiga(struct individuo *ind, struct hormiga *hormiga)
{
    // Si ya existía una hormiga, liberar su flota primero
    if (ind->hormiga != NULL && ind->hormiga->flota != NULL)
        liberar_lista_vehiculos(ind->hormiga->flota);

    // Si la hormiga no estaba creada aún, asignarla
    if (ind->hormiga == NULL)
        ind->hormiga = asignar_memoria_hormigas(1);

    // Copiar los campos escalares
    ind->hormiga->id_hormiga = hormiga->id_hormiga;
    ind->fitness = hormiga->fitness_global;
    ind->hormiga->fitness_global = hormiga->fitness_global;
    ind->hormiga->vehiculos_maximos = hormiga->vehiculos_maximos;
    ind->hormiga->vehiculos_necesarios = hormiga->vehiculos_necesarios;

    // Copiar flota
    ind->hormiga->flota = copiar_lista_vehiculos(hormiga->flota);
}

void refuerzo_feromona_mejor_ruta(struct hormiga *hormiga, double **instancia_feromona, double delta)
{
    // Accedemos al primer vehículo de la flota de la hormiga
    struct nodo_vehiculo *vehiculo_actual = hormiga->flota->cabeza;

    // Recorremos cada vehículo de la flota de la hormiga
    while (vehiculo_actual != NULL)
    {
        lista_ruta *ruta = vehiculo_actual->vehiculo->ruta; // Obtenemos la ruta del vehículo
        nodo_ruta *nodo_actual = ruta->cabeza;              // Empezamos desde el primer cliente de la ruta

        // Recorremos la ruta del vehículo
        while (nodo_actual != NULL && nodo_actual->siguiente != NULL)
        {
            int cliente_actual = nodo_actual->cliente;               // Cliente actual en la ruta
            int cliente_siguiente = nodo_actual->siguiente->cliente; // Cliente siguiente en la ruta

            // Si el cliente actual no es el mismo que el siguiente, actualizamos la feromona
            if (cliente_actual != cliente_siguiente)
            {
                // Aumentamos la feromona en la arista entre el cliente actual y el siguiente
                instancia_feromona[cliente_actual][cliente_siguiente] += delta; // Arista directa
                instancia_feromona[cliente_siguiente][cliente_actual] += delta; // Arista inversa
            }
            else // Si el cliente actual y el siguiente son el mismo (probablemente el depósito), eliminamos la feromona
            {
                instancia_feromona[cliente_actual][cliente_siguiente] = 0.0; // Eliminamos la feromona en esta arista
            }

            nodo_actual = nodo_actual->siguiente; // Avanzamos al siguiente nodo en la ruta
        }

        vehiculo_actual = vehiculo_actual->siguiente; // Avanzamos al siguiente vehículo en la flota de la hormiga
    }
}

void actualizar_feromona(struct individuo *ind, struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_feromona, double delta)
{
    // Primero, reducimos la cantidad de feromona en todas las aristas de la matriz de feromona
    // Iteramos sobre todas las posibles combinaciones de clientes en la matriz de feromona
    for (int i = 0; i < vrp->num_clientes; i++)
        for (int j = 0; j < vrp->num_clientes; j++)
            if (i != j)                                       // Evitamos la diagonal (i == j) que representaría un cliente consigo mismo
                instancia_feromona[i][j] *= (1.0 - ind->rho); // Reducimos la feromona por el factor rho

    // Iteramos sobre todas las hormigas para actualizar las feromonas de acuerdo a sus rutas
    for (int i = 0; i < ind->numHormigas; i++)
    {
        struct nodo_vehiculo *vehiculo_actual = hormiga[i].flota->cabeza; // Obtenemos el primer vehículo de la flota de la hormiga

        // Recorremos cada vehículo de la flota de la hormiga
        while (vehiculo_actual != NULL)
        {
            lista_ruta *ruta = vehiculo_actual->vehiculo->ruta; // Obtenemos la ruta del vehículo
            nodo_ruta *nodo_actual = ruta->cabeza;              // Empezamos desde el primer cliente de la ruta

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

void evaluaFO_ACO(struct hormiga *hormiga, double **instancia_distancias)
{
    // Recorremos cada vehículo en la flota de la hormiga
    struct nodo_vehiculo *vehiculo_actual = hormiga->flota->cabeza;

    // Mientras haya vehículos en la flota
    while (vehiculo_actual != NULL)
    {
        // Inicializamos el fitness del vehículo en 0
        double fitness_vehiculo = 0.0;

        // Accedemos a la ruta del vehículo actual
        lista_ruta *ruta = vehiculo_actual->vehiculo->ruta; // Obtenemos la lista de clientes en la ruta del vehículo
        nodo_ruta *nodo_actual = ruta->cabeza;              // Empezamos desde el primer cliente de la ruta

        // Recorremos la ruta del vehículo sumando las distancias entre clientes consecutivos
        while (nodo_actual != NULL && nodo_actual->siguiente != NULL)
        {
            // Obtenemos los índices de los clientes actuales y siguientes
            int cliente_actual = nodo_actual->cliente;
            int cliente_siguiente = nodo_actual->siguiente->cliente;

            // Sumamos la distancia entre el cliente actual y el siguiente cliente en la ruta
            fitness_vehiculo += instancia_distancias[cliente_actual][cliente_siguiente];

            // Avanzamos al siguiente nodo en la ruta
            nodo_actual = nodo_actual->siguiente;
        }

        // Actualizamos el fitness del vehículo con el fitness calculado para este vehículo
        vehiculo_actual->vehiculo->fitness_vehiculo = fitness_vehiculo;

        // Sumamos el fitness del vehículo al fitness global de la hormiga
        hormiga->fitness_global += fitness_vehiculo;

        // Pasamos al siguiente vehículo en la flota
        vehiculo_actual = vehiculo_actual->siguiente;
    }
}

void inicializar_hormiga(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga)
{
    // Iteramos sobre todas las hormigas para inicializarlas
    for (int i = 0; i < ind->numHormigas; i++)
    {
        // Asignamos un ID único a cada hormiga, que es i + 1 (para que empiece desde 1)
        hormiga[i].id_hormiga = i + 1;

        // Asignamos memoria para el arreglo "tabu" que contiene los clientes que ya han sido visitados
        // Este arreglo tendrá el tamaño del número de clientes en el VRP
        hormiga[i].tabu = asignar_memoria_arreglo_int(vrp->num_clientes);

        // Inicializamos el contador de "tabu" en 0, ya que al principio no se ha visitado ningún cliente
        hormiga[i].tabu_contador = 0;

        // Asignamos memoria para el arreglo "posibles_clientes", que indica qué clientes pueden ser visitados
        hormiga[i].posibles_clientes = asignar_memoria_arreglo_int(vrp->num_clientes);

        // Inicializamos el contador de posibles clientes en 0
        hormiga[i].posibles_clientes_contador = 0;

        // Asignamos memoria para el arreglo "probabilidades", que contendrá las probabilidades de cada cliente
        hormiga[i].probabilidades = asignar_memoria_arreglo_double(vrp->num_clientes);

        // Inicializamos el número de vehículos necesarios en 0
        hormiga[i].vehiculos_necesarios = 0;

        // Asignamos el número máximo de vehículos que la hormiga puede usar
        hormiga[i].vehiculos_maximos = vrp->num_vehiculos;

        // Asignamos memoria para la flota de vehículos que la hormiga tendrá
        hormiga[i].flota = asignar_memoria_lista_vehiculos();

        // Insertamos el primer vehículo en la flota de la hormiga
        inserta_vehiculo_flota(&hormiga[i], vrp, hormiga[i].vehiculos_necesarios + 1);

        // Incrementamos el número de vehículos necesarios después de agregar el primer vehículo
        hormiga[i].vehiculos_necesarios++;
    }
}

void calcular_posibles_clientes(int origen, struct vehiculo *vehiculo, struct vrp_configuracion *vrp, struct hormiga *hormiga)
{
    // Iteramos sobre todos los clientes para verificar si pueden ser visitados
    for (int i = 1; i < vrp->num_clientes; i++) // Comenzamos en 1 porque el índice 0 es el depósito, que ya está agregado
    {
        // Verificamos si el cliente aún no ha sido visitado (tabu[i] es 0 si no ha sido visitado)
        if (hormiga->tabu[i] == 0)
        {
            // Verificamos si el vehículo tiene suficiente capacidad para atender al cliente
            if (vehiculo->capacidad_acumulada + vrp->clientes[i].demanda_capacidad <= vehiculo->capacidad_maxima)
            {
                // Si todas las condiciones se cumplen, marcamos al cliente como posible
                hormiga->posibles_clientes[i] = 1;     // Marcamos al cliente como posible
                hormiga->posibles_clientes_contador++; // Incrementamos el contador de posibles clientes
            }
        }
    }
}

double calcular_probabilidad(int origen, int destino, struct individuo *ind, struct vrp_configuracion *vrp, struct hormiga *hormiga, double **instancia_feromona, double **instancia_visibilidad)
{
    // Establecer un valor mínimo para evitar valores extremadamente pequeños
    double epsilon = 1e-6, feromona, visibilidad, numerador, probabilidad;

    // Validar valores del numerador
    feromona = fmax(instancia_feromona[origen][destino], epsilon);
    visibilidad = fmax(instancia_visibilidad[origen][destino], epsilon);

    // Calculamos el numerador de la fórmula de probabilidad
    numerador = pow(feromona, ind->alpha) * pow(visibilidad, ind->beta);

    // Inicializamos la suma de probabilidades (denominador)
    hormiga->suma_probabilidades = 0.0;

    for (int i = 0; i < vrp->num_clientes; i++)
    {
        if (i != origen && hormiga->posibles_clientes[i] == 1)
        {
            double feromona = fmax(instancia_feromona[origen][i], epsilon);
            double visibilidad = fmax(instancia_visibilidad[origen][i], epsilon);

            hormiga->suma_probabilidades += pow(feromona, ind->alpha) * pow(visibilidad, ind->beta);
        }
    }

    // Protección contra división por cero
    if (hormiga->suma_probabilidades == 0.0)
        return 0.0;

    // Retornamos la probabilidad
    probabilidad = numerador / hormiga->suma_probabilidades;
    return probabilidad;
}

void aco(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, double **instancia_feromona, double **instancia_visibilidad)
{
    // Apuntadores iniciales a la flota y vehículo actual
    struct nodo_vehiculo *flota_vehiculo = hormiga->flota->cabeza;
    struct vehiculo *vehiculo = flota_vehiculo->vehiculo;
    struct lista_ruta *ruta = vehiculo->ruta;
    struct nodo_ruta *ultimo_cliente_ruta;
    int origen;

    // Mientras queden clientes por visitar
    while (hormiga->tabu_contador < vrp->num_clientes)
    {
        // Reiniciar el vector de posibles clientes
        for (int i = 0; i < vrp->num_clientes; i++)
            hormiga->posibles_clientes[i] = 0;
        hormiga->posibles_clientes_contador = 0;

        // Obtener la ruta actual y el último cliente en ella
        ruta = vehiculo->ruta;
        ultimo_cliente_ruta = ruta->cola;
        origen = ultimo_cliente_ruta->cliente;

        // Calcular los clientes posibles a visitar desde el cliente actual (origen)
        calcular_posibles_clientes(origen, vehiculo, vrp, hormiga);

        // Si no hay clientes posibles, hay que cerrar la ruta o usar un nuevo vehículo
        if (hormiga->posibles_clientes_contador == 0)
        {
            // Verifica si puede usar otro vehículo
            if (hormiga->vehiculos_necesarios + 1 <= hormiga->vehiculos_maximos)
            {
                // Asegura cerrar la ruta actual con el depósito si no lo está
                if (ruta->cola->cliente != 0)
                    insertar_cliente_ruta(hormiga, vehiculo, &(vrp->clientes[0])); // Cliente 0 es el depósito

                // Si aún hay clientes pendientes, se añade un nuevo vehículo
                if (hormiga->tabu_contador < vrp->num_clientes)
                {
                    inserta_vehiculo_flota(hormiga, vrp, hormiga->vehiculos_necesarios + 1);
                    hormiga->vehiculos_necesarios++;
                    flota_vehiculo = hormiga->flota->cola;
                    vehiculo = flota_vehiculo->vehiculo;
                }
            }
            else
            {
                // Si no hay más vehículos disponibles, reiniciar la hormiga y empezar desde cero
                reiniciar_hormiga(hormiga, vrp);
            }
        }
        else
        {
            int proximo_cliente = -1;

            // Inicializar las probabilidades de cada cliente
            for (int i = 0; i < vrp->num_clientes; i++)
                hormiga->probabilidades[i] = 0.0;

            // Calcular las probabilidades de transición hacia cada cliente posible
            for (int i = 0; i < vrp->num_clientes; i++)
                if (hormiga->posibles_clientes[i] == 1)
                    hormiga->probabilidades[i] = calcular_probabilidad(
                        origen, i, ind, vrp, hormiga, instancia_feromona, instancia_visibilidad);

            // Selección estocástica del siguiente cliente usando ruleta
            double aleatorio_seleccion = ((double)rand() / RAND_MAX);
            double acumulador = 0.0;

            for (int i = 0; i < vrp->num_clientes; i++)
            {
                if (hormiga->posibles_clientes[i] == 1)
                {
                    acumulador += hormiga->probabilidades[i];

                    if (aleatorio_seleccion <= acumulador)
                    {
                        proximo_cliente = i;
                        break;
                    }
                }
            }

            // Si se seleccionó un cliente, se inserta en la ruta
            if (proximo_cliente != -1)
            {
                insertar_cliente_ruta(hormiga, vehiculo, &(vrp->clientes[proximo_cliente]));
                vehiculo->capacidad_acumulada += vrp->clientes[proximo_cliente].demanda_capacidad;
            }
        }
    }

    // Cierra la última ruta añadiendo el depósito si no lo tiene
    if (ruta->cola->cliente != 0)
        insertar_cliente_ruta(hormiga, vehiculo, &(vrp->clientes[0]));
}


void vrp_aco(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_visiblidad, double **instancia_distancias, double **instancia_feromona)
{
    // Asignamos memoria para el número de hormigas
    struct hormiga *hormiga = asignar_memoria_hormigas(ind->numHormigas);
    double delta;    // Variable para almacenar el mejor fitness de cada iteración
    int indice = -1; // Índice de la mejor hormiga en cada iteración, inicializado en -1
    // Inicializamos las hormigas con valores iniciales
    inicializar_hormiga(vrp, ind, hormiga);

    // Bucle principal de iteraciones del algoritmo ACO
    for (int i = 0; i < ind->numIteracionesACO; i++)
    {

        // Recorremos todas las hormigas para construir sus soluciones
        for (int j = 0; j < ind->numHormigas; j++)
        {
            // Generamos la ruta de la hormiga j usando el algoritmo ACO
            aco(vrp, ind, &hormiga[j], instancia_feromona, instancia_visiblidad);

            // Calculamos el fitness de la ruta generada por la hormiga j
            evaluaFO_ACO(&hormiga[j], instancia_distancias);
        }

        // Buscamos la hormiga con el mejor fitness en esta iteración
        delta = INFINITY;
        for (int j = 0; j < ind->numHormigas; j++)
        {
            if (hormiga[j].fitness_global < delta)
            {
                delta = 1.0 / hormiga[j].fitness_global;
                indice = j; // Guardamos el índice de la mejor hormiga
            }
        }

        // Actualizamos la matriz de feromonas con base en las soluciones generadas
        actualizar_feromona(ind, hormiga, vrp, instancia_feromona, delta);

        // Aplicamos un refuerzo de feromonas a la mejor ruta de la iteración
        refuerzo_feromona_mejor_ruta(&hormiga[indice], instancia_feromona, delta);

        // Si no es la última iteración, reiniciamos las hormigas para la siguiente generación
        if (i < ind->numIteracionesACO - 1)
            for (int j = 0; j < ind->numHormigas; j++)
                reiniciar_hormiga(&hormiga[j], vrp);
    }

    // Guardamos la mejor hormiga encontrada en la estructura individuo
    recuperamos_mejor_hormiga(ind, &hormiga[indice]);
    vrp_sa(vrp, ind, instancia_distancias);

    // Imprimimos las hormigas
    // imprimir_hormigas(hormiga, vrp, ind);

    // Liberamos la memoria utilizada por las hormigas al final del proceso
    liberar_memoria_hormiga(hormiga, ind);
}
