#include <stdlib.h>
#include <stdio.h>
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

void liberar_individuos(struct individuo *ind)
{
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
        vrp = NULL;
    }
}

/*Para la estructura clientes*/
struct cliente *asignar_memoria_clientes(struct vrp_configuracion *vrp) { return (struct cliente *)malloc(vrp->num_clientes * sizeof(struct cliente)); }

/*Para estructura de hormiga*/
struct hormiga *asignar_memoria_hormigas(struct individuo *ind)
{
    struct hormiga *hormiga = malloc(sizeof(struct hormiga) * ind->numHormigas);
    if (hormiga == NULL)
    {
        printf("Error: No se pudo asignar memoria para hormigas.\n");
        exit(EXIT_FAILURE);
    }
    return hormiga;
}

void liberar_memoria_hormiga(struct hormiga * hormiga, struct individuo * ind)
{
    // Liberar las estructuras dinámicas dentro de cada hormiga
    for (int i = 0; i < ind->numHormigas; i++)
    {
        // Liberar la memoria de la tabla tabu
        free(hormiga[i].tabu);
        // Liberar las probabilidades si están asignadas dinámicamente
        free(hormiga[i].probabilidades);

        free(hormiga[i].posibles_clientes);

        // Liberar la flota (si está asignada dinámicamente)
        struct nodo_vehiculo *vehiculo_actual = hormiga[i].flota->cabeza;
        while (vehiculo_actual != NULL)
        {
            // Liberar las rutas de cada vehículo
            struct nodo_ruta *nodo_actual = vehiculo_actual->vehiculo->ruta->cabeza;
            while (nodo_actual != NULL)
            {
                struct nodo_ruta *temp = nodo_actual;
                nodo_actual = nodo_actual->siguiente;
                free(temp);
            }
            free(vehiculo_actual->vehiculo->ruta);
            vehiculo_actual = vehiculo_actual->siguiente;
        }
        free(hormiga[i].flota);
    }
    // Finalmente, liberar la memoria de las hormigas
    free(hormiga);
}

void reiniciar_hormiga(struct hormiga *hormiga, struct vrp_configuracion *vrp)
{
    // Reset ant's data
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

    // Clear the vehicle fleet but preserve the fleet structure itself
    if (hormiga->flota)
    {
        struct nodo_vehiculo *nodo_actual = hormiga->flota->cabeza;
        while (nodo_actual)
        {
            struct nodo_vehiculo *temp = nodo_actual;
            nodo_actual = nodo_actual->siguiente;

            // Free the vehicle's route
            if (temp->vehiculo && temp->vehiculo->ruta)
            {
                struct nodo_ruta *nodo_ruta = temp->vehiculo->ruta->cabeza;
                while (nodo_ruta)
                {
                    struct nodo_ruta *temp_ruta = nodo_ruta;
                    nodo_ruta = nodo_ruta->siguiente;
                    free(temp_ruta);
                }

                // Free the route structure
                free(temp->vehiculo->ruta);
            }

            // Free the vehicle
            free(temp->vehiculo);
            free(temp);
        }

        // Reset the fleet pointers
        hormiga->flota->cabeza = NULL;
        hormiga->flota->cola = NULL;
    }

    // Reset vehicle count
    hormiga->vehiculos_necesarios = 0;

    // Reinitialize the fleet with a first vehicle
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