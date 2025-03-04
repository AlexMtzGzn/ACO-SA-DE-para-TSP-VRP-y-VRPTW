#include <stdlib.h>
#include <stdbool.h>
#include "aed.h"
#include "vrp_tw_aco.h"

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
    return (int *)malloc(sizeof(int) * (tamanio_arreglo));
}


void liberar_instancia(double **instancia, int tamanio_instancia)
{
    for (int i = 0; i < tamanio_instancia; i++)
        free(instancia[i]);
    free(instancia);
}

/*Para estructuras del individuo*/

individuo *asignar_memoria_individuos(int poblacion)
{
    return (individuo *)malloc(sizeof(individuo) * poblacion);
}

void liberar_individuos(individuo *ind, bool bandera)
{
    free(ind);
    /*if (bandera == true)
        free(ind->ruta); Falta ajustar esto*/
}

/*Para la estructura vrp_configuracion*/
vrp_configuracion *asignar_memoria_vrp_configuracion() { return (struct vrp_configuracion *)malloc(sizeof(struct vrp_configuracion)); }

/*Para la estructura clientes*/
cliente *asignar_memoria_clientes(struct vrp_configuracion *vrp) { return (struct cliente *)malloc(vrp->num_clientes * sizeof(struct cliente)); }

/*Para la estructura de la hormiga*/
hormiga *asignar_memoria_hormiga(individuo *ind)
{
    return (hormiga *)malloc(sizeof(hormiga) * ind->numHormigas);
}

/*Para la estructura clientes*/
vehiculo *asignar_memoria_vehiculo() { return (vehiculo *)malloc(sizeof(vehiculo));}

/*generacion *asignar_memoria_generaciones(int poblacion, int generaciones) { return (generacion *)malloc(sizeof(generacion) * (poblacion * generaciones)); }







int *asignar_memoria_ruta(int tamanio_instancia)
{
    return (int *)malloc(sizeof(int) * (tamanio_instancia));
}


double *asignar_memoria_posibilidades(int tamanio_instancia)
{
    return (double *)malloc(sizeof(double) * (tamanio_instancia));
}

void liberar_hormigas(hormiga *hor, individuo *ind)
{
    for (int i = 0; i < ind->numHormigas; i++)
    {
        free(hor[i].ruta);
        free(hor[i].tabu);

    }
    free(hor->probabilidades);
    free(hor);
}*/