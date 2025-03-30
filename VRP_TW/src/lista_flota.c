#include <stdio.h>
#include <stdlib.h>
#include "../includes/estructuras.h"
#include "../includes/control_memoria.h"
#include "../includes/lista_flota.h"
#include "../includes/lista_ruta.h"

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

struct vehiculo* copiar_vehiculo(struct vehiculo *original)
{
    struct vehiculo *nuevo_vehiculo = malloc(sizeof(struct vehiculo));
    
    // Copiar todos los campos básicos
    nuevo_vehiculo->id_vehiculo = original->id_vehiculo;
    nuevo_vehiculo->capacidad_maxima = original->capacidad_maxima;
    nuevo_vehiculo->capacidad_acumulada = original->capacidad_acumulada;
    nuevo_vehiculo->vt_actual = original->vt_actual;
    nuevo_vehiculo->vt_final = original->vt_final;
    nuevo_vehiculo->vt_inicial = original->vt_inicial;
    nuevo_vehiculo->velocidad = original->velocidad;
    nuevo_vehiculo->clientes_contados = original->clientes_contados;
    nuevo_vehiculo->fitness_vehiculo = original->fitness_vehiculo;
    
    // Usar la función copiar_ruta para la ruta
    nuevo_vehiculo->ruta = copiar_ruta(original);
    
    return nuevo_vehiculo;
}


struct lista_vehiculos* copiar_lista_vehiculos(struct lista_vehiculos *original)
{
    if (original == NULL)
        return NULL;
        
    struct lista_vehiculos *nueva_lista = malloc(sizeof(struct lista_vehiculos));
    nueva_lista->cabeza = NULL;
    nueva_lista->cola = NULL;

    struct nodo_vehiculo *actual = original->cabeza;
    while (actual != NULL)
    {
        struct nodo_vehiculo *nuevo_nodo = malloc(sizeof(struct nodo_vehiculo));
        nuevo_nodo->vehiculo = copiar_vehiculo(actual->vehiculo);
        nuevo_nodo->siguiente = NULL;

        if (nueva_lista->cabeza == NULL)
        {
            nueva_lista->cabeza = nuevo_nodo;
            nueva_lista->cola = nuevo_nodo;
        }
        else
        {
            nueva_lista->cola->siguiente = nuevo_nodo;
            nueva_lista->cola = nuevo_nodo;
        }

        actual = actual->siguiente;
    }

    return nueva_lista;
}

void liberar_vehiculo(struct vehiculo *vehiculo)
{
    if (vehiculo == NULL)
        return;
        
    // Liberar la ruta
    liberar_ruta(vehiculo->ruta);
    
    // Liberar el vehículo
    free(vehiculo);
}

void liberar_lista_vehiculos(struct lista_vehiculos *lista)
{
    if (lista == NULL)
        return;
        
    struct nodo_vehiculo *actual = lista->cabeza;
    while (actual != NULL)
    {
        struct nodo_vehiculo *temp = actual;
        actual = actual->siguiente;

        // Liberar el vehículo usando la función dedicada
        liberar_vehiculo(temp->vehiculo);

        // Liberar el nodo de vehículo
        free(temp);
    }

    free(lista);
}