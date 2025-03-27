#include <stdio.h>
#include <stdlib.h>
#include "vrp_tw_aco.h"
#include "configuracion_vrp_tw.h"
#include "control_memoria.h"
#include "lista_flota.h"
#include "lista_ruta.h"

struct nodo_vehiculo *crearNodo(struct hormiga *hormiga, struct vrp_configuracion *vrp, int id)
{
    if (!vrp || !vrp->clientes) {
        fprintf(stderr, "Error: Configuración VRP inválida\n");
        return NULL;
    }

    struct nodo_vehiculo *vehiculo_nuevo = malloc(sizeof(struct nodo_vehiculo));
    if (!vehiculo_nuevo) {
        fprintf(stderr, "Error: No se pudo asignar memoria para nodo_vehiculo\n");
        return NULL;
    }

    vehiculo_nuevo->vehiculo = malloc(sizeof(struct vehiculo));
    if (!vehiculo_nuevo->vehiculo) {
        fprintf(stderr, "Error: No se pudo asignar memoria para vehiculo\n");
        free(vehiculo_nuevo);
        return NULL;
    }

    vehiculo_nuevo->vehiculo->id_vehiculo = id;
    vehiculo_nuevo->vehiculo->capacidad_maxima = vrp->num_capacidad;
    vehiculo_nuevo->vehiculo->capacidad_acumulada = 0.0;
    vehiculo_nuevo->vehiculo->vt_actual = 0.0;
    vehiculo_nuevo->vehiculo->vt_final = vrp->clientes[0].vt_final;
    vehiculo_nuevo->vehiculo->vt_inicial = vrp->clientes[0].vt_inicial;
    vehiculo_nuevo->vehiculo->clientes_contados = 0;
    vehiculo_nuevo->vehiculo->fitness_vehiculo = 0.0;
    vehiculo_nuevo->vehiculo->velocidad = 1.0;

    vehiculo_nuevo->vehiculo->ruta = asignar_memoria_lista_ruta();
    if (!vehiculo_nuevo->vehiculo->ruta) {
        fprintf(stderr, "Error: No se pudo asignar memoria para la lista ruta\n");
        free(vehiculo_nuevo->vehiculo);
        free(vehiculo_nuevo);
        return NULL;
    }

    vehiculo_nuevo->siguiente = NULL;
    insertar_cliente_ruta(hormiga, vehiculo_nuevo->vehiculo, &(vrp->clientes[0]));

    return vehiculo_nuevo;
}


bool es_Vacia_Lista(struct lista_vehiculos *flota)
{
    return flota->cabeza == NULL;
}

void inserta_vehiculo_flota(struct hormiga *hormiga, struct vrp_configuracion *vrp, int id)
{
    struct nodo_vehiculo *vehiculo_nuevo = crearNodo(hormiga, vrp, id); 

    if (vehiculo_nuevo != NULL)
    {
        if (es_Vacia_Lista(hormiga->flota))
        {
            hormiga->flota->cabeza = hormiga->flota->cola = vehiculo_nuevo;
        }
        else
        {
            hormiga->flota->cola->siguiente = vehiculo_nuevo;
            hormiga->flota->cola = vehiculo_nuevo;
        }
    }
    else
        printf("\nError al asignar memoria al nodo del vehiculo.");
}
