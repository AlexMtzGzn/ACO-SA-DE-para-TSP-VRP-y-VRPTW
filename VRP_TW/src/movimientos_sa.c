#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/estructuras.h"
#include "../include/control_memoria.h"
#include "../include/lista_ruta.h"
#include "../include/lista_flota.h"
#include "../include/salida_datos.h"

// Intercambia dos clientes aleatorios dentro de un mismo vehículo
bool swap_intra(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    int vehiculo_aleatorio, intentos_maximos = 10, cliente1_idx, cliente2_idx, temp;
    nodo_vehiculo *vehiculo_actual = NULL;
    nodo_ruta *nodo1 = NULL, *nodo2 = NULL;

    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        vehiculo_aleatorio = (rand() % hormiga->vehiculos_necesarios) + 1;

        vehiculo_actual = hormiga->flota->cabeza;
        while (vehiculo_actual != NULL)
        {
            if (vehiculo_actual->vehiculo->id_vehiculo == vehiculo_aleatorio)
                break;
            vehiculo_actual = vehiculo_actual->siguiente;
        }

        if (vehiculo_actual != NULL && vehiculo_actual->vehiculo->clientes_contados >= 2)
            break;
    }

    if (vehiculo_actual == NULL || vehiculo_actual->vehiculo->clientes_contados < 2)
        return false;

    if (vehiculo_actual->vehiculo->clientes_contados == 2)
    {
        cliente1_idx = 0;
        cliente2_idx = 1;
    }
    else
    {
        do
        {
            cliente1_idx = rand() % vehiculo_actual->vehiculo->clientes_contados;
            cliente2_idx = rand() % vehiculo_actual->vehiculo->clientes_contados;
        } while (cliente1_idx == cliente2_idx);
    }

    nodo1 = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
    nodo2 = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;

    for (int i = 0; i < cliente1_idx; i++)
    {
        if (nodo1 == NULL)
            return false;
        nodo1 = nodo1->siguiente;
    }

    for (int i = 0; i < cliente2_idx; i++)
    {
        if (nodo2 == NULL)
            return false;
        nodo2 = nodo2->siguiente;
    }

    if (nodo1 == NULL || nodo2 == NULL)
        return false;

    // Opcional: Verificar que no sean depósitos
    if (nodo1->cliente == 0 || nodo2->cliente == 0)
        return false;

    temp = nodo1->cliente;
    nodo1->cliente = nodo2->cliente;
    nodo2->cliente = temp;

    if (!verificarRestricciones(vehiculo_actual->vehiculo, vrp, instancia_distancias))
    {
        // Revertir
        temp = nodo1->cliente;
        nodo1->cliente = nodo2->cliente;
        nodo2->cliente = temp;
        return false;
    }

    return true;
}

bool opt_2(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    nodo_vehiculo *vehiculo_actual = NULL;
    int intentos_maximos = 10;

    // Seleccionar vehículo con al menos 3 clientes
    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        vehiculo_actual = seleccionar_vehiculo_aleatorio(hormiga);
        if (vehiculo_actual && vehiculo_actual->vehiculo && vehiculo_actual->vehiculo->clientes_contados >= 3)
            break;
    }

    if (vehiculo_actual == NULL || vehiculo_actual->vehiculo->clientes_contados < 3)
        return false;

    int total_clientes = vehiculo_actual->vehiculo->clientes_contados;

    // Seleccionar dos índices aleatorios distintos entre 1 y total_clientes-1 (excluyendo depósito)
    int idx1 = (rand() % (total_clientes - 1)) + 1;
    int idx2 = (rand() % (total_clientes - 1)) + 1;

    if (idx1 == idx2)
        return false;

    if (idx1 > idx2)
    {
        int temp = idx1;
        idx1 = idx2;
        idx2 = temp;
    }

    int tamanio_segmento = idx2 - idx1 + 1;
    if (tamanio_segmento < 2)
        return false;

    // Asignar memoria
    int *clientes_originales = asignar_memoria_arreglo_int(tamanio_segmento);
    if (!clientes_originales)
        return false;

    nodo_ruta **nodos = malloc(total_clientes * sizeof(nodo_ruta *));
    if (!nodos)
    {
        liberar_memoria_arreglo_int(clientes_originales);
        return false;
    }

    // Llenar array nodos
    nodo_ruta *nodo_temp = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
    int count = 0;
    while (nodo_temp && count < total_clientes)
    {
        nodos[count++] = nodo_temp;
        nodo_temp = nodo_temp->siguiente;
    }

    if (count < total_clientes || idx2 >= count)
    {
        free(nodos);
        liberar_memoria_arreglo_int(clientes_originales);
        return false;
    }

    // Guardar estado original
    for (int i = 0; i < tamanio_segmento; i++)
        clientes_originales[i] = nodos[idx1 + i]->cliente;

    // Invertir segmento
    for (int i = 0; i < tamanio_segmento / 2; i++)
    {
        int temp = nodos[idx1 + i]->cliente;
        nodos[idx1 + i]->cliente = nodos[idx2 - i]->cliente;
        nodos[idx2 - i]->cliente = temp;
    }

    // Verificar restricciones
    bool factible = verificarRestricciones(vehiculo_actual->vehiculo, vrp, instancia_distancias);

    if (!factible)
    {
        // Revertir
        for (int i = 0; i < tamanio_segmento; i++)
            nodos[idx1 + i]->cliente = clientes_originales[i];
    }

    free(nodos);
    liberar_memoria_arreglo_int(clientes_originales);
    return factible;
}


// Intercambio entre vehículos
bool swap_inter(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    nodo_vehiculo *primer_vehiculo = NULL, *segundo_vehiculo = NULL;
    int intentos_maximos = 10;

    // Seleccionar dos vehículos diferentes con clientes
    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        primer_vehiculo = seleccionar_vehiculo_aleatorio(hormiga);
        segundo_vehiculo = seleccionar_vehiculo_aleatorio(hormiga);

        if (!primer_vehiculo || !segundo_vehiculo ||
            !primer_vehiculo->vehiculo || !segundo_vehiculo->vehiculo)
            continue;

        if (primer_vehiculo == segundo_vehiculo)
            continue;

        if (primer_vehiculo->vehiculo->clientes_contados >= 1 &&
            segundo_vehiculo->vehiculo->clientes_contados >= 1)
            break;
    }

    if (!primer_vehiculo || !segundo_vehiculo)
        return false;

    int total1 = primer_vehiculo->vehiculo->clientes_contados;
    int total2 = segundo_vehiculo->vehiculo->clientes_contados;

    if (total1 == 0 || total2 == 0)
    return false;


    // Seleccionar posiciones válidas (evitando depósitos)
    int pos1 = rand() % total1; // Posición 0 a total1-1
    int pos2 = rand() % total2; // Posición 0 a total2-1

    // Navegar a posiciones (saltando el depósito inicial)
    nodo_ruta *nodo1 = primer_vehiculo->vehiculo->ruta->cabeza->siguiente;
    nodo_ruta *nodo2 = segundo_vehiculo->vehiculo->ruta->cabeza->siguiente;

    for (int i = 0; i < pos1 && nodo1; i++)
        nodo1 = nodo1->siguiente;
    for (int i = 0; i < pos2 && nodo2; i++)
        nodo2 = nodo2->siguiente;

    if (!nodo1 || !nodo2 || nodo1->cliente == 0 || nodo2->cliente == 0)
        return false;

    // Realizar intercambio
    int temp = nodo1->cliente;
    nodo1->cliente = nodo2->cliente;
    nodo2->cliente = temp;

    // Verificar restricciones en ambos vehículos
    bool factible = verificarRestricciones(primer_vehiculo->vehiculo, vrp, instancia_distancias) &&
                    verificarRestricciones(segundo_vehiculo->vehiculo, vrp, instancia_distancias);

    if (!factible)
    {
        // Revertir intercambio
        temp = nodo1->cliente;
        nodo1->cliente = nodo2->cliente;
        nodo2->cliente = temp;
    }

    return factible;
}


// Reinserción intra e inter vehículo
bool reinsercion_intra_inter(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    nodo_vehiculo *vehiculo_origen = NULL, *vehiculo_destino = NULL;
    int intentos_maximos = 10;

    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        vehiculo_origen = seleccionar_vehiculo_aleatorio(hormiga);
        vehiculo_destino = seleccionar_vehiculo_aleatorio(hormiga);

        if (!vehiculo_origen || !vehiculo_destino ||
            !vehiculo_origen->vehiculo || !vehiculo_destino->vehiculo)
            continue;

        if (vehiculo_origen->vehiculo->clientes_contados < 1)
            continue;

        if (vehiculo_origen == vehiculo_destino &&
            vehiculo_origen->vehiculo->clientes_contados < 2)
            continue;

        break;
    }

    if (!vehiculo_origen || !vehiculo_destino)
        return false;

    int total_origen = vehiculo_origen->vehiculo->clientes_contados;
    int total_destino = vehiculo_destino->vehiculo->clientes_contados;

    if (total_origen == 0)
        return false;

    int pos_origen = rand() % total_origen; // 0 a total_origen-1

    nodo_ruta *nodo_cliente = vehiculo_origen->vehiculo->ruta->cabeza->siguiente;
    for (int i = 0; i < pos_origen && nodo_cliente; i++)
        nodo_cliente = nodo_cliente->siguiente;

    if (!nodo_cliente || nodo_cliente->cliente == 0)
        return false;

    int cliente = nodo_cliente->cliente;

    // CORRECCIÓN clave: pos_destino puede ser desde 0 a total_destino
    int pos_destino = rand() % (total_destino + 1);

    eliminar_cliente_ruta(vehiculo_origen->vehiculo, vrp, cliente, instancia_distancias);

    bool factible = insertarClienteEnPosicion(vehiculo_destino->vehiculo, vrp, cliente,
                                              pos_destino, instancia_distancias);

    if (factible)
    {
        bool restricciones_ok = verificarRestricciones(vehiculo_origen->vehiculo, vrp, instancia_distancias) &&
                                verificarRestricciones(vehiculo_destino->vehiculo, vrp, instancia_distancias);

        if (restricciones_ok)
        {
            return true;
        }

        eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp, cliente, instancia_distancias);
    }

    // Restaurar en origen (pos_origen + 1 si insertarClienteEnPosicion usa índice 1-based)
    insertarClienteEnPosicion(vehiculo_origen->vehiculo, vrp, cliente, pos_origen, instancia_distancias);
    return false;
}


// Relocación de segmento (2-opt con 5)
bool opt_2_5(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    int intentos_maximos = 10;
    nodo_vehiculo *vehiculo_origen = NULL, *vehiculo_destino = NULL;

    // Seleccionar vehículo origen con al menos 2 clientes
    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        vehiculo_origen = seleccionar_vehiculo_aleatorio(hormiga);
        if (vehiculo_origen && vehiculo_origen->vehiculo &&
            vehiculo_origen->vehiculo->clientes_contados >= 2)
            break;
    }

    if (!vehiculo_origen || vehiculo_origen->vehiculo->clientes_contados < 2)
        return false;

    // Seleccionar vehículo destino (puede ser el mismo)
    vehiculo_destino = seleccionar_vehiculo_aleatorio(hormiga);
    if (!vehiculo_destino || !vehiculo_destino->vehiculo)
        return false;

    int total_origen = vehiculo_origen->vehiculo->clientes_contados;

    // Evitar valores negativos en el rango para inicio_segmento
    if (total_origen < 1)
        return false;

    // Seleccionar tamaño segmento: 1 o 2 clientes
    int tamanio_segmento = 1 + (rand() % 2);

    // Ajustar tamaño si es mayor al total disponible
    if (tamanio_segmento > total_origen)
        tamanio_segmento = total_origen;

    // Seleccionar posición inicio del segmento en origen
    int max_inicio = total_origen - tamanio_segmento;
    if (max_inicio < 0)
        return false;

    int inicio_segmento = rand() % (max_inicio + 1); // 0 a max_inicio inclusive

    // Guardar clientes del segmento en array
    int *clientes_segmento = malloc(tamanio_segmento * sizeof(int));
    if (!clientes_segmento)
        return false;

    // Ubicar el nodo inicial en la ruta origen (primer cliente: cabeza->siguiente)
    nodo_ruta *nodo_temp = vehiculo_origen->vehiculo->ruta->cabeza->siguiente;
    for (int i = 0; i < inicio_segmento; i++)
        nodo_temp = nodo_temp->siguiente;

    for (int i = 0; i < tamanio_segmento; i++)
    {
        if (!nodo_temp)
        {
            free(clientes_segmento);
            return false;
        }
        clientes_segmento[i] = nodo_temp->cliente;
        nodo_temp = nodo_temp->siguiente;
    }

    // Eliminar segmento del vehículo origen (en orden inverso)
    for (int i = tamanio_segmento - 1; i >= 0; i--)
    {
        eliminar_cliente_ruta(vehiculo_origen->vehiculo, vrp, clientes_segmento[i], instancia_distancias);
    }

    int total_destino = vehiculo_destino->vehiculo->clientes_contados;

    // Posición de inserción en destino: 0 a total_destino inclusive
    int pos_insercion = rand() % (total_destino + 1);

    // Insertar segmento en destino en orden original
    bool factible = true;
    for (int i = 0; i < tamanio_segmento && factible; i++)
    {
        factible = insertarClienteEnPosicion(vehiculo_destino->vehiculo, vrp,
                                             clientes_segmento[i], pos_insercion + i, instancia_distancias);
    }

    if (factible)
    {
        // Verificar restricciones en ambos vehículos
        bool restricciones_ok = verificarRestricciones(vehiculo_origen->vehiculo, vrp, instancia_distancias) &&
                                verificarRestricciones(vehiculo_destino->vehiculo, vrp, instancia_distancias);

        if (restricciones_ok)
        {
            free(clientes_segmento);
            return true;
        }

        // Revertir: eliminar segmento insertado en destino
        for (int i = tamanio_segmento - 1; i >= 0; i--)
        {
            eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp, clientes_segmento[i], instancia_distancias);
        }
    }

    // Revertir: restaurar segmento en origen en la posición original
    for (int i = 0; i < tamanio_segmento; i++)
    {
        insertarClienteEnPosicion(vehiculo_origen->vehiculo, vrp,
                                  clientes_segmento[i], inicio_segmento + i, instancia_distancias);
    }

    free(clientes_segmento);
    return false;
}


bool or_opt(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    nodo_vehiculo *vehiculo_actual = NULL;
    int intentos_maximos = 10;

    // Seleccionar vehículo con al menos 2 clientes
    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        vehiculo_actual = seleccionar_vehiculo_aleatorio(hormiga);
        if (vehiculo_actual && vehiculo_actual->vehiculo &&
            vehiculo_actual->vehiculo->clientes_contados >= 2)
            break;
    }

    if (!vehiculo_actual || vehiculo_actual->vehiculo->clientes_contados < 2)
        return false;

    int total_clientes = vehiculo_actual->vehiculo->clientes_contados;

    // Seleccionar tamaño del segmento (1, 2 o 3 clientes)
    int tamanio_segmento = 1 + (rand() % 3);
    if (tamanio_segmento > total_clientes)
        tamanio_segmento = total_clientes;

    // Seleccionar posición inicial del segmento (0-based)
    int inicio_segmento = rand() % (total_clientes - tamanio_segmento + 1);

    // Seleccionar nueva posición (0 a total_clientes)
    int nueva_posicion;
    int intentos_posicion = 0;
    do
    {
        nueva_posicion = rand() % (total_clientes + 1); // desde 0 hasta total_clientes inclusive
        intentos_posicion++;

        if (intentos_posicion > 20)
            return false;

        // Evitar posiciones dentro del segmento o justo después cuando el segmento es todo el vehículo
    } while ((nueva_posicion >= inicio_segmento &&
              nueva_posicion < inicio_segmento + tamanio_segmento) ||
             (nueva_posicion == inicio_segmento + tamanio_segmento && tamanio_segmento == total_clientes));

    // Guardar segmento a mover
    int *clientes_segmento = malloc(tamanio_segmento * sizeof(int));
    if (!clientes_segmento)
        return false;

    // Validar que la lista no es NULL
    if (!vehiculo_actual->vehiculo->ruta || !vehiculo_actual->vehiculo->ruta->cabeza)
    {
        free(clientes_segmento);
        return false;
    }

    // Extraer clientes del segmento
    nodo_ruta *nodo_temp = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
    for (int i = 0; i < inicio_segmento; i++)
        nodo_temp = nodo_temp->siguiente;

    for (int i = 0; i < tamanio_segmento; i++)
    {
        if (!nodo_temp)
        {
            free(clientes_segmento);
            return false;
        }
        clientes_segmento[i] = nodo_temp->cliente;
        nodo_temp = nodo_temp->siguiente;
    }

    // Eliminar segmento de posición original
    for (int i = tamanio_segmento - 1; i >= 0; i--)
    {
        eliminar_cliente_ruta(vehiculo_actual->vehiculo, vrp,
                              clientes_segmento[i], instancia_distancias);
    }

    // Ajustar nueva posición si es necesario
    if (nueva_posicion > inicio_segmento)
        nueva_posicion -= tamanio_segmento;

    // Insertar segmento en nueva posición
    bool factible = true;
    for (int i = 0; i < tamanio_segmento && factible; i++)
    {
        factible = insertarClienteEnPosicion(vehiculo_actual->vehiculo, vrp,
                                             clientes_segmento[i],
                                             nueva_posicion + i, instancia_distancias);
    }

    if (factible)
    {
        // Verificar restricciones
        bool restricciones_ok = verificarRestricciones(vehiculo_actual->vehiculo,
                                                       vrp, instancia_distancias);
        if (restricciones_ok)
        {
            free(clientes_segmento);
            return true;
        }

        // Revertir: eliminar de nueva posición
        for (int i = tamanio_segmento - 1; i >= 0; i--)
        {
            eliminar_cliente_ruta(vehiculo_actual->vehiculo, vrp,
                                  clientes_segmento[i], instancia_distancias);
        }
    }

    // Restaurar en posición original (compensar indexación)
    for (int i = 0; i < tamanio_segmento; i++)
    {
        insertarClienteEnPosicion(vehiculo_actual->vehiculo, vrp,
                                  clientes_segmento[i],
                                  inicio_segmento + i, instancia_distancias);
    }

    free(clientes_segmento);
    return false;
}



// Cross-exchange - Intercambio de segmentos entre dos vehículos
bool cross_exchange(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    nodo_vehiculo *primer_vehiculo = NULL, *segundo_vehiculo = NULL;
    int intentos_maximos = 10;

    // Seleccionar dos vehículos diferentes con al menos 1 cliente cada uno
    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        primer_vehiculo = seleccionar_vehiculo_aleatorio(hormiga);
        segundo_vehiculo = seleccionar_vehiculo_aleatorio(hormiga);

        if (!primer_vehiculo || !segundo_vehiculo ||
            !primer_vehiculo->vehiculo || !segundo_vehiculo->vehiculo)
            continue;

        if (primer_vehiculo == segundo_vehiculo)
            continue;

        if (primer_vehiculo->vehiculo->clientes_contados >= 1 &&
            segundo_vehiculo->vehiculo->clientes_contados >= 1)
            break;
    }

    if (!primer_vehiculo || !segundo_vehiculo)
        return false;

    int total1 = primer_vehiculo->vehiculo->clientes_contados;
    int total2 = segundo_vehiculo->vehiculo->clientes_contados;

    // Tamaños de segmento (1 o 2 clientes) ajustados al total disponible
    int tamanio1 = 1 + (rand() % 2);
    int tamanio2 = 1 + (rand() % 2);

    if (tamanio1 > total1)
        tamanio1 = total1;
    if (tamanio2 > total2)
        tamanio2 = total2;

    // Posiciones de inicio (0-based)
    int inicio1 = rand() % (total1 - tamanio1 + 1);
    int inicio2 = rand() % (total2 - tamanio2 + 1);

    // Validar que las rutas no sean NULL
    if (!primer_vehiculo->vehiculo->ruta || !primer_vehiculo->vehiculo->ruta->cabeza ||
        !segundo_vehiculo->vehiculo->ruta || !segundo_vehiculo->vehiculo->ruta->cabeza)
        return false;

    // Reservar memoria para segmentos
    int *segmento1 = malloc(tamanio1 * sizeof(int));
    int *segmento2 = malloc(tamanio2 * sizeof(int));
    if (!segmento1 || !segmento2)
    {
        free(segmento1);
        free(segmento2);
        return false;
    }

    // Extraer segmento del primer vehículo
    nodo_ruta *nodo_temp = primer_vehiculo->vehiculo->ruta->cabeza->siguiente;
    for (int i = 0; i < inicio1; i++)
        nodo_temp = nodo_temp->siguiente;

    for (int i = 0; i < tamanio1; i++)
    {
        segmento1[i] = nodo_temp->cliente;
        nodo_temp = nodo_temp->siguiente;
    }

    // Extraer segmento del segundo vehículo
    nodo_temp = segundo_vehiculo->vehiculo->ruta->cabeza->siguiente;
    for (int i = 0; i < inicio2; i++)
        nodo_temp = nodo_temp->siguiente;

    for (int i = 0; i < tamanio2; i++)
    {
        segmento2[i] = nodo_temp->cliente;
        nodo_temp = nodo_temp->siguiente;
    }

    // Eliminar segmentos originales (desde el final hacia el inicio)
    for (int i = tamanio1 - 1; i >= 0; i--)
        eliminar_cliente_ruta(primer_vehiculo->vehiculo, vrp, segmento1[i], instancia_distancias);

    for (int i = tamanio2 - 1; i >= 0; i--)
        eliminar_cliente_ruta(segundo_vehiculo->vehiculo, vrp, segmento2[i], instancia_distancias);

    bool factible = true;

    // Insertar segmento2 en primer vehículo en la posición inicio1 (0-based)
    for (int i = 0; i < tamanio2 && factible; i++)
        factible = insertarClienteEnPosicion(primer_vehiculo->vehiculo, vrp, segmento2[i], inicio1 + i, instancia_distancias);

    // Insertar segmento1 en segundo vehículo en la posición inicio2 (0-based)
    for (int i = 0; i < tamanio1 && factible; i++)
        factible = insertarClienteEnPosicion(segundo_vehiculo->vehiculo, vrp, segmento1[i], inicio2 + i, instancia_distancias);

    if (factible)
    {
        // Verificar restricciones en ambos vehículos
        bool restricciones_ok = verificarRestricciones(primer_vehiculo->vehiculo, vrp, instancia_distancias) &&
                                verificarRestricciones(segundo_vehiculo->vehiculo, vrp, instancia_distancias);

        if (restricciones_ok)
        {
            free(segmento1);
            free(segmento2);
            return true;
        }

        // Revertir eliminación de segmentos intercambiados
        for (int i = tamanio2 - 1; i >= 0; i--)
            eliminar_cliente_ruta(primer_vehiculo->vehiculo, vrp, segmento2[i], instancia_distancias);

        for (int i = tamanio1 - 1; i >= 0; i--)
            eliminar_cliente_ruta(segundo_vehiculo->vehiculo, vrp, segmento1[i], instancia_distancias);
    }

    // Restaurar segmentos originales
    for (int i = 0; i < tamanio1; i++)
        insertarClienteEnPosicion(primer_vehiculo->vehiculo, vrp, segmento1[i], inicio1 + i, instancia_distancias);

    for (int i = 0; i < tamanio2; i++)
        insertarClienteEnPosicion(segundo_vehiculo->vehiculo, vrp, segmento2[i], inicio2 + i, instancia_distancias);

    free(segmento1);
    free(segmento2);
    return false;
}

// Relocate-chain - Relocación de cadena de clientes consecutivos (VERSIÓN CORREGIDA)
bool relocate_chain(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    nodo_vehiculo *vehiculo_origen = NULL, *vehiculo_destino = NULL;
    int intentos_maximos = 10;

    // 1. Seleccionar un vehículo origen que tenga al menos 2 clientes para poder mover una cadena
    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        vehiculo_origen = seleccionar_vehiculo_aleatorio(hormiga);
        if (vehiculo_origen && vehiculo_origen->vehiculo &&
            vehiculo_origen->vehiculo->clientes_contados >= 2)
            break;
    }
    // Si no se encontró vehículo origen adecuado, se aborta
    if (!vehiculo_origen || vehiculo_origen->vehiculo->clientes_contados < 2)
        return false;

    // 2. Seleccionar vehículo destino, puede ser el mismo u otro diferente
    vehiculo_destino = seleccionar_vehiculo_aleatorio(hormiga);
    if (!vehiculo_destino || !vehiculo_destino->vehiculo)
        return false;

    int total_origen = vehiculo_origen->vehiculo->clientes_contados;

    // 3. Calcular tamaño máximo posible de la cadena a mover
    // Se limita a un máximo de 4 clientes o la mitad de los clientes del vehículo origen
    int max_cadena = (total_origen / 2 > 4) ? 4 : total_origen / 2;
    if (max_cadena < 2)
        max_cadena = 2;

    // Si es el mismo vehículo origen y destino, no podemos mover toda la ruta, dejamos al menos 1 cliente
    if (vehiculo_origen == vehiculo_destino && max_cadena >= total_origen)
        max_cadena = total_origen - 1;

    if (max_cadena < 2)
        return false; // No es posible mover una cadena válida

    // 4. Elegir tamaño de la cadena aleatoriamente entre 2 y max_cadena
    int tamanio_cadena;
    if (max_cadena == 2)
        tamanio_cadena = 2;
    else
        tamanio_cadena = 2 + (rand() % (max_cadena - 2 + 1));

    // Ajustar si la cadena es demasiado grande para el total de clientes
    if (tamanio_cadena > total_origen - 1)
        tamanio_cadena = total_origen - 1;

    // 5. Seleccionar posición inicial segura para extraer la cadena (indexada desde 1)
    int max_inicio = total_origen - tamanio_cadena + 1;
    if (max_inicio <= 0)
        return false;

    int inicio_cadena = (rand() % max_inicio) + 1;

    // 6. Reservar memoria para almacenar la cadena de clientes y sus posiciones originales
    int *cadena_clientes = malloc(tamanio_cadena * sizeof(int));
    if (!cadena_clientes)
        return false;

    int *posiciones_originales = malloc(tamanio_cadena * sizeof(int));
    if (!posiciones_originales)
    {
        free(cadena_clientes);
        return false;
    }

    // 7. Navegar a la posición inicial en la ruta del vehículo origen
    nodo_ruta *nodo_temp = vehiculo_origen->vehiculo->ruta->cabeza->siguiente;
    for (int i = 1; i < inicio_cadena && nodo_temp; i++)
        nodo_temp = nodo_temp->siguiente;

    if (!nodo_temp)
    {
        free(cadena_clientes);
        free(posiciones_originales);
        return false;
    }

    // 8. Extraer clientes consecutivos formando la cadena
    for (int i = 0; i < tamanio_cadena && nodo_temp; i++)
    {
        cadena_clientes[i] = nodo_temp->cliente;
        posiciones_originales[i] = inicio_cadena + i;  // Guardar posición original para restaurar
        nodo_temp = nodo_temp->siguiente;
    }

    // 9. Eliminar la cadena del vehículo origen, desde la posición más alta para evitar invalidar la lista
    for (int i = tamanio_cadena - 1; i >= 0; i--)
    {
        eliminar_cliente_ruta(vehiculo_origen->vehiculo, vrp,
                              cadena_clientes[i], instancia_distancias);
    }

    // 10. Elegir posición aleatoria para insertar la cadena en el vehículo destino
    int total_destino = vehiculo_destino->vehiculo->clientes_contados;
    int pos_insercion = rand() % (total_destino + 1) + 1;

    // 11. Insertar la cadena en el vehículo destino manteniendo el orden
    bool factible = true;
    int clientes_insertados = 0;
    for (int i = 0; i < tamanio_cadena && factible; i++)
    {
        factible = insertarClienteEnPosicion(vehiculo_destino->vehiculo, vrp,
                                             cadena_clientes[i],
                                             pos_insercion + i, instancia_distancias);
        if (factible)
            clientes_insertados++;
    }

    // 12. Si la inserción fue factible, verificar restricciones de ambos vehículos
    if (factible)
    {
        bool restricciones_ok = verificarRestricciones(vehiculo_origen->vehiculo, vrp, instancia_distancias) &&
                                verificarRestricciones(vehiculo_destino->vehiculo, vrp, instancia_distancias);

        if (restricciones_ok)
        {
            // Éxito, liberar memoria y salir
            free(cadena_clientes);
            free(posiciones_originales);
            return true;
        }

        // Si restricciones no se cumplen, revertir cambios eliminando la cadena insertada
        for (int i = clientes_insertados - 1; i >= 0; i--)
        {
            eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp,
                                  cadena_clientes[i], instancia_distancias);
        }
    }
    else
    {
        // Si la inserción falló, eliminar clientes insertados parcialmente
        for (int i = clientes_insertados - 1; i >= 0; i--)
        {
            eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp,
                                  cadena_clientes[i], instancia_distancias);
        }
    }

    // 13. Restaurar la cadena en el vehículo origen, intentando insertarla en su posición original
    for (int i = 0; i < tamanio_cadena; i++)
    {
        bool restaurado = insertarClienteEnPosicion(vehiculo_origen->vehiculo, vrp,
                                                    cadena_clientes[i],
                                                    inicio_cadena + i, instancia_distancias);
        if (!restaurado)
        {
            // Si no se puede insertar en la posición original, insertarla al final
            insertarClienteEnPosicion(vehiculo_origen->vehiculo, vrp,
                                      cadena_clientes[i],
                                      vehiculo_origen->vehiculo->clientes_contados + 1,
                                      instancia_distancias);
        }
    }

    // 14. Liberar memoria y devolver fallo (false) porque no se pudo hacer la reubicación
    free(cadena_clientes);
    free(posiciones_originales);
    return false;
}

