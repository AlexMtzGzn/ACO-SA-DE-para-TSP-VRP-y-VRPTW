#include <stdlib.h>
#include <stdbool.h>
#include "../include/estructuras.h"
#include "../include/lista_flota.h"
#include "../include/salida_datos.h"

// Funciones para asignar y liberar memoria para arreglos e instancias

// Funcion para asigna memoria para una matriz de doble puntero (instancia) de tamaño [tamanio_instancia x tamanio_instancia].
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

// Funcion que libera la memoria de la instancia (matriz) de tamaño 'tamanio_instancia'.
void liberar_instancia(double **instancia, int tamanio_instancia)
{
    for (int i = 0; i < tamanio_instancia; i++)
        free(instancia[i]);
    free(instancia);
}

// Asigna memoria para un arreglo de enteros de tamaño 'tamanio_arreglo'.
int *asignar_memoria_arreglo_int(int tamanio_arreglo)
{
    int *arreglo = (int *)calloc(tamanio_arreglo, sizeof(int));
    if (arreglo == NULL)
    {
        imprimir_mensaje("No se pudo asignar memoria para el arreglo entero");
        exit(EXIT_FAILURE);
    }
    return arreglo;
}

// Libera la memoria del arreglo de enteros.
void liberar_memoria_arreglo_int(int *arreglo)
{
    free(arreglo);
}

// Asigna memoria para un arreglo de tipo double de tamaño 'tamanio_arreglo'.
double *asignar_memoria_arreglo_double(int tamanio_arreglo)
{
    double *arreglo = (double *)calloc(tamanio_arreglo, sizeof(double));
    if (arreglo == NULL)
    {
        imprimir_mensaje("No se pudo asignar memoria para el arreglo double");
        exit(EXIT_FAILURE);
    }
    return arreglo;
}

// Libera la memoria del arreglo de tipo double.
void liberar_memoria_arreglo_double(double *arreglo)
{
    free(arreglo);
}

// Funciones para asignar y liberar memoria para la estructura de rangos

// Asigna memoria para la estructura de rangos.
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

// Libera la memoria de la estructura de rangos.
void liberar_rangos(struct rangos *rango)
{
    free(rango);
}

/*Funciones para la estructura del individuo*/

// Asigna memoria para una cantidad de individuos definida por 'poblacion'.
struct individuo *asignar_memoria_individuos(int poblacion)
{
    struct individuo *individuo = (struct individuo *)calloc(poblacion, sizeof(struct individuo));
    if (individuo == NULL)
    {
        imprimir_mensaje("No se pudo asignar memoria para los individuos");
        exit(EXIT_FAILURE);
    }
    return individuo;
}

// Libera la memoria de un arreglo de individuos.
void liberar_individuos(struct individuo *ind, int num_poblacion, bool tipo)
{
    if (tipo)
        for (int i = 0; i < num_poblacion; i++)
            vaciar_lista_vehiculos(ind[i].hormiga->flota);
            
    free(ind);
}

// Funciones para asignar y liberar memoria para la estructura `vrp_configuracion`

// Asigna memoria para la estructura 'vrp_configuracion'.
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

// Libera la memoria de la estructura 'vrp_configuracion'.
void liberar_memoria_vrp_configuracion(struct vrp_configuracion *vrp)
{
    if (vrp)
    {
        if (vrp->clientes)
            free(vrp->clientes);
        free(vrp);
    }
}

// Funciones para asignar memoria para la estructura de clientes

// Asigna memoria para los clientes de acuerdo al número de clientes definido en la configuración VRP.
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

// Asigna memoria para un arreglo de datos de clientes.
struct datos_cliente *asignar_memoria_datos_clientes(int numClientes)
{
    struct datos_cliente *datos_cliente = (struct datos_cliente *)malloc(numClientes * sizeof(struct datos_cliente));
    if (!datos_cliente)
    {
        imprimir_mensaje("No se pudo asiganar memoria para el arreglo datos clientes");
        exit(EXIT_FAILURE);
    }
    return datos_cliente;
}

// Libera la memoria del arreglo de datos de clientes.
void liberar_memoria_datos_cliente(struct datos_cliente *datos_cliente)
{
    free(datos_cliente);
}

// Funciones para asignar y liberar memoria para la estructura de hormigas

// Asigna memoria para un arreglo de hormigas de tamaño 'numHormigas'.
struct hormiga *asignar_memoria_hormigas(int numHormigas)
{
    struct hormiga *hormiga = (struct hormiga *)calloc(numHormigas, sizeof(struct hormiga));
    if (hormiga == NULL)
    {
        imprimir_mensaje("No se pudo asignar memoria para hormigas.");
        exit(EXIT_FAILURE);
    }
    return hormiga;
}

// Libera la memoria asociada a las hormigas, incluyendo las estructuras dentro de cada hormiga.
void liberar_memoria_hormiga(struct hormiga *hormiga, int numHormigas)
{
    for (int i = 0; i < numHormigas; i++)
    {
        if (hormiga[i].tabu)
            liberar_memoria_arreglo_int(hormiga[i].tabu);
        if (hormiga[i].posibles_clientes)
            liberar_memoria_arreglo_int(hormiga[i].posibles_clientes);
        if (hormiga[i].probabilidades)
            liberar_memoria_arreglo_double(hormiga[i].probabilidades);
        if (hormiga[i].flota)
            liberar_lista_vehiculos(hormiga[i].flota);
    }
    free(hormiga);
}

// Reinicia la información de una hormiga, incluyendo sus arreglos internos y flota de vehículos.
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

// Funciones para asignar y liberar memoria para la estructura de mejores hormigas

// Asigna memoria para un arreglo de mejores hormigas de tamaño 'numHormigas'.
struct mejores_hormigas *asignar_memoria_mejores_hormigas(int numHormigas)
{
    struct mejores_hormigas *mejores_hormigas = (struct mejores_hormigas *)calloc(numHormigas, sizeof(struct mejores_hormigas));
    if (!mejores_hormigas)
    {
        imprimir_mensaje("No se pudo asignar memoria para arreglo mejores_hormigas");
        exit(EXIT_FAILURE);
    }
    return mejores_hormigas;
}

// Libera la memoria de un arreglo de mejores hormigas.
void liberar_memoria_mejores_hormigas(struct mejores_hormigas *mejores_hormigas)
{
    free(mejores_hormigas);
}

// Funciones para asignar memoria para las estructuras de rutas

// Asigna memoria para una nueva lista de rutas.
struct lista_ruta *asignar_memoria_lista_ruta()
{
    struct lista_ruta *nueva_lista = (struct lista_ruta *)malloc(sizeof(struct lista_ruta));
    if (nueva_lista == NULL)
    {
        imprimir_mensaje("No se pudo asignar memoria para la lista de rutas.");
        exit(EXIT_FAILURE);
    }

    nueva_lista->cabeza = NULL;
    nueva_lista->cola = NULL;

    return nueva_lista;
}

// Asigna memoria para un nuevo nodo de ruta.
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

// Asigna memoria para un arreglo de nodos de ruta, útil para almacenar rutas de clientes.
struct nodo_ruta **asignar_memoria_arreglo_nodo_ruta(int total_clientes)
{
    struct nodo_ruta **arreglo_nodos = malloc(total_clientes * sizeof(nodo_ruta *));
    if (!arreglo_nodos)
    {
        imprimir_mensaje("No se pudo asignar memoria para el arreglo de nodos ruta.");
        exit(EXIT_FAILURE);
    }
    return arreglo_nodos;
}

// Libera la memoria de un arreglo de nodos de ruta.
void liberar_memoria_arreglo_nodo_ruta(struct nodo_ruta **arreglo)
{
    if (arreglo)
        free(arreglo);
}

// Libera la memoria de un nodo de ruta.
void liberar_memoria_nodo_ruta(struct nodo_ruta *nodo_ruta)
{
    if (nodo_ruta)
        free(nodo_ruta);
}

// Funciones para asignar memoria para las estructuras de vehículos

// Asigna memoria para una lista de vehículos
struct lista_vehiculos *asignar_memoria_lista_vehiculos()
{
    struct lista_vehiculos *nueva_lista = (struct lista_vehiculos *)malloc(sizeof(struct lista_vehiculos));
    if (nueva_lista == NULL)
    {
        imprimir_mensaje("No se pudo asignar memoria para la lista de vehículos.");
        exit(EXIT_FAILURE);
    }

    nueva_lista->cabeza = NULL;
    nueva_lista->cola = NULL;

    return nueva_lista;
}

// Asigna memoria para un nuevo vehículo.
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

// Asigna memoria para un nuevo nodo de vehículo.
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

// Libera la memoria de un nodo de vehículo.
void liberar_memoria_nodo_vehiculo(struct nodo_vehiculo *nodo_vehiculo)
{
    free(nodo_vehiculo);
}

// Libera la memoria de una lista de vehículos.
void liberar_lista_ruta(struct lista_ruta *lista)
{
    struct nodo_ruta *nodo_aux;
    while (lista->cabeza != NULL)
    {
        nodo_aux = lista->cabeza;
        lista->cabeza = lista->cabeza->siguiente;
        free(nodo_aux);
    }
    free(lista);
}
