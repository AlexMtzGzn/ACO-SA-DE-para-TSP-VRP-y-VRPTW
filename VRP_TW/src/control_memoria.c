#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "../includes/estructuras.h"
#include "../includes/lista_flota.h"
/*Para arreglos y instancias*/

double **asignar_memoria_instancia(int tamanio_instancia)
{
    double **instancia = (double **)malloc(tamanio_instancia * sizeof(double *));
    for (int i = 0; i < tamanio_instancia; i++)
        instancia[i] = (double *)malloc(tamanio_instancia * sizeof(double));

    return instancia;
}

int *asignar_memoria_arreglo_int(int tamanio_arreglo)
{
    int *arreglo = (int *)malloc(sizeof(int) * (tamanio_arreglo));
    return arreglo;
}

double *asignar_memoria_arreglo_double(int tamanio_instancia)
{
    double *arreglo = (double *)malloc(sizeof(double) * (tamanio_instancia));
    return arreglo;
}

void liberar_memoria_arreglo_int(int *arreglo)
{
    free(arreglo);
}

void liberar_memoria_arreglo_double(double *arreglo)
{
    free(arreglo);
}

void liberar_instancia(double **instancia, int tamanio_instancia)
{
    for (int i = 0; i < tamanio_instancia; i++)
        free(instancia[i]);
    free(instancia);
}

/*Para estructuras del individuo*/

struct individuo *asignar_memoria_individuos(int poblacion)
{
    return (struct individuo *)malloc(sizeof(struct individuo) * poblacion);
}

void liberar_individuos(struct individuo *ind, int num_poblacion, bool tipo)
{

    if (tipo)
        for (int i = 0; i < num_poblacion; i++)
            liberar_lista_vehiculos(ind[i].hormiga->flota);

    free(ind);
}

/*Para la estructura vrp_configuracion*/
struct vrp_configuracion *asignar_memoria_vrp_configuracion() { return (struct vrp_configuracion *)malloc(sizeof(struct vrp_configuracion)); }

void liberar_memoria_vrp_configuracion(struct vrp_configuracion *vrp)
{
    if (vrp != NULL)
    {
        free(vrp->clientes);
        vrp->clientes = NULL;
        free(vrp);
    }
}

/*Para la estructura clientes*/
struct cliente *asignar_memoria_clientes(struct vrp_configuracion *vrp) { return (struct cliente *)malloc(vrp->num_clientes * sizeof(struct cliente)); }

/*Para estructura de hormiga*/
struct hormiga *asignar_memoria_hormigas(int numHormigas)
{
    struct hormiga *hormiga = (struct hormiga *)malloc(sizeof(struct hormiga) * numHormigas);
    if (hormiga == NULL)
    {
        printf("Error: No se pudo asignar memoria para hormigas.\n");
        exit(EXIT_FAILURE);
    }
    return hormiga;
}

void liberar_memoria_hormiga(struct hormiga *hormiga, struct individuo *ind)
{
    // Liberar las estructuras dinámicas dentro de cada hormiga
    for (int i = 0; i < ind->numHormigas; i++)
    {
        // Liberar la memoria de la tabla tabu
        liberar_memoria_arreglo_int(hormiga[i].tabu);
        liberar_memoria_arreglo_int(hormiga[i].posibles_clientes);
        liberar_memoria_arreglo_double(hormiga[i].probabilidades);
        liberar_lista_vehiculos(hormiga[i].flota);
    }
    // Finalmente, liberar la memoria de las hormigas
    free(hormiga);
}

void reiniciar_hormiga(struct hormiga *hormiga, struct vrp_configuracion *vrp)
{
    // Reiniciamos los datos de los arreglos tabu, posibles_clientes y probabilidades cada uno 0 en s posicion
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
    liberar_lista_vehiculos(hormiga->flota);
    hormiga->vehiculos_necesarios = 0;
    inserta_vehiculo_flota(hormiga, vrp, hormiga->vehiculos_necesarios + 1);
    hormiga->vehiculos_necesarios++;
}

/*Para estructura de lista_ruta*/
struct lista_ruta *asignar_memoria_lista_ruta()
{
    struct lista_ruta *nueva_lista = (struct lista_ruta *)malloc(sizeof(struct lista_ruta));
    if (nueva_lista == NULL)
        return NULL;
    nueva_lista->cabeza = NULL;
    nueva_lista->cola = NULL;
    return nueva_lista;
}

/*Para estructura de lista_vehiculos*/
lista_vehiculos *asignar_memoria_lista_vehiculos()
{
    lista_vehiculos *nueva_lista = (lista_vehiculos *)malloc(sizeof(lista_vehiculos));
    if (nueva_lista != NULL)
    {
        nueva_lista->cabeza = NULL;
        nueva_lista->cola = NULL;
    }
    return nueva_lista;
}