#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "../include/vrp_tw_sa.h"
#include "../include/estructuras.h"
#include "../include/lista_flota.h"
#include "../include/lista_ruta.h"
#include "../include/control_memoria.h"
#include "../include/salida_datos.h"
#include "../include/movimientos_sa.h"

void calculamosVentanasCapacidad(struct lista_vehiculos *flota, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    struct nodo_vehiculo *nodoVehiculo = flota->cabeza;

    while (nodoVehiculo != NULL)
    {
        struct vehiculo *vehiculo = nodoVehiculo->vehiculo;
        struct nodo_ruta *clienteActual = vehiculo->ruta->cabeza;
        struct nodo_ruta *clienteAnterior = NULL;

        double tiempo = 0.0;
        double capacidad = 0.0;
        double inicio = 0.0;
        double fin = 0.0;

        if (vehiculo->clientes_contados > 0)
        {
            int clientesReales = 0;

            struct nodo_ruta *temp = vehiculo->ruta->cabeza;
            while (temp != NULL)
            {
                if (temp->cliente != 0)
                {
                    clientesReales++;
                }
                temp = temp->siguiente;
            }

            if (clientesReales > 0)
            {
                while (clienteActual != NULL)
                {
                    int id_actual = clienteActual->cliente;

                    if (clienteAnterior == NULL)
                    {
                        if (id_actual != 0)
                        {
                            tiempo += instancia_distancias[0][id_actual] / vehiculo->velocidad;

                            if (tiempo < vrp->clientes[id_actual].vt_inicial)
                            {
                                tiempo = vrp->clientes[id_actual].vt_inicial;
                            }

                            inicio = tiempo;
                            capacidad += vrp->clientes[id_actual].demanda_capacidad;

                            if (tiempo > vrp->clientes[id_actual].vt_final)
                            {
                                printf("Violación de ventana de tiempo: Cliente %d, llegada %.2f > ventana final %.2f\n",
                                       id_actual, tiempo, vrp->clientes[id_actual].vt_final);
                            }
                        }
                    }
                    else
                    {
                        if (clienteAnterior->cliente != 0)
                        {
                            tiempo += vrp->clientes[clienteAnterior->cliente].tiempo_servicio;
                        }

                        tiempo += instancia_distancias[clienteAnterior->cliente][id_actual] / vehiculo->velocidad;

                        if (tiempo < vrp->clientes[id_actual].vt_inicial)
                        {
                            tiempo = vrp->clientes[id_actual].vt_inicial;
                        }

                        if (id_actual != 0)
                        {
                            capacidad += vrp->clientes[id_actual].demanda_capacidad;

                            if (tiempo > vrp->clientes[id_actual].vt_final)
                            {
                                printf("Violación de ventana de tiempo: Cliente %d, llegada %.2f > ventana final %.2f\n",
                                       id_actual, tiempo, vrp->clientes[id_actual].vt_final);
                            }
                        }
                    }

                    if (clienteActual->siguiente == NULL || clienteActual->siguiente->cliente == 0)
                    {
                        if (id_actual != 0)
                        {
                            tiempo += vrp->clientes[id_actual].tiempo_servicio;
                        }

                        tiempo += instancia_distancias[id_actual][0] / vehiculo->velocidad;
                        fin = tiempo;

                        if (clienteActual->siguiente != NULL && clienteActual->siguiente->cliente == 0)
                        {
                            clienteActual = clienteActual->siguiente;
                        }
                    }

                    clienteAnterior = clienteActual;
                    clienteActual = clienteActual->siguiente;
                }
            }
        }

        vehiculo->capacidad_acumulada = capacidad;
        vehiculo->tiempo_salida_vehiculo = inicio;
        vehiculo->tiempo_llegada_vehiculo = fin;

        // Verificación de restricción de capacidad
        if (capacidad > vehiculo->capacidad_maxima)
        {
            printf("Violación de capacidad en Vehículo ID %d: %.2f > %.2f\n",
                   vehiculo->id_vehiculo, capacidad, vehiculo->capacidad_maxima);
        }

        nodoVehiculo = nodoVehiculo->siguiente;
    }
}

void evaluaFO_SA(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    hormiga->metal->fitness_solucion_vecina = 0.0;

    struct lista_ruta *ruta;
    nodo_ruta *nodo_actual;
    double fitness_vehiculo;
    int cliente_actual, cliente_siguiente;
    struct nodo_vehiculo *vehiculo_actual = hormiga->metal->solucion_vecina->cabeza;

    // Recorre todos los vehículos de la solución vecina
    while (vehiculo_actual != NULL)
    {
        fitness_vehiculo = 0.0;
        ruta = vehiculo_actual->vehiculo->ruta;
        nodo_actual = ruta->cabeza;

        // Suma la distancia entre pares consecutivos de clientes
        while (nodo_actual->siguiente != NULL)
        {
            cliente_actual = nodo_actual->cliente;
            cliente_siguiente = nodo_actual->siguiente->cliente;

            fitness_vehiculo += instancia_distancias[cliente_actual][cliente_siguiente];
            nodo_actual = nodo_actual->siguiente;
        }

        vehiculo_actual->vehiculo->fitness_vehiculo = fitness_vehiculo;
        hormiga->metal->fitness_solucion_vecina += fitness_vehiculo;
        vehiculo_actual = vehiculo_actual->siguiente;
    }
}

// bool moverDosClientesVehiculos(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
// {
//     nodo_vehiculo *primer_vehiculo = NULL, *segundo_vehiculo = NULL;
//     nodo_ruta *primer_nodo_cliente = NULL, *segundo_nodo_cliente = NULL;
//     int primer_cliente, segundo_cliente;
//     int primer_posicion, segunda_posicion;
//     bool factible = false;

//     for (int i = 0; i < 10; i++)
//     {
//         bool vehiculos_validos = false;
//         for (int j = 0; j < 10; j++)
//         {
//             primer_vehiculo = seleccionar_vehiculo_aleatorio(hormiga);
//             segundo_vehiculo = seleccionar_vehiculo_aleatorio(hormiga);

//             if (!primer_vehiculo || !segundo_vehiculo || !primer_vehiculo->vehiculo || !segundo_vehiculo->vehiculo)
//                 continue;

//             if (primer_vehiculo->vehiculo->clientes_contados > 2 &&
//                 segundo_vehiculo->vehiculo->clientes_contados > 2)
//             {
//                 vehiculos_validos = true;
//                 break;
//             }
//         }
//         if (!vehiculos_validos)
//             return false;

//         // Selección de posiciones
//         bool posiciones_validas = false;
//         for (int j = 0; j < 10; j++)
//         {
//             primer_posicion = rand() % (primer_vehiculo->vehiculo->clientes_contados - 2) + 1;
//             segunda_posicion = rand() % (segundo_vehiculo->vehiculo->clientes_contados - 2) + 1;

//             // Si es el mismo vehículo, no permitir misma posición
//             if (primer_vehiculo != segundo_vehiculo || primer_posicion != segunda_posicion)
//             {
//                 posiciones_validas = true;
//                 break;
//             }
//         }
//         if (!posiciones_validas)
//             return false;

//         // Obtener los nodos
//         primer_nodo_cliente = primer_vehiculo->vehiculo->ruta->cabeza->siguiente;
//         for (int k = 0; k < primer_posicion && primer_nodo_cliente; k++)
//             primer_nodo_cliente = primer_nodo_cliente->siguiente;

//         segundo_nodo_cliente = segundo_vehiculo->vehiculo->ruta->cabeza->siguiente;
//         for (int k = 0; k < segunda_posicion && segundo_nodo_cliente; k++)
//             segundo_nodo_cliente = segundo_nodo_cliente->siguiente;

//         if (!primer_nodo_cliente || !segundo_nodo_cliente)
//             continue;

//         // Clientes
//         primer_cliente = primer_nodo_cliente->cliente;
//         segundo_cliente = segundo_nodo_cliente->cliente;

//         // Si es el mismo vehículo, hacer swap
//         if (primer_vehiculo == segundo_vehiculo)
//         {
//             int temp = primer_nodo_cliente->cliente;
//             primer_nodo_cliente->cliente = segundo_nodo_cliente->cliente;
//             segundo_nodo_cliente->cliente = temp;

//             factible = verificarRestricciones(primer_vehiculo->vehiculo, vrp, instancia_distancias);

//             if (!factible)
//             {
//                 // Revertir el swap
//                 temp = primer_nodo_cliente->cliente;
//                 primer_nodo_cliente->cliente = segundo_nodo_cliente->cliente;
//                 segundo_nodo_cliente->cliente = temp;
//             }

//             return factible;
//         }
//         else
//         {
//             // Vehículos distintos → eliminación e inserción cruzada
//             eliminar_cliente_ruta(primer_vehiculo->vehiculo, vrp, primer_cliente, instancia_distancias);
//             eliminar_cliente_ruta(segundo_vehiculo->vehiculo, vrp, segundo_cliente, instancia_distancias);

//             bool insercion1 = insertarClienteEnPosicion(primer_vehiculo->vehiculo, vrp, segundo_cliente, primer_posicion, instancia_distancias);
//             bool insercion2 = insertarClienteEnPosicion(segundo_vehiculo->vehiculo, vrp, primer_cliente, segunda_posicion, instancia_distancias);

//             factible = insercion1 && insercion2 &&
//                        verificarRestricciones(primer_vehiculo->vehiculo, vrp, instancia_distancias) &&
//                        verificarRestricciones(segundo_vehiculo->vehiculo, vrp, instancia_distancias);

//             if (!factible)
//             {
//                 eliminar_cliente_ruta(primer_vehiculo->vehiculo, vrp, segundo_cliente, instancia_distancias);
//                 eliminar_cliente_ruta(segundo_vehiculo->vehiculo, vrp, primer_cliente, instancia_distancias);

//                 insertarClienteEnPosicion(primer_vehiculo->vehiculo, vrp, primer_cliente, primer_posicion, instancia_distancias);
//                 insertarClienteEnPosicion(segundo_vehiculo->vehiculo, vrp, segundo_cliente, segunda_posicion, instancia_distancias);
//             }
//             else
//             {
//                 return factible;
//             }
//         }
//     }

//     return factible;
// }

// bool moverClienteEntreVehiculos(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
// {
//     nodo_vehiculo *vehiculo_origen = NULL, *vehiculo_destino = NULL;
//     nodo_ruta *nodo_cliente = NULL;
//     int cliente, posicion_origen;

//     for (int intento = 0; intento < 10; intento++)
//     {
//         // Intentar seleccionar vehículos válidos
//         bool vehiculos_validos = false;
//         for (int j = 0; j < 10; j++)
//         {
//             vehiculo_origen = seleccionar_vehiculo_aleatorio(hormiga);
//             vehiculo_destino = seleccionar_vehiculo_aleatorio(hormiga);

//             if (!vehiculo_origen || !vehiculo_destino ||
//                 !vehiculo_origen->vehiculo || !vehiculo_destino->vehiculo)
//                 continue;

//             if (vehiculo_origen == vehiculo_destino)
//                 continue;

//             if (vehiculo_origen->vehiculo->clientes_contados > 2) // debe tener al menos un cliente
//             {
//                 vehiculos_validos = true;
//                 break;
//             }
//         }

//         if (!vehiculos_validos)
//             continue;

//         int total_clientes_origen = vehiculo_origen->vehiculo->clientes_contados;

//         if (total_clientes_origen <= 2)
//             continue;

//         posicion_origen = rand() % (total_clientes_origen - 2) + 1;

//         nodo_cliente = vehiculo_origen->vehiculo->ruta->cabeza->siguiente;
//         for (int k = 0; k < posicion_origen && nodo_cliente; k++)
//             nodo_cliente = nodo_cliente->siguiente;

//         if (!nodo_cliente)
//             continue;

//         cliente = nodo_cliente->cliente;

//         // Eliminar cliente del vehículo origen
//         eliminar_cliente_ruta(vehiculo_origen->vehiculo, vrp, cliente, instancia_distancias);

//         int posiciones_destino = vehiculo_destino->vehiculo->clientes_contados - 1; // evitar depósito final

//         for (int pos = 1; pos < posiciones_destino; pos++)
//         {
//             bool insertado = insertarClienteEnPosicion(vehiculo_destino->vehiculo, vrp, cliente, pos, instancia_distancias);

//             if (insertado &&
//                 verificarRestricciones(vehiculo_origen->vehiculo, vrp, instancia_distancias) &&
//                 verificarRestricciones(vehiculo_destino->vehiculo, vrp, instancia_distancias))
//             {
//                 return true; // Movimiento exitoso
//             }

//             if (insertado)
//             {
//                 // Revertir si no fue factible
//                 eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp, cliente, instancia_distancias);
//             }
//         }

//         // Reinsertar en origen si no se logró insertar en el destino
//         insertarClienteEnPosicion(vehiculo_origen->vehiculo, vrp, cliente, posicion_origen, instancia_distancias);
//     }

//     return false; // No se logró mover el cliente
// }

// // Intercambia dos clientes aleatorios dentro de un mismo vehículo
// bool intercambiarClienteRuta(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
// {
//     int vehiculo_aleatorio, intentos = 10, cliente1_idx, cliente2_idx, temp;
//     nodo_vehiculo *vehiculo_actual = NULL;
//     nodo_ruta *nodo1 = NULL, *nodo2 = NULL;

//     while (intentos--)
//     {
//         vehiculo_aleatorio = (rand() % hormiga->vehiculos_necesarios) + 1;

//         vehiculo_actual = hormiga->metal->solucion_vecina->cabeza;
//         while (vehiculo_actual != NULL)
//         {
//             if (vehiculo_actual->vehiculo->id_vehiculo == vehiculo_aleatorio)
//                 break;
//             vehiculo_actual = vehiculo_actual->siguiente;
//         }

//         if (vehiculo_actual == NULL || vehiculo_actual->vehiculo->clientes_contados < 2)
//             continue;

//         break;
//     }

//     if (vehiculo_actual == NULL || vehiculo_actual->vehiculo->clientes_contados < 2)
//         return false;

//     do
//     {
//         cliente1_idx = rand() % vehiculo_actual->vehiculo->clientes_contados;
//         cliente2_idx = rand() % vehiculo_actual->vehiculo->clientes_contados;
//     } while (cliente1_idx == cliente2_idx);

//     nodo1 = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
//     nodo2 = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;

//     for (int i = 0; i < cliente1_idx; i++)
//         nodo1 = nodo1->siguiente;
//     for (int i = 0; i < cliente2_idx; i++)
//         nodo2 = nodo2->siguiente;

//     if (nodo1 == NULL || nodo2 == NULL)
//         return false;

//     // Intercambio tentativo
//     temp = nodo1->cliente;
//     nodo1->cliente = nodo2->cliente;
//     nodo2->cliente = temp;
//     // Verificamos si sigue siendo factible con respecto a la ventana de tiempo y capacidad
//     if (!verificarRestricciones(vehiculo_actual->vehiculo, vrp, instancia_distancias))
//     {
//         // Revertimos el intercambio si viola las restricciones
//         temp = nodo1->cliente;
//         nodo1->cliente = nodo2->cliente;
//         nodo2->cliente = temp;

//         return false;
//     }

//     return true;
// }

// // Invierte el orden de un segmento de la ruta
// bool invertirSegmentoRuta(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias)
// {

//     nodo_vehiculo *vehiculo_actual = NULL;
//     nodo_ruta *inicio = NULL, *fin = NULL;
//     int idx1, idx2, total_clientes;
//     int i;
//     int intentos_maximos = 10; // Limitamos el número de intentos para evitar recursión infinita

//     // Seleccionamos un vehículo aleatorio que tenga al menos 3 clientes
//     for (int intento = 0; intento < 10; intento++)
//     {
//         vehiculo_actual = seleccionar_vehiculo_aleatorio(hormiga);

//         if (vehiculo_actual && vehiculo_actual->vehiculo->clientes_contados >= 3)
//             break;
//     }

//     if (vehiculo_actual == NULL || vehiculo_actual->vehiculo->clientes_contados < 3)
//         return false;

//     // Iteramos hasta encontrar una inversión válida o agotar intentos
//     for (int intento = 0; intento < intentos_maximos; intento++)
//     {
//         total_clientes = vehiculo_actual->vehiculo->clientes_contados;

//         // Elegimos dos posiciones distintas idx1 < idx2 dentro de la ruta del vehículo
//         idx1 = rand() % total_clientes;
//         idx2 = rand() % total_clientes;

//         while (idx1 == idx2)
//             idx2 = rand() % total_clientes;

//         if (idx1 > idx2)
//         {
//             int tmp = idx1;
//             idx1 = idx2;
//             idx2 = tmp;
//         }

//         // Guardamos el orden original de los clientes para poder revertir si es necesario
//         int *clientes_originales = asignar_memoria_arreglo_int(idx2 - idx1 + 1);

//         nodo_ruta *nodo_temp = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
//         for (i = 0; i < idx1; i++)
//             nodo_temp = nodo_temp->siguiente;

//         for (i = 0; i <= idx2 - idx1; i++)
//         {
//             clientes_originales[i] = nodo_temp->cliente;
//             nodo_temp = nodo_temp->siguiente;
//         }

//         // Obtenemos punteros a los nodos en las posiciones idx1 e idx2
//         inicio = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
//         for (i = 0; i < idx1; i++)
//             inicio = inicio->siguiente;

//         fin = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
//         for (i = 0; i < idx2; i++)
//             fin = fin->siguiente;

//         if (inicio == NULL || fin == NULL)
//         {
//             free(clientes_originales);
//             return false;
//         }

//         // Invertimos el segmento entre idx1 e idx2
//         int left = idx1;
//         int right = idx2;

//         while (left < right)
//         {
//             nodo_ruta *nodo_left = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
//             for (i = 0; i < left; i++)
//                 nodo_left = nodo_left->siguiente;

//             nodo_ruta *nodo_right = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
//             for (i = 0; i < right; i++)
//                 nodo_right = nodo_right->siguiente;

//             int temp = nodo_left->cliente;
//             nodo_left->cliente = nodo_right->cliente;
//             nodo_right->cliente = temp;

//             left++;
//             right--;
//         }

//         // Verificamos que las restricciones sigan válidas
//         if (verificarRestricciones(vehiculo_actual->vehiculo, vrp, instancia_distancias))
//         {
//             free(clientes_originales);
//             return true; // Inversión exitosa y factible
//         }

//         // Si no es válido, revertimos a la configuración original
//         nodo_temp = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
//         for (i = 0; i < idx1; i++)
//             nodo_temp = nodo_temp->siguiente;

//         for (i = 0; i <= idx2 - idx1; i++)
//         {
//             nodo_temp->cliente = clientes_originales[i];
//             nodo_temp = nodo_temp->siguiente;
//         }

//         free(clientes_originales);
//         // Continuamos al siguiente intento
//     }

//     return false; // No se encontró una inversión válida después de varios intentos
// }

// Copia la solución actual como base para generar un vecino
void generar_vecino(struct hormiga *hormiga, struct vrp_configuracion *vrp)
{
    if (hormiga->metal->solucion_vecina)
        liberar_lista_vehiculos(hormiga->metal->solucion_vecina);

    hormiga->metal->solucion_vecina = copiar_lista_vehiculos(hormiga->metal->solucion_actual);
}

// Algoritmo de Recocido Simulado (SA)
void sa(struct vrp_configuracion *vrp, struct hormiga *hormiga, struct individuo *ind, double **instancia_distancias)
{
    double temperatura = ind->temperatura_inicial, delta, prob, factor;
    bool aceptado = false;

    // Inicialización de soluciones
    hormiga->metal->mejor_solucion = copiar_lista_vehiculos(hormiga->metal->solucion_inicial);
    hormiga->metal->fitness_mejor_solucion = hormiga->metal->fitness_solucion_inicial;
    hormiga->metal->solucion_actual = copiar_lista_vehiculos(hormiga->metal->solucion_inicial);
    hormiga->metal->fitness_solucion_actual = hormiga->metal->fitness_solucion_inicial;

    // Ciclo de enfriamiento
    while (temperatura > ind->temperatura_final)
    {
        //>>> Imprime la temperatura y los valores de fitness en cada paso del enfriamiento
        // printf("Temp: %.4f | Fitness actual: %.4f | Mejor: %.4f\n",
        //        temperatura,
        //        hormiga->metal->fitness_solucion_actual,
        //        hormiga->metal->fitness_mejor_solucion);
        // <<<

        for (int i = 0; i < ind->numIteracionesSA; i++)
        {
            generar_vecino(hormiga, vrp);

            double prob = (double)rand() / RAND_MAX;
            double temp_invertida = temperatura / ind->temperatura_inicial; // va de 0 a 1 (inicio a fin)
            bool aceptado = false;

            if (hormiga->vehiculos_necesarios > 1)
            {
                // Para múltiples vehículos, usar distribución proporcional que dependa de la temperatura invertida
                // Usamos temp_invertida para favorecer ciertos movimientos a medida que la temperatura baja
                double umbral1 = 0.2 * temp_invertida;           // movimientos locales al final (exploitation)
                double umbral2 = umbral1 + 0.2 * temp_invertida; // swap intra
                double umbral3 = umbral2 + 0.2;                  // reinserción (fijo, balanceado)
                double umbral4 = umbral3 + 0.2;                  // swap inter (exploración)
                // resto para opt_2_5 (exploración)

                if (prob < umbral1)
                    aceptado = opt_2(hormiga, vrp, instancia_distancias);
                else if (prob < umbral2)
                    aceptado = swap_inter(hormiga, vrp, instancia_distancias);

                else if (prob < umbral3)
                    aceptado = reinsercion_intra_inter(hormiga, vrp, instancia_distancias);
                else if (prob < umbral4)
                    aceptado = swap_intra(hormiga, vrp, instancia_distancias);
                //else
                    //aceptado = opt_2_5(hormiga, vrp, instancia_distancias);
            }
            else
            {
                // Para un solo vehículo, más simple, con distribución igualada pero adaptada por factor (que baja con la temperatura)
                double factor = ind->factor_control * (1.0 - temp_invertida);
                double umbral1 = factor / 3.0;
                double umbral2 = 2.0 * factor / 3.0;

                if (prob < umbral1)
                    aceptado = swap_intra(hormiga, vrp, instancia_distancias);
                else if (prob < umbral2)
                    aceptado = opt_2(hormiga, vrp, instancia_distancias);
                // else
                // aceptado = opt_2_5(hormiga, vrp, instancia_distancias);
            }

            if (!aceptado)
                continue;

            evaluaFO_SA(hormiga, vrp, instancia_distancias);
            delta = hormiga->metal->fitness_solucion_vecina - hormiga->metal->fitness_solucion_actual;

            bool aceptar = false;
            if (delta < 0 || (double)rand() / RAND_MAX < exp(-delta / temperatura * factor))
                aceptar = true;

            if (aceptar)
            {
                liberar_lista_vehiculos(hormiga->metal->solucion_actual);
                hormiga->metal->solucion_actual = copiar_lista_vehiculos(hormiga->metal->solucion_vecina);
                hormiga->metal->fitness_solucion_actual = hormiga->metal->fitness_solucion_vecina;

                if (hormiga->metal->fitness_solucion_actual < hormiga->metal->fitness_mejor_solucion)
                {
                    liberar_lista_vehiculos(hormiga->metal->mejor_solucion);
                    hormiga->metal->mejor_solucion = copiar_lista_vehiculos(hormiga->metal->solucion_actual);
                    hormiga->metal->fitness_mejor_solucion = hormiga->metal->fitness_solucion_actual;
                }
            }
        }

        temperatura *= ind->factor_enfriamiento; // Disminuye temperatura
    }

    liberar_lista_vehiculos(hormiga->metal->solucion_actual);
}

// Inicializa la estructura metal de un individuo
void inicializar_metal(struct hormiga *hormiga)
{
    hormiga->metal = asignar_memoria_metal();
    hormiga->metal->solucion_vecina = NULL;
    hormiga->metal->solucion_actual = NULL;
    hormiga->metal->mejor_solucion = NULL;
    hormiga->metal->fitness_solucion_actual = 0.0;
    hormiga->metal->fitness_solucion_vecina = 0.0;
    hormiga->metal->fitness_mejor_solucion = 0.0;
    hormiga->metal->solucion_inicial = copiar_lista_vehiculos(hormiga->flota);
    hormiga->metal->fitness_solucion_inicial = hormiga->fitness_global;
}

// Función principal que ejecuta SA sobre un individuo del VRP
void vrp_tw_sa(struct vrp_configuracion *vrp, struct hormiga *hormiga, struct individuo *ind, double **instancia_distancias)
{
    inicializar_metal(hormiga);
    sa(vrp, hormiga, ind, instancia_distancias);
    calculamosVentanasCapacidad(hormiga->metal->mejor_solucion, vrp, instancia_distancias);

    // Si se mejora la solución global, se guarda
    if (hormiga->metal->fitness_mejor_solucion < hormiga->fitness_global)
    {
        printf("\nMejoro de %.2lf a %.2lf", hormiga->fitness_global, hormiga->metal->fitness_mejor_solucion);
        hormiga->fitness_global = hormiga->metal->fitness_mejor_solucion;

        if (hormiga->flota)
            liberar_lista_vehiculos(hormiga->flota);

        hormiga->flota = copiar_lista_vehiculos(hormiga->metal->mejor_solucion);
    }
    else
    {
        printf("\nNo mejora %.2lf", hormiga->fitness_global);
    }

    liberar_memoria_metal(hormiga); // Limpieza final
}