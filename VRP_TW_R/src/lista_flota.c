#include <stdio.h>
#include <stdlib.h>
#include "vrp_tw_aco.h"
#include "configuracion_vrp_tw.h"
#include "control_memoria.h"
#include "lista_flota.h"
#include "lista_ruta.h"

struct nodo_vehiculo *crearNodo(struct hormiga *hormiga, struct vrp_configuracion *vrp)
{
    struct nodo_vehiculo *vehiculo_nuevo;
    vehiculo_nuevo = malloc(sizeof(struct nodo_vehiculo));
    vehiculo_nuevo->vehiculo = malloc(sizeof(struct vehiculo));
    vehiculo_nuevo->vehiculo->id_vehiculo = hormiga->vehiculos_contados + 1; // Inicializamos el id_vehiculo con vehiculos contados -1
    vehiculo_nuevo->vehiculo->capacidad_maxima = vrp->num_capacidad;         // Inicializamos capacidad maxima con lo que soporta la unidad
    vehiculo_nuevo->vehiculo->capacidad_restante = vrp->num_capacidad;       // iniciamos la capacidad maxima
    vehiculo_nuevo->vehiculo->tiempo_consumido = 0.0;                        // Inicializamos tiempo consumido en 0.0
    vehiculo_nuevo->vehiculo->tiempo_maximo = vrp->clientes[0].tiempo_final; // inicializamos el tiempo maximo del vehiculo
    vehiculo_nuevo->vehiculo->clientes_contados = 0;                         // Inicializamos clientes contado en 0
    vehiculo_nuevo->vehiculo->fitness_vehiculo = 0.0;
    do
    {
        vehiculo_nuevo->vehiculo->ruta = asignar_memoria_lista_ruta(); // Asiganamos memoria para la lista ruta

    } while (vehiculo_nuevo->vehiculo->ruta == NULL); // inicializamos el fitness_vehiculo en 0.0
    vehiculo_nuevo->siguiente = NULL;
    hormiga->vehiculos_contados++;                                                 // EL nodo siguiente en NULL
    insertar_cliente_ruta(hormiga, vehiculo_nuevo->vehiculo, &(vrp->clientes[0])); // Insertamos el cliente a la ruta
    return vehiculo_nuevo;
}

bool es_Vacia_Lista(struct lista_vehiculos *flota)
{
    return flota->cabeza == NULL;
}

void inserta_vehiculo_flota(struct hormiga *hormiga, struct vrp_configuracion *vrp)
{
    struct nodo_vehiculo *vehiculo_nuevo = crearNodo(hormiga, vrp);

    if (vehiculo_nuevo != NULL)
    {
        if (es_Vacia_Lista(hormiga->flota))

            hormiga->flota->cabeza = hormiga->flota->cola = vehiculo_nuevo;

        else
        {
            hormiga->flota->cola->siguiente = vehiculo_nuevo;
            hormiga->flota->cola = vehiculo_nuevo;
        }
    }
    else
        printf("\nError al asignar memoria al nodo del vehiculo.");
}
