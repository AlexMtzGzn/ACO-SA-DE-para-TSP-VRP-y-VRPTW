#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/estructuras.h"
#include "../include/control_memoria.h"
#include "../include/lista_ruta.h"
#include "../include/lista_flota.h"
#include "../include/salida_datos.h"

// Swap intra vehículo - Intercambia dos clientes dentro de la misma ruta de un vehículo
bool swap_intra(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    int vehiculo_aleatorio, intentos_maximos = 10, cliente1_idx, cliente2_idx, temp;
    nodo_vehiculo *vehiculo_actual = NULL;
    nodo_ruta *nodo1 = NULL, *nodo2 = NULL;

    // Selecciona un vehículo aleatorio que tenga al menos 2 clientes
    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        vehiculo_aleatorio = (rand() % hormiga->vehiculos_necesarios) + 1;

        // Busca el vehículo por ID dentro de la lista de vehículos
        vehiculo_actual = hormiga->flota->cabeza;
        while (vehiculo_actual != NULL)
        {
            if (vehiculo_actual->vehiculo->id_vehiculo == vehiculo_aleatorio)
                break;
            vehiculo_actual = vehiculo_actual->siguiente;
        }

        // Si encontró uno con al menos 2 clientes, lo usa
        if (vehiculo_actual != NULL && vehiculo_actual->vehiculo->clientes_contados >= 2)
            break;
    }

    // Si no se encontró un vehículo válido, termina
    if (vehiculo_actual == NULL || vehiculo_actual->vehiculo->clientes_contados < 2)
        return false;

    // Selecciona dos posiciones aleatorias distintas dentro de la ruta del vehículo
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
        } while (cliente1_idx == cliente2_idx); // Asegura que sean diferentes
    }

    // Inicializa los punteros para recorrer la lista hasta los clientes seleccionados
    nodo1 = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
    nodo2 = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;

    // Avanza nodo1 hasta la posición cliente1_idx
    for (int i = 0; i < cliente1_idx; i++)
    {
        if (nodo1 == NULL)
            return false;
        nodo1 = nodo1->siguiente;
    }

    // Avanza nodo2 hasta la posición cliente2_idx
    for (int i = 0; i < cliente2_idx; i++)
    {
        if (nodo2 == NULL)
            return false;
        nodo2 = nodo2->siguiente;
    }

    // Verifica que ambos nodos existen
    if (nodo1 == NULL || nodo2 == NULL)
        return false;

    // Evita intercambiar si alguno es un depósito (cliente 0)
    if (nodo1->cliente == 0 || nodo2->cliente == 0)
        return false;

    // Intercambia los clientes entre ambas posiciones
    temp = nodo1->cliente;
    nodo1->cliente = nodo2->cliente;
    nodo2->cliente = temp;

    // Verifica si las restricciones se siguen cumpliendo después del intercambio
    if (!verificar_restricciones(vehiculo_actual->vehiculo, vrp, instancia_distancias))
    {
        // Si no se cumple, revierte el cambio
        temp = nodo1->cliente;
        nodo1->cliente = nodo2->cliente;
        nodo2->cliente = temp;
        return false;
    }

    // Si todo fue correcto, retorna éxito
    return true;
}

// 2-opt - Invierte un segmento de la ruta
bool opt_2(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    nodo_vehiculo *vehiculo_actual = NULL;
    int intentos_maximos = 10;

    // Seleccionar un vehículo con al menos 3 clientes (para que exista un segmento invertible)
    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        vehiculo_actual = seleccionar_vehiculo_aleatorio(hormiga);
        if (vehiculo_actual && vehiculo_actual->vehiculo && vehiculo_actual->vehiculo->clientes_contados >= 3)
            break;
    }

    // Si no se encuentra un vehículo válido, salir
    if (vehiculo_actual == NULL || vehiculo_actual->vehiculo->clientes_contados < 3)
        return false;

    int total_clientes = vehiculo_actual->vehiculo->clientes_contados;

    // Seleccionar aleatoriamente dos índices que definan el segmento a invertir
    int idx1 = rand() % (total_clientes - 1); // inicio del segmento
    int idx2 = (rand() % (total_clientes - idx1 - 1)) + idx1 + 1; // final del segmento

    int tamanio_segmento = idx2 - idx1 + 1;

    if (tamanio_segmento < 2)
        return false;

    // Reservar memoria para guardar el estado original y punteros a los nodos
    int *clientes_originales = asignar_memoria_arreglo_int(tamanio_segmento);
    nodo_ruta **nodos = asignar_memoria_arreglo_nodo_ruta(total_clientes);

    // Llenar arreglo con los nodos actuales de la ruta, saltando el depósito
    nodo_ruta *nodo_temp = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
    int count = 0;
    while (nodo_temp && count < total_clientes)
    {
        nodos[count++] = nodo_temp;
        nodo_temp = nodo_temp->siguiente;
    }

    // Validar que se hayan llenado todos los nodos necesarios
    if (count < total_clientes || idx2 >= count)
    {
        liberar_memoria_arreglo_nodo_ruta(nodos);
        liberar_memoria_arreglo_int(clientes_originales);
        return false;
    }

    // Guardar los clientes actuales del segmento antes de invertir
    for (int i = 0; i < tamanio_segmento; i++)
        clientes_originales[i] = nodos[idx1 + i]->cliente;

    // Invertir el segmento seleccionado
    for (int i = 0; i < tamanio_segmento / 2; i++)
    {
        int temp = nodos[idx1 + i]->cliente;
        nodos[idx1 + i]->cliente = nodos[idx2 - i]->cliente;
        nodos[idx2 - i]->cliente = temp;
    }

    // Verificar si la ruta sigue siendo válida tras el cambio
    bool factible = verificar_restricciones(vehiculo_actual->vehiculo, vrp, instancia_distancias);

    // Si no se cumple alguna restricción, revertir el cambio
    if (!factible)
    {
        for (int i = 0; i < tamanio_segmento; i++)
            nodos[idx1 + i]->cliente = clientes_originales[i];
    }

    // Liberar la memoria usada
    liberar_memoria_arreglo_nodo_ruta(nodos);
    liberar_memoria_arreglo_int(clientes_originales);

    return factible;
}


// Swap_inter - Intercambia un cliente de un vehículo con un cliente de otro vehículo
bool swap_inter(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    nodo_vehiculo *primer_vehiculo = NULL, *segundo_vehiculo = NULL;
    int intentos_maximos = 10;

    // Intenta seleccionar dos vehículos distintos que tengan al menos un cliente cada uno
    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        primer_vehiculo = seleccionar_vehiculo_aleatorio(hormiga);
        segundo_vehiculo = seleccionar_vehiculo_aleatorio(hormiga);

        // Verifica que ambos vehículos y sus estructuras internas sean válidos
        if (!primer_vehiculo || !segundo_vehiculo ||
            !primer_vehiculo->vehiculo || !segundo_vehiculo->vehiculo)
            continue;

        // Asegura que los vehículos seleccionados no sean el mismo
        if (primer_vehiculo == segundo_vehiculo)
            continue;

        // Ambos vehículos deben tener al menos un cliente
        if (primer_vehiculo->vehiculo->clientes_contados >= 1 &&
            segundo_vehiculo->vehiculo->clientes_contados >= 1)
            break;
    }

    // Si no se pudo seleccionar una combinación válida, termina
    if (!primer_vehiculo || !segundo_vehiculo)
        return false;

    int total1 = primer_vehiculo->vehiculo->clientes_contados;
    int total2 = segundo_vehiculo->vehiculo->clientes_contados;

    // Revalidación de seguridad por si acaso
    if (total1 == 0 || total2 == 0)
        return false;

    // Se eligen posiciones aleatorias de cada vehículo (índices 0-based)
    int pos1 = rand() % total1;
    int pos2 = rand() % total2;

    // Se navega por las rutas de ambos vehículos, comenzando después del depósito
    nodo_ruta *nodo1 = primer_vehiculo->vehiculo->ruta->cabeza->siguiente;
    nodo_ruta *nodo2 = segundo_vehiculo->vehiculo->ruta->cabeza->siguiente;

    for (int i = 0; i < pos1 && nodo1; i++)
        nodo1 = nodo1->siguiente;
    for (int i = 0; i < pos2 && nodo2; i++)
        nodo2 = nodo2->siguiente;

    // Validación final: nodos válidos y que no sean depósitos
    if (!nodo1 || !nodo2 || nodo1->cliente == 0 || nodo2->cliente == 0)
        return false;

    // Se realiza el intercambio de clientes entre los dos nodos
    int temp = nodo1->cliente;
    nodo1->cliente = nodo2->cliente;
    nodo2->cliente = temp;

    // Se verifica que el intercambio mantenga las restricciones válidas en ambas rutas
    bool factible = verificar_restricciones(primer_vehiculo->vehiculo, vrp, instancia_distancias) &&
                    verificar_restricciones(segundo_vehiculo->vehiculo, vrp, instancia_distancias);

    if (!factible)
    {
        // Si no fue válido, se revierte el intercambio
        temp = nodo1->cliente;
        nodo1->cliente = nodo2->cliente;
        nodo2->cliente = temp;
    }

    // Devuelve si el intercambio fue factible
    return factible;
}


// Reinserción intra e inter vehículo - Mueve un cliente de una posición a otra, ya sea dentro del mismo vehículo (intra) o entre vehículos diferentes (inter)
bool reinsercion_intra_inter(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    nodo_vehiculo *vehiculo_origen = NULL, *vehiculo_destino = NULL;
    int intentos_maximos = 10;

    // Se intenta seleccionar dos vehículos: uno de origen (que tenga al menos 1 cliente) y uno de destino
    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        vehiculo_origen = seleccionar_vehiculo_aleatorio(hormiga);
        vehiculo_destino = seleccionar_vehiculo_aleatorio(hormiga);

        // Si alguno no es válido, se intenta de nuevo
        if (!vehiculo_origen || !vehiculo_destino ||
            !vehiculo_origen->vehiculo || !vehiculo_destino->vehiculo)
            continue;

        // El vehículo de origen debe tener al menos un cliente
        if (vehiculo_origen->vehiculo->clientes_contados < 1)
            continue;

        // Si se trata del mismo vehículo, debe tener al menos 2 clientes para poder mover uno
        if (vehiculo_origen == vehiculo_destino &&
            vehiculo_origen->vehiculo->clientes_contados < 2)
            continue;

        break; // Se encontró una configuración válida
    }

    // Si no se logró obtener una combinación válida de vehículos, se retorna false
    if (!vehiculo_origen || !vehiculo_destino)
        return false;

    int total_origen = vehiculo_origen->vehiculo->clientes_contados;

    // Por seguridad, se valida nuevamente
    if (total_origen == 0)
        return false;

    // Se elige aleatoriamente la posición del cliente a mover dentro del vehículo origen
    int pos_origen = rand() % total_origen;

    // Se navega hasta el nodo que contiene al cliente en esa posición
    nodo_ruta *nodo_cliente = vehiculo_origen->vehiculo->ruta->cabeza->siguiente;
    for (int i = 0; i < pos_origen && nodo_cliente; i++)
        nodo_cliente = nodo_cliente->siguiente;

    // Si el nodo es nulo o es un depósito, se termina la operación
    if (!nodo_cliente || nodo_cliente->cliente == 0)
        return false;

    int cliente = nodo_cliente->cliente;

    // El cliente se elimina temporalmente de su vehículo original
    eliminar_cliente_ruta(vehiculo_origen->vehiculo, vrp, cliente, instancia_distancias);

    // Se elige aleatoriamente una posición donde se va a insertar el cliente en el vehículo destino
    int total_destino_actual = vehiculo_destino->vehiculo->clientes_contados;
    int pos_destino = rand() % (total_destino_actual + 1); // puede insertarse al final

    // Se intenta insertar el cliente en la nueva posición (ajustado a base 1 si es necesario)
    bool factible = inserta_cliente_en_posicion(vehiculo_destino->vehiculo, vrp, cliente,
                                              pos_destino + 1, instancia_distancias);

    if (factible)
    {
        // Se verifica que ambas rutas, origen y destino, sean viables tras el cambio
        bool restricciones_ok = verificar_restricciones(vehiculo_origen->vehiculo, vrp, instancia_distancias) &&
                                verificar_restricciones(vehiculo_destino->vehiculo, vrp, instancia_distancias);

        if (restricciones_ok)
        {
            // Si todo está correcto, se confirma la operación
            return true;
        }

        // Si no se cumplen las restricciones, se revierte: se elimina del destino
        eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp, cliente, instancia_distancias);
    }

    // Si no fue factible o se violaron restricciones, se intenta restaurar el cliente en su posición original
    inserta_cliente_en_posicion(vehiculo_origen->vehiculo, vrp, cliente, pos_origen + 1, instancia_distancias);
    return false;
}


// 2.5opt Relocación de segmento entre vehículos o Mueve un segmento de 1-2 clientes desde un vehículo origen hacia un vehículo destino
bool opt_2_5(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    int intentos_maximos = 10;
    nodo_vehiculo *vehiculo_origen = NULL, *vehiculo_destino = NULL;

    // Selecciona un vehículo origen que tenga al menos 2 clientes
    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        vehiculo_origen = seleccionar_vehiculo_aleatorio(hormiga);
        if (vehiculo_origen && vehiculo_origen->vehiculo &&
            vehiculo_origen->vehiculo->clientes_contados >= 2)
            break;
    }

    // Si no se encontró un vehículo válido, termina la operación
    if (!vehiculo_origen || vehiculo_origen->vehiculo->clientes_contados < 2)
        return false;

    // Selecciona un vehículo destino (puede ser el mismo que el origen)
    vehiculo_destino = seleccionar_vehiculo_aleatorio(hormiga);
    if (!vehiculo_destino || !vehiculo_destino->vehiculo)
        return false;

    int total_origen = vehiculo_origen->vehiculo->clientes_contados;

    // Determina el tamaño del segmento a mover (1 o 2 clientes)
    int tamanio_segmento = 1 + (rand() % 2);
    if (tamanio_segmento > total_origen)
        tamanio_segmento = total_origen;

    // Selecciona aleatoriamente la posición inicial del segmento a mover
    int max_inicio = total_origen - tamanio_segmento;
    if (max_inicio < 0)
        return false;

    int inicio_segmento = rand() % (max_inicio + 1);

    // Reserva memoria para guardar temporalmente el segmento de clientes
    int *clientes_segmento = asignar_memoria_arreglo_int(tamanio_segmento);

    // Extrae los clientes del segmento desde la ruta del vehículo origen
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

    // Elimina el segmento de clientes del vehículo origen (en orden inverso)
    for (int i = tamanio_segmento - 1; i >= 0; i--)
    {
        eliminar_cliente_ruta(vehiculo_origen->vehiculo, vrp, clientes_segmento[i], instancia_distancias);
    }

    int total_destino = vehiculo_destino->vehiculo->clientes_contados;

    // Selecciona aleatoriamente una posición de inserción en la ruta del destino
    int pos_insercion = rand() % (total_destino + 1);

    // Intenta insertar el segmento de clientes en el vehículo destino
    bool factible = true;
    for (int i = 0; i < tamanio_segmento && factible; i++)
    {
        factible = inserta_cliente_en_posicion(vehiculo_destino->vehiculo, vrp,
                                             clientes_segmento[i], pos_insercion + i + 1, instancia_distancias);
    }

    // Si la inserción fue exitosa, verifica las restricciones de ambos vehículos
    if (factible)
    {
        bool restricciones_ok = verificar_restricciones(vehiculo_origen->vehiculo, vrp, instancia_distancias) &&
                                verificar_restricciones(vehiculo_destino->vehiculo, vrp, instancia_distancias);

        // Si ambas rutas cumplen restricciones, se completa la operación
        if (restricciones_ok)
        {
            liberar_memoria_arreglo_int(clientes_segmento);
            return true;
        }

        // Si fallan las restricciones, se eliminan los clientes insertados en el destino
        for (int i = tamanio_segmento - 1; i >= 0; i--)
        {
            eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp, clientes_segmento[i], instancia_distancias);
        }
    }

    // Si la inserción no fue factible o las restricciones fallaron, se restaura el segmento en el origen
    for (int i = 0; i < tamanio_segmento; i++)
    {
        inserta_cliente_en_posicion(vehiculo_origen->vehiculo, vrp,
                                  clientes_segmento[i], inicio_segmento + i + 1, instancia_distancias);
    }

    liberar_memoria_arreglo_int(clientes_segmento);
    return false;
}


// Or-opt - Mueve un segmento de clientes dentro de la misma ruta de un vehículo
bool or_opt(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    nodo_vehiculo *vehiculo_actual = NULL;
    int intentos_maximos = 10;

    // Selecciona aleatoriamente un vehículo con al menos 3 clientes
    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        vehiculo_actual = seleccionar_vehiculo_aleatorio(hormiga);
        if (vehiculo_actual && vehiculo_actual->vehiculo &&
            vehiculo_actual->vehiculo->clientes_contados >= 3)
            break;
    }

    // Si no se encontró un vehículo adecuado, se termina la función
    if (!vehiculo_actual || vehiculo_actual->vehiculo->clientes_contados < 3)
        return false;

    int total_clientes = vehiculo_actual->vehiculo->clientes_contados;

    // Se selecciona aleatoriamente el tamaño del segmento a mover (1, 2 o 3 clientes)
    int tamanio_segmento = 1 + (rand() % 3);
    if (tamanio_segmento >= total_clientes)
        tamanio_segmento = total_clientes - 1;

    if (tamanio_segmento < 1)
        return false;

    // Se escoge la posición inicial del segmento dentro de la ruta (índice base 0)
    int inicio_segmento = rand() % (total_clientes - tamanio_segmento + 1);

    // Se escoge una nueva posición donde se moverá el segmento, distinta y no solapada
    int nueva_posicion;
    int intentos_posicion = 0;
    do
    {
        nueva_posicion = rand() % (total_clientes - tamanio_segmento + 1);
        intentos_posicion++;

        // Evita ciclos infinitos si no se encuentra una nueva posición válida
        if (intentos_posicion > 20)
            return false;

    } while (nueva_posicion == inicio_segmento ||
             (nueva_posicion > inicio_segmento && nueva_posicion < inicio_segmento + tamanio_segmento));

    // Se reserva memoria para almacenar los clientes del segmento que se moverá
    int *clientes_segmento = asignar_memoria_arreglo_int(tamanio_segmento);

    // Validación de ruta
    if (!vehiculo_actual->vehiculo->ruta || !vehiculo_actual->vehiculo->ruta->cabeza)
    {
        liberar_memoria_arreglo_int(clientes_segmento);
        return false;
    }

    // Se navega hasta el nodo donde comienza el segmento a mover
    nodo_ruta *nodo_temp = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
    for (int i = 0; i < inicio_segmento; i++)
        nodo_temp = nodo_temp->siguiente;

    // Se copian los clientes del segmento a mover en el arreglo temporal
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

    // Se eliminan los clientes del segmento de su posición original
    for (int i = tamanio_segmento - 1; i >= 0; i--)
    {
        eliminar_cliente_ruta(vehiculo_actual->vehiculo, vrp,
                              clientes_segmento[i], instancia_distancias);
    }

    // Se insertan los clientes en la nueva posición (ajustada a base 1)
    bool factible = true;
    for (int i = 0; i < tamanio_segmento && factible; i++)
    {
        factible = inserta_cliente_en_posicion(vehiculo_actual->vehiculo, vrp,
                                             clientes_segmento[i],
                                             nueva_posicion + i + 1, instancia_distancias);
    }

    if (factible)
    {
        // Se verifican las restricciones del vehículo después del movimiento
        bool restricciones_ok = verificar_restricciones(vehiculo_actual->vehiculo,
                                                       vrp, instancia_distancias);
        if (restricciones_ok)
        {
            liberar_memoria_arreglo_int(clientes_segmento);
            return true;
        }

        // Si las restricciones no se cumplen, se eliminan los clientes en la nueva posición
        for (int i = tamanio_segmento - 1; i >= 0; i--)
        {
            eliminar_cliente_ruta(vehiculo_actual->vehiculo, vrp,
                                  clientes_segmento[i], instancia_distancias);
        }
    }

    // Si no fue factible o las restricciones no se cumplieron, se restaura el segmento en la posición original
    for (int i = 0; i < tamanio_segmento; i++)
    {
        inserta_cliente_en_posicion(vehiculo_actual->vehiculo, vrp,
                                  clientes_segmento[i],
                                  inicio_segmento + i + 1, instancia_distancias);
    }

    liberar_memoria_arreglo_int(clientes_segmento);
    return false;
}


// Cross-exchange - Intercambio de segmentos entre dos vehículos
bool cross_exchange(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    nodo_vehiculo *primer_vehiculo = NULL, *segundo_vehiculo = NULL;
    int intentos_maximos = 10;

    // Intentar seleccionar dos vehículos distintos con al menos un cliente cada uno
    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        primer_vehiculo = seleccionar_vehiculo_aleatorio(hormiga);
        segundo_vehiculo = seleccionar_vehiculo_aleatorio(hormiga);

        // Validar que ambos sean válidos y distintos
        if (!primer_vehiculo || !segundo_vehiculo ||
            !primer_vehiculo->vehiculo || !segundo_vehiculo->vehiculo)
            continue;

        if (primer_vehiculo == segundo_vehiculo)
            continue;

        if (primer_vehiculo->vehiculo->clientes_contados >= 1 &&
            segundo_vehiculo->vehiculo->clientes_contados >= 1)
            break;
    }

    // Si no se pudieron seleccionar correctamente, retornar falso
    if (!primer_vehiculo || !segundo_vehiculo)
        return false;

    int total1 = primer_vehiculo->vehiculo->clientes_contados;
    int total2 = segundo_vehiculo->vehiculo->clientes_contados;

    // Determinar tamaño del segmento a intercambiar (1 o 2 clientes)
    int tamanio1 = 1 + (rand() % 2);
    int tamanio2 = 1 + (rand() % 2);

    // Ajustar tamaño si excede la cantidad de clientes
    if (tamanio1 > total1)
        tamanio1 = total1;
    if (tamanio2 > total2)
        tamanio2 = total2;

    // Seleccionar posiciones iniciales para los segmentos (índices 1-based)
    int inicio1 = (rand() % (total1 - tamanio1 + 1)) + 1;
    int inicio2 = (rand() % (total2 - tamanio2 + 1)) + 1;

    // Validar que las rutas no estén vacías
    if (!primer_vehiculo->vehiculo->ruta || !primer_vehiculo->vehiculo->ruta->cabeza ||
        !segundo_vehiculo->vehiculo->ruta || !segundo_vehiculo->vehiculo->ruta->cabeza)
        return false;

    // Reservar memoria para almacenar los segmentos
    int *segmento1 = asignar_memoria_arreglo_int(tamanio1);
    int *segmento2 = asignar_memoria_arreglo_int(tamanio2);

    // Extraer clientes del primer segmento navegando desde el inicio correspondiente
    nodo_ruta *nodo_temp = primer_vehiculo->vehiculo->ruta->cabeza->siguiente;
    for (int i = 1; i < inicio1; i++)
        nodo_temp = nodo_temp->siguiente;

    for (int i = 0; i < tamanio1; i++)
    {
        segmento1[i] = nodo_temp->cliente;
        nodo_temp = nodo_temp->siguiente;
    }

    // Extraer clientes del segundo segmento de manera similar
    nodo_temp = segundo_vehiculo->vehiculo->ruta->cabeza->siguiente;
    for (int i = 1; i < inicio2; i++)
        nodo_temp = nodo_temp->siguiente;

    for (int i = 0; i < tamanio2; i++)
    {
        segmento2[i] = nodo_temp->cliente;
        nodo_temp = nodo_temp->siguiente;
    }

    // Eliminar los clientes de ambos segmentos de sus respectivas rutas
    for (int i = tamanio1 - 1; i >= 0; i--)
        eliminar_cliente_ruta(primer_vehiculo->vehiculo, vrp, segmento1[i], instancia_distancias);

    for (int i = tamanio2 - 1; i >= 0; i--)
        eliminar_cliente_ruta(segundo_vehiculo->vehiculo, vrp, segmento2[i], instancia_distancias);

    bool factible = true;

    // Insertar el segundo segmento en la ruta del primer vehículo
    for (int i = 0; i < tamanio2 && factible; i++)
        factible = inserta_cliente_en_posicion(primer_vehiculo->vehiculo, vrp, segmento2[i], inicio1 + i, instancia_distancias);

    // Insertar el primer segmento en la ruta del segundo vehículo
    for (int i = 0; i < tamanio1 && factible; i++)
        factible = inserta_cliente_en_posicion(segundo_vehiculo->vehiculo, vrp, segmento1[i], inicio2 + i, instancia_distancias);

    // Si ambas inserciones fueron exitosas, verificar restricciones
    if (factible)
    {
        bool restricciones_ok = verificar_restricciones(primer_vehiculo->vehiculo, vrp, instancia_distancias) &&
                                verificar_restricciones(segundo_vehiculo->vehiculo, vrp, instancia_distancias);

        if (restricciones_ok)
        {
            liberar_memoria_arreglo_int(segmento1);
            liberar_memoria_arreglo_int(segmento2);
            return true;
        }

        // Si las restricciones fallan, revertir eliminaciones e intercambios
        for (int i = tamanio2 - 1; i >= 0; i--)
            eliminar_cliente_ruta(primer_vehiculo->vehiculo, vrp, segmento2[i], instancia_distancias);

        for (int i = tamanio1 - 1; i >= 0; i--)
            eliminar_cliente_ruta(segundo_vehiculo->vehiculo, vrp, segmento1[i], instancia_distancias);
    }

    // Restaurar los segmentos originales en sus posiciones iniciales
    for (int i = 0; i < tamanio1; i++)
        inserta_cliente_en_posicion(primer_vehiculo->vehiculo, vrp, segmento1[i], inicio1 + i, instancia_distancias);

    for (int i = 0; i < tamanio2; i++)
        inserta_cliente_en_posicion(segundo_vehiculo->vehiculo, vrp, segmento2[i], inicio2 + i, instancia_distancias);

    liberar_memoria_arreglo_int(segmento1);
    liberar_memoria_arreglo_int(segmento2);
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

    // Seleccionar vehículo destino (puede ser el mismo)
    vehiculo_destino = seleccionar_vehiculo_aleatorio(hormiga);
    if (!vehiculo_destino || !vehiculo_destino->vehiculo)
        return false;

    int total_origen = vehiculo_origen->vehiculo->clientes_contados;

    // Determinar el tamaño máximo de la cadena a mover (máx 4 o mitad de la ruta)
    int max_cadena = (total_origen / 2 > 4) ? 4 : total_origen / 2;
    if (max_cadena < 2)
        max_cadena = 2;

    // Si origen y destino son el mismo, no mover toda la ruta
    if (vehiculo_origen == vehiculo_destino && max_cadena >= total_origen)
        max_cadena = total_origen - 1;

    if (max_cadena < 2)
        return false;

    // Elegir aleatoriamente el tamaño de la cadena (mínimo 2)
    int tamanio_cadena = (max_cadena == 2) ? 2 : 2 + (rand() % (max_cadena - 1));

    // Ajustar si excede
    if (tamanio_cadena > total_origen - 1)
        tamanio_cadena = total_origen - 1;

    // Elegir posición inicial (índice 1-based por claridad)
    int max_inicio = total_origen - tamanio_cadena + 1;
    if (max_inicio <= 0)
        return false;

    int inicio_cadena = (rand() % max_inicio) + 1;

    //  Reservar memoria para la cadena y sus posiciones originales
    int *cadena_clientes = asignar_memoria_arreglo_int(tamanio_cadena);
    int *posiciones_originales = asignar_memoria_arreglo_int(tamanio_cadena);

    //  Obtener puntero al nodo inicial de la cadena (navegar desde cabeza->siguiente)
    nodo_ruta *nodo_temp = vehiculo_origen->vehiculo->ruta->cabeza->siguiente;
    for (int i = 1; i < inicio_cadena && nodo_temp; i++)
        nodo_temp = nodo_temp->siguiente;

    if (!nodo_temp)
    {
        liberar_memoria_arreglo_int(cadena_clientes);
        liberar_memoria_arreglo_int(posiciones_originales);
        return false;
    }

    // Guardar los clientes de la cadena y sus posiciones
    for (int i = 0; i < tamanio_cadena && nodo_temp; i++)
    {
        cadena_clientes[i] = nodo_temp->cliente;
        posiciones_originales[i] = inicio_cadena + i;
        nodo_temp = nodo_temp->siguiente;
    }

    // Eliminar la cadena del vehículo origen (en orden inverso)
    for (int i = tamanio_cadena - 1; i >= 0; i--)
    {
        eliminar_cliente_ruta(vehiculo_origen->vehiculo, vrp,
                              cadena_clientes[i], instancia_distancias);
    }

    // Posición de inserción en el destino (1-based)
    int total_destino = vehiculo_destino->vehiculo->clientes_contados;
    int pos_insercion = rand() % (total_destino + 1) + 1;

    // 11. Insertar la cadena uno por uno en orden
    bool factible = true;
    int clientes_insertados = 0;
    for (int i = 0; i < tamanio_cadena && factible; i++)
    {
        factible = inserta_cliente_en_posicion(vehiculo_destino->vehiculo, vrp,
                                             cadena_clientes[i],
                                             pos_insercion + i, instancia_distancias);
        if (factible)
            clientes_insertados++;
    }

    // Verificar restricciones en ambas rutas
    if (factible)
    {
        bool restricciones_ok = verificar_restricciones(vehiculo_origen->vehiculo, vrp, instancia_distancias) &&
                                verificar_restricciones(vehiculo_destino->vehiculo, vrp, instancia_distancias);

        if (restricciones_ok)
        {
            liberar_memoria_arreglo_int(cadena_clientes);
            liberar_memoria_arreglo_int(posiciones_originales);
            return true; // Movimiento exitoso
        }

        // Si las restricciones fallan, revertir
        for (int i = clientes_insertados - 1; i >= 0; i--)
        {
            eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp,
                                  cadena_clientes[i], instancia_distancias);
        }
    }
    else
    {
        // Si falló al insertar parcialmente, revertir también
        for (int i = clientes_insertados - 1; i >= 0; i--)
        {
            eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp,
                                  cadena_clientes[i], instancia_distancias);
        }
    }

    // Restaurar la cadena original en el vehículo de origen
    for (int i = 0; i < tamanio_cadena; i++)
    {
        bool restaurado = inserta_cliente_en_posicion(vehiculo_origen->vehiculo, vrp,
                                                    cadena_clientes[i],
                                                    posiciones_originales[i], instancia_distancias);
        if (!restaurado)
        {
            // Si no se puede en la posición exacta, insertarla al final
            inserta_cliente_en_posicion(vehiculo_origen->vehiculo, vrp,
                                      cadena_clientes[i],
                                      vehiculo_origen->vehiculo->clientes_contados + 1,
                                      instancia_distancias);
        }
    }

    // Liberar memoria auxiliar
    liberar_memoria_arreglo_int(cadena_clientes);
    liberar_memoria_arreglo_int(posiciones_originales);
    return false; // Movimiento fallido
}
