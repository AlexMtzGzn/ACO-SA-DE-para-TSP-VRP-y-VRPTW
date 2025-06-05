#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/estructuras.h"
#include "../include/control_memoria.h"
#include "../include/lista_ruta.h"
#include "../include/lista_flota.h"

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

        int length_segmento = idx2 - idx1 + 1;
        int *clientes_originales = asignar_memoria_arreglo_int(length_segmento);

        if (clientes_originales == NULL)
            return false;

        nodo_ruta *nodo_temp = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;

        for (int i = 0; i < idx1; i++)
            nodo_temp = nodo_temp->siguiente;

        for (int i = 0; i < length_segmento; i++)
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

        for (int i = 0; i < length_segmento; i++)
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
        for (int j = 0; j < intentos_maximos; j++) // corregido aquí
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

bool reinsercion_intra_inter(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias) {
    
    nodo_vehiculo *vehiculo_origen = NULL, *vehiculo_destino = NULL;
    nodo_ruta *nodo_cliente = NULL;
    int cliente, posicion_origen, posicion_destino;
    int intentos_maximos = 10;

    for (int intento = 0; intento < intentos_maximos; intento++) {
      
        
        bool vehiculos_validos = false;
        for (int j = 0; j < intentos_maximos; j++) {
            
            vehiculo_origen = seleccionar_vehiculo_aleatorio(hormiga);
            vehiculo_destino = seleccionar_vehiculo_aleatorio(hormiga);

           

            if (!vehiculo_origen || !vehiculo_destino ||
                !vehiculo_origen->vehiculo || !vehiculo_destino->vehiculo) {
                
                continue;
            }

            
            if (vehiculo_origen->vehiculo->clientes_contados < 2) {
                continue;
            }

            // Para inter-ruta: vehículo destino debe tener al menos 1 cliente (depósitos no cuentan)
            // Para intra-ruta: mismo vehículo necesita al menos 2 clientes
            if (vehiculo_origen == vehiculo_destino) {
                if (vehiculo_origen->vehiculo->clientes_contados < 3) {
                    continue;
                }
            } else {
                // Inter-ruta: vehículo destino debe tener al menos estructura mínima
                if (vehiculo_destino->vehiculo->clientes_contados < 1) {
                    continue;
                }
            }

            vehiculos_validos = true;
            break;
        }
        
        if (!vehiculos_validos) {
            continue;
        }

        int total_clientes_origen = vehiculo_origen->vehiculo->clientes_contados;

        // Seleccionar posición del cliente a mover
        posicion_origen = rand() % (total_clientes_origen - 2) + 1;

        // Seleccionar posición destino diferente a la de origen
        int total_clientes_destino = vehiculo_destino->vehiculo->clientes_contados;
        
        // Validar que hay al menos posiciones válidas para insertar
        if (total_clientes_destino < 1) {
            continue;
        }
        
        // Para inserción: posiciones válidas van de 1 a total_clientes_destino
        // (después del depósito inicial, hasta antes del depósito final)
        posicion_destino = rand() % total_clientes_destino + 1;
      

        if (vehiculo_origen == vehiculo_destino && posicion_origen == posicion_destino) {
            continue;
        }

        // Obtener nodo cliente a mover
        nodo_cliente = vehiculo_origen->vehiculo->ruta->cabeza->siguiente;
        
        if (!nodo_cliente) {
            continue;
        }
        
        for (int k = 0; k < posicion_origen && nodo_cliente; k++) {
            nodo_cliente = nodo_cliente->siguiente;
        }

        if (!nodo_cliente) {
            continue;
        }

        cliente = nodo_cliente->cliente;

        // Guardar estado antes de modificar (para debug)
        double costo_origen_antes = vehiculo_origen->vehiculo->fitness_vehiculo;
        double costo_destino_antes = vehiculo_destino->vehiculo->fitness_vehiculo;
       
        eliminar_cliente_ruta(vehiculo_origen->vehiculo, vrp, cliente, instancia_distancias);
        
        
        bool factible = false;

        // Insertar cliente en ruta destino en posición destino
        
        factible = insertarClienteEnPosicion(vehiculo_destino->vehiculo, vrp, cliente, 
                                           posicion_destino, instancia_distancias);
        
        
   

        // Verificar restricciones en ambas rutas
        bool restricciones_origen_ok = false, restricciones_destino_ok = false;
        
        if (factible) {
            restricciones_origen_ok = verificarRestricciones(vehiculo_origen->vehiculo, vrp, instancia_distancias);
            restricciones_destino_ok = verificarRestricciones(vehiculo_destino->vehiculo, vrp, instancia_distancias);
            
            
        }

        if (factible && restricciones_origen_ok && restricciones_destino_ok) {
           
            return true;
        }

        // Si no fue factible revertir cambios
       
        if (factible) {
            eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp, cliente, instancia_distancias);
        }
        
        // Reinsertar cliente en la ruta origen en la posición original

        bool reinsercion_ok = insertarClienteEnPosicion(vehiculo_origen->vehiculo, vrp, cliente, 
                                                       posicion_origen, instancia_distancias);
        
       
    }

    return false; // No se pudo hacer la reinserción factible
}


// Implementación del algoritmo 2.5-opt
// Elimina un segmento de una ruta y lo reinserta en otra posición
bool opt_2_5(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    // Validaciones iniciales
    if (!hormiga || !vrp || !instancia_distancias)
        return false;
    
    if (!hormiga->metal || !hormiga->metal->solucion_vecina)
        return false;
    
    nodo_vehiculo *vehiculo_origen = NULL, *vehiculo_destino = NULL;
    int intentos_maximos = 10;
    int longitud_segmento = 1; // Empezar con segmento pequeño
    
    // Seleccionar vehículo origen con suficientes clientes
    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        vehiculo_origen = seleccionar_vehiculo_aleatorio(hormiga);
        
        if (vehiculo_origen && vehiculo_origen->vehiculo && 
            vehiculo_origen->vehiculo->clientes_contados >= 3) // Mínimo 3 clientes
            break;
    }
    
    if (vehiculo_origen == NULL || vehiculo_origen->vehiculo == NULL || 
        vehiculo_origen->vehiculo->clientes_contados < 3)
        return false;
    
    // Decidir si será intra-ruta o inter-ruta
    bool es_inter_ruta = (rand() % 2 == 0);
    
    if (es_inter_ruta)
    {
        // Seleccionar vehículo destino diferente
        for (int intento = 0; intento < intentos_maximos; intento++)
        {
            vehiculo_destino = seleccionar_vehiculo_aleatorio(hormiga);
            
            if (vehiculo_destino && vehiculo_destino != vehiculo_origen && 
                vehiculo_destino->vehiculo->clientes_contados >= 2)
                break;
        }
        
        if (vehiculo_destino == NULL || vehiculo_destino == vehiculo_origen)
            vehiculo_destino = vehiculo_origen; // Fallback a intra-ruta
    }
    else
    {
        vehiculo_destino = vehiculo_origen;
    }
    
    // Determinar longitud del segmento a mover (1-2 clientes máximo)
    int clientes_disponibles = vehiculo_origen->vehiculo->clientes_contados - 2; // Excluir depósitos
    if (clientes_disponibles <= 0) return false;
    
    int max_longitud = (clientes_disponibles > 2) ? 2 : 1; // Máximo 2 clientes
    longitud_segmento = (rand() % max_longitud) + 1;
    
    // Realizar varios intentos para encontrar una mejora
    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        // Validar que aún tengamos suficientes clientes
        if (vehiculo_origen->vehiculo->clientes_contados < longitud_segmento + 2)
            break;
        
        // Seleccionar posición inicial del segmento a extraer (evitar depósito)
        int clientes_validos = vehiculo_origen->vehiculo->clientes_contados - 2; // Sin depósitos
        if (clientes_validos < longitud_segmento) break;
        
        int pos_inicio_segmento = (rand() % (clientes_validos - longitud_segmento + 1)) + 1;
        
        // Seleccionar posición de inserción en vehículo destino
        int pos_insercion;
        if (vehiculo_destino == vehiculo_origen)
        {
            // Intra-ruta: evitar insertar en la misma posición o adyacente
            int clientes_destino = vehiculo_destino->vehiculo->clientes_contados - 2;
            if (clientes_destino <= 0) break;
            
            do {
                pos_insercion = (rand() % clientes_destino) + 1;
            } while (abs(pos_insercion - pos_inicio_segmento) <= longitud_segmento);
        }
        else
        {
            // Inter-ruta: cualquier posición válida
            int clientes_destino = vehiculo_destino->vehiculo->clientes_contados - 1;
            if (clientes_destino <= 0) break;
            
            pos_insercion = (rand() % clientes_destino) + 1;
        }
        
        // Guardar el segmento a mover
        int *segmento = asignar_memoria_arreglo_int(longitud_segmento);
        if (!segmento) return false;
        
        nodo_ruta *nodo_temp = vehiculo_origen->vehiculo->ruta->cabeza;
        if (!nodo_temp || !nodo_temp->siguiente) 
        {
            liberar_memoria_arreglo_int(segmento);
            return false;
        }
        
        nodo_temp = nodo_temp->siguiente; // Saltar depósito inicial
        
        // Navegar hasta la posición inicial del segmento
        for (int i = 0; i < pos_inicio_segmento && nodo_temp; i++)
            nodo_temp = nodo_temp->siguiente;
        
        if (!nodo_temp)
        {
            liberar_memoria_arreglo_int(segmento);
            return false;
        }
        
        // Extraer el segmento
        for (int i = 0; i < longitud_segmento; i++)
        {
            if (!nodo_temp)
            {
                liberar_memoria_arreglo_int(segmento);
                return false;
            }
            segmento[i] = nodo_temp->cliente;
            nodo_temp = nodo_temp->siguiente;
        }
        
        // Eliminar el segmento de la ruta origen (en orden inverso para mantener índices)
        for (int i = longitud_segmento - 1; i >= 0; i--)
        {
            eliminar_cliente_ruta(vehiculo_origen->vehiculo, vrp, segmento[i], instancia_distancias);
        }
        
        // Insertar el segmento en la posición destino
        bool insercion_exitosa = true;
        int pos_actual = pos_insercion;
        
        for (int i = 0; i < longitud_segmento; i++)
        {
            if (!insertarClienteEnPosicion(vehiculo_destino->vehiculo, vrp, segmento[i], pos_actual, instancia_distancias))
            {
                insercion_exitosa = false;
                // Revertir inserciones parciales
                for (int j = i - 1; j >= 0; j--)
                {
                    eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp, segmento[j], instancia_distancias);
                }
                break;
            }
            pos_actual++;
        }
        
        // Verificar restricciones si la inserción fue exitosa
        bool movimiento_valido = false;
        if (insercion_exitosa)
        {
            if (vehiculo_destino == vehiculo_origen)
            {
                // Intra-ruta: verificar solo un vehículo
                movimiento_valido = verificarRestricciones(vehiculo_origen->vehiculo, vrp, instancia_distancias);
            }
            else
            {
                // Inter-ruta: verificar ambos vehículos
                movimiento_valido = verificarRestricciones(vehiculo_origen->vehiculo, vrp, instancia_distancias) &&
                                  verificarRestricciones(vehiculo_destino->vehiculo, vrp, instancia_distancias);
            }
        }
        
        if (movimiento_valido)
        {
            liberar_memoria_arreglo_int(segmento);
            return true; // Movimiento 2.5-opt exitoso
        }
        
        // Revertir cambios si no fue válido
        if (insercion_exitosa)
        {
            // Eliminar el segmento del destino
            for (int i = longitud_segmento - 1; i >= 0; i--)
            {
                eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp, segmento[i], instancia_distancias);
            }
        }
        
        // Reinsertar el segmento en la posición original
        for (int i = 0; i < longitud_segmento; i++)
        {
            insertarClienteEnPosicion(vehiculo_origen->vehiculo, vrp, segmento[i], pos_inicio_segmento + i, instancia_distancias);
        }
        
        liberar_memoria_arreglo_int(segmento);
    }
    
    return false; // No se encontró un movimiento 2.5-opt válido
}


// // Variante más agresiva de 2.5-opt que prueba diferentes longitudes de segmento
// bool opt_2_5_adaptativo(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
// {
//     int intentos_maximos = 15;
    
//     // Probar diferentes longitudes de segmento
//     for (int longitud = 1; longitud <= 3; longitud++)
//     {
//         for (int intento = 0; intento < intentos_maximos; intento++)
//         {
//             nodo_vehiculo *vehiculo_origen = seleccionar_vehiculo_aleatorio(hormiga);
            
//             if (!vehiculo_origen || vehiculo_origen->vehiculo->clientes_contados < longitud + 2)
//                 continue;
            
//             // Decidir tipo de movimiento
//             bool es_inter_ruta = (rand() % 3 == 0); // 33% probabilidad inter-ruta
//             nodo_vehiculo *vehiculo_destino = vehiculo_origen;
            
//             if (es_inter_ruta)
//             {
//                 vehiculo_destino = seleccionar_vehiculo_aleatorio(hormiga);
//                 if (!vehiculo_destino || vehiculo_destino == vehiculo_origen)
//                     vehiculo_destino = vehiculo_origen;
//             }
            
//             // Seleccionar posiciones
//             int pos_inicio = (rand() % (vehiculo_origen->vehiculo->clientes_contados - longitud - 1)) + 1;
//             int pos_destino;
            
//             if (vehiculo_destino == vehiculo_origen)
//             {
//                 // Intra-ruta: evitar solapamiento
//                 do {
//                     pos_destino = (rand() % (vehiculo_destino->vehiculo->clientes_contados - 1)) + 1;
//                 } while (abs(pos_destino - pos_inicio) < longitud + 1);
//             }
//             else
//             {
//                 pos_destino = (rand() % (vehiculo_destino->vehiculo->clientes_contados - 1)) + 1;
//             }
            
//             // Guardar configuración original
//             int *segmento = asignar_memoria_arreglo_int(longitud);
//             nodo_ruta *nodo_temp = vehiculo_origen->vehiculo->ruta->cabeza->siguiente;
            
//             for (int i = 0; i < pos_inicio; i++)
//                 nodo_temp = nodo_temp->siguiente;
            
//             for (int i = 0; i < longitud; i++)
//             {
//                 segmento[i] = nodo_temp->cliente;
//                 nodo_temp = nodo_temp->siguiente;
//             }
            
//             // Realizar movimiento
//             bool exito = realizar_movimiento_25opt(vehiculo_origen, vehiculo_destino, segmento, 
//                                                  longitud, pos_inicio, pos_destino, vrp, instancia_distancias);
            
//             if (exito)
//             {
//                 liberar_memoria_arreglo_int(segmento);
//                 return true;
//             }
            
//             liberar_memoria_arreglo_int(segmento);
//         }
//     }
    
//     return false;
// }

// // Función auxiliar para realizar el movimiento 2.5-opt
// bool realizar_movimiento_25opt(nodo_vehiculo *vehiculo_origen, nodo_vehiculo *vehiculo_destino,
//                               int *segmento, int longitud, int pos_inicio, int pos_destino,
//                               struct vrp_configuracion *vrp, double **instancia_distancias)
// {
//     // Eliminar segmento de origen
//     for (int i = longitud - 1; i >= 0; i--)
//     {
//         eliminar_cliente_ruta(vehiculo_origen->vehiculo, vrp, segmento[i], instancia_distancias);
//     }
    
//     // Insertar en destino
//     bool insercion_exitosa = true;
//     for (int i = 0; i < longitud; i++)
//     {
//         if (!insertarClienteEnPosicion(vehiculo_destino->vehiculo, vrp, segmento[i], 
//                                      pos_destino + i, instancia_distancias))
//         {
//             insercion_exitosa = false;
//             // Revertir inserciones parciales
//             for (int j = i - 1; j >= 0; j--)
//             {
//                 eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp, segmento[j], instancia_distancias);
//             }
//             break;
//         }
//     }
    
//     // Verificar factibilidad
//     bool es_factible = false;
//     if (insercion_exitosa)
//     {
//         if (vehiculo_destino == vehiculo_origen)
//         {
//             es_factible = verificarRestricciones(vehiculo_origen->vehiculo, vrp, instancia_distancias);
//         }
//         else
//         {
//             es_factible = verificarRestricciones(vehiculo_origen->vehiculo, vrp, instancia_distancias) &&
//                          verificarRestricciones(vehiculo_destino->vehiculo, vrp, instancia_distancias);
//         }
//     }
    
//     if (es_factible)
//         return true;
    
//     // Revertir si no es factible
//     if (insercion_exitosa)
//     {
//         for (int i = longitud - 1; i >= 0; i--)
//         {
//             eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp, segmento[i], instancia_distancias);
//         }
//     }
    
//     // Restaurar en posición original
//     for (int i = 0; i < longitud; i++)
//     {
//         insertarClienteEnPosicion(vehiculo_origen->vehiculo, vrp, segmento[i], 
//                                 pos_inicio + i, instancia_distancias);
//     }
    
//     return false;
// }