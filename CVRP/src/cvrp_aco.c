#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include "../include/estructuras.h"
#include "../include/cvrp_aco.h"
#include "../include/lista_flota.h"
#include "../include/lista_ruta.h"
#include "../include/control_memoria.h"
#include "../include/salida_datos.h"

void recuperamos_mejor_hormiga(struct individuo *ind, struct hormiga *hormiga)
{
    if (ind->hormiga == NULL)
        ind->hormiga = asignar_memoria_hormigas(1);

    // Copiamos la ID de la hormiga mejorada al individuo
    ind->hormiga->id_hormiga = hormiga->id_hormiga;

    // Copiamos el fitness global de la hormiga mejorada al individuo
    ind->fitness = hormiga->fitness_global;
    ind->hormiga->fitness_global = hormiga->fitness_global;

    // Copiamos los vehículos máximos que la hormiga puede utilizar
    ind->hormiga->vehiculos_maximos = hormiga->vehiculos_maximos;

    // Copiamos los vehículos necesarios utilizados por la hormiga
    ind->hormiga->vehiculos_necesarios = hormiga->vehiculos_necesarios;

    // Verificamos si la flota de la hormiga es nula (no se ha asignado aún)
    if (hormiga->flota == NULL)
        liberar_lista_vehiculos(ind->hormiga->flota); // Liberamos cualquier memoria previamente asignada a la flota

    // Copiamos la flota de vehículos de la hormiga al individuo
    ind->hormiga->flota = copiar_lista_vehiculos(hormiga->flota); // Se crea una copia de la flota de la hormiga para el individuo
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

void calcular_fitness(struct hormiga *hormiga, double **instancia_distancias)
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
        inserta_vehiculo_flota(&hormiga[i], vrp, hormiga->vehiculos_necesarios + 1);

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
    // Calculamos el numerador de la fórmula de probabilidad
    // La fórmula se basa en tres componentes: la cantidad de feromona, la visibilidad y las ventanas de tiempo.
    // Cada uno de estos valores se eleva a un exponente, que está definido por los parámetros alpha y beta del individuo.
    double numerador = pow(instancia_feromona[origen][destino], ind->alpha) *
                       pow(instancia_visibilidad[origen][destino], ind->beta);

    // Inicializamos la suma de probabilidades a 0 antes de calcular el denominador
    hormiga->suma_probabilidades = 0.0;

    // Calculamos la suma de probabilidades (denominador)
    // Sumamos todas las probabilidades de los posibles clientes que no sean el origen y que sean accesibles
    for (int i = 0; i < vrp->num_clientes; i++)
        // Verificamos que el cliente no sea el origen y que sea un cliente posible
        if (i != origen && hormiga->posibles_clientes[i] == 1)
            // Acumulamos la probabilidad de cada cliente posible usando los mismos términos que en el numerador
            hormiga->suma_probabilidades += pow(instancia_feromona[origen][i], ind->alpha) *
                                            pow(instancia_visibilidad[origen][i], ind->beta);

    // Protección contra la división por cero
    if (hormiga->suma_probabilidades == 0.0)
        return 0.0; // O alguna otra estrategia para manejar este caso (como devolver 1.0 o el valor predeterminado)

    // Retornamos la probabilidad de elegir el cliente destino dado el origen
    // La probabilidad es el valor del numerador dividido por la suma de probabilidades (denominador)
    double probabilidad = 0;
    probabilidad = numerador / hormiga->suma_probabilidades;
    return probabilidad;
}

void aco(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, double **instancia_feromona, double **instancia_visibilidad)
{
    // Seleccionamos la flota de la hormiga
    struct nodo_vehiculo *flota_vehiculo = hormiga->flota->cabeza;

    // Seleccionamos el primer vehículo de la flota de la hormiga
    struct vehiculo *vehiculo = flota_vehiculo->vehiculo;

    // Apuntador a la lista de rutas del vehículo
    struct lista_ruta *ruta = vehiculo->ruta;

    // Apuntador al último cliente en la ruta actual
    struct nodo_ruta *ultimo_cliente_ruta;

    int origen; // Variable para almacenar el nodo de origen en cada iteración

    // Bucle principal: continúa hasta que la hormiga haya visitado todos los clientes
    while (hormiga->tabu_contador < vrp->num_clientes)
    {
        // Reiniciamos el registro de clientes posibles a visitar
        for (int i = 0; i < vrp->num_clientes; i++)
            hormiga->posibles_clientes[i] = 0;
        hormiga->posibles_clientes_contador = 0; // Reiniciamos el contador de clientes posibles

        // Asignamos la ruta del vehículo y obtenemos el último cliente visitado
        ruta = vehiculo->ruta;
        ultimo_cliente_ruta = ruta->cola;
        origen = ultimo_cliente_ruta->cliente;

        // Calculamos qué clientes pueden ser visitados desde el nodo actual
        calcular_posibles_clientes(origen, vehiculo, vrp, hormiga);

        if (hormiga->posibles_clientes_contador == 0)
        {
            // Verificamos si es posible agregar un nuevo vehículo a la flota
            if (hormiga->vehiculos_necesarios + 1 <= hormiga->vehiculos_maximos)
            {
                // Si el vehículo aún no ha regresado al depósito, lo agregamos
                if (ruta->cola->cliente != 0)
                    insertar_cliente_ruta(hormiga, vehiculo, &(vrp->clientes[0])); // Regreso al depósito

                // Si aún quedan clientes por visitar, agregamos un nuevo vehículo
                if (hormiga->tabu_contador < vrp->num_clientes)
                {

                    inserta_vehiculo_flota(hormiga, vrp, hormiga->vehiculos_necesarios + 1); // Añadimos un nuevo vehículo
                    hormiga->vehiculos_necesarios++;                                         // Incrementamos el número de vehículos en uso
                    flota_vehiculo = hormiga->flota->cola;                                   // Apuntamos al nuevo vehículo agregado
                    vehiculo = flota_vehiculo->vehiculo;                                     // Lo seleccionamos como vehículo actual

                }else{
                    break;
                }
            }
            else{
                reiniciar_hormiga(hormiga,vrp);
                break;
            }
        }
        else
        {
            int proximo_cliente = -1; // Inicializamos la variable del siguiente cliente a visitar

            // Reiniciamos las probabilidades de cada cliente
            for (int i = 0; i < vrp->num_clientes; i++)
                hormiga->probabilidades[i] = 0.0;

            // Calculamos la probabilidad de visitar cada cliente disponible
            for (int i = 0; i < vrp->num_clientes; i++)
                if (hormiga->posibles_clientes[i] == 1)
                    hormiga->probabilidades[i] = calcular_probabilidad(origen, i, ind, vrp, hormiga, instancia_feromona, instancia_visibilidad);

            // Generamos un número aleatorio entre 0 y 1 para la selección probabilística del siguiente cliente
            double aleatorio_seleccion = ((double)rand() / RAND_MAX);
            double acumulador = 0.0; // Variable para almacenar la suma de probabilidades acumuladas

            // Seleccionamos el siguiente cliente basado en la distribución de probabilidad acumulada
            for (int i = 0; i < vrp->num_clientes; i++)
            {
                if (hormiga->posibles_clientes[i] == 1) // Si el cliente es viable
                {
                    acumulador += hormiga->probabilidades[i]; // Sumamos su probabilidad

                    if (aleatorio_seleccion <= acumulador) // Si el número aleatorio cae dentro de este rango de probabilidad
                    {
                        proximo_cliente = i; // Asignamos este cliente como el siguiente en la ruta
                        break;               // Salimos del bucle
                    }
                }
            }

            // Si se seleccionó un cliente válido, lo agregamos a la ruta del vehículo
            if (proximo_cliente != -1)
            {
                insertar_cliente_ruta(hormiga, vehiculo, &(vrp->clientes[proximo_cliente])); // Agregamos el cliente a la ruta
                // Actualizamos la carga del vehículo con la demanda del cliente visitado
                vehiculo->capacidad_acumulada += vrp->clientes[proximo_cliente].demanda_capacidad;
            }
        }
    }

    // Verificamos si el depósito fue agregado al final de la ruta
    if (ruta->cola->cliente != 0)
        insertar_cliente_ruta(hormiga, vehiculo, &(vrp->clientes[0])); // Agregamos el depósito al final
}

void cvrp_aco(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_visiblidad, double **instancia_distancias, double **instancia_feromona)
{

    // Asignamos memoria para el número de hormigas
    struct hormiga *hormiga = asignar_memoria_hormigas(ind->numHormigas);
    double delta;    // Variable para almacenar el mejor fitness de cada iteración
    int indice = -1; // Índice de la mejor hormiga en cada iteración, inicializado en -1
    // Inicializamos las hormigas con valores iniciales
    inicializar_hormiga(vrp, ind, hormiga);

    
    // Bucle principal de iteraciones del algoritmo ACO
    for (int i = 0; i < ind->numIteraciones; i++)
    {

        // Recorremos todas las hormigas para construir sus soluciones
        for (int j = 0; j < ind->numHormigas; j++)
        {
            // Generamos la ruta de la hormiga j usando el algoritmo ACO
            aco(vrp, ind, &hormiga[j], instancia_feromona, instancia_visiblidad);

            // Calculamos el fitness de la ruta generada por la hormiga j
            calcular_fitness(&hormiga[j], instancia_distancias);
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
        if (i < ind->numIteraciones - 1)
            for (int j = 0; j < ind->numHormigas; j++)
                reiniciar_hormiga(&hormiga[j], vrp);
    }

    // Guardamos la mejor hormiga encontrada en la estructura individuo
    recuperamos_mejor_hormiga(ind, &hormiga[indice]);

    // Imprimimos las hormigas
     //imprimir_hormigas(hormiga, vrp, ind);

    // Liberamos la memoria utilizada por las hormigas al final del proceso
    liberar_memoria_hormiga(hormiga, ind);
}
