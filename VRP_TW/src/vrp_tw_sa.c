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

// Evalúa la función objetivo para la solución vecina
void evaluaFO_SA(struct individuo *ind, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    ind->metal->fitness_solucion_vecina = 0.0;

    struct lista_ruta *ruta;
    nodo_ruta *nodo_actual;
    double fitness_vehiculo;
    int cliente_actual, cliente_siguiente;
    struct nodo_vehiculo *vehiculo_actual = ind->metal->solucion_vecina->cabeza;

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
        ind->metal->fitness_solucion_vecina += fitness_vehiculo;
        vehiculo_actual = vehiculo_actual->siguiente;
    }
}

// Mueve un cliente aleatorio de un vehículo a otro (si es válido)
bool moverClienteVehiculo(struct individuo *ind, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    int cliente_posicion, cliente;
    nodo_vehiculo *vehiculo_origen = NULL, *vehiculo_destino = NULL;
    nodo_ruta *nodo_ruta_cliente = NULL;
    bool bandera = false;

    vehiculo_origen = seleccionar_vehiculo_aleatorio(ind);
    if (vehiculo_origen == NULL)
        return false;

    // Seleccionamos aleatoriamente la posicion del cliente del vehiculo ejemplo [0,5,6,8,0] solo selecciona entre 5 y 8 como indice el 0 el 5 y inicde 2 el 8 ejemplod e 3 clientes contados
    cliente_posicion = (rand() % vehiculo_origen->vehiculo->clientes_contados);

    // Asignamos nodo ruta el sigiente de la cabeza proque la cavbeza ruta seria el 0 el siguiente seria el 5, suponiendo que elige el 8 la seleccion del ejemplo
    nodo_ruta_cliente = vehiculo_origen->vehiculo->ruta->cabeza->siguiente;

    for (int i = 0; i < cliente_posicion; i++)
        nodo_ruta_cliente = nodo_ruta_cliente->siguiente;

    if (nodo_ruta_cliente == NULL)
        return false;

    cliente = nodo_ruta_cliente->cliente;

    // printf("🔄 Intentando mover cliente %d desde la ruta origen %d\n", cliente, vehiculo_origen->vehiculo->id_vehiculo);

    eliminar_cliente_ruta(vehiculo_origen->vehiculo, vrp, cliente, instancia_distancias);

    vehiculo_destino = ind->metal->solucion_vecina->cabeza;

    while (vehiculo_destino != NULL)
    {
        if (vehiculo_destino == vehiculo_origen)
        {
            vehiculo_destino = vehiculo_destino->siguiente;
            continue;
        }

        for (int posicion = 0; posicion <= vehiculo_destino->vehiculo->clientes_contados; posicion++)
        {
            // Intentamos insertar el cliente en la posición especificada
            bandera = insertarClienteEnPosicion(vehiculo_destino->vehiculo, vrp, cliente, posicion, instancia_distancias);

            // Comprobamos si la inserción fue exitosa y si cumple con las restricciones
            if (bandera &&
                vehiculo_destino->vehiculo->capacidad_acumulada <= vrp->num_capacidad &&
                vehiculo_destino->vehiculo->vt_actual <= vehiculo_destino->vehiculo->vt_final)
            {
                // Inserción exitosa y dentro de los límites
                // printf("✅ Cliente %d insertado con éxito en ruta destino %d en posición %d\n",
                //         cliente, vehiculo_destino->vehiculo->id_vehiculo, posicion);
                return true; // Salimos de la función si la inserción es exitosa
            }

            // Revertimos si la inserción no es factible (capacidad o ventana de tiempo violada)
            eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp, cliente, instancia_distancias);

            // Información sobre la reversión
            // printf("❌ Cliente %d no fue factible en ruta %d en posición %d (capacidad o ventana de tiempo violada)\n",
            //        cliente, vehiculo_destino->vehiculo->id_vehiculo, posicion);
        }

        vehiculo_destino = vehiculo_destino->siguiente;
    }

    // printf("⚠️  Cliente %d no pudo ser reinsertado en ninguna otra ruta\n", cliente);
    return false;
}

// Intercambia dos clientes aleatorios dentro de un mismo vehículo
bool intercambiarClienteRuta(struct individuo *ind, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    int vehiculo_aleatorio, intentos = 10, cliente1_idx, cliente2_idx, temp;
    nodo_vehiculo *vehiculo_actual = NULL;
    nodo_ruta *nodo1 = NULL, *nodo2 = NULL;

    while (intentos--)
    {
        vehiculo_aleatorio = (rand() % ind->hormiga->vehiculos_necesarios) + 1;

        vehiculo_actual = ind->metal->solucion_vecina->cabeza;
        while (vehiculo_actual != NULL)
        {
            if (vehiculo_actual->vehiculo->id_vehiculo == vehiculo_aleatorio)
                break;
            vehiculo_actual = vehiculo_actual->siguiente;
        }

        if (vehiculo_actual->vehiculo->clientes_contados < 2)
            continue;

        break;
    }

    if (vehiculo_actual == NULL || vehiculo_actual->vehiculo->clientes_contados < 2)
        return false;

    do
    {
        cliente1_idx = rand() % vehiculo_actual->vehiculo->clientes_contados;
        cliente2_idx = rand() % vehiculo_actual->vehiculo->clientes_contados;
    } while (cliente1_idx == cliente2_idx);

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

    // Recalcular tiempo de ruta
    vehiculo_actual->vehiculo->vt_actual = recalcular_tiempo_ruta(vehiculo_actual->vehiculo->ruta, vrp, instancia_distancias);

    // Verificamos si sigue siendo factible con respecto a la ventana
    if (vehiculo_actual->vehiculo->vt_actual > vehiculo_actual->vehiculo->vt_final)
    {
        // Revertimos el intercambio
        temp = nodo1->cliente;
        nodo1->cliente = nodo2->cliente;
        nodo2->cliente = temp;

        // Restauramos el tiempo original
        vehiculo_actual->vehiculo->vt_actual = recalcular_tiempo_ruta(vehiculo_actual->vehiculo->ruta, vrp, instancia_distancias);

        return false;
    }

    return true;
}

// Copia la solución actual como base para generar un vecino
void generar_vecino(struct individuo *ind, struct vrp_configuracion *vrp)
{
    if (ind->metal->solucion_vecina)
        liberar_lista_vehiculos(ind->metal->solucion_vecina);

    ind->metal->solucion_vecina = copiar_lista_vehiculos(ind->metal->solucion_actual);
}

// Algoritmo de Recocido Simulado (SA)
// Algoritmo de Recocido Simulado (SA)
void sa(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_distancias)
{
    double temperatura = ind->temperatura_inicial;
    double delta;
    bool aceptado = false;

    // Inicialización de soluciones
    ind->metal->mejor_solucion = copiar_lista_vehiculos(ind->metal->solucion_inicial);
    ind->metal->fitness_mejor_solucion = ind->metal->fitness_solucion_inicial;
    ind->metal->solucion_actual = copiar_lista_vehiculos(ind->metal->solucion_inicial);
    ind->metal->fitness_solucion_actual = ind->metal->fitness_solucion_inicial;

    // Ciclo de enfriamiento
    while (temperatura > ind->temperatura_final)
    {
        // // >>> Imprime la temperatura y los valores de fitness en cada paso del enfriamiento
        // printf("Temp: %.4f | Fitness actual: %.4f | Mejor: %.4f\n",
        //        temperatura,
        //        ind->metal->fitness_solucion_actual,
        //        ind->metal->fitness_mejor_solucion);
        // <<<

        for (int i = 0; i < ind->numIteracionesSA; i++)
        {
            generar_vecino(ind, vrp);

            // Se decide qué movimiento hacer según una probabilidad controlada por temperatura
            if (ind->hormiga->vehiculos_necesarios > 1)
            {
                if ((double)rand() / RAND_MAX < ind->factor_control * (1.0 - (temperatura / ind->temperatura_inicial)))
                    aceptado = intercambiarClienteRuta(ind, vrp, instancia_distancias);
                else
                    aceptado = moverClienteVehiculo(ind, vrp, instancia_distancias);
            }
            else
            {
                aceptado = intercambiarClienteRuta(ind, vrp, instancia_distancias);
            }

            if (!aceptado)
                continue;

            evaluaFO_SA(ind, vrp, instancia_distancias);
            delta = ind->metal->fitness_solucion_vecina - ind->metal->fitness_solucion_actual;

            bool aceptar = false;
            if (delta < 0)
                aceptar = true;
            else if ((double)rand() / RAND_MAX < exp(-delta / temperatura))
                aceptar = true;

            if (aceptar)
            {
                liberar_lista_vehiculos(ind->metal->solucion_actual);
                ind->metal->solucion_actual = copiar_lista_vehiculos(ind->metal->solucion_vecina);
                ind->metal->fitness_solucion_actual = ind->metal->fitness_solucion_vecina;

                if (ind->metal->fitness_solucion_actual < ind->metal->fitness_mejor_solucion)
                {
                    liberar_lista_vehiculos(ind->metal->mejor_solucion);
                    ind->metal->mejor_solucion = copiar_lista_vehiculos(ind->metal->solucion_actual);
                    ind->metal->fitness_mejor_solucion = ind->metal->fitness_solucion_actual;
                }
            }
        }

        temperatura *= ind->factor_enfriamiento; // Disminuye temperatura
    }

    liberar_lista_vehiculos(ind->metal->solucion_actual);
}

// Inicializa la estructura metal de un individuo
void inicializar_metal(struct individuo *ind)
{
    ind->metal = asignar_memoria_metal();
    ind->metal->solucion_vecina = NULL;
    ind->metal->solucion_actual = NULL;
    ind->metal->mejor_solucion = NULL;
    ind->metal->fitness_solucion_actual = 0.0;
    ind->metal->fitness_solucion_vecina = 0.0;
    ind->metal->fitness_mejor_solucion = 0.0;
    ind->metal->solucion_inicial = copiar_lista_vehiculos(ind->hormiga->flota);
    ind->metal->fitness_solucion_inicial = ind->hormiga->fitness_global;
}

// Función principal que ejecuta SA sobre un individuo del VRP
void vrp_tw_sa(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_distancias)
{

    inicializar_metal(ind);
    sa(vrp, ind, instancia_distancias);

    // Si se mejora la solución global, se guarda
    if (ind->metal->fitness_mejor_solucion < ind->hormiga->fitness_global)
    {
        ind->fitness = ind->metal->fitness_mejor_solucion;
        ind->hormiga->fitness_global = ind->metal->fitness_mejor_solucion;

        if (ind->hormiga->flota)
            liberar_lista_vehiculos(ind->hormiga->flota);

        ind->hormiga->flota = copiar_lista_vehiculos(ind->metal->mejor_solucion);
    }

    liberar_memoria_metal(ind); // Limpieza final
}
