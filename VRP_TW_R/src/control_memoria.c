#include <stdlib.h>
#include <stdbool.h>
#include "aed.h"
#include "vrp_tw_aco.h"
#include "configuracion_vrp_tw.h"
#include "lista_ruta.h"
#include "lista_flota.h"

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

void liberar_individuos(struct individuo *ind, bool bandera)
{
    free(ind);
    /*(bandera == true){
        liberar_memoria_arreglo_int(ind->ruta);
    }*/
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
        vrp = NULL;
    }
}

/*Para la estructura clientes*/
struct cliente *asignar_memoria_clientes(struct vrp_configuracion *vrp) { return (struct cliente *)malloc(vrp->num_clientes * sizeof(struct cliente)); }

/*Para estructura de hormiga*/
struct hormiga *asignar_memoria_hormigas(struct individuo *ind)
{
    return (struct hormiga *)malloc(sizeof(struct hormiga) * ind->numHormigas);
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
lista_vehiculos* asignar_memoria_lista_vehiculos() {
    lista_vehiculos* nueva_lista = (lista_vehiculos*)malloc(sizeof(lista_vehiculos));
    if (nueva_lista != NULL) {
        nueva_lista->cabeza = NULL;
        nueva_lista->cola = NULL;
    }
    return nueva_lista;
}