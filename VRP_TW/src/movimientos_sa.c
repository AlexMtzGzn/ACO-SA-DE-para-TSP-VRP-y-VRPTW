#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/estructuras.h"
#include "../include/control_memoria.h"
#include "../include/lista_ruta.h"
#include "../include/lista_flota.h"
#include "../include/salida_datos.h"

bool swap_intra(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    int vehiculo_aleatorio, intentos_maximos = 10, cliente1_idx, cliente2_idx, temp;
    nodo_vehiculo *vehiculo_actual = NULL;
    nodo_ruta *nodo1 = NULL, *nodo2 = NULL;

    // Seleccionar vehículo con al menos 2 clientes
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

    // Seleccionar dos posiciones diferentes (0-based)
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

    // Navegar a las posiciones (saltando el depósito inicial)
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

    // Verificar que no sean depósitos
    if (nodo1->cliente == 0 || nodo2->cliente == 0)
        return false;

    // Realizar intercambio
    temp = nodo1->cliente;
    nodo1->cliente = nodo2->cliente;
    nodo2->cliente = temp;

    // Verificar restricciones
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

// 2-opt: Invierte un segmento de la ruta
// Ruta ejemplo: 0-6-8-9-5-0 → puede invertir [8,9] → 0-6-9-8-5-0
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

    // Seleccionar dos índices aleatorios (0-based, excluyendo último para tener segmento válido)
    int idx1 = rand() % (total_clientes - 1);                     // 0 a total_clientes-2
    int idx2 = (rand() % (total_clientes - idx1 - 1)) + idx1 + 1; // idx1+1 a total_clientes-1

    int tamanio_segmento = idx2 - idx1 + 1;
    if (tamanio_segmento < 2)
        return false;

    // Asignar memoria
    int *clientes_originales = asignar_memoria_arreglo_int(tamanio_segmento);
    nodo_ruta **nodos = asignar_memoria_arreglo_nodo_ruta(total_clientes);

    // Llenar array de nodos (saltando depósito inicial)
    nodo_ruta *nodo_temp = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
    int count = 0;
    while (nodo_temp && count < total_clientes)
    {
        nodos[count++] = nodo_temp;
        nodo_temp = nodo_temp->siguiente;
    }

    if (count < total_clientes || idx2 >= count)
    {
        liberar_memoria_arreglo_nodos_ruta(nodos);
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

    liberar_memoria_arreglo_nodos_ruta(nodos);
    liberar_memoria_arreglo_int(clientes_originales);
    return factible;
}

// Intercambio entre vehículos
// Intercambia un cliente de un vehículo con un cliente de otro vehículo
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

    // Seleccionar posiciones aleatorias (0-based)
    int pos1 = rand() % total1;
    int pos2 = rand() % total2;

    // Navegar a posiciones (saltando depósito inicial)
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
// Mueve un cliente de una posición a otra, dentro del mismo vehículo o entre vehículos
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

        // Si es el mismo vehículo, necesita al menos 2 clientes
        if (vehiculo_origen == vehiculo_destino &&
            vehiculo_origen->vehiculo->clientes_contados < 2)
            continue;

        break;
    }

    if (!vehiculo_origen || !vehiculo_destino)
        return false;

    int total_origen = vehiculo_origen->vehiculo->clientes_contados;

    if (total_origen == 0)
        return false;

    // Seleccionar cliente a mover (0-based)
    int pos_origen = rand() % total_origen;

    nodo_ruta *nodo_cliente = vehiculo_origen->vehiculo->ruta->cabeza->siguiente;
    for (int i = 0; i < pos_origen && nodo_cliente; i++)
        nodo_cliente = nodo_cliente->siguiente;

    if (!nodo_cliente || nodo_cliente->cliente == 0)
        return false;

    int cliente = nodo_cliente->cliente;

    // Eliminar cliente del vehículo origen
    eliminar_cliente_ruta(vehiculo_origen->vehiculo, vrp, cliente, instancia_distancias);

    // Seleccionar posición destino (0-based, puede ser al final)
    int total_destino_actual = vehiculo_destino->vehiculo->clientes_contados;
    int pos_destino = rand() % (total_destino_actual + 1);

    // Convertir a 1-based si insertarClienteEnPosicion lo requiere
    bool factible = insertarClienteEnPosicion(vehiculo_destino->vehiculo, vrp, cliente,
                                              pos_destino + 1, instancia_distancias);

    if (factible)
    {
        bool restricciones_ok = verificarRestricciones(vehiculo_origen->vehiculo, vrp, instancia_distancias) &&
                                verificarRestricciones(vehiculo_destino->vehiculo, vrp, instancia_distancias);

        if (restricciones_ok)
        {
            return true;
        }

        // Revertir: eliminar de destino
        eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp, cliente, instancia_distancias);
    }

    // Restaurar en origen
    insertarClienteEnPosicion(vehiculo_origen->vehiculo, vrp, cliente, pos_origen + 1, instancia_distancias);
    return false;
}

// Relocación de segmento entre vehículos (2-opt con 5)
// Mueve un segmento de 1-2 clientes de un vehículo a otro
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

    // Seleccionar tamaño segmento: 1 o 2 clientes
    int tamanio_segmento = 1 + (rand() % 2);
    if (tamanio_segmento > total_origen)
        tamanio_segmento = total_origen;

    // Seleccionar posición inicio del segmento (0-based)
    int max_inicio = total_origen - tamanio_segmento;
    if (max_inicio < 0)
        return false;

    int inicio_segmento = rand() % (max_inicio + 1);

    // Guardar clientes del segmento
    int *clientes_segmento = asignar_memoria_arreglo_int(tamanio_segmento);

    // Extraer clientes del segmento
    nodo_ruta *nodo_temp = vehiculo_origen->vehiculo->ruta->cabeza->siguiente;
    for (int i = 0; i < inicio_segmento; i++)
        nodo_temp = nodo_temp->siguiente;

    for (int i = 0; i < tamanio_segmento; i++)
    {
        if (!nodo_temp)
        {
            liberar_memoria_arreglo_int(clientes_segmento);
            return false;
        }
        clientes_segmento[i] = nodo_temp->cliente;
        nodo_temp = nodo_temp->siguiente;
    }

    // Eliminar segmento del vehículo origen (orden inverso)
    for (int i = tamanio_segmento - 1; i >= 0; i--)
    {
        eliminar_cliente_ruta(vehiculo_origen->vehiculo, vrp, clientes_segmento[i], instancia_distancias);
    }

    int total_destino = vehiculo_destino->vehiculo->clientes_contados;

    // Posición de inserción (0-based, puede ser al final)
    int pos_insercion = rand() % (total_destino + 1);

    // Insertar segmento en destino
    bool factible = true;
    for (int i = 0; i < tamanio_segmento && factible; i++)
    {
        factible = insertarClienteEnPosicion(vehiculo_destino->vehiculo, vrp,
                                             clientes_segmento[i], pos_insercion + i + 1, instancia_distancias);
    }

    if (factible)
    {
        // Verificar restricciones
        bool restricciones_ok = verificarRestricciones(vehiculo_origen->vehiculo, vrp, instancia_distancias) &&
                                verificarRestricciones(vehiculo_destino->vehiculo, vrp, instancia_distancias);

        if (restricciones_ok)
        {
            liberar_memoria_arreglo_int(clientes_segmento);
            return true;
        }

        // Revertir: eliminar segmento de destino
        for (int i = tamanio_segmento - 1; i >= 0; i--)
        {
            eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp, clientes_segmento[i], instancia_distancias);
        }
    }

    // Restaurar segmento en origen
    for (int i = 0; i < tamanio_segmento; i++)
    {
        insertarClienteEnPosicion(vehiculo_origen->vehiculo, vrp,
                                  clientes_segmento[i], inicio_segmento + i + 1, instancia_distancias);
    }

    liberar_memoria_arreglo_int(clientes_segmento);
    return false;
}

// Or-opt: Mueve un segmento dentro del mismo vehículo
// Ruta ejemplo: 0-6-8-9-5-0 → mover [8,9] después de 5 → 0-6-5-8-9-0
bool or_opt(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    nodo_vehiculo *vehiculo_actual = NULL;
    int intentos_maximos = 10;

    // Seleccionar vehículo con al menos 3 clientes
    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        vehiculo_actual = seleccionar_vehiculo_aleatorio(hormiga);
        if (vehiculo_actual && vehiculo_actual->vehiculo &&
            vehiculo_actual->vehiculo->clientes_contados >= 3)
            break;
    }

    if (!vehiculo_actual || vehiculo_actual->vehiculo->clientes_contados < 3)
        return false;

    int total_clientes = vehiculo_actual->vehiculo->clientes_contados;

    // Seleccionar tamaño del segmento (1, 2 o 3 clientes)
    int tamanio_segmento = 1 + (rand() % 3);
    if (tamanio_segmento >= total_clientes) // Dejar al menos 1 cliente
        tamanio_segmento = total_clientes - 1;

    if (tamanio_segmento < 1)
        return false;

    // Seleccionar posición inicial del segmento (0-based)
    int inicio_segmento = rand() % (total_clientes - tamanio_segmento + 1);

    // Seleccionar nueva posición
    int nueva_posicion;
    int intentos_posicion = 0;
    do
    {
        nueva_posicion = rand() % (total_clientes - tamanio_segmento + 1);
        intentos_posicion++;

        if (intentos_posicion > 20)
            return false;

    } while (nueva_posicion == inicio_segmento ||
             (nueva_posicion > inicio_segmento && nueva_posicion < inicio_segmento + tamanio_segmento));

    // Guardar segmento a mover
    int *clientes_segmento = asignar_memoria_arreglo_int(tamanio_segmento);

    // Validar ruta
    if (!vehiculo_actual->vehiculo->ruta || !vehiculo_actual->vehiculo->ruta->cabeza)
    {
        liberar_memoria_arreglo_int(clientes_segmento);
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
            liberar_memoria_arreglo_int(clientes_segmento);
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

    // Insertar segmento en nueva posición (convertir a 1-based)
    bool factible = true;
    for (int i = 0; i < tamanio_segmento && factible; i++)
    {
        factible = insertarClienteEnPosicion(vehiculo_actual->vehiculo, vrp,
                                             clientes_segmento[i],
                                             nueva_posicion + i + 1, instancia_distancias);
    }

    if (factible)
    {
        // Verificar restricciones
        bool restricciones_ok = verificarRestricciones(vehiculo_actual->vehiculo,
                                                       vrp, instancia_distancias);
        if (restricciones_ok)
        {
            liberar_memoria_arreglo_int(clientes_segmento);
            return true;
        }

        // Revertir: eliminar de nueva posición
        for (int i = tamanio_segmento - 1; i >= 0; i--)
        {
            eliminar_cliente_ruta(vehiculo_actual->vehiculo, vrp,
                                  clientes_segmento[i], instancia_distancias);
        }
    }

    // Restaurar en posición original
    for (int i = 0; i < tamanio_segmento; i++)
    {
        insertarClienteEnPosicion(vehiculo_actual->vehiculo, vrp,
                                  clientes_segmento[i],
                                  inicio_segmento + i + 1, instancia_distancias);
    }

    liberar_memoria_arreglo_int(clientes_segmento);
    return false;
}

// Cross-exchange CORREGIDO - Intercambio de segmentos entre dos vehículos
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

    // CORRECCIÓN: Posiciones de inicio 1-based para consistencia con insertarClienteEnPosicion
    int inicio1 = (rand() % (total1 - tamanio1 + 1)) + 1; // 1-based
    int inicio2 = (rand() % (total2 - tamanio2 + 1)) + 1; // 1-based

    // Validar que las rutas no sean NULL
    if (!primer_vehiculo->vehiculo->ruta || !primer_vehiculo->vehiculo->ruta->cabeza ||
        !segundo_vehiculo->vehiculo->ruta || !segundo_vehiculo->vehiculo->ruta->cabeza)
        return false;

    // Reservar memoria para segmentos
    int *segmento1 = asignar_memoria_arreglo_int(tamanio1);
    int *segmento2 = asignar_memoria_arreglo_int(tamanio2);

    // CORRECCIÓN: Extraer segmento del primer vehículo (navegación 1-based)
    nodo_ruta *nodo_temp = primer_vehiculo->vehiculo->ruta->cabeza->siguiente;
    for (int i = 1; i < inicio1; i++) // Navegar hasta posición inicio1
        nodo_temp = nodo_temp->siguiente;

    for (int i = 0; i < tamanio1; i++)
    {
        segmento1[i] = nodo_temp->cliente;
        nodo_temp = nodo_temp->siguiente;
    }

    // CORRECCIÓN: Extraer segmento del segundo vehículo (navegación 1-based)
    nodo_temp = segundo_vehiculo->vehiculo->ruta->cabeza->siguiente;
    for (int i = 1; i < inicio2; i++) // Navegar hasta posición inicio2
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

    // CORRECCIÓN: Insertar segmento2 en primer vehículo en la posición inicio1 (1-based)
    for (int i = 0; i < tamanio2 && factible; i++)
        factible = insertarClienteEnPosicion(primer_vehiculo->vehiculo, vrp, segmento2[i], inicio1 + i, instancia_distancias);

    // CORRECCIÓN: Insertar segmento1 en segundo vehículo en la posición inicio2 (1-based)
    for (int i = 0; i < tamanio1 && factible; i++)
        factible = insertarClienteEnPosicion(segundo_vehiculo->vehiculo, vrp, segmento1[i], inicio2 + i, instancia_distancias);

    if (factible)
    {
        // Verificar restricciones en ambos vehículos
        bool restricciones_ok = verificarRestricciones(primer_vehiculo->vehiculo, vrp, instancia_distancias) &&
                                verificarRestricciones(segundo_vehiculo->vehiculo, vrp, instancia_distancias);

        if (restricciones_ok)
        {
            liberar_memoria_arreglo_int(segmento1);
            liberar_memoria_arreglo_int(segmento2);
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

    liberar_memoria_arreglo_int(segmento1);
    liberar_memoria_arreglo_int(segmento2);
    return false;
}

// Relocate-chain CORREGIDO - Relocación de cadena de clientes consecutivos
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

    if (!vehiculo_origen || vehiculo_origen->vehiculo->clientes_contados < 2)
        return false;

    // 2. Seleccionar vehículo destino
    vehiculo_destino = seleccionar_vehiculo_aleatorio(hormiga);
    if (!vehiculo_destino || !vehiculo_destino->vehiculo)
        return false;

    int total_origen = vehiculo_origen->vehiculo->clientes_contados;

    // 3. Calcular tamaño máximo de la cadena
    int max_cadena = (total_origen / 2 > 4) ? 4 : total_origen / 2;
    if (max_cadena < 2)
        max_cadena = 2;

    // Si es el mismo vehículo, no mover toda la ruta
    if (vehiculo_origen == vehiculo_destino && max_cadena >= total_origen)
        max_cadena = total_origen - 1;

    if (max_cadena < 2)
        return false;

    // 4. Elegir tamaño de la cadena
    int tamanio_cadena = (max_cadena == 2) ? 2 : 2 + (rand() % (max_cadena - 2 + 1));

    if (tamanio_cadena > total_origen - 1)
        tamanio_cadena = total_origen - 1;

    // 5. CORRECCIÓN: Seleccionar posición inicial (1-based) para consistencia
    int max_inicio = total_origen - tamanio_cadena + 1;
    if (max_inicio <= 0)
        return false;

    int inicio_cadena = (rand() % max_inicio) + 1; // 1-based

    // 6. Reservar memoria
    int *cadena_clientes = asignar_memoria_arreglo_int(tamanio_cadena);

    int *posiciones_originales = asignar_memoria_arreglo_int(tamanio_cadena);

    // 7. CORRECCIÓN: Navegar a la posición inicial (1-based)
    nodo_ruta *nodo_temp = vehiculo_origen->vehiculo->ruta->cabeza->siguiente;
    for (int i = 1; i < inicio_cadena && nodo_temp; i++) // Navegación 1-based
        nodo_temp = nodo_temp->siguiente;

    if (!nodo_temp)
    {
        liberar_memoria_arreglo_int(cadena_clientes);
        liberar_memoria_arreglo_int(posiciones_originales);
        return false;
    }

    // 8. Extraer clientes consecutivos
    for (int i = 0; i < tamanio_cadena && nodo_temp; i++)
    {
        cadena_clientes[i] = nodo_temp->cliente;
        posiciones_originales[i] = inicio_cadena + i; // Posición original 1-based
        nodo_temp = nodo_temp->siguiente;
    }

    // 9. Eliminar la cadena del vehículo origen
    for (int i = tamanio_cadena - 1; i >= 0; i--)
    {
        eliminar_cliente_ruta(vehiculo_origen->vehiculo, vrp,
                              cadena_clientes[i], instancia_distancias);
    }

    // 10. Elegir posición de inserción (1-based)
    int total_destino = vehiculo_destino->vehiculo->clientes_contados;
    int pos_insercion = rand() % (total_destino + 1) + 1; // 1-based

    // 11. Insertar la cadena manteniendo el orden
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

    // 12. Verificar factibilidad y restricciones
    if (factible)
    {
        bool restricciones_ok = verificarRestricciones(vehiculo_origen->vehiculo, vrp, instancia_distancias) &&
                                verificarRestricciones(vehiculo_destino->vehiculo, vrp, instancia_distancias);

        if (restricciones_ok)
        {
            liberar_memoria_arreglo_int(cadena_clientes);
            liberar_memoria_arreglo_int(posiciones_originales);
            return true;
        }

        // Revertir inserción
        for (int i = clientes_insertados - 1; i >= 0; i--)
        {
            eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp,
                                  cadena_clientes[i], instancia_distancias);
        }
    }
    else
    {
        // Eliminar clientes insertados parcialmente
        for (int i = clientes_insertados - 1; i >= 0; i--)
        {
            eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp,
                                  cadena_clientes[i], instancia_distancias);
        }
    }

    // 13. Restaurar la cadena en el vehículo origen
    for (int i = 0; i < tamanio_cadena; i++)
    {
        bool restaurado = insertarClienteEnPosicion(vehiculo_origen->vehiculo, vrp,
                                                    cadena_clientes[i],
                                                    posiciones_originales[i], instancia_distancias);
        if (!restaurado)
        {
            // Si falla, insertar al final
            insertarClienteEnPosicion(vehiculo_origen->vehiculo, vrp,
                                      cadena_clientes[i],
                                      vehiculo_origen->vehiculo->clientes_contados + 1,
                                      instancia_distancias);
        }
    }

    liberar_memoria_arreglo_int(cadena_clientes);
    liberar_memoria_arreglo_int(posiciones_originales);
    return false;
}