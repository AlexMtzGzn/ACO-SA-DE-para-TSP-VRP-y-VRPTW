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

void calculamosVentanasCapacidad(struct lista_vehiculos *flota, struct vrp_configuracion *vrp, double **instancia_distancias) {
    struct nodo_vehiculo *nodoVehiculo = flota->cabeza;

    while (nodoVehiculo != NULL) {
        struct vehiculo *vehiculo = nodoVehiculo->vehiculo;
        struct nodo_ruta *clienteActual = vehiculo->ruta->cabeza;
        struct nodo_ruta *clienteAnterior = NULL;

        double tiempo = 0.0;
        double capacidad = 0.0;
        double inicio = 0.0;
        double fin = 0.0;

        if (vehiculo->clientes_contados > 0) {
            int clientesReales = 0;

            struct nodo_ruta *temp = vehiculo->ruta->cabeza;
            while (temp != NULL) {
                if (temp->cliente != 0) {
                    clientesReales++;
                }
                temp = temp->siguiente;
            }

            if (clientesReales > 0) {
                while (clienteActual != NULL) {
                    int id_actual = clienteActual->cliente;

                    if (clienteAnterior == NULL) {
                        if (id_actual != 0) {
                            tiempo += instancia_distancias[0][id_actual] / vehiculo->velocidad;

                            if (tiempo < vrp->clientes[id_actual].vt_inicial) {
                                tiempo = vrp->clientes[id_actual].vt_inicial;
                            }

                            inicio = tiempo;
                            capacidad += vrp->clientes[id_actual].demanda_capacidad;

                            if (tiempo > vrp->clientes[id_actual].vt_final) {
                                printf("Violación de ventana de tiempo: Cliente %d, llegada %.2f > ventana final %.2f\n",
                                       id_actual, tiempo, vrp->clientes[id_actual].vt_final);
                            }
                        }
                    } else {
                        if (clienteAnterior->cliente != 0) {
                            tiempo += vrp->clientes[clienteAnterior->cliente].tiempo_servicio;
                        }

                        tiempo += instancia_distancias[clienteAnterior->cliente][id_actual] / vehiculo->velocidad;

                        if (tiempo < vrp->clientes[id_actual].vt_inicial) {
                            tiempo = vrp->clientes[id_actual].vt_inicial;
                        }

                        if (id_actual != 0) {
                            capacidad += vrp->clientes[id_actual].demanda_capacidad;

                            if (tiempo > vrp->clientes[id_actual].vt_final) {
                                printf("Violación de ventana de tiempo: Cliente %d, llegada %.2f > ventana final %.2f\n",
                                       id_actual, tiempo, vrp->clientes[id_actual].vt_final);
                            }
                        }
                    }

                    if (clienteActual->siguiente == NULL || clienteActual->siguiente->cliente == 0) {
                        if (id_actual != 0) {
                            tiempo += vrp->clientes[id_actual].tiempo_servicio;
                        }

                        tiempo += instancia_distancias[id_actual][0] / vehiculo->velocidad;
                        fin = tiempo;

                        if (clienteActual->siguiente != NULL && clienteActual->siguiente->cliente == 0) {
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

        //Verificación de restricción de capacidad
        if (capacidad > vehiculo->capacidad_maxima) {
            printf("Violación de capacidad en Vehículo ID %d: %.2f > %.2f\n",
                   vehiculo->id_vehiculo, capacidad, vehiculo->capacidad_maxima);
        }

        nodoVehiculo = nodoVehiculo->siguiente;
    }
}

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

bool moverClienteVehiculo(struct individuo *ind, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    int cliente_posicion, cliente;
    nodo_vehiculo *vehiculo_origen = NULL, *vehiculo_destino = NULL;
    nodo_ruta *nodo_ruta_cliente = NULL;
    bool bandera = false;

    vehiculo_origen = seleccionar_vehiculo_aleatorio(ind);
    if (vehiculo_origen == NULL)
        return false;

    // Seleccionamos aleatoriamente la posicion del cliente del vehiculo
    cliente_posicion = (rand() % vehiculo_origen->vehiculo->clientes_contados);

    // Asignamos nodo ruta el siguiente de la cabeza porque la cabeza ruta seria el 0
    nodo_ruta_cliente = vehiculo_origen->vehiculo->ruta->cabeza->siguiente;

    for (int i = 0; i < cliente_posicion; i++)
        nodo_ruta_cliente = nodo_ruta_cliente->siguiente;

    if (nodo_ruta_cliente == NULL)
        return false;

    cliente = nodo_ruta_cliente->cliente;

    // Guardamos la posición original del cliente para poder restaurarla si es necesario
    int posicion_original = cliente_posicion;
    
    // Eliminamos el cliente de la ruta de origen
    eliminar_cliente_ruta(vehiculo_origen->vehiculo, vrp, cliente, instancia_distancias);

    vehiculo_destino = ind->metal->solucion_vecina->cabeza;
    bool insercion_exitosa = false;

    while (vehiculo_destino != NULL && !insercion_exitosa)
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
            if (bandera && verificarRestricciones(vehiculo_destino->vehiculo, vrp, instancia_distancias))
            {
                insercion_exitosa = true;
                break; // Salimos del bucle si la inserción es exitosa
            }

            // Revertimos si la inserción no es factible
            if (bandera)
                eliminar_cliente_ruta(vehiculo_destino->vehiculo, vrp, cliente, instancia_distancias);
        }

        if (insercion_exitosa)
            break;

        vehiculo_destino = vehiculo_destino->siguiente;
    }

    // Si no se pudo insertar en ningún otro vehículo, lo devolvemos al vehículo original
    if (!insercion_exitosa)
    {
        // Reinsertamos el cliente en su posición original
        insertarClienteEnPosicion(vehiculo_origen->vehiculo, vrp, cliente, posicion_original, instancia_distancias);
        return false;
    }

    return true;
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

        if (vehiculo_actual == NULL || vehiculo_actual->vehiculo->clientes_contados < 2)
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

// Invierte el orden de un segmento de la ruta
bool invertirSegmentoRuta(struct individuo *ind, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    nodo_vehiculo *vehiculo_actual = NULL;
    nodo_ruta *inicio = NULL, *fin = NULL;
    int idx1, idx2, total_clientes;
    int i;
    int intentos_maximos = 3;  // Limitamos el número de intentos para evitar recursión infinita

    // Seleccionamos un vehículo aleatorio que tenga al menos 3 clientes
    for (int intento = 0; intento < 10; intento++)
    {
        vehiculo_actual = seleccionar_vehiculo_aleatorio(ind);

        if (vehiculo_actual && vehiculo_actual->vehiculo->clientes_contados >= 3)
            break;
    }

    if (vehiculo_actual == NULL || vehiculo_actual->vehiculo->clientes_contados < 3)
        return false;

    // Iteramos hasta encontrar una inversión válida o agotar intentos
    for (int intento = 0; intento < intentos_maximos; intento++)
    {
        total_clientes = vehiculo_actual->vehiculo->clientes_contados;

        // Elegimos dos posiciones distintas idx1 < idx2 dentro de la ruta del vehículo
        idx1 = rand() % total_clientes;
        idx2 = rand() % total_clientes;

        while (idx1 == idx2)
            idx2 = rand() % total_clientes;

        if (idx1 > idx2)
        {
            int tmp = idx1;
            idx1 = idx2;
            idx2 = tmp;
        }

        // Guardamos el orden original de los clientes para poder revertir si es necesario
        int *clientes_originales = asignar_memoria_arreglo_int(idx2 - idx1 + 1);
            
        nodo_ruta *nodo_temp = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
        for (i = 0; i < idx1; i++)
            nodo_temp = nodo_temp->siguiente;
            
        for (i = 0; i <= idx2 - idx1; i++) {
            clientes_originales[i] = nodo_temp->cliente;
            nodo_temp = nodo_temp->siguiente;
        }

        // Obtenemos punteros a los nodos en las posiciones idx1 e idx2
        inicio = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
        for (i = 0; i < idx1; i++)
            inicio = inicio->siguiente;

        fin = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
        for (i = 0; i < idx2; i++)
            fin = fin->siguiente;

        if (inicio == NULL || fin == NULL) {
            free(clientes_originales);
            return false;
        }

        // Invertimos el segmento entre idx1 e idx2
        int left = idx1;
        int right = idx2;
        
        while (left < right)
        {
            nodo_ruta *nodo_left = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
            for (i = 0; i < left; i++)
                nodo_left = nodo_left->siguiente;
                
            nodo_ruta *nodo_right = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
            for (i = 0; i < right; i++)
                nodo_right = nodo_right->siguiente;
                
            int temp = nodo_left->cliente;
            nodo_left->cliente = nodo_right->cliente;
            nodo_right->cliente = temp;
            
            left++;
            right--;
        }

        // Verificamos que las restricciones sigan válidas
        if (verificarRestricciones(vehiculo_actual->vehiculo, vrp, instancia_distancias)) {
            free(clientes_originales);
            return true;  // Inversión exitosa y factible
        }
        
        // Si no es válido, revertimos a la configuración original
        nodo_temp = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
        for (i = 0; i < idx1; i++)
            nodo_temp = nodo_temp->siguiente;
            
        for (i = 0; i <= idx2 - idx1; i++) {
            nodo_temp->cliente = clientes_originales[i];
            nodo_temp = nodo_temp->siguiente;
        }
        
        free(clientes_originales);
        // Continuamos al siguiente intento
    }

    return false;  // No se encontró una inversión válida después de varios intentos
}

// Copia la solución actual como base para generar un vecino
void generar_vecino(struct individuo *ind, struct vrp_configuracion *vrp)
{
    if (ind->metal->solucion_vecina)
        liberar_lista_vehiculos(ind->metal->solucion_vecina);

    ind->metal->solucion_vecina = copiar_lista_vehiculos(ind->metal->solucion_actual);
}

// Algoritmo de Recocido Simulado (SA)
void sa(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_distancias)
{
    double temperatura = ind->temperatura_inicial, delta, prob, factor;
    bool aceptado = false;

    // Inicialización de soluciones
    ind->metal->mejor_solucion = copiar_lista_vehiculos(ind->metal->solucion_inicial);
    ind->metal->fitness_mejor_solucion = ind->metal->fitness_solucion_inicial;
    ind->metal->solucion_actual = copiar_lista_vehiculos(ind->metal->solucion_inicial);
    ind->metal->fitness_solucion_actual = ind->metal->fitness_solucion_inicial;

    // Ciclo de enfriamiento
    while (temperatura > ind->temperatura_final)
    {
        // >>> Imprime la temperatura y los valores de fitness en cada paso del enfriamiento
        // printf("Temp: %.4f | Fitness actual: %.4f | Mejor: %.4f\n",
        //        temperatura,
        //        ind->metal->fitness_solucion_actual,
        //        ind->metal->fitness_mejor_solucion);
        // <<<

        for (int i = 0; i < ind->numIteracionesSA; i++)
        {
            generar_vecino(ind, vrp);

            prob = (double)rand() / RAND_MAX;
            factor = 0.1 + ind->factor_control * (1.0 - (temperatura / ind->temperatura_inicial));
            aceptado = false;

            if (ind->hormiga->vehiculos_necesarios > 1)
            {
                // Distribuye el factor entre los tres movimientos, puedes ajustar los pesos si lo deseas
                if (prob < factor / 3.0)
                    aceptado = intercambiarClienteRuta(ind, vrp, instancia_distancias);
                else if (prob < 2.0 * factor / 3.0)
                    aceptado = moverClienteVehiculo(ind, vrp, instancia_distancias);
                else
                    aceptado = invertirSegmentoRuta(ind, vrp, instancia_distancias);
            }
            else
            {
                // Si solo hay un vehículo, mover cliente no tiene sentido
                if (prob < factor)
                    aceptado = intercambiarClienteRuta(ind, vrp, instancia_distancias);
                else
                    aceptado = invertirSegmentoRuta(ind, vrp, instancia_distancias);
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
    calculamosVentanasCapacidad(ind->metal->mejor_solucion, vrp, instancia_distancias);

    // Si se mejora la solución global, se guarda
    if (ind->metal->fitness_mejor_solucion < ind->hormiga->fitness_global)
    {
        printf("\nMejoro de %.2lf a %.2lf",ind->hormiga->fitness_global , ind->metal->fitness_mejor_solucion);
        ind->fitness = ind->metal->fitness_mejor_solucion;
        ind->hormiga->fitness_global = ind->metal->fitness_mejor_solucion;

        if (ind->hormiga->flota)
            liberar_lista_vehiculos(ind->hormiga->flota);

        ind->hormiga->flota = copiar_lista_vehiculos(ind->metal->mejor_solucion);
    }
   

    liberar_memoria_metal(ind); // Limpieza final
}
