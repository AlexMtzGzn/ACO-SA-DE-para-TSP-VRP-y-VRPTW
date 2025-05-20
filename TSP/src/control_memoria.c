#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "../include/estructuras.h"
#include "../include/salida_datos.h"
#include "../include/lista_ruta.h"

/*=========================*/
/*== MEMORIA BÁSICA ======*/
/*=========================*/

// Asignar memoria para una matriz
double **asignar_memoria_instancia(int tamanio_instancia)
{
    double **instancia = (double **)calloc(tamanio_instancia, sizeof(double *));
    if (instancia == NULL)
    {
        imprimir_mensaje("Error: No se pudo asignar memoria para las filas de la instancia");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < tamanio_instancia; i++)
    {
        instancia[i] = (double *)calloc(tamanio_instancia, sizeof(double));
        if (instancia[i] == NULL)
        {
            imprimir_mensaje("Error: No se pudo asignar memoria para la columna");
            for (int j = 0; j < i; j++)
                free(instancia[j]);
            free(instancia);
            exit(EXIT_FAILURE);
        }
    }
    return instancia;
}

// Liberar memoria de una matriz
void liberar_instancia(double **instancia, int tamanio_instancia)
{
    for (int i = 0; i < tamanio_instancia; i++)
        free(instancia[i]);
    free(instancia);
}

// Asigana memoria para un arreglo de enteros
int *asignar_memoria_arreglo_int(int tamanio_arreglo)
{
    int *arreglo = (int *)calloc(tamanio_arreglo, sizeof(int)); // Inicializamos a cero
    if (arreglo == NULL)
    {
        imprimir_mensaje("Error: No se pudo asignar memoria para el arreglo entero");
        exit(EXIT_FAILURE);
    }
    return arreglo;
}

// Libera memoria de un arreglo de enteros
void liberar_memoria_arreglo_int(int *arreglo)
{
    free(arreglo);
}

// Asigana memoria para un arreglo de double
double *asignar_memoria_arreglo_double(int tamanio_arreglo)
{
    double *arreglo = (double *)calloc(tamanio_arreglo, sizeof(double)); // Inicializamos a cero
    if (arreglo == NULL)
    {
        imprimir_mensaje("No se pudo asignar memoria para el arreglo double");
        exit(EXIT_FAILURE);
    }
    return arreglo;
}

// Asigna memoria para arreglo de double
void liberar_memoria_arreglo_double(double *arreglo)
{
    free(arreglo);
}

/*=========================*/
/*== ESTRUCTURA: RANGOS ==*/
/*=========================*/

// Asiganamos memoria para la estructura rangos
struct rangos *asignar_memoria_rangos()
{
    struct rangos *rango = (struct rangos *)calloc(1, sizeof(struct rangos)); // Inicializamos a cero
    if (rango == NULL)
    {
        imprimir_mensaje("Error: No se pudo asignar memoria para los rangos");
        exit(EXIT_FAILURE);
    }
    return rango;
}

// Liberamos memoria de la estructura rangos
void liberar_rangos(struct rangos *rango)
{
    free(rango);
}

/*===============================*/
/*== ESTRUCTURA: CONFIGURACIÓN ==*/
/*===============================*/

// Asignamos memoria para la estructura tsp_configuracion
struct tsp_configuracion *asignar_memoria_tsp_configuracion()
{
    struct tsp_configuracion *tsp = (struct tsp_configuracion *)calloc(1, sizeof(struct tsp_configuracion)); // Inicializamos a cero
    if (tsp == NULL)
    {
        imprimir_mensaje("Error: No se pudo asignar memoria para la configuración tsp");
        exit(EXIT_FAILURE);
    }
    return tsp;
}

// Liberamos memoria de la estructura tsp_configuracion
void liberar_memoria_tsp_configuracion(struct tsp_configuracion *tsp)
{
    if (tsp != NULL)
    {
        free(tsp->clientes);
        tsp->clientes = NULL;
        free(tsp);
    }
}

/*===========================*/
/*== ESTRUCTURA: CLIENTES ==*/
/*===========================*/

// Asignamos memoria para la estructura cliente
struct cliente *asignar_memoria_clientes(struct tsp_configuracion *tsp)
{
    struct cliente *cliente = (struct cliente *)calloc(tsp->num_clientes, sizeof(struct cliente)); // Inicializamos a cero
    if (cliente == NULL)
    {
        imprimir_mensaje("Error: No se pudo asignar memoria para los clientes");
        exit(EXIT_FAILURE);
    }
    return cliente;
}

/*==========================*/
/*== ESTRUCTURA: HORMIGA ==*/
/*==========================*/

// Asignamos memoria para la estructura hormiga
struct hormiga *asignar_memoria_hormigas(int numHormigas)
{
    struct hormiga *hormiga = (struct hormiga *)calloc(numHormigas, sizeof(struct hormiga)); // Inicializamos a cero
    if (hormiga == NULL)
    {
        imprimir_mensaje("Error: No se pudo asignar memoria para hormigas.");
        exit(EXIT_FAILURE);
    }
    return hormiga;
}

// Reiniciamos la hormiga
void reiniciar_hormiga(struct hormiga *hormiga, struct individuo *ind, struct tsp_configuracion *tsp)
{
    for (int i = 0; i < tsp->num_clientes; i++)
    {
        hormiga->tabu[i] = 0;
        hormiga->posibles_clientes[i] = 0;
        hormiga->probabilidades[i] = 0.0;
    }

    hormiga->tabu_contador = 0;
    hormiga->posibles_clientes_contador = 0;
    hormiga->suma_probabilidades = 0.0;
    hormiga->fitness_global = 0.0;

    vaciar_lista_ruta(hormiga->ruta);
    insertar_cliente_ruta(hormiga, &tsp->clientes[0]);
}

// Liberamos memoria de la estructura hormiga
void liberar_memoria_hormiga(struct hormiga *hormiga)
{
    if(hormiga->tabu != NULL)
        liberar_memoria_arreglo_int(hormiga->tabu);
    if(hormiga->posibles_clientes != NULL)
        liberar_memoria_arreglo_int(hormiga->posibles_clientes);
    if(hormiga->probabilidades != NULL)
        liberar_memoria_arreglo_double(hormiga->probabilidades);
    if(hormiga->ruta != NULL)
        liberar_lista_ruta(hormiga->ruta);
}

/*=============================*/
/*== ESTRUCTURA: INDIVIDUOS ==*/
/*=============================*/

// Asignamos memoria para la estructura individuo
struct individuo *asignar_memoria_individuos(int poblacion)
{
    struct individuo *individuo = (struct individuo *)calloc(poblacion, sizeof(struct individuo)); // Inicializamos a cero
    if (individuo == NULL)
    {
        imprimir_mensaje("Error: No se pudo asignar memoria para los individuos");
        exit(EXIT_FAILURE);
    }
    return individuo;
}

// Liberamos memoria de la estructura individuo
void liberar_individuos(struct individuo *ind, int num_poblacion, bool tipo)
{
    if (ind == NULL)
        return;

    if (tipo)
        if(ind->hormiga != NULL)
        {
            liberar_memoria_hormiga(ind->hormiga);
            ind->hormiga = NULL;
        }

    free(ind);
}


/*=========================*/
/*== ESTRUCTURA: METAL ====*/
/*=========================*/

// Asignamos memoria para la estructura metal
struct metal *asignar_memoria_metal()
{
    struct metal *metal = (struct metal *)calloc(1, sizeof(struct metal)); // Inicializamos a cero
    if (metal == NULL)
    {
        imprimir_mensaje("Error: No se pudo asignar memoria para la estructura metal");
        exit(EXIT_FAILURE);
    }
    return metal;
}

// Liberamos memoria de la estructura metal
void liberar_memoria_metal(struct individuo *ind)
{
    if (ind->metal->solucion_inicial)
        liberar_lista_ruta(ind->metal->solucion_inicial);

    if (ind->metal->solucion_vecina)
        liberar_lista_ruta(ind->metal->solucion_vecina);

    if (ind->metal->mejor_solucion)
        liberar_lista_ruta(ind->metal->mejor_solucion);

    free(ind->metal);
}

/*==========================*/
/*== ESTRUCTURA: RUTA ======*/
/*==========================*/

// Asignamos memoria para la estructura lista_ruta
struct lista_ruta *asignar_memoria_lista_ruta()
{
    struct lista_ruta *nueva_lista = (struct lista_ruta *)calloc(1, sizeof(struct lista_ruta)); // Inicializamos a cero
    if (nueva_lista == NULL)
    {
        imprimir_mensaje("Error: No se pudo asignar memoria para la lista de rutas.");
        exit(EXIT_FAILURE);
    }

    nueva_lista->cabeza = NULL;
    nueva_lista->cola = NULL;

    return nueva_lista;
}

// Liberamos memoria de la estructura lista_ruta
struct nodo_ruta *asignar_memoria_nodo_ruta()
{
    struct nodo_ruta *nodo_nuevo = (struct nodo_ruta *)calloc(1, sizeof(struct nodo_ruta)); // Inicializamos a cero
    if (!nodo_nuevo)
    {
        imprimir_mensaje("Error: No se pudo asignar memoria para nodo_ruta");
        exit(EXIT_FAILURE);
    }
    return nodo_nuevo;
}
