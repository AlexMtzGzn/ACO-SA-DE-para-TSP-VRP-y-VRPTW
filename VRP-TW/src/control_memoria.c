#include <stdlib.h>
#include <stdbool.h>
#include "vrp_tw_aco.h"
#include "aed.h"
#include "configuracion_vrp_tw.h"

/*Para arreglos y instancias*/

double **asignar_memoria_instancia(int tamanio_instancia)
{
    double **instancia = (double **)malloc(tamanio_instancia * sizeof(double *));
    for (int i = 0; i < tamanio_instancia; i++)
        instancia[i] = (double *)malloc(tamanio_instancia * sizeof(double));

    return instancia;
}

int *asignar_memoria_arreglo(int tamanio_arreglo)
{
    int *arreglo = (int *)malloc(sizeof(int) * (tamanio_arreglo));
    return arreglo;
}

double *asignar_memoria_arreglo_double(int tamanio_instancia)
{
    double *arreglo = (double *)malloc(sizeof(double) * (tamanio_instancia));
    return arreglo;
}

int *reasignar_memoria_ruta(int *ruta, int nuevos_clientes)
{
    int *nueva_ruta = realloc(ruta, nuevos_clientes * sizeof(int));

    if (nueva_ruta == NULL)
    {
        // Falta hacer algo en caso de que no lo regrese ebien
        return ruta; // Devolvemos la ruta original para evitar pérdida de memoria.
    }

    return nueva_ruta;
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
    /*if (bandera == true)
        free(ind->ruta); Falta ajustar esto*/
}

/*Para la estructura vrp_configuracion*/
struct vrp_configuracion *asignar_memoria_vrp_configuracion() { return (struct vrp_configuracion *)malloc(sizeof(struct vrp_configuracion)); }

/*Para la estructura clientes*/
struct cliente *asignar_memoria_clientes(struct vrp_configuracion *vrp) { return (struct cliente *)malloc(vrp->num_clientes * sizeof(struct cliente)); }

// /*Para la estructura de la hormiga*/
struct hormiga *asignar_memoria_hormiga(struct individuo *ind)
{
    return (struct hormiga *)malloc(ind->numHormigas * sizeof(struct hormiga));
}

/*Para la estructura clientes*/
struct vehiculo *asignar_memoria_vehiculo(struct vrp_configuracion *vrp)
{
    return (struct vehiculo *)malloc(vrp->num_vehiculos * sizeof(struct vehiculo));
}
struct vehiculo *redimensionar_memoria_vehiculo(struct hormiga *hormiga)
{
    return (struct vehiculo *)realloc(hormiga->flota, hormiga->vehiculos_contados * sizeof(struct vehiculo));
}
