#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/estructuras.h"
#include "../include/control_memoria.h"
#include "../include/lista_ruta.h"
#include "../include/lista_flota.h"
#include "../include/salida_datos.h"

// Invierte un segmento de la ruta
bool opt_2(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    nodo_vehiculo *vehiculo_actual = NULL;
    int intentos_maximos = 10;

    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        vehiculo_actual = seleccionar_vehiculo_aleatorio(hormiga);
        if (vehiculo_actual && vehiculo_actual->vehiculo && vehiculo_actual->vehiculo->clientes_contados >= 3)
            break;
    }

    if (vehiculo_actual == NULL || vehiculo_actual->vehiculo == NULL || vehiculo_actual->vehiculo->clientes_contados < 3)
        return false;

    int idx1, idx2;

    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        if (vehiculo_actual->vehiculo->clientes_contados == 3)
        {
            idx1 = 0;
            idx2 = 2;
        }
        else
        {
            idx1 = rand() % vehiculo_actual->vehiculo->clientes_contados;
            idx2 = rand() % vehiculo_actual->vehiculo->clientes_contados;

            while (idx1 == idx2)
                idx2 = rand() % vehiculo_actual->vehiculo->clientes_contados;

            if (idx1 > idx2)
            {
                int tmp = idx1;
                idx1 = idx2;
                idx2 = tmp;
            }
        }

        int tamanio_segmento = idx2 - idx1 + 1;
        int *clientes_originales = asignar_memoria_arreglo_int(tamanio_segmento);

        if (clientes_originales == NULL)
            return false;

        nodo_ruta *nodo_temp = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;

        for (int i = 0; i < idx1; i++)
            nodo_temp = nodo_temp->siguiente;

        for (int i = 0; i < tamanio_segmento; i++)
        {
            clientes_originales[i] = nodo_temp->cliente;
            nodo_temp = nodo_temp->siguiente;
        }

        nodo_ruta *inicio = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
        for (int i = 0; i < idx1; i++)
            inicio = inicio->siguiente;

        nodo_ruta *fin = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
        for (int i = 0; i < idx2; i++)
            fin = fin->siguiente;

        if (inicio == NULL || fin == NULL)
        {
            liberar_memoria_arreglo_int(clientes_originales);
            return false;
        }

        int izq = idx1;
        int der = idx2;

        while (izq < der)
        {
            nodo_ruta *nodo_izq = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
            for (int i = 0; i < izq; i++)
                nodo_izq = nodo_izq->siguiente;

            nodo_ruta *nodo_der = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
            for (int i = 0; i < der; i++)
                nodo_der = nodo_der->siguiente;

            int temp = nodo_izq->cliente;
            nodo_izq->cliente = nodo_der->cliente;
            nodo_der->cliente = temp;

            izq++;
            der--;
        }

        if (verificarRestricciones(vehiculo_actual->vehiculo, vrp, instancia_distancias))
        {
            liberar_memoria_arreglo_int(clientes_originales);
            return true;
        }

        // Revertir cambios
        nodo_temp = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
        for (int i = 0; i < idx1; i++)
            nodo_temp = nodo_temp->siguiente;

        for (int i = 0; i < tamanio_segmento; i++)
        {
            nodo_temp->cliente = clientes_originales[i];
            nodo_temp = nodo_temp->siguiente;
        }

        liberar_memoria_arreglo_int(clientes_originales);
    }

    return false;
}

// Intercambia dos clientes aleatorios dentro de un mismo vehículo
bool swap_intra(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    int vehiculo_aleatorio, intentos_maximos = 10, cliente1_idx, cliente2_idx, temp;
    nodo_vehiculo *vehiculo_actual = NULL;
    nodo_ruta *nodo1 = NULL, *nodo2 = NULL;

    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        vehiculo_aleatorio = (rand() % hormiga->vehiculos_necesarios) + 1;

        vehiculo_actual = hormiga->metal->solucion_vecina->cabeza;
        while (vehiculo_actual != NULL)
        {
            if (vehiculo_actual->vehiculo->id_vehiculo == vehiculo_aleatorio)
                break;
            vehiculo_actual = vehiculo_actual->siguiente;
        }

        if (vehiculo_actual == NULL || vehiculo_actual->vehiculo->clientes_contados < 2)
            continue;

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
        nodo1 = nodo1->siguiente;
    for (int i = 0; i < cliente2_idx; i++)
        nodo2 = nodo2->siguiente;

    if (nodo1 == NULL || nodo2 == NULL)
        return false;

    // Intercambio tentativo
    temp = nodo1->cliente;
    nodo1->cliente = nodo2->cliente;
    nodo2->cliente = temp;
    // Verificamos si sigue siendo factible con respecto a la ventana de tiempo y capacidad
    if (!verificarRestricciones(vehiculo_actual->vehiculo, vrp, instancia_distancias))
    {
        // Revertimos el intercambio si viola las restricciones
        temp = nodo1->cliente;
        nodo1->cliente = nodo2->cliente;
        nodo2->cliente = temp;

        return false;
    }

    return true;
}

// Intercambia dos clientes de diferentes vehiculos
bool swap_inter(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    nodo_vehiculo *primer_vehiculo = NULL, *segundo_vehiculo = NULL;
    nodo_ruta *primer_nodo_cliente = NULL, *segundo_nodo_cliente = NULL;
    int intentos_maximos = 10, primer_cliente, segundo_cliente;
    int primer_posicion, segunda_posicion;
    bool factible = false;

    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        bool vehiculos_validos = false;
        for (int j = 0; j < intentos_maximos; j++)
        {
            primer_vehiculo = seleccionar_vehiculo_aleatorio(hormiga);
            segundo_vehiculo = seleccionar_vehiculo_aleatorio(hormiga);

            if (!primer_vehiculo || !segundo_vehiculo || !primer_vehiculo->vehiculo || !segundo_vehiculo->vehiculo)
                continue;

            if (primer_vehiculo == segundo_vehiculo)
                continue;

            if (primer_vehiculo->vehiculo->clientes_contados > 1 &&
                segundo_vehiculo->vehiculo->clientes_contados > 1)
            {
                vehiculos_validos = true;
                break;
            }
        }

        if (!vehiculos_validos)
            return false;

        // Evitar división por cero en rand() % ...
        if (primer_vehiculo->vehiculo->clientes_contados <= 2 ||
            segundo_vehiculo->vehiculo->clientes_contados <= 2)
        {
            continue;
        }

        primer_posicion = rand() % (primer_vehiculo->vehiculo->clientes_contados - 2) + 1;
        segunda_posicion = rand() % (segundo_vehiculo->vehiculo->clientes_contados - 2) + 1;

        primer_nodo_cliente = primer_vehiculo->vehiculo->ruta->cabeza->siguiente;
        for (int k = 0; k < primer_posicion && primer_nodo_cliente; k++)
            primer_nodo_cliente = primer_nodo_cliente->siguiente;

        segundo_nodo_cliente = segundo_vehiculo->vehiculo->ruta->cabeza->siguiente;
        for (int k = 0; k < segunda_posicion && segundo_nodo_cliente; k++)
            segundo_nodo_cliente = segundo_nodo_cliente->siguiente;

        if (!primer_nodo_cliente || !segundo_nodo_cliente)
            continue;

        primer_cliente = primer_nodo_cliente->cliente;
        segundo_cliente = segundo_nodo_cliente->cliente;

        // swap
        int temp = primer_nodo_cliente->cliente;
        primer_nodo_cliente->cliente = segundo_nodo_cliente->cliente;
        segundo_nodo_cliente->cliente = temp;

        // verificar ambos vehículos
        factible = verificarRestricciones(primer_vehiculo->vehiculo, vrp, instancia_distancias) &&
                   verificarRestricciones(segundo_vehiculo->vehiculo, vrp, instancia_distancias);

        if (!factible)
        {
            // revertir swap
            temp = primer_nodo_cliente->cliente;
            primer_nodo_cliente->cliente = segundo_nodo_cliente->cliente;
            segundo_nodo_cliente->cliente = temp;
        }
        else
        {
            return true; // swap exitoso
        }
    }

    return false; // no se pudo hacer swap factible
}

bool reinsercion_intra_inter(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    nodo_vehiculo *vehiculo_origen = NULL, *vehiculo_destino = NULL;
    nodo_ruta *nodo_cliente = NULL;
    int cliente, posicion_origen, posicion_destino;
    int intentos_maximos = 10;

    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        bool vehiculos_validos = false;

        for (int j = 0; j < intentos_maximos; j++)
        {
            vehiculo_origen = seleccionar_vehiculo_aleatorio(hormiga);
            vehiculo_destino = seleccionar_vehiculo_aleatorio(hormiga);

            if (!vehiculo_origen || !vehiculo_destino || !vehiculo_origen->vehiculo || !vehiculo_destino->vehiculo)
            {
                continue;
            }

            if (vehiculo_origen->vehiculo->clientes_contados < 2)
            {
                continue;
            }

            if (vehiculo_origen == vehiculo_destino)
            {
                if (vehiculo_origen->vehiculo->clientes_contados < 3)
                {
                    continue;
                }
            }
            else
            {
                if (vehiculo_destino->vehiculo->clientes_contados < 1)
                {
                    continue;
                }
            }

            vehiculos_validos = true;
            break;
        }

        if (!vehiculos_validos)
        {
            continue;
        }

        int total_clientes_origen = vehiculo_origen->vehiculo->clientes_contados;
        if (total_clientes_origen < 3)
        {
            continue;
        }

        posicion_origen = rand() % (total_clientes_origen - 2) + 1;

        int total_clientes_destino = vehiculo_destino->vehiculo->clientes_contados;
        if (total_clientes_destino < 1)
        {
            continue;
        }

        posicion_destino = rand() % total_clientes_destino + 1;

        if (vehiculo_origen == vehiculo_destino && posicion_origen == posicion_destino)
        {
            continue;
        }

        nodo_cliente = vehiculo_origen->vehiculo->ruta->cabeza->siguiente;
        for (int k = 0; k < posicion_origen && nodo_cliente; k++)
        {
            nodo_cliente = nodo_cliente->siguiente;
        }

        if (!nodo_cliente)
        {
            continue;
        }

        cliente = nodo_cliente->cliente;

        eliminar_cliente_ruta(vehiculo_origen->vehiculo, vrp, cliente, instancia_distancias);

        bool factible = insertarClienteEnPosicion(vehiculo_destino->vehiculo, vrp, cliente,
                                                  posicion_destino, instancia_distancias);

        bool restricciones_origen_ok = false, restricciones_destino_ok = false;

        if (factible)
        {
            restricciones_origen_ok = verificarRestricciones(vehiculo_origen->vehiculo, vrp, instancia_distancias);
            restricciones_destino_ok = verificarRestricciones(vehiculo_destino->vehiculo, vrp, instancia_distancias);
        }

        if (factible && restricciones_origen_ok && restricciones_destino_ok)
        {
            return true;
        }

        if (factible)
        {
            eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp, cliente, instancia_distancias);
        }

        insertarClienteEnPosicion(vehiculo_origen->vehiculo, vrp, cliente,
                                  posicion_origen, instancia_distancias);
    }

    return false;
}

// Implementación del algoritmo 2.5-opt
// Elimina un segmento de una ruta y lo reinserta en otra posición
bool opt_2_5(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    int intentos_maximos = 10;
    struct nodo_vehiculo *vehiculo_aleatorio;

    for (int intentos = 0; intentos < intentos_maximos; intentos++)
    {

        vehiculo_aleatorio = seleccionar_vehiculo_aleatorio(hormiga);

        if (vehiculo_aleatorio == NULL)
            continue;

        if (vehiculo_aleatorio->vehiculo->clientes_contados < 3)
            continue;

        if (vehiculo_aleatorio != NULL && vehiculo_aleatorio->vehiculo->clientes_contados > 3)
            break;
    }

    int posicion_origen, cliente_origen;
    struct nodo_ruta *cliente;

    // for (int intentos = 0; intentos < intentos_maximos; intentos++)
    // {
    posicion_origen = rand() % (vehiculo_aleatorio->vehiculo->clientes_contados - 2) + 1;
    printf("Ruta Inicial\n");
    imprimir_ruta(vehiculo_aleatorio->vehiculo->ruta, vehiculo_aleatorio->vehiculo->id_vehiculo);
    cliente = vehiculo_aleatorio->vehiculo->ruta->cabeza->siguiente;

    for (int i = 0; i < posicion_origen; i++)
        cliente = cliente->siguiente;

    cliente_origen = cliente->cliente;

    eliminar_cliente_ruta(vehiculo_aleatorio->vehiculo, vrp, cliente_origen, instancia_distancias);

    printf("Ruta actualizada\n");
    imprimir_ruta(vehiculo_aleatorio->vehiculo->ruta, vehiculo_aleatorio->vehiculo->id_vehiculo);

    //}
}
