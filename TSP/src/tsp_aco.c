#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include "../include/estructuras.h"
#include "../include/tsp_aco.h"
#include "../include/tsp_sa.h"
#include "../include/lista_ruta.h"
#include "../include/control_memoria.h"
#include "../include/salida_datos.h"

// Función para liberar la memoria de una hormiga
void recuperamos_mejor_hormiga(struct individuo *ind, struct hormiga *hormiga)
{
    // Verificamos que la hormiga del individuo esté inicializada
    if (ind->hormiga != NULL && ind->hormiga->ruta != NULL)
        liberar_lista_ruta(ind->hormiga->ruta); // Libera completamente la ruta anterior

    // Si la hormiga no estaba creada aún, asignarla
    if (ind->hormiga == NULL)
        ind->hormiga = asignar_memoria_hormigas(1); // Asegúrate de inicializar su ruta como NULL

    // Copiamos los datos simples
    ind->hormiga->id_hormiga = hormiga->id_hormiga;
    ind->fitness = hormiga->fitness_global;
    ind->hormiga->fitness_global = hormiga->fitness_global;

    // Copiamos la ruta
    ind->hormiga->ruta = copiar_lista_ruta(hormiga->ruta);
}

// Función para reforzar la feromona en la mejor ruta de una hormiga
void refuerzo_feromona_mejor_ruta(struct hormiga *hormiga, double **instancia_feromona, double delta)
{
    // Accedemos a la ruta de la hormiga
    nodo_ruta *nodo_actual = hormiga->ruta->cabeza;

    // Recorremos la ruta de la hormiga
    while (nodo_actual != NULL && nodo_actual->siguiente != NULL)
    {
        int cliente_actual = nodo_actual->cliente;
        int cliente_siguiente = nodo_actual->siguiente->cliente;

        if (cliente_actual != cliente_siguiente)
        {
            // Reforzamiento de feromona en ambas direcciones
            instancia_feromona[cliente_actual][cliente_siguiente] += delta;
            instancia_feromona[cliente_siguiente][cliente_actual] += delta;
        }
        else
        {
            // Si son iguales, eliminamos la feromona por seguridad (aunque no debería pasar si está bien formada la ruta)
            instancia_feromona[cliente_actual][cliente_siguiente] = 0.0;
        }

        nodo_actual = nodo_actual->siguiente;
    }
}

// Función para actualizar la feromona en la matriz de feromona
void actualizar_feromona(struct individuo *ind, struct hormiga *hormigas, struct tsp_configuracion *tsp, double **instancia_feromona, double delta)
{
    // Evaporación de feromonas
    for (int i = 0; i < tsp->num_clientes; i++)
        for (int j = 0; j < tsp->num_clientes; j++)
            if (i != j)
                instancia_feromona[i][j] *= (1.0 - ind->rho);

    // Reforzamiento de feromonas por cada hormiga
    for (int i = 0; i < ind->numHormigas; i++)
    {
        nodo_ruta *nodo_actual = hormigas[i].ruta->cabeza;

        while (nodo_actual != NULL && nodo_actual->siguiente != NULL)
        {
            int cliente_actual = nodo_actual->cliente;
            int cliente_siguiente = nodo_actual->siguiente->cliente;

            if (cliente_actual != cliente_siguiente)
            {
                instancia_feromona[cliente_actual][cliente_siguiente] += delta;
                instancia_feromona[cliente_siguiente][cliente_actual] += delta;
            }

            nodo_actual = nodo_actual->siguiente;
        }
    }
}

// Función para evaluar la función objetivo (fitness) de una hormiga
void evaluaFO_ACO(struct hormiga *hormiga, double **instancia_distancias)
{
    hormiga->fitness_global = 0.0;

    // Empezamos desde el primer nodo de la ruta
    nodo_ruta *nodo_actual = hormiga->ruta->cabeza;

    // Recorremos los nodos mientras existan al menos dos consecutivos
    while (nodo_actual != NULL && nodo_actual->siguiente != NULL)
    {
        int cliente_actual = nodo_actual->cliente;
        int cliente_siguiente = nodo_actual->siguiente->cliente;

        // Sumamos la distancia entre el cliente actual y el siguiente
        hormiga->fitness_global += instancia_distancias[cliente_actual][cliente_siguiente];

        // Avanzamos
        nodo_actual = nodo_actual->siguiente;
    }
}

// Función para inicializar las hormigas
void inicializar_hormiga(struct tsp_configuracion *tsp, struct individuo *ind, struct hormiga *hormiga)
{
    // Iteramos sobre todas las hormigas para inicializarlas
    for (int i = 0; i < ind->numHormigas; i++)
    {
        // Asignamos un ID único a cada hormiga, que es i + 1 (para que empiece desde 1)
        hormiga[i].id_hormiga = i + 1;

        // Asignamos memoria para el arreglo "tabu" que contiene los clientes que ya han sido visitados
        // Este arreglo tendrá el tamaño del número de clientes en el tsp
        hormiga[i].tabu = asignar_memoria_arreglo_int(tsp->num_clientes);

        // Inicializamos el contador de "tabu" en 0, ya que al principio no se ha visitado ningún cliente
        hormiga[i].tabu_contador = 0;

        // Asignamos memoria para el arreglo "posibles_clientes", que indica qué clientes pueden ser visitados
        hormiga[i].posibles_clientes = asignar_memoria_arreglo_int(tsp->num_clientes);

        // Inicializamos el contador de posibles clientes en 0
        hormiga[i].posibles_clientes_contador = 0;

        // Asignamos memoria para el arreglo "probabilidades", que contendrá las probabilidades de cada cliente
        hormiga[i].probabilidades = asignar_memoria_arreglo_double(tsp->num_clientes);

        // Asiganmos memoria para la ruta de la hormiga
        hormiga[i].ruta = asignar_memoria_lista_ruta();

        // Insertamos el primer cliente (depósito) en la ruta de la hormiga
        insertar_cliente_ruta(&hormiga[i], &tsp->clientes[0]);
    }
}

// Función para calcular los posibles clientes que pueden ser visitados por la hormiga
void calcular_posibles_clientes(struct tsp_configuracion *tsp, struct hormiga *hormiga)
{
    // Iteramos sobre todos los clientes para verificar si pueden ser visitados
    for (int i = 1; i < tsp->num_clientes; i++) // Comenzamos en 1 porque el índice 0 es el depósito, que ya está agregado
    {
        // Verificamos si el cliente aún no ha sido visitado (tabu[i] es 0 si no ha sido visitado)
        if (hormiga->tabu[i] == 0)
        {
            // Si todas las condiciones se cumplen, marcamos al cliente como posible
            hormiga->posibles_clientes[i] = 1;     // Marcamos al cliente como posible
            hormiga->posibles_clientes_contador++; // Incrementamos el contador de posibles clientes
        }
    }
}

// Función para calcular la probabilidad de elegir un cliente
double calcular_probabilidad(int origen, int destino, struct individuo *ind, struct tsp_configuracion *tsp, struct hormiga *hormiga, double **instancia_feromona, double **instancia_visibilidad)
{
    // Establecer un valor mínimo para evitar valores extremadamente pequeños
    double epsilon = 1e-6, feromona, visibilidad, numerador, probabilidad;

    // Validar los valores de las matrices en el numerador
    feromona = fmax(instancia_feromona[origen][destino], epsilon);
    visibilidad = fmax(instancia_visibilidad[origen][destino], epsilon);

    // Calculamos el numerador: Feromona^alpha * Visibilidad^beta
    numerador = pow(feromona, ind->alpha) * pow(visibilidad, ind->beta);

    // Inicializamos la suma de probabilidades (denominador)
    hormiga->suma_probabilidades = 0.0;

    // Calculamos la suma de probabilidades
    for (int i = 0; i < tsp->num_clientes; i++)
    {
        if (i != origen && hormiga->posibles_clientes[i] == 1)
        {
            feromona = fmax(instancia_feromona[origen][i], epsilon);
            visibilidad = fmax(instancia_visibilidad[origen][i], epsilon);

            hormiga->suma_probabilidades += pow(feromona, ind->alpha) *
                                            pow(visibilidad, ind->beta);
        }
    }

    // Protección contra la división por cero
    if (hormiga->suma_probabilidades == 0.0)
        return 0.0;

    // Retornamos la probabilidad
    probabilidad = numerador / hormiga->suma_probabilidades;
    return probabilidad;
}

// Funcion para seleccionar clientes y contruir la ruta de la hormiga
void aco(struct tsp_configuracion *tsp, struct individuo *ind, struct hormiga *hormiga, double **instancia_visibilidad, double **instancia_feromona, double **instancia_distancias)
{
    struct lista_ruta *ruta = NULL;
    struct nodo_ruta *ultimo_cliente_ruta = NULL;
    int origen = 0; // Empezamos siempre en el depósito

    while (hormiga->tabu_contador < tsp->num_clientes)
    {
        // Reiniciar los posibles clientes
        for (int i = 0; i < tsp->num_clientes; i++)
            hormiga->posibles_clientes[i] = 0;
        hormiga->posibles_clientes_contador = 0;

        // Calculamos los posibles clientes desde el origen actual
        calcular_posibles_clientes(tsp, hormiga);

        // Si no hay posibles clientes, reiniciamos la hormiga
        if (hormiga->posibles_clientes_contador == 0)
        {
            reiniciar_hormiga(hormiga, ind, tsp);
            origen = 0;
            continue;
        }

        ruta = hormiga->ruta;
        ultimo_cliente_ruta = ruta->cola;
        origen = ultimo_cliente_ruta->cliente;

        int proximo_cliente = -1;

        // Reiniciamos probabilidades
        for (int i = 0; i < tsp->num_clientes; i++)
            hormiga->probabilidades[i] = 0.0;

        // Calculamos las probabilidades
        for (int i = 0; i < tsp->num_clientes; i++)
            if (hormiga->posibles_clientes[i] == 1)
                hormiga->probabilidades[i] = calcular_probabilidad(origen, i, ind, tsp, hormiga, instancia_feromona, instancia_visibilidad);

        // Selección por ruleta
        double aleatorio_seleccion = ((double)rand() / RAND_MAX);
        double acumulador = 0.0;

        for (int i = 0; i < tsp->num_clientes; i++)
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

        // Si no se seleccionó con la ruleta, elegir al azar entre los posibles
        if (proximo_cliente == -1)
        {
            int posibles[tsp->num_clientes];
            int num_posibles = 0;

            for (int i = 0; i < tsp->num_clientes; i++)
                if (hormiga->posibles_clientes[i] == 1)
                    posibles[num_posibles++] = i;

            if (num_posibles > 0)
            {
                int indice_aleatorio = rand() % num_posibles;
                proximo_cliente = posibles[indice_aleatorio];
            }
            else
            {
                //Reiniciamos la hormiga
                reiniciar_hormiga(hormiga, ind, tsp);
                origen = 0;
                continue;
            }
        }

        // Insertar el cliente en la ruta
        insertar_cliente_ruta(hormiga, &tsp->clientes[proximo_cliente]);
    }

    // Insertar el cliente 0 al final de la ruta (regreso al depósito)
    insertar_cliente_ruta(hormiga, &tsp->clientes[0]);
}

// Función principal del algoritmo ACO para resolver el TSP
void tsp_aco(struct tsp_configuracion *tsp, struct individuo *ind, double **instancia_visiblidad, double **instancia_distancias, double **instancia_feromona)
{
    // Asignamos memoria para el número de hormigas
    struct hormiga *hormiga = asignar_memoria_hormigas(ind->numHormigas);
    double delta;    // Variable para almacenar el mejor fitness de cada iteración
    int indice = -1; // Índice de la mejor hormiga en cada iteración, inicializado en -1
    // Inicializamos las hormigas con valores iniciales
    inicializar_hormiga(tsp, ind, hormiga);
    // Bucle principal de iteraciones del algoritmo ACO
    for (int i = 0; i < ind->numIteracionesACO; i++)
    {
        // Recorremos todas las hormigas para construir sus soluciones
        for (int j = 0; j < ind->numHormigas; j++)
        {
            // Generamos la ruta de la hormiga j usando el algoritmo ACO
            aco(tsp, ind, &hormiga[j], instancia_visiblidad, instancia_feromona, instancia_distancias);
            // Calculamos el fitness de la ruta generada por la hormiga j
            evaluaFO_ACO(&hormiga[j], instancia_distancias);
        }

        // Buscamos la hormiga con el mejor fitness en esta iteración
        delta = INFINITY; // Definimos el delta como infinito
        for (int j = 0; j < ind->numHormigas; j++)
        {
            if (hormiga[j].fitness_global < delta)
            {
                delta = 1.0 / hormiga[j].fitness_global; // calculamos el delta
                indice = j;                              // Guardamos el índice de la mejor hormiga
            }
        }

        // Actualizamos la matriz de feromonas con base en las soluciones generadas
        actualizar_feromona(ind, hormiga, tsp, instancia_feromona, delta);

        // Aplicamos un refuerzo de feromonas a la mejor ruta de la iteración
        refuerzo_feromona_mejor_ruta(&hormiga[indice], instancia_feromona, delta);

        // Si no es la última iteración, reiniciamos las hormigas para la siguiente generación
        if (i < ind->numIteracionesACO - 1)
            for (int j = 0; j < ind->numHormigas; j++)
                reiniciar_hormiga(&hormiga[j], ind, tsp); // Reiniciamos la hormiga i
    }

    // Guardamos la mejor hormiga encontrada en la estructura individuo
    recuperamos_mejor_hormiga(ind, &hormiga[indice]);

    // Aqui debemos mandar al recocido simulado
    tsp_sa(tsp, ind, instancia_distancias);
    // imprimir_hormigas(hormiga, tsp, ind);
    //   Liberamos la memoria utilizada por las hormigas al final del proceso
    for (int i = 0; i < ind->numHormigas; i++) // Liberamos cada hormiga
        liberar_memoria_hormiga(&hormiga[i]);
    free(hormiga); // Liberamos la memoria del arreglo de hormigas
}
