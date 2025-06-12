#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/estructuras.h"
#include "../include/control_memoria.h"
#include "../include/lista_ruta.h"
#include "../include/lista_flota.h"
#include "../include/salida_datos.h"

// // Intercambia dos clientes aleatorios dentro de un mismo vehículo
bool swap_intra(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    int vehiculo_aleatorio, intentos_maximos = 10, cliente1_idx, cliente2_idx, temp;
    nodo_vehiculo *vehiculo_actual = NULL;
    nodo_ruta *nodo1 = NULL, *nodo2 = NULL;

    // Buscar un vehículo válido con al menos 2 clientes
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

        // Verificar que el vehículo existe y tiene al menos 2 clientes
        if (vehiculo_actual != NULL && vehiculo_actual->vehiculo->clientes_contados >= 2)
            break;
    }

    // Si no se encontró un vehículo válido después de todos los intentos
    if (vehiculo_actual == NULL || vehiculo_actual->vehiculo->clientes_contados < 2)
        return false;

    // Seleccionar dos clientes diferentes para intercambiar
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

    // Encontrar los nodos correspondientes a los índices seleccionados
    // Empezamos desde el primer cliente (después del depósito)
    nodo1 = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
    nodo2 = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;

    // Navegar hasta el primer cliente
    for (int i = 0; i < cliente1_idx; i++)
    {
        if (nodo1 == NULL)
            return false; // Verificación de seguridad
        nodo1 = nodo1->siguiente;
    }

    // Navegar hasta el segundo cliente
    for (int i = 0; i < cliente2_idx; i++)
    {
        if (nodo2 == NULL)
            return false; // Verificación de seguridad
        nodo2 = nodo2->siguiente;
    }

    // Verificación final de que ambos nodos existen
    if (nodo1 == NULL || nodo2 == NULL)
        return false;

    // Realizar el intercambio tentativo
    temp = nodo1->cliente;
    nodo1->cliente = nodo2->cliente;
    nodo2->cliente = temp;

    // Verificar si el intercambio mantiene la factibilidad
    if (!verificarRestricciones(vehiculo_actual->vehiculo, vrp, instancia_distancias))
    {
        // Revertir el intercambio si viola las restricciones
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

    // Seleccionar segmento a invertir (evitando depósitos)
    int idx1 = rand() % (total_clientes - 2) + 1; // Evita primer depósito
    int idx2 = rand() % (total_clientes - 2) + 1; // Evita último depósito

    if (idx1 == idx2)
        return false;

    if (idx1 > idx2)
    {
        int temp = idx1;
        idx1 = idx2;
        idx2 = temp;
    }

    // Guardar estado original
    int tamanio_segmento = idx2 - idx1 + 1;
    int *clientes_originales = asignar_memoria_arreglo_int(tamanio_segmento);
    if (clientes_originales == NULL)
        return false;

    // Crear array de punteros a nodos para acceso eficiente
    nodo_ruta **nodos = malloc(total_clientes * sizeof(nodo_ruta *));
    if (nodos == NULL)
    {
        liberar_memoria_arreglo_int(clientes_originales);
        return false;
    }

    // Llenar array de nodos
    nodo_ruta *nodo_temp = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
    for (int i = 0; i < total_clientes && nodo_temp; i++)
    {
        nodos[i] = nodo_temp;
        nodo_temp = nodo_temp->siguiente;
    }

    // Guardar estado original
    for (int i = 0; i < tamanio_segmento; i++)
    {
        clientes_originales[i] = nodos[idx1 + i]->cliente;
    }

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
        // Revertir cambios
        for (int i = 0; i < tamanio_segmento; i++)
        {
            nodos[idx1 + i]->cliente = clientes_originales[i];
        }
    }

    free(nodos);
    liberar_memoria_arreglo_int(clientes_originales);
    return factible;
}

// Versión corregida de swap_inter - Intercambio entre vehículos
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

    // CORRECCIÓN: Permitir intercambio en cualquier posición (incluso primera y última)
    int pos1 = rand() % total1 + 1; // Posición 1 a total1 (evita solo depósito inicial)
    int pos2 = rand() % total2 + 1; // Posición 1 a total2 (evita solo depósito inicial)

    // Navegar a posiciones
    nodo_ruta *nodo1 = primer_vehiculo->vehiculo->ruta->cabeza->siguiente;
    nodo_ruta *nodo2 = segundo_vehiculo->vehiculo->ruta->cabeza->siguiente;

    for (int i = 1; i < pos1 && nodo1; i++)
        nodo1 = nodo1->siguiente;
    for (int i = 1; i < pos2 && nodo2; i++)
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

// Versión corregida de reinsercion_intra_inter
bool reinsercion_intra_inter(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    nodo_vehiculo *vehiculo_origen = NULL, *vehiculo_destino = NULL;
    int intentos_maximos = 10;

    // Seleccionar vehículos
    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        vehiculo_origen = seleccionar_vehiculo_aleatorio(hormiga);
        vehiculo_destino = seleccionar_vehiculo_aleatorio(hormiga);

        if (!vehiculo_origen || !vehiculo_destino ||
            !vehiculo_origen->vehiculo || !vehiculo_destino->vehiculo)
            continue;

        // Vehículo origen debe tener al menos 1 cliente
        if (vehiculo_origen->vehiculo->clientes_contados < 1)
            continue;

        // Si es el mismo vehículo, debe tener al menos 2 clientes
        if (vehiculo_origen == vehiculo_destino &&
            vehiculo_origen->vehiculo->clientes_contados < 2)
            continue;

        break;
    }

    if (!vehiculo_origen || !vehiculo_destino)
        return false;

    int total_origen = vehiculo_origen->vehiculo->clientes_contados;
    int total_destino = vehiculo_destino->vehiculo->clientes_contados;

    // CORRECCIÓN: Permitir selección de cualquier cliente real
    int pos_origen = rand() % total_origen + 1; // Posición 1 a total_origen

    // Navegar al cliente origen
    nodo_ruta *nodo_cliente = vehiculo_origen->vehiculo->ruta->cabeza->siguiente;
    for (int i = 1; i < pos_origen && nodo_cliente; i++)
        nodo_cliente = nodo_cliente->siguiente;

    if (!nodo_cliente || nodo_cliente->cliente == 0)
        return false;

    int cliente = nodo_cliente->cliente;

    // Posición destino: después del depósito inicial hasta después del último cliente
    int pos_destino = rand() % (total_destino + 1) + 1; // Posición 1 a total_destino+1

    // Eliminar cliente de origen
    eliminar_cliente_ruta(vehiculo_origen->vehiculo, vrp, cliente, instancia_distancias);

    // Insertar en destino
    bool factible = insertarClienteEnPosicion(vehiculo_destino->vehiculo, vrp, cliente,
                                              pos_destino, instancia_distancias);

    if (factible)
    {
        // Verificar restricciones en ambos vehículos
        bool restricciones_ok = verificarRestricciones(vehiculo_origen->vehiculo, vrp, instancia_distancias) &&
                                verificarRestricciones(vehiculo_destino->vehiculo, vrp, instancia_distancias);

        if (restricciones_ok)
        {
            return true;
        }

        // Revertir: eliminar de destino
        eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp, cliente, instancia_distancias);
    }

    // Revertir: restaurar en origen
    insertarClienteEnPosicion(vehiculo_origen->vehiculo, vrp, cliente, pos_origen, instancia_distancias);
    return false;
}

// Versión completa de opt_2_5 - Relocación de segmento
bool opt_2_5(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    int intentos_maximos = 10;
    struct nodo_vehiculo *vehiculo_origen = NULL, *vehiculo_destino = NULL;

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
    if (!vehiculo_destino)
        return false;

    int total_origen = vehiculo_origen->vehiculo->clientes_contados;

    // Seleccionar segmento de 1-2 clientes
    int inicio_segmento = rand() % total_origen + 1; // Posición 1 a total_origen
    int tamanio_segmento = 1 + (rand() % 2);         // 1 o 2 clientes

    // Ajustar tamaño si excede límites
    if (inicio_segmento + tamanio_segmento - 1 > total_origen)
        tamanio_segmento = total_origen - inicio_segmento + 1;

    // Guardar clientes del segmento
    int *clientes_segmento = malloc(tamanio_segmento * sizeof(int));
    if (!clientes_segmento)
        return false;

    nodo_ruta *nodo_temp = vehiculo_origen->vehiculo->ruta->cabeza->siguiente;
    for (int i = 1; i < inicio_segmento; i++)
        nodo_temp = nodo_temp->siguiente;

    for (int i = 0; i < tamanio_segmento; i++)
    {
        clientes_segmento[i] = nodo_temp->cliente;
        nodo_temp = nodo_temp->siguiente;
    }

    // Eliminar segmento del origen
    for (int i = tamanio_segmento - 1; i >= 0; i--)
    {
        eliminar_cliente_ruta(vehiculo_origen->vehiculo, vrp, clientes_segmento[i], instancia_distancias);
    }

    // Seleccionar posición de inserción en destino
    int total_destino = vehiculo_destino->vehiculo->clientes_contados;
    int pos_insercion = rand() % (total_destino + 1) + 1;

    // Insertar segmento en destino
    bool factible = true;
    for (int i = 0; i < tamanio_segmento && factible; i++)
    {
        factible = insertarClienteEnPosicion(vehiculo_destino->vehiculo, vrp,
                                             clientes_segmento[i], pos_insercion + i, instancia_distancias);
    }

    if (factible)
    {
        // Verificar restricciones
        bool restricciones_ok = verificarRestricciones(vehiculo_origen->vehiculo, vrp, instancia_distancias) &&
                                verificarRestricciones(vehiculo_destino->vehiculo, vrp, instancia_distancias);

        if (restricciones_ok)
        {
            free(clientes_segmento);
            return true;
        }

        // Revertir: eliminar de destino
        for (int i = tamanio_segmento - 1; i >= 0; i--)
        {
            eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp, clientes_segmento[i], instancia_distancias);
        }
    }

    // Revertir: restaurar en origen
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

    // Seleccionar posición inicial del segmento
    int inicio_segmento = rand() % (total_clientes - tamanio_segmento + 1) + 1;

    // Guardar segmento a mover
    int *clientes_segmento = malloc(tamanio_segmento * sizeof(int));
    if (!clientes_segmento)
        return false;

    // Seleccionar nueva posición (debe ser diferente del rango actual)
    int nueva_posicion;
    int intentos_posicion = 0;
    do
    {
        nueva_posicion = rand() % (total_clientes + 1) + 1; // Permite insertar al final
        intentos_posicion++;

        // Evitar bucle infinito si no hay posiciones válidas
        if (intentos_posicion > 20)
        {
            free(clientes_segmento);
            return false;
        }
    } while ((nueva_posicion >= inicio_segmento &&
              nueva_posicion < inicio_segmento + tamanio_segmento) ||
             (nueva_posicion == inicio_segmento + tamanio_segmento && tamanio_segmento == total_clientes));

    // Guardar segmento a mover
    clientes_segmento = malloc(tamanio_segmento * sizeof(int));

    // Extraer clientes del segmento
    nodo_ruta *nodo_temp = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
    for (int i = 1; i < inicio_segmento; i++)
        nodo_temp = nodo_temp->siguiente;

    for (int i = 0; i < tamanio_segmento; i++)
    {
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

    // Restaurar en posición original
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

    // Seleccionar tamaños de segmentos (1-2 clientes cada uno)
    int tamanio1 = 1 + (rand() % 2);
    int tamanio2 = 1 + (rand() % 2);

    if (tamanio1 > total1)
        tamanio1 = total1;
    if (tamanio2 > total2)
        tamanio2 = total2;

    // Seleccionar posiciones de inicio
    int inicio1 = rand() % (total1 - tamanio1 + 1) + 1;
    int inicio2 = rand() % (total2 - tamanio2 + 1) + 1;

    // Guardar segmentos
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
    for (int i = 1; i < inicio1; i++)
        nodo_temp = nodo_temp->siguiente;

    for (int i = 0; i < tamanio1; i++)
    {
        segmento1[i] = nodo_temp->cliente;
        nodo_temp = nodo_temp->siguiente;
    }

    // Extraer segmento del segundo vehículo
    nodo_temp = segundo_vehiculo->vehiculo->ruta->cabeza->siguiente;
    for (int i = 1; i < inicio2; i++)
        nodo_temp = nodo_temp->siguiente;

    for (int i = 0; i < tamanio2; i++)
    {
        segmento2[i] = nodo_temp->cliente;
        nodo_temp = nodo_temp->siguiente;
    }

    // Eliminar segmentos de sus vehículos originales
    for (int i = tamanio1 - 1; i >= 0; i--)
    {
        eliminar_cliente_ruta(primer_vehiculo->vehiculo, vrp,
                              segmento1[i], instancia_distancias);
    }
    for (int i = tamanio2 - 1; i >= 0; i--)
    {
        eliminar_cliente_ruta(segundo_vehiculo->vehiculo, vrp,
                              segmento2[i], instancia_distancias);
    }

    // Insertar segmentos intercambiados
    bool factible = true;

    // Insertar segmento2 en primer vehículo
    for (int i = 0; i < tamanio2 && factible; i++)
    {
        factible = insertarClienteEnPosicion(primer_vehiculo->vehiculo, vrp,
                                             segmento2[i], inicio1 + i, instancia_distancias);
    }

    // Insertar segmento1 en segundo vehículo
    for (int i = 0; i < tamanio1 && factible; i++)
    {
        factible = insertarClienteEnPosicion(segundo_vehiculo->vehiculo, vrp,
                                             segmento1[i], inicio2 + i, instancia_distancias);
    }

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

        // Revertir: eliminar segmentos intercambiados
        for (int i = tamanio2 - 1; i >= 0; i--)
        {
            eliminar_cliente_ruta(primer_vehiculo->vehiculo, vrp,
                                  segmento2[i], instancia_distancias);
        }
        for (int i = tamanio1 - 1; i >= 0; i--)
        {
            eliminar_cliente_ruta(segundo_vehiculo->vehiculo, vrp,
                                  segmento1[i], instancia_distancias);
        }
    }

    // Restaurar segmentos originales
    for (int i = 0; i < tamanio1; i++)
    {
        insertarClienteEnPosicion(primer_vehiculo->vehiculo, vrp,
                                  segmento1[i], inicio1 + i, instancia_distancias);
    }
    for (int i = 0; i < tamanio2; i++)
    {
        insertarClienteEnPosicion(segundo_vehiculo->vehiculo, vrp,
                                  segmento2[i], inicio2 + i, instancia_distancias);
    }

    free(segmento1);
    free(segmento2);
    return false;
}

// Relocate-chain - Relocación de cadena de clientes consecutivos
bool relocate_chain(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    nodo_vehiculo *vehiculo_origen = NULL, *vehiculo_destino = NULL;
    int intentos_maximos = 10;

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

    // Seleccionar vehículo destino (puede ser diferente o el mismo)
    vehiculo_destino = seleccionar_vehiculo_aleatorio(hormiga);
    if (!vehiculo_destino)
        return false;

    int total_origen = vehiculo_origen->vehiculo->clientes_contados;

    // Seleccionar tamaño de la cadena (2-4 clientes o hasta la mitad del total)
    int max_cadena = (total_origen / 2 > 4) ? 4 : total_origen / 2;
    if (max_cadena < 2)
        max_cadena = 2;

    int tamanio_cadena = 2 + (rand() % (max_cadena - 1));
    if (tamanio_cadena > total_origen - 1) // Dejar al menos 1 cliente en origen
        tamanio_cadena = total_origen - 1;

    // Seleccionar posición inicial de la cadena
    int inicio_cadena = rand() % (total_origen - tamanio_cadena + 1) + 1;

    // Si es el mismo vehículo, debe quedar al menos 1 cliente después del movimiento
    if (vehiculo_origen == vehiculo_destino && tamanio_cadena >= total_origen)
        return false;

    // Guardar cadena de clientes
    int *cadena_clientes = malloc(tamanio_cadena * sizeof(int));
    if (!cadena_clientes)
        return false;

    // Extraer clientes de la cadena
    nodo_ruta *nodo_temp = vehiculo_origen->vehiculo->ruta->cabeza->siguiente;
    for (int i = 1; i < inicio_cadena; i++)
        nodo_temp = nodo_temp->siguiente;

    for (int i = 0; i < tamanio_cadena; i++)
    {
        cadena_clientes[i] = nodo_temp->cliente;
        nodo_temp = nodo_temp->siguiente;
    }

    // Eliminar cadena del vehículo origen
    for (int i = tamanio_cadena - 1; i >= 0; i--)
    {
        eliminar_cliente_ruta(vehiculo_origen->vehiculo, vrp,
                              cadena_clientes[i], instancia_distancias);
    }

    // Seleccionar posición de inserción en destino
    int total_destino = vehiculo_destino->vehiculo->clientes_contados;
    int pos_insercion = rand() % (total_destino + 1) + 1;

    // Insertar cadena en vehículo destino manteniendo el orden
    bool factible = true;
    for (int i = 0; i < tamanio_cadena && factible; i++)
    {
        factible = insertarClienteEnPosicion(vehiculo_destino->vehiculo, vrp,
                                             cadena_clientes[i],
                                             pos_insercion + i, instancia_distancias);
    }

    if (factible)
    {
        // Verificar restricciones en ambos vehículos
        bool restricciones_ok = verificarRestricciones(vehiculo_origen->vehiculo, vrp, instancia_distancias) &&
                                verificarRestricciones(vehiculo_destino->vehiculo, vrp, instancia_distancias);

        if (restricciones_ok)
        {
            free(cadena_clientes);
            return true;
        }

        // Revertir: eliminar cadena del destino
        for (int i = tamanio_cadena - 1; i >= 0; i--)
        {
            eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp,
                                  cadena_clientes[i], instancia_distancias);
        }
    }

    // Restaurar cadena en vehículo origen
    for (int i = 0; i < tamanio_cadena; i++)
    {
        insertarClienteEnPosicion(vehiculo_origen->vehiculo, vrp,
                                  cadena_clientes[i],
                                  inicio_cadena + i, instancia_distancias);
    }

    free(cadena_clientes);
    return false;
}