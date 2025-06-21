#include <stdlib.h>
#include <stdbool.h>
#include "../include/estructuras.h"
#include "../include/lista_flota.h"
#include "../include/salida_datos.h"

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

void liberar_instancia(double **instancia, int tamanio_instancia)
{
    for (int i = 0; i < tamanio_instancia; i++)
        free(instancia[i]);
    free(instancia);
}

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

void liberar_memoria_arreglo_int(int *arreglo)
{
    free(arreglo);
}

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

void liberar_memoria_arreglo_double(double *arreglo)
{
    free(arreglo);
}


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

void liberar_rangos(struct rangos *rango)
{
    free(rango);
}

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

void liberar_memoria_vrp_configuracion(struct vrp_configuracion *vrp)
{
    if (vrp)
    {
        if (vrp->clientes)
            free(vrp->clientes);
        free(vrp);
    }
}


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

void liberar_memoria_datos_cliente(struct datos_cliente *datos_cliente)
{
    free(datos_cliente);
}

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

void liberar_memoria_arreglo_nodo_ruta(struct nodo_ruta **arreglo)
{
    if (arreglo)
        free(arreglo);
}

void liberar_memoria_nodo_ruta(struct nodo_ruta *nodo_ruta)
{
    if (nodo_ruta)
        free(nodo_ruta);
}

void liberar_lista_ruta(struct lista_ruta *ruta)
{
    struct nodo_ruta *cliente_actual = ruta->cabeza;

    while (cliente_actual)
    {
        struct nodo_ruta *cliente_temp = cliente_actual;
        cliente_actual = cliente_actual->siguiente;
        free(cliente_temp);
    }

    free(ruta);
}

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

void liberar_memoria_nodo_vehiculo(struct nodo_vehiculo *nodo_vehiculo)
{
    free(nodo_vehiculo);
}

void liberar_vehiculo(struct vehiculo *vehiculo)
{
    if (vehiculo->ruta)
        liberar_lista_ruta(vehiculo->ruta);
    if (vehiculo->datos_cliente)
        liberar_memoria_datos_cliente(vehiculo->datos_cliente);
    free(vehiculo);
}

void vaciar_lista_vehiculos(struct lista_vehiculos *flota)
{
    struct nodo_vehiculo *vehiculo_actual = flota->cabeza;
    while (vehiculo_actual)
    {
        struct nodo_vehiculo *vehiculo_temp = vehiculo_actual;
        vehiculo_actual = vehiculo_actual->siguiente;

        if (vehiculo_temp->vehiculo)
            liberar_vehiculo(vehiculo_temp->vehiculo);

        free(vehiculo_temp);
    }

    flota->cabeza = NULL;
    flota->cola = NULL;
}

void liberar_lista_vehiculos(struct lista_vehiculos *flota)
{
    struct nodo_vehiculo *vehiculo_actual = flota->cabeza;
    while (vehiculo_actual)
    {
        struct nodo_vehiculo *vehiculo_temp = vehiculo_actual;
        vehiculo_actual = vehiculo_actual->siguiente;

        if (vehiculo_temp->vehiculo)
            liberar_vehiculo(vehiculo_temp->vehiculo);

        free(vehiculo_temp);
    }
    free(flota);
}

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

void liberar_individuos(struct individuo *ind, int num_poblacion, bool tipo)
{
    if (tipo)
        for (int i = 0; i < num_poblacion; i++)
            vaciar_lista_vehiculos(ind[i].hormiga->flota);

    free(ind);
}

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

void liberar_memoria_mejores_hormigas(struct mejores_hormigas *mejores_hormigas)
{
    free(mejores_hormigas);
}
