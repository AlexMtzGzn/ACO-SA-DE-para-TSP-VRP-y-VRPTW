#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "../include/vrp_sa.h"
#include "../include/estructuras.h"
#include "../include/lista_flota.h"
#include "../include/lista_ruta.h"
#include "../include/control_memoria.h"

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
bool moverClienteVehiculo(struct individuo *ind, struct vrp_configuracion *vrp)
{
    nodo_vehiculo *vehiculo_origen = NULL;
    nodo_vehiculo *vehiculo_destino = NULL;
    int intentos = 10, id_vehiculo_origen, id_vehiculo_destino, pos_cliente, id_cliente;
    double demanda;
    nodo_ruta *actual = NULL, *prev = NULL, *penultimo = NULL;

    while (intentos--)
    {
        // Selecciona dos vehículos (pueden ser iguales)
        id_vehiculo_origen = (rand() % ind->hormiga->vehiculos_necesarios) + 1;
        id_vehiculo_destino = (rand() % ind->hormiga->vehiculos_necesarios) + 1;

        // Buscar vehículos por ID
        vehiculo_origen = ind->metal->solucion_vecina->cabeza;
        while (vehiculo_origen && vehiculo_origen->vehiculo->id_vehiculo != id_vehiculo_origen)
            vehiculo_origen = vehiculo_origen->siguiente;

        vehiculo_destino = ind->metal->solucion_vecina->cabeza;
        while (vehiculo_destino && vehiculo_destino->vehiculo->id_vehiculo != id_vehiculo_destino)
            vehiculo_destino = vehiculo_destino->siguiente;

        if (!vehiculo_origen || !vehiculo_destino)
            continue;

        if (vehiculo_origen->vehiculo->clientes_contados <= 2) // solo depósitos
            continue;

        // Selecciona posición válida (evitando depósitos)
        pos_cliente = rand() % (vehiculo_origen->vehiculo->clientes_contados - 2) + 1;

        prev = vehiculo_origen->vehiculo->ruta->cabeza;
        actual = prev->siguiente;

        for (int i = 1; i < pos_cliente && actual && actual->siguiente; i++)
        {
            prev = actual;
            actual = actual->siguiente;
        }

        if (!actual || actual->cliente == 0)
            continue;

        id_cliente = actual->cliente;

        if (id_cliente < 0 || id_cliente >= vrp->num_clientes)
            continue;

        demanda = vrp->clientes[id_cliente].demanda_capacidad;

        // Verificar capacidad del destino
        if ((vehiculo_destino->vehiculo->capacidad_acumulada + demanda) > vehiculo_destino->vehiculo->capacidad_maxima)
            continue;

        // Eliminar nodo del origen
        prev->siguiente = actual->siguiente;
        if (actual == vehiculo_origen->vehiculo->ruta->cola)
            vehiculo_origen->vehiculo->ruta->cola = prev;

        vehiculo_origen->vehiculo->capacidad_acumulada -= demanda;
        vehiculo_origen->vehiculo->clientes_contados--;

        // Insertar antes del depósito final del destino
        penultimo = vehiculo_destino->vehiculo->ruta->cabeza;
        while (penultimo->siguiente != vehiculo_destino->vehiculo->ruta->cola)
            penultimo = penultimo->siguiente;

        actual->siguiente = vehiculo_destino->vehiculo->ruta->cola;
        penultimo->siguiente = actual;

        vehiculo_destino->vehiculo->capacidad_acumulada += demanda;
        vehiculo_destino->vehiculo->clientes_contados++;

        return true;
    }

    return false;
}

bool invertirSegmentoRuta(struct individuo *ind)
{

    nodo_vehiculo *vehiculo_actual = NULL;
    nodo_ruta *inicio = NULL, *fin = NULL;
    int idx1, idx2, total_clientes;
    int i;
    int intentos_maximos = 10; // Limitamos el número de intentos para evitar recursión infinita

    // Seleccionamos un vehículo aleatorio que tenga al menos 3 clientes
    for (int intento = 0; intento < intentos_maximos; intento++)
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

        for (i = 0; i <= idx2 - idx1; i++)
        {
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

        if (inicio == NULL || fin == NULL)
        {
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
        if (vehiculo_actual->vehiculo->capacidad_acumulada <= vehiculo_actual->vehiculo->capacidad_maxima)
        // Recalculamos la función objetivo para la solución
        {
            free(clientes_originales);
            return true; // Inversión exitosa y factible
        }

        // Si no es válido, revertimos a la configuración original
        nodo_temp = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
        for (i = 0; i < idx1; i++)
            nodo_temp = nodo_temp->siguiente;

        for (i = 0; i <= idx2 - idx1; i++)
        {
            nodo_temp->cliente = clientes_originales[i];
            nodo_temp = nodo_temp->siguiente;
        }

        free(clientes_originales);
        // Continuamos al siguiente intento
    }

    return false; // No se encontró una inversión válida después de varios intentos
}

bool moverDosClientesVehiculos(struct individuo *ind, struct vrp_configuracion *vrp, double **instancia_distancias) {
    nodo_vehiculo *veh1 = NULL, *veh2 = NULL;
    nodo_ruta *nodo1 = NULL, *nodo2 = NULL;
    int pos1, pos2;
    int cliente1, cliente2;
    double demanda1, demanda2;
    int intentos_maximos = 10; // Limitamos el número de intentos para evitar recursión infinita

    
    // Intentar 10 veces realizar el movimiento
    for (int intento = 0; intento < intentos_maximos; intento++) {
        // Seleccionar dos vehículos con al menos 3 clientes (incluyendo depósitos)
        veh1 = seleccionar_vehiculo_aleatorio(ind);
        veh2 = seleccionar_vehiculo_aleatorio(ind);

        // Validar que sean diferentes vehículos y que ambos tengan al menos 3 clientes
        if (!veh1 || !veh2 || veh1 == veh2 || veh1->vehiculo->clientes_contados < 3 || veh2->vehiculo->clientes_contados < 3) {
            continue;
        }

        // Escoger posiciones internas válidas (sin incluir depósitos)
        bool posiciones_validas = false;
        for (int j = 0; j < 10; j++) {
            pos1 = rand() % (veh1->vehiculo->clientes_contados - 2) + 1; // Asegurarse de no seleccionar el depósito
            pos2 = rand() % (veh2->vehiculo->clientes_contados - 2) + 1;

            if (pos1 != pos2 || veh1 != veh2) {
                posiciones_validas = true;
                break;
            }
        }

        if (!posiciones_validas) {
            continue;
        }

        // Obtener nodos de los vehículos en las posiciones seleccionadas
        nodo1 = veh1->vehiculo->ruta->cabeza;
        if (!nodo1 || !nodo1->siguiente) {
            continue;
        }
        nodo1 = nodo1->siguiente; // Saltar depósito

        for (int i = 1; i < pos1 && nodo1; i++) {
            nodo1 = nodo1->siguiente;
        }

        nodo2 = veh2->vehiculo->ruta->cabeza;
        if (!nodo2 || !nodo2->siguiente) {
            continue;
        }
        nodo2 = nodo2->siguiente; // Saltar depósito

        for (int i = 1; i < pos2 && nodo2; i++) {
            nodo2 = nodo2->siguiente;
        }

        // Verificar que los nodos no son depósitos
        if (!nodo1 || !nodo2 || nodo1->cliente == 0 || nodo2->cliente == 0) {
            continue;
        }

        cliente1 = nodo1->cliente;
        cliente2 = nodo2->cliente;

        // Verificar que los clientes estén dentro de los límites
        if (cliente1 < 0 || cliente1 >= vrp->num_clientes || cliente2 < 0 || cliente2 >= vrp->num_clientes) {
            continue;
        }

        demanda1 = vrp->clientes[cliente1].demanda_capacidad;
        demanda2 = vrp->clientes[cliente2].demanda_capacidad;

        // Verificar las restricciones de capacidad después del intercambio
        double nueva_cap_veh1 = veh1->vehiculo->capacidad_acumulada - demanda1 + demanda2;
        double nueva_cap_veh2 = veh2->vehiculo->capacidad_acumulada - demanda2 + demanda1;

        if (nueva_cap_veh1 > veh1->vehiculo->capacidad_maxima || nueva_cap_veh2 > veh2->vehiculo->capacidad_maxima ||
            nueva_cap_veh1 < 0 || nueva_cap_veh2 < 0) {
            continue;
        }

        // Realizar el intercambio de clientes entre los vehículos
        nodo1->cliente = cliente2;
        nodo2->cliente = cliente1;

        // Actualizar las capacidades acumuladas de los vehículos
        veh1->vehiculo->capacidad_acumulada = nueva_cap_veh1;
        veh2->vehiculo->capacidad_acumulada = nueva_cap_veh2;

        // Si llegamos aquí, el intercambio fue exitoso
        return true;
    }
    
    return false; // No se logró un intercambio válido en los intentos dados
}

// Intercambia dos clientes aleatorios dentro de un mismo vehículo
bool intercambiarClientes(struct individuo *ind, struct vrp_configuracion *vrp)
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

    if (vehiculo_actual->vehiculo->clientes_contados < 2)
        return false;

    // Selecciona dos índices distintos para intercambiar
    do
    {
        cliente1_idx = rand() % vehiculo_actual->vehiculo->clientes_contados;
        cliente2_idx = rand() % vehiculo_actual->vehiculo->clientes_contados;
    } while (cliente1_idx == cliente2_idx);

    nodo1 = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;
    nodo2 = vehiculo_actual->vehiculo->ruta->cabeza->siguiente;

    if (nodo1 == NULL || nodo2 == NULL)
        return false;

    for (int i = 0; i < cliente1_idx; i++)
        nodo1 = nodo1->siguiente;
    for (int i = 0; i < cliente2_idx; i++)
        nodo2 = nodo2->siguiente;

    // Intercambia los clientes
    temp = nodo1->cliente;
    nodo1->cliente = nodo2->cliente;
    nodo2->cliente = temp;

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
        for (int i = 0; i < ind->numIteracionesSA; i++)
        {
            generar_vecino(ind, vrp);

            prob = (double)rand() / RAND_MAX;
            factor = ind->factor_control * (1.0 - (temperatura / ind->temperatura_inicial));
            aceptado = false;

            // Se decide qué movimiento hacer según una probabilidad controlada por temperatura
            if (ind->hormiga->vehiculos_necesarios > 1)
            {

                if (prob < factor / 3.0)
                    aceptado = moverClienteVehiculo(ind, vrp);
                else if (prob < 2.0 * factor / 3.0)
                    aceptado = invertirSegmentoRuta(ind);
                else
                    aceptado = moverDosClientesVehiculos(ind,vrp, instancia_distancias);
            }
            else
            {
                aceptado = false;
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
    ind->metal = asignar_memoria_metal(); // Asignación de memoria para la estructura metal
    ind->metal->solucion_vecina = NULL; // Inicializa la solución vecina
    ind->metal->solucion_actual = NULL; // Inicializa la solución actual
    ind->metal->mejor_solucion = NULL; // Inicializa la mejor solución
    ind->metal->fitness_solucion_actual = 0.0; // Inicializa el fitness de la solución actual
    ind->metal->fitness_solucion_vecina = 0.0; // Inicializa el fitness de la solución vecina
    ind->metal->fitness_mejor_solucion = 0.0; // Inicializa el fitness de la mejor solución
    ind->metal->solucion_inicial = copiar_lista_vehiculos(ind->hormiga->flota); // Copia la solución inicial
    ind->metal->fitness_solucion_inicial = ind->hormiga->fitness_global; // Guarda el fitness de la solución inicial
}

// Función principal que ejecuta SA sobre un individuo del VRP
void vrp_sa(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_distancias)
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
