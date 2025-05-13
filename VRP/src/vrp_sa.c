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
// Función para mover un cliente de un vehículo a otro dentro de una solución de VRP
bool mover_cliente_vehiculo(struct individuo *ind, struct vrp_configuracion *vrp)
{
    // Variables para los nodos de vehículos origen y destino
    nodo_vehiculo *origen = NULL;
    nodo_vehiculo *destino = NULL;
    
    // Intentos de mover el cliente entre vehículos (máximo 10 intentos)
    int intentos = 10;

    // Realiza intentos para mover el cliente
    while (intentos--)
    {
        // Selecciona aleatoriamente un vehículo de origen y uno de destino
        int id_origen = (rand() % ind->hormiga->vehiculos_necesarios) + 1;
        int id_destino = (rand() % ind->hormiga->vehiculos_necesarios) + 1;

        // Busca el vehículo origen por ID en la lista de vehículos
        origen = ind->metal->solucion_vecina->cabeza;
        while (origen && origen->vehiculo->id_vehiculo != id_origen)
            origen = origen->siguiente;

        // Busca el vehículo destino por ID en la lista de vehículos
        destino = ind->metal->solucion_vecina->cabeza;
        while (destino && destino->vehiculo->id_vehiculo != id_destino)
            destino = destino->siguiente;

        // Si no se encuentran los vehículos o si el vehículo origen tiene menos de 3 clientes, se intenta de nuevo
        if (!origen || !destino || origen->vehiculo->clientes_contados <= 2)
            continue;

        // Selecciona aleatoriamente un cliente del vehículo origen (excepto el primer y último cliente)
        int pos_cliente = rand() % (origen->vehiculo->clientes_contados - 2) + 1;

        // Buscar el cliente en la ruta del vehículo origen
        nodo_ruta *anterior = origen->vehiculo->ruta->cabeza;
        nodo_ruta *actual = anterior->siguiente;
        for (int i = 1; i < pos_cliente && actual && actual->siguiente; i++)
        {
            anterior = actual;
            actual = actual->siguiente;
        }

        // Verificar que el cliente es válido (no negativo o fuera de los límites)
        if (!actual || actual->cliente <= 0 || actual->cliente >= vrp->num_clientes)
            continue;

        // Obtener la demanda del cliente
        double demanda = vrp->clientes[actual->cliente].demanda_capacidad;

        // Comprobar si el vehículo destino tiene suficiente capacidad para añadir el cliente
        if (destino->vehiculo->capacidad_acumulada + demanda > destino->vehiculo->capacidad_maxima)
            continue;

        // Eliminar cliente del vehículo origen
        anterior->siguiente = actual->siguiente;
        if (actual == origen->vehiculo->ruta->cola)
            origen->vehiculo->ruta->cola = anterior;
        origen->vehiculo->capacidad_acumulada -= demanda;
        origen->vehiculo->clientes_contados--;

        // Insertar cliente en el vehículo destino antes del depósito final
        nodo_ruta *penultimo = destino->vehiculo->ruta->cabeza;
        while (penultimo->siguiente != destino->vehiculo->ruta->cola)
            penultimo = penultimo->siguiente;

        // Insertar el cliente en la ruta del destino
        actual->siguiente = destino->vehiculo->ruta->cola;
        penultimo->siguiente = actual;
        destino->vehiculo->capacidad_acumulada += demanda;
        destino->vehiculo->clientes_contados++;

        // Si todo ha ido bien, se retorna verdadero indicando que el movimiento fue exitoso
        return true;
    }

    // Si después de 10 intentos no se pudo mover el cliente, retorna falso
    return false;
}


// Función que invierte un segmento de la ruta de un vehículo de manera aleatoria
bool invertir_segmento_ruta(struct individuo *ind)
{
    // Nodo que representa el vehículo seleccionado
    nodo_vehiculo *vehiculo = NULL;
    
    // Número de intentos para encontrar un vehículo con al menos 3 clientes
    int intentos = 10;

    // Intento de seleccionar un vehículo que tenga al menos 3 clientes
    for (int i = 0; i < intentos; i++)
    {
        // Selección aleatoria de un vehículo
        vehiculo = seleccionar_vehiculo_aleatorio(ind);
        
        // Verifica si el vehículo tiene al menos 3 clientes
        if (vehiculo && vehiculo->vehiculo->clientes_contados >= 3)
            break;
    }

    // Si no se encontró un vehículo válido o el vehículo tiene menos de 3 clientes, retorna falso
    if (!vehiculo || vehiculo->vehiculo->clientes_contados < 3)
        return false;

    // Realiza hasta 10 intentos para invertir un segmento de la ruta del vehículo
    for (int i = 0; i < intentos; i++)
    {
        // Obtiene el número total de clientes del vehículo
        int total = vehiculo->vehiculo->clientes_contados;
        
        // Selecciona aleatoriamente un punto de inicio y fin para el segmento a invertir
        int inicio = rand() % total;
        int fin = rand() % total;

        // Asegura que el punto de inicio y el de fin no sean iguales
        while (inicio == fin)
            fin = rand() % total;

        // Si el punto de inicio es mayor que el de fin, intercambia los valores
        if (inicio > fin)
        {
            int tmp = inicio;
            inicio = fin;
            fin = tmp;
        }

        // Calcula la longitud del segmento a invertir
        int longitud = fin - inicio + 1;

        // Asigna memoria para almacenar los valores del segmento
        int *segmento = asignar_memoria_arreglo_int(longitud);
        if (!segmento)
            return false; // Si no se pudo asignar memoria, retorna falso

        // Se obtiene el nodo inicial de la ruta del vehículo
        nodo_ruta *nodo = vehiculo->vehiculo->ruta->cabeza->siguiente;
        
        // Avanza hasta el nodo de inicio del segmento a invertir
        for (int j = 0; j < inicio; j++)
            nodo = nodo->siguiente;

        // Guarda los clientes del segmento seleccionado en el arreglo 'segmento'
        nodo_ruta *iter = nodo;
        for (int j = 0; j < longitud; j++)
        {
            segmento[j] = iter->cliente;
            iter = iter->siguiente;
        }

        // Invierte el arreglo 'segmento'
        for (int j = 0; j < longitud / 2; j++)
        {
            int temp = segmento[j];
            segmento[j] = segmento[longitud - 1 - j];
            segmento[longitud - 1 - j] = temp;
        }

        // Vuelve al nodo de inicio y reemplaza los clientes en la ruta por el segmento invertido
        nodo = vehiculo->vehiculo->ruta->cabeza->siguiente;
        for (int j = 0; j < inicio; j++)
            nodo = nodo->siguiente;

        // Reemplaza los clientes del segmento en la ruta con los valores invertidos
        for (int j = 0; j < longitud; j++)
        {
            nodo->cliente = segmento[j];
            nodo = nodo->siguiente;
        }

        // Libera la memoria asignada para el segmento
        free(segmento);

        // Retorna verdadero indicando que se realizó la inversión exitosamente
        return true;
    }

    // Si después de 10 intentos no se pudo invertir el segmento, retorna falso
    return false;
}


// Función que intercambia dos clientes entre dos vehículos de forma aleatoria
bool mover_dos_clientes_vehiculos(struct individuo *ind, struct vrp_configuracion *vrp, double **instancia_distancias)
{
    // Punteros a los vehículos seleccionados
    nodo_vehiculo *veh1 = NULL, *veh2 = NULL;

    // Punteros a los nodos (clientes) dentro de las rutas de los vehículos
    nodo_ruta *nodo1 = NULL, *nodo2 = NULL;

    // Número de intentos para encontrar dos vehículos válidos
    int intentos = 10;

    // Intento de encontrar dos vehículos válidos que tengan al menos 3 clientes
    while (intentos--)
    {
        // Selección aleatoria de dos vehículos diferentes
        veh1 = seleccionar_vehiculo_aleatorio(ind);
        veh2 = seleccionar_vehiculo_aleatorio(ind);

        // Verifica que los dos vehículos sean diferentes y que tengan al menos 3 clientes
        if (!veh1 || !veh2 || veh1 == veh2)
            continue;

        // Verifica que ambos vehículos tengan al menos 3 clientes
        if (veh1->vehiculo->clientes_contados < 3 || veh2->vehiculo->clientes_contados < 3)
            continue;

        // Selección aleatoria de posiciones dentro de la ruta de ambos vehículos
        int pos1 = rand() % (veh1->vehiculo->clientes_contados - 2) + 1;
        int pos2 = rand() % (veh2->vehiculo->clientes_contados - 2) + 1;

        // Recorrido de la ruta del primer vehículo hasta el cliente en la posición pos1
        nodo1 = veh1->vehiculo->ruta->cabeza->siguiente;
        for (int i = 1; i < pos1 && nodo1; i++)
            nodo1 = nodo1->siguiente;

        // Recorrido de la ruta del segundo vehículo hasta el cliente en la posición pos2
        nodo2 = veh2->vehiculo->ruta->cabeza->siguiente;
        for (int i = 1; i < pos2 && nodo2; i++)
            nodo2 = nodo2->siguiente;

        // Verifica que ambos nodos seleccionados no sean nulos y que los clientes sean válidos
        if (!nodo1 || !nodo2 || nodo1->cliente == 0 || nodo2->cliente == 0)
            continue;

        // Obtiene los IDs de los clientes seleccionados
        int id1 = nodo1->cliente;
        int id2 = nodo2->cliente;

        // Verifica que los IDs de los clientes sean válidos
        if (id1 < 0 || id2 < 0 || id1 >= vrp->num_clientes || id2 >= vrp->num_clientes)
            continue;

        // Obtiene la demanda de capacidad de los clientes seleccionados
        double d1 = vrp->clientes[id1].demanda_capacidad;
        double d2 = vrp->clientes[id2].demanda_capacidad;

        // Calcula la nueva capacidad acumulada de ambos vehículos después de intercambiar los clientes
        double nueva_cap1 = veh1->vehiculo->capacidad_acumulada - d1 + d2;
        double nueva_cap2 = veh2->vehiculo->capacidad_acumulada - d2 + d1;

        // Verifica que los vehículos no excedan su capacidad máxima después del intercambio
        if (nueva_cap1 > veh1->vehiculo->capacidad_maxima || nueva_cap2 > veh2->vehiculo->capacidad_maxima)
            continue;

        // Si el intercambio es válido, realiza el intercambio de clientes entre las rutas
        nodo1->cliente = id2;
        nodo2->cliente = id1;

        // Actualiza las capacidades acumuladas de los vehículos
        veh1->vehiculo->capacidad_acumulada = nueva_cap1;
        veh2->vehiculo->capacidad_acumulada = nueva_cap2;

        // Retorna verdadero indicando que el intercambio se realizó con éxito
        return true;
    }

    // Si después de 10 intentos no se pudo realizar el intercambio, retorna falso
    return false;
}

// Intercambia dos clientes aleatorios distintos en la solución vecina
bool intercambiar_clientes(struct individuo *ind, struct vrp_configuracion *vrp)
{
    int intentos_maximos = 10;

    while (intentos_maximos--)
    {
        int cliente1 = (rand() % (vrp->num_clientes - 2)) + 1; 
        int cliente2 = (rand() % (vrp->num_clientes - 2)) + 1;

        if (cliente1 == cliente2)
            continue;

        nodo_ruta *nodo1 = NULL;
        nodo_ruta *nodo2 = NULL;

        // Buscar los nodos que contienen a cliente1 y cliente2
        nodo_vehiculo *vehiculo_actual = ind->metal->solucion_vecina->cabeza;
        while (vehiculo_actual && (!nodo1 || !nodo2))
        {
            struct lista_ruta *ruta = vehiculo_actual->vehiculo->ruta;
            nodo_ruta *nodo_actual = ruta->cabeza;

            while (nodo_actual)
            {
                if (nodo_actual->cliente == cliente1 && !nodo1)
                    nodo1 = nodo_actual;
                else if (nodo_actual->cliente == cliente2 && !nodo2)
                    nodo2 = nodo_actual;

                if (nodo1 && nodo2)
                    break;

                nodo_actual = nodo_actual->siguiente;
            }

            vehiculo_actual = vehiculo_actual->siguiente;
        }

        if (nodo1 && nodo2)
        {
            // Encontrar vehículos a los que pertenecen
            nodo_vehiculo *vehiculo1 = NULL;
            nodo_vehiculo *vehiculo2 = NULL;

            vehiculo_actual = ind->metal->solucion_vecina->cabeza;
            while (vehiculo_actual && (!vehiculo1 || !vehiculo2))
            {
                struct lista_ruta *ruta = vehiculo_actual->vehiculo->ruta;
                nodo_ruta *nodo_actual = ruta->cabeza;

                while (nodo_actual)
                {
                    if (nodo_actual == nodo1)
                        vehiculo1 = vehiculo_actual;
                    else if (nodo_actual == nodo2)
                        vehiculo2 = vehiculo_actual;

                    if (vehiculo1 && vehiculo2)
                        break;

                    nodo_actual = nodo_actual->siguiente;
                }

                vehiculo_actual = vehiculo_actual->siguiente;
            }

            // Verificación de capacidad si están en vehículos distintos
            if (vehiculo1 && vehiculo2 && vehiculo1 != vehiculo2)
            {
                double demanda1 = vrp->clientes[nodo1->cliente].demanda_capacidad;
                double demanda2 = vrp->clientes[nodo2->cliente].demanda_capacidad;

                double nueva_cap1 = vehiculo1->vehiculo->capacidad_acumulada - demanda1 + demanda2;
                double nueva_cap2 = vehiculo2->vehiculo->capacidad_acumulada - demanda2 + demanda1;

                if (nueva_cap1 > vehiculo1->vehiculo->capacidad_maxima ||
                    nueva_cap2 > vehiculo2->vehiculo->capacidad_maxima)
                    continue;

                vehiculo1->vehiculo->capacidad_acumulada = nueva_cap1;
                vehiculo2->vehiculo->capacidad_acumulada = nueva_cap2;
            }

            // Realizar el intercambio
            int temp = nodo1->cliente;
            nodo1->cliente = nodo2->cliente;
            nodo2->cliente = temp;

            return true;
        }
    }

    return false;
}


// Copia la solución actual como base para generar un vecino
// Función que genera una vecina a partir de la solución actual
void generar_vecino(struct individuo *ind, struct vrp_configuracion *vrp)
{
    // Si ya existe una solución vecina, se libera para evitar fugas de memoria
    if (ind->metal->solucion_vecina)
        liberar_lista_vehiculos(ind->metal->solucion_vecina);

    // Copia la solución actual en la solución vecina para generar una nueva variante
    ind->metal->solucion_vecina = copiar_lista_vehiculos(ind->metal->solucion_actual);
}


// Algoritmo de Recocido Simulado (SA)
// Función que implementa el algoritmo de Recocido Simulado (Simulated Annealing) para VRP
void sa(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_distancias)
{
    // Inicialización de la temperatura y variables auxiliares
    double temperatura = ind->temperatura_inicial, delta, prob, factor;
    bool aceptado = false;

    // Copia la solución inicial como la mejor conocida y como solución actual
    ind->metal->mejor_solucion = copiar_lista_vehiculos(ind->metal->solucion_inicial);
    ind->metal->fitness_mejor_solucion = ind->metal->fitness_solucion_inicial;
    ind->metal->solucion_actual = copiar_lista_vehiculos(ind->metal->solucion_inicial);
    ind->metal->fitness_solucion_actual = ind->metal->fitness_solucion_inicial;

    // Bucle principal de enfriamiento
    while (temperatura > ind->temperatura_final)
    {
        // Para cada temperatura, se hacen varias iteraciones
        for (int i = 0; i < ind->numIteracionesSA; i++)
        {
            // Se genera una solución vecina a partir de la solución actual
            generar_vecino(ind, vrp);

            // Se calcula una probabilidad aleatoria para decidir el tipo de movimiento
            prob = (double)rand() / RAND_MAX;

            // El factor controla qué tan intensamente explorar el vecindario
            factor = ind->factor_control * (1.0 - (temperatura / ind->temperatura_inicial));
            aceptado = false;

            // Se elige qué tipo de movimiento aplicar según el número de vehículos y probabilidad
            if (ind->hormiga->vehiculos_necesarios > 1)
            {
                if (prob < factor / 3.0)
                    aceptado = mover_cliente_vehiculo(ind, vrp); // Mover un cliente de un vehículo a otro
                else if (prob < 2.0 * factor / 3.0)
                    aceptado = invertir_segmento_ruta(ind);     // Invertir una subruta
                else
                    aceptado = mover_dos_clientes_vehiculos(ind, vrp, instancia_distancias); // Mover dos clientes
            }
            else
            {
                if (prob < factor / 2.0)
                    aceptado = invertir_segmento_ruta(ind);     // Solo un vehículo: invertir segmento
                else
                    aceptado = intercambiar_clientes(ind, vrp); // Intercambiar dos clientes
            }

            // Si no se generó una solución válida, se salta a la siguiente iteración
            if (!aceptado)
                continue;

            // Evalúa el fitness de la solución vecina generada
            evaluaFO_SA(ind, vrp, instancia_distancias);

            // Se calcula la diferencia de fitness (delta)
            delta = ind->metal->fitness_solucion_vecina - ind->metal->fitness_solucion_actual;

            // Decide si se acepta la solución vecina
            bool aceptar = false;
            if (delta < 0)
                aceptar = true; // Si mejora, se acepta directamente
            else if ((double)rand() / RAND_MAX < exp(-delta / temperatura))
                aceptar = true; // Si no mejora, se acepta con cierta probabilidad

            if (aceptar)
            {
                // Libera la solución actual y copia la nueva solución aceptada
                liberar_lista_vehiculos(ind->metal->solucion_actual);
                ind->metal->solucion_actual = copiar_lista_vehiculos(ind->metal->solucion_vecina);
                ind->metal->fitness_solucion_actual = ind->metal->fitness_solucion_vecina;

                // Si es la mejor solución encontrada hasta ahora, se actualiza
                if (ind->metal->fitness_solucion_actual < ind->metal->fitness_mejor_solucion)
                {
                    liberar_lista_vehiculos(ind->metal->mejor_solucion);
                    ind->metal->mejor_solucion = copiar_lista_vehiculos(ind->metal->solucion_actual);
                    ind->metal->fitness_mejor_solucion = ind->metal->fitness_solucion_actual;
                }
            }
        }

        // Enfriamiento: se reduce la temperatura multiplicándola por el factor de enfriamiento
        temperatura *= ind->factor_enfriamiento;
    }

    // Limpieza final: se libera la última solución actual, ya no es necesaria
    liberar_lista_vehiculos(ind->metal->solucion_actual);
}


// Inicializa la estructura metal de un individuo
void inicializar_metal(struct individuo *ind)
{
    ind->metal = asignar_memoria_metal();                                       // Asignación de memoria para la estructura metal
    ind->metal->solucion_vecina = NULL;                                         // Inicializa la solución vecina
    ind->metal->solucion_actual = NULL;                                         // Inicializa la solución actual
    ind->metal->mejor_solucion = NULL;                                          // Inicializa la mejor solución
    ind->metal->fitness_solucion_actual = 0.0;                                  // Inicializa el fitness de la solución actual
    ind->metal->fitness_solucion_vecina = 0.0;                                  // Inicializa el fitness de la solución vecina
    ind->metal->fitness_mejor_solucion = 0.0;                                   // Inicializa el fitness de la mejor solución
    ind->metal->solucion_inicial = copiar_lista_vehiculos(ind->hormiga->flota); // Copia la solución inicial
    ind->metal->fitness_solucion_inicial = ind->hormiga->fitness_global;        // Guarda el fitness de la solución inicial
}

// Función principal que ejecuta SA sobre un individuo del VRP
// Función principal que ejecuta el Recocido Simulado (SA) sobre un individuo de VRP
void vrp_sa(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_distancias)
{
    // Inicializa la estructura "metal" dentro del individuo
    // Esto incluye crear una copia de la solución actual y configuraciones necesarias para SA
    inicializar_metal(ind);

    // Ejecuta el algoritmo de Recocido Simulado sobre el individuo
    sa(vrp, ind, instancia_distancias);

    // Si se encuentra una mejor solución (menor fitness), se actualiza la mejor solución global del individuo
    if (ind->metal->fitness_mejor_solucion < ind->hormiga->fitness_global)
    {
        // Actualiza el fitness del individuo
        ind->fitness = ind->metal->fitness_mejor_solucion;

        // Actualiza el mejor fitness global conocido por la hormiga
        ind->hormiga->fitness_global = ind->metal->fitness_mejor_solucion;

        // Libera la solución anterior en la flota si existe
        if (ind->hormiga->flota)
            liberar_lista_vehiculos(ind->hormiga->flota);

        // Copia la mejor solución encontrada por SA a la flota del individuo
        ind->hormiga->flota = copiar_lista_vehiculos(ind->metal->mejor_solucion);
    }

    // Libera la memoria utilizada por la estructura metal tras la ejecución de SA
    liberar_memoria_metal(ind);
}

