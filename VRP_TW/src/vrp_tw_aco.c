#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include "../include/estructuras.h"
#include "../include/vrp_tw_aco.h"
#include "../include/vrp_tw_sa.h"
#include "../include/lista_flota.h"
#include "../include/lista_ruta.h"
#include "../include/control_memoria.h"
#include "../include/salida_datos.h"

// Recupera la mejor hormiga de un individuo y copia sus datos a la estructura del individuo
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

// Refuerza la feromona en la mejor ruta de la hormiga
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

// Actualiza la feromona en el sistema basado en las mejores hormigas encontradas
void actualizar_feromona(struct individuo *ind, struct hormiga *hormiga, struct mejores_hormigas *mejores_hormigas, struct vrp_configuracion *vrp, double **instancia_feromona, double delta)
{
    // Primero, reducimos la cantidad de feromona en todas las aristas de la matriz de feromona
    // Iteramos sobre todas las posibles combinaciones de clientes en la matriz de feromona
    for (int i = 0; i < vrp->num_clientes; i++)
        for (int j = 0; j < vrp->num_clientes; j++)
            if (i != j)                                       // Evitamos la diagonal (i == j) que representaría un cliente consigo mismo
                instancia_feromona[i][j] *= (1.0 - ind->rho); // Reducimos la feromona por el factor rho

    // Iteramos sobre todas las hormigas para actualizar las feromonas de acuerdo a sus rutas
    for (int i = 0; i < (int)ceil((double)(ind->numHormigas * ind->porcentajeHormigas)); i++)
    {
        struct nodo_vehiculo *vehiculo_actual = hormiga[mejores_hormigas[i].id - 1].flota->cabeza; // Obtenemos el primer vehículo de la flota de la hormiga

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
                    instancia_feromona[cliente_actual][cliente_siguiente] += delta;
                    instancia_feromona[cliente_siguiente][cliente_actual] += delta;
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

// Calcula el fitness de una hormiga basado en las distancias recorridas por sus vehículos
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

// Inicializa una hormiga con la configuración del VRP y asigna memoria para sus estructuras internas
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

// Calcula los posibles clientes que una hormiga puede visitar desde un origen en su ruta
void calcular_posibles_clientes(int origen, struct vehiculo *vehiculo, struct vrp_configuracion *vrp, struct hormiga *hormiga, double **instancia_distancias)
{
    // Iteramos sobre todos los clientes para verificar si pueden ser visitados
    for (int i = 1; i < vrp->num_clientes; i++) // Comenzamos en 1 porque el índice 0 es el depósito, que ya está agregado
    {
        // Verificamos si el cliente aún no ha sido visitado (tabu[i] es 0 si no ha sido visitado)
        if (hormiga->tabu[i] == 0)
        {
            // Calculamos la distancia y el tiempo de viaje desde el origen al cliente actual
            double distancia_viaje = instancia_distancias[origen][i];
            double tiempo_viaje = distancia_viaje / vehiculo->velocidad;

            // Calculamos la distancia y el tiempo de regreso al depósito desde el cliente actual
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

// Calcula la probabilidad de elegir un cliente destino dado un origen, considerando feromonas, visibilidad y ventanas de tiempo
double calcular_probabilidad(int origen, int destino, struct individuo *ind, struct vrp_configuracion *vrp, struct hormiga *hormiga, double **instancia_feromona, double **instancia_visibilidad, double **instancia_ventanas_tiempo)
{
    // Establecer un valor mínimo para evitar valores extremadamente pequeños
    double epsilon = 1e-6, feromona, visibilidad, ventana_tiempo, numerador, probabilidad;

    // Validar los valores de las matrices
    feromona = fmax(instancia_feromona[origen][destino], epsilon);
    visibilidad = fmax(instancia_visibilidad[origen][destino], epsilon);
    ventana_tiempo = fmax(instancia_ventanas_tiempo[origen][destino], epsilon);

    // Calculamos el numerador de la fórmula de probabilidad
    numerador = pow(feromona, ind->alpha) *
                pow(visibilidad, ind->beta) *
                pow(ventana_tiempo, ind->gamma);

    // Inicializamos la suma de probabilidades a 0 antes de calcular el denominador
    hormiga->suma_probabilidades = 0.0;

    // Calculamos la suma de probabilidades (denominador)
    for (int i = 0; i < vrp->num_clientes; i++)
    {
        if (i != origen && hormiga->posibles_clientes[i] == 1)
        {
            // Validar los valores antes de usarlos
            feromona = fmax(instancia_feromona[origen][i], epsilon);
            visibilidad = fmax(instancia_visibilidad[origen][i], epsilon);
            ventana_tiempo = fmax(instancia_ventanas_tiempo[origen][i], epsilon);

            // Acumulamos la probabilidad de cada cliente posible usando los mismos términos que en el numerador
            hormiga->suma_probabilidades += pow(feromona, ind->alpha) *
                                            pow(visibilidad, ind->beta) *
                                            pow(ventana_tiempo, ind->gamma);
        }
    }

    // Protección contra la división por cero
    if (hormiga->suma_probabilidades == 0.0)
        return 0.0; // O alguna otra estrategia para manejar este caso

    // Retornamos la probabilidad de elegir el cliente destino dado el origen
    probabilidad = numerador / hormiga->suma_probabilidades;
    return probabilidad;
}

void aco(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, double **instancia_visibilidad, double **instancia_feromona, double **instancia_distancias, double **instancia_ventanas_tiempo)
{
    struct nodo_vehiculo *flota_vehiculo = hormiga->flota->cabeza;
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

        // Mientras no haya clientes posibles y el vehículo aún esté dentro de su ventana de tiempo
        while (hormiga->posibles_clientes_contador == 0 && vehiculo->vt_actual < vrp->clientes[0].vt_final)
        {
            // Asignamos la ruta del vehículo y obtenemos el último cliente visitado
            ruta = vehiculo->ruta;
            ultimo_cliente_ruta = ruta->cola;
            origen = ultimo_cliente_ruta->cliente;

            // Calculamos qué clientes pueden ser visitados desde el nodo actual
            calcular_posibles_clientes(origen, vehiculo, vrp, hormiga, instancia_distancias);

            // Si no hay clientes disponibles, avanzamos el tiempo del vehículo
            if (hormiga->posibles_clientes_contador == 0)
            {
                vehiculo->vt_actual += 1; // Aumentamos el tiempo del vehículo en un minuto
                if (vehiculo->vt_actual > vehiculo->vt_final)
                    vehiculo->vt_actual = vehiculo->vt_final;
            }
        }

        // Si no hay clientes posibles después del avance de tiempo
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
                }
            }
            else
            {
                reiniciar_hormiga(hormiga, vrp);
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
                    hormiga->probabilidades[i] = calcular_probabilidad(origen, i, ind, vrp, hormiga, instancia_feromona, instancia_visibilidad, instancia_ventanas_tiempo);

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

                // Actualizamos el tiempo del vehículo sumando el tiempo de viaje y el tiempo de servicio
                vehiculo->vt_actual += (instancia_distancias[origen][proximo_cliente] / vehiculo->velocidad) + vrp->clientes[proximo_cliente].tiempo_servicio;

                // Actualizamos la carga del vehículo con la demanda del cliente visitado
                vehiculo->capacidad_acumulada += vrp->clientes[proximo_cliente].demanda_capacidad;
            }
        }
    }

    // Verificamos si el depósito fue agregado al final de la ruta
    if (ruta->cola->cliente != 0)
        insertar_cliente_ruta(hormiga, vehiculo, &(vrp->clientes[0])); // Agregamos el depósito al final
}

// Ordena las hormigas según su fitness usando el algoritmo Quicksort
void quicksort_mejores(struct mejores_hormigas *mejores_hormigas, int bajo, int alto)
{
    if (bajo < alto) // Caso base: si el rango es válido
    {
        // Elegimos el pivote como el fitness del último elemento en el rango
        double pivote = mejores_hormigas[alto].fitness;
        int i = bajo - 1; // Índice para el elemento más pequeño

        // Recorremos desde 'bajo' hasta 'alto-1'
        for (int j = bajo; j < alto; j++)
        {
            // Si el fitness del elemento actual es menor o igual al pivote
            if (mejores_hormigas[j].fitness <= pivote)
            {
                i++; // Incrementamos el índice de elementos menores
                // Intercambiamos el elemento actual con el elemento en la posición i
                struct mejores_hormigas temp = mejores_hormigas[i];
                mejores_hormigas[i] = mejores_hormigas[j];
                mejores_hormigas[j] = temp; 
            }
        }

        // Colocamos el pivote en su posición correcta, justo después de los menores
        struct mejores_hormigas temp = mejores_hormigas[i + 1];
        mejores_hormigas[i + 1] = mejores_hormigas[alto];
        mejores_hormigas[alto] = temp;

        int pi = i + 1; // Índice de partición, donde está el pivote ahora

        // Recursivamente ordenamos la sublista izquierda al pivote
        quicksort_mejores(mejores_hormigas, bajo, pi - 1);

        // Recursivamente ordenamos la sublista derecha al pivote
        quicksort_mejores(mejores_hormigas, pi + 1, alto);
    }
}

// Implementa el algoritmo ACO para resolver el problema VRP con ventanas de tiempo
void vrp_tw_aco(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_visiblidad, double **instancia_distancias, double **instancia_feromona, double **instancia_ventanas_tiempo)
{
    // Asignamos memoria para el número de hormigas
    struct hormiga *hormiga = asignar_memoria_hormigas(ind->numHormigas);
    struct hormiga *hormiga_chismosa = asignar_memoria_hormigas(1);
    hormiga_chismosa->fitness_global = INFINITY; // Inicializamos fitness alto para la mejor solución
    struct mejores_hormigas *mejores_hormigas = asignar_memoria_mejores_hormigas(ind->numHormigas);
    double delta; // Variable para almacenar el mejor fitness inverso (para feromonas)

    // Inicializamos las hormigas con valores iniciales
    inicializar_hormiga(vrp, ind, hormiga);

    // Bucle principal de iteraciones del algoritmo ACO
    for (int i = 0; i < ind->numIteracionesACO; i++)
    {
        // Recorremos todas las hormigas para construir sus soluciones
        for (int j = 0; j < ind->numHormigas; j++)
        {
            // Generamos la ruta de la hormiga j usando el algoritmo ACO
            aco(vrp, ind, &hormiga[j], instancia_visiblidad, instancia_feromona, instancia_distancias, instancia_ventanas_tiempo);

            // Calculamos el fitness de la ruta generada por la hormiga j
            evaluaFO_ACO(&hormiga[j], instancia_distancias);
        }

        // Guardamos las hormigas con su fitness para ordenarlas
        for (int j = 0; j < ind->numHormigas; j++)
        {
            mejores_hormigas[j].id = hormiga[j].id_hormiga;
            mejores_hormigas[j].fitness = hormiga[j].fitness_global;
        }

        // Ordenamos las hormigas según fitness (menor fitness = mejor)
        quicksort_mejores(mejores_hormigas, 0, ind->numHormigas - 1);

        // Aplicamos Simulated Annealing (SA) a un porcentaje de las mejores hormigas
        for (int j = 0; j < (int)ceil((double)(ind->numHormigas * ind->porcentajeHormigas)); j++)
            vrp_tw_sa(vrp, &hormiga[mejores_hormigas[j].id - 1], ind, instancia_distancias);

        // Actualizamos la mejor solución 'hormiga_chismosa' si encontramos una mejor
        for (int j = 0; j < (int)ceil((double)(ind->numHormigas * ind->porcentajeHormigas)); j++)
        {
            if (hormiga[mejores_hormigas[j].id - 1].fitness_global < hormiga_chismosa->fitness_global)
            {
                if (hormiga_chismosa->flota)
                    liberar_memoria_hormiga(hormiga_chismosa, 1);

                hormiga_chismosa = copiar_hormiga_sa(&hormiga[mejores_hormigas[j].id - 1]);
            }
        }

        // Calculamos delta para actualizar la matriz de feromonas (inverso del fitness)
        delta = 1.0 / hormiga[mejores_hormigas[0].id - 1].fitness_global;

        // Actualizamos la matriz de feromonas con base en las soluciones generadas
        actualizar_feromona(ind, hormiga, mejores_hormigas, vrp, instancia_feromona, delta);

        // Refuerzo extra de feromona en la mejor ruta encontrada
        delta = (1.0 / hormiga_chismosa->fitness_global)*2;
        refuerzo_feromona_mejor_ruta(hormiga_chismosa, instancia_feromona, delta);

        // Si no es la última iteración, reiniciamos las hormigas para la siguiente generación
        if (i < ind->numIteracionesACO - 1)
            for (int j = 0; j < ind->numHormigas; j++)
                reiniciar_hormiga(&hormiga[j], vrp);
    }

    // Recuperamos la mejor hormiga encontrada al final
    if (hormiga_chismosa->fitness_global < hormiga[mejores_hormigas[0].id - 1].fitness_global)
        recuperamos_mejor_hormiga(ind, &hormiga[mejores_hormigas[0].id - 1]);
    else
        recuperamos_mejor_hormiga(ind, hormiga_chismosa);

    // Liberamos memoria usada para hormigas y estructuras auxiliares
    liberar_memoria_hormiga(hormiga, ind->numHormigas);
    liberar_memoria_hormiga(hormiga_chismosa, 1);
    liberar_memoria_mejores_hormigas(mejores_hormigas);
}