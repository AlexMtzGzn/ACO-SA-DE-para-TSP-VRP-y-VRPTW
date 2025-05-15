#include <stdlib.h>
#include <stdbool.h>

#include "../include/estructuras.h"
#include "../include/lista_flota.h"
#include "../include/salida_datos.h"

/*=========================*/
/*== MEMORIA BÁSICA ======*/
/*=========================*/

// Asignar memoria para una matriz
double **asignar_memoria_instancia(int tamanio_instancia)
{
    double **instancia = (double **)malloc(tamanio_instancia * sizeof(double *));
    if (instancia == NULL)
    {
        imprimir_mensaje("No se pudo asignar memoria para las filas de la instancia");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < tamanio_instancia; i++)
    {
        instancia[i] = (double *)malloc(tamanio_instancia * sizeof(double));
        if (instancia[i] == NULL)
        {
            imprimir_mensaje("No se pudo asignar memoria para la columna");
            for (int j = 0; j < i; j++)
                free(instancia[j]);
            free(instancia);
            exit(EXIT_FAILURE);
        }
    }

    return instancia;
}

//Asigana memoria para un arreglo de enteros
int *asignar_memoria_arreglo_int(int tamanio_arreglo)
{
    int *arreglo = (int *)calloc(tamanio_arreglo,sizeof(int));
    if (arreglo == NULL)
    {
        imprimir_mensaje("No se pudo asignar memoria para el arreglo entero");
        exit(EXIT_FAILURE);
    }
    return arreglo;
}

// Asigna memoria para un arreglo de tipo double
double *asignar_memoria_arreglo_double(int tamanio_arreglo)
{
    double *arreglo = (double *)calloc(tamanio_arreglo,sizeof(double));
    if (arreglo == NULL)
    {
        imprimir_mensaje("No se pudo asignar memoria para el arreglo double");
        exit(EXIT_FAILURE);
    }
    return arreglo;
}


//Libera la memoria del arreglo de enteros.
void liberar_memoria_arreglo_int(int *arreglo)
{
    free(arreglo);
}

// Libera la memoria del arreglo de double.
void liberar_memoria_arreglo_double(double *arreglo)
{
    free(arreglo);
}

// Libera la memoria de una matriz
void liberar_instancia(double **instancia, int tamanio_instancia)
{
    for (int i = 0; i < tamanio_instancia; i++)
        free(instancia[i]);
    free(instancia);
}

/*=========================*/
/*== ESTRUCTURA: RANGOS ==*/
/*=========================*/

// Asigna memoria para la estructura de rangos
struct rangos *asignar_memoria_rangos()
{
    struct rangos *rango = (struct rangos *)malloc(sizeof(struct rangos));
    if (rango == NULL)
    {
        imprimir_mensaje("No se pudo asignar memoria para los rangos");
        exit(EXIT_FAILURE);
    }
    return rango;
}

// Libera la memoria de la estructura de rangos
void liberar_rangos(struct rangos *rango)
{
    free(rango);
}

/*=============================*/
/*== ESTRUCTURA: INDIVIDUOS ==*/
/*=============================*/

// Asigna memoria para la estructura de individuos
struct individuo *asignar_memoria_individuos(int poblacion)
{
    struct individuo *individuo = (struct individuo *)calloc(poblacion,sizeof(struct individuo));
    if (individuo == NULL)
    {
        imprimir_mensaje("No se pudo asignar memoria para los individuos");
        exit(EXIT_FAILURE);
    }
    return individuo;
}

// Libera la memoria de la estructura de individuos
void liberar_individuos(struct individuo *ind, int num_poblacion, bool tipo)
{
    if (ind == NULL)
        return;

    if (tipo)
    {
        for (int i = 0; i < num_poblacion; i++)
        {
            if (ind[i].hormiga != NULL)
            {
                liberar_lista_vehiculos(ind[i].hormiga->flota);
                ind[i].hormiga->flota = NULL;

                free(ind[i].hormiga->tabu);
                free(ind[i].hormiga->probabilidades);
                free(ind[i].hormiga->posibles_clientes);

                free(ind[i].hormiga);
                ind[i].hormiga = NULL;
            }
        }
    }

    free(ind);
}



/*===============================*/
/*== ESTRUCTURA: CONFIGURACIÓN ==*/
/*===============================*/

//Libera la memoria de la estructura 'vrp_configuracion'.
struct vrp_configuracion *asignar_memoria_vrp_configuracion()
{
    struct vrp_configuracion *vrp = (struct vrp_configuracion *)malloc(sizeof(struct vrp_configuracion));
    if (vrp == NULL)
    {
        imprimir_mensaje("No se pudo asignar memoria para la configuración VRP");
        exit(EXIT_FAILURE);
    }
    return vrp;
}

//Libera la memoria de la estructura 'vrp_configuracion'.
void liberar_memoria_vrp_configuracion(struct vrp_configuracion *vrp)
{
    if (vrp != NULL)
    {
        free(vrp->clientes);
        vrp->clientes = NULL;
        free(vrp);
    }
}

/*===========================*/
/*== ESTRUCTURA: CLIENTES ==*/
/*===========================*/

// Asignamos memoria para la estructura cliente
struct cliente *asignar_memoria_clientes(struct vrp_configuracion *vrp)
{
    struct cliente *cliente = (struct cliente *)malloc(vrp->num_clientes * sizeof(struct cliente));
    if (cliente == NULL)
    {
        imprimir_mensaje("No se pudo asignar memoria para los clientes");
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

    struct hormiga *hormiga = (struct hormiga *)calloc(numHormigas,sizeof(struct hormiga));
    if (hormiga == NULL)
    {
        imprimir_mensaje("No se pudo asignar memoria para hormigas.");
        exit(EXIT_FAILURE);
    }
    return hormiga;
}

// Liberamos memoria de la estructura hormiga
void liberar_memoria_hormiga(struct hormiga *hormiga, int numHormigas)
{
    for (int i = 0; i < numHormigas; i++)
    {
        liberar_memoria_arreglo_int(hormiga[i].tabu);
        liberar_memoria_arreglo_int(hormiga[i].posibles_clientes);
        liberar_memoria_arreglo_double(hormiga[i].probabilidades);
        liberar_lista_vehiculos(hormiga[i].flota);
    }
    free(hormiga);
}

// Reiniciamos la hormiga
void reiniciar_hormiga(struct hormiga *hormiga, struct vrp_configuracion *vrp)
{
    for (int i = 0; i < vrp->num_clientes; i++)
    {
        hormiga->tabu[i] = 0;
        hormiga->posibles_clientes[i] = 0;
        hormiga->probabilidades[i] = 0.0;
    }


    hormiga->tabu_contador = 0;
    hormiga->posibles_clientes_contador = 0;
    hormiga->suma_probabilidades = 0.0;
    hormiga->fitness_global = 0.0;
    
    vaciar_lista_vehiculos(hormiga->flota);
    hormiga->vehiculos_necesarios = 0;
    
    inserta_vehiculo_flota(hormiga, vrp, hormiga->vehiculos_necesarios + 1);
    hormiga->vehiculos_necesarios++;
}


/*==========================*/
/*== ESTRUCTURA: RUTA ======*/
/*==========================*/

// Asignamos memoria para la estructura lista_ruta
struct lista_ruta *asignar_memoria_lista_ruta()
{
    struct lista_ruta *nueva_lista = (struct lista_ruta *)malloc(sizeof(struct lista_ruta));
    if (nueva_lista == NULL)
    {
        imprimir_mensaje("Error: No se pudo asignar memoria para la lista de rutas.");
        exit(EXIT_FAILURE);
    }

    nueva_lista->cabeza = NULL;
    nueva_lista->cola = NULL;

    return nueva_lista;
}

// Asignamos memoria para la estructura nodo_ruta
struct nodo_ruta *asignar_memoria_nodo_ruta()
{
    struct nodo_ruta *nodo_nuevo = (struct nodo_ruta *)malloc(sizeof(struct nodo_ruta));
    if (!nodo_nuevo)
    {
        imprimir_mensaje("No se pudo asignar memoria para nodo_ruta");
        exit(EXIT_FAILURE);
    }
    return nodo_nuevo;
}

// Asignamos memoria para la estructura lista_vehiculos
lista_vehiculos *asignar_memoria_lista_vehiculos()
{
    lista_vehiculos *nueva_lista = (lista_vehiculos *)malloc(sizeof(lista_vehiculos));
    if (nueva_lista == NULL)
    {
        imprimir_mensaje("No se pudo asignar memoria para la lista de vehículos.");
        exit(EXIT_FAILURE);
    }

    nueva_lista->cabeza = NULL;
    nueva_lista->cola = NULL;

    return nueva_lista;
}

/*==========================*/
/*== ESTRUCTURA: VEHICULO ==*/
/*==========================*/

// Asignamos memoria para la estructura vehiculo
struct vehiculo *asignar_memoria_vehiculo()
{
    struct vehiculo *vehiculo = (struct vehiculo *)malloc(sizeof(struct vehiculo));
    if (!vehiculo)
    {
        imprimir_mensaje("No se pudo asignar memoria para el vehiculo.");
        exit(EXIT_FAILURE);
    }
    return vehiculo;
}

// Asignamos memoria para la estructura nodo_vehiculo
struct nodo_vehiculo *asignar_memoria_nodo_vehiculo()
{
    struct nodo_vehiculo *nodo_vehiculo = (struct nodo_vehiculo *)malloc(sizeof(struct nodo_vehiculo));
    if (!nodo_vehiculo)
    {
        imprimir_mensaje("No se pudo asignar memoria para nodo_vehiculo.");
        exit(EXIT_FAILURE);
    }
    return nodo_vehiculo;
}

/*=========================*/
/*== ESTRUCTURA: METAL ====*/
/*=========================*/

// Asignamos memoria para la estructura metal
struct metal *asignar_memoria_metal()
{
    struct metal *metal = (struct metal *)malloc(sizeof(struct metal));
    if (metal == NULL)
    {
        imprimir_mensaje("No se pudo asignar memoria para el metal.");
        exit(EXIT_FAILURE);
    }
    return metal;
}

// Liberamos memoria de la estructura metal
void liberar_memoria_metal(struct individuo *ind)
{
    if (ind->metal)
    {
        if (ind->metal->solucion_vecina)
            liberar_lista_vehiculos(ind->metal->solucion_vecina);
        if (ind->metal->solucion_inicial)
            liberar_lista_vehiculos(ind->metal->solucion_inicial);
        if (ind->metal->mejor_solucion)
            liberar_lista_vehiculos(ind->metal->mejor_solucion);
        free(ind->metal);
    }
}


