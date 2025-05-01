#include <stdio.h>
#include <stdlib.h>

#include "../include/estructuras.h"

// Función para imprimir un mensaje de error
void imprimir_mensaje(char *texto_instancia)
{
    fprintf(stderr, "Error: %s\n", texto_instancia);
}

// Función para imprimir una matriz de instancia VRP
void imprimir_instancia(double **matriz_instancia, struct vrp_configuracion *vrp, char *texto_instancia)
{
    printf("\n\n%s\n\n", texto_instancia);
    for (int i = 0; i < vrp->num_clientes; i++)
    {                                                  // Recorre las filas
        for (int j = 0; j < vrp->num_clientes; j++)    // Recorre las columnas
            printf(" %.2lf ", matriz_instancia[i][j]); // Imprime cada valor con 2 decimales
        printf("\n");                                  // Salto de línea después de cada fila
    }
}

// Función para imprimir una ruta de un vehículo (lista enlazada de clientes)
void imprimir_ruta(struct lista_ruta *ruta, int vehiculo_id)
{
    printf("    - Ruta del vehículo: ");

    struct nodo_ruta *actual = ruta->cabeza; // Apunta al primer nodo de la lista

    printf("Depósito -> "); // Se asume que la ruta comienza en el depósito
    while (actual != NULL)
    { // Recorre la lista de clientes en la ruta
        if (actual->cliente != 0)
            printf("Cliente %d", actual->cliente);

        if (actual->siguiente != NULL && actual->cliente != 0)
            printf(" -> "); // Imprime un separador entre clientes

        actual = actual->siguiente; // Avanza al siguiente nodo
    }
    printf("Depósito\n"); // Se asume que la ruta regresa al depósito
}

// Función para imprimir la información de un vehículo
void imprimir_vehiculo(struct vehiculo *vehiculo)
{
    printf("  + Vehículo ID: %d\n", vehiculo->id_vehiculo);
    printf("    - Capacidad máxima: %.2f\n", vehiculo->capacidad_maxima);
    printf("    - Capacidad acumulada: %.2f\n", vehiculo->capacidad_acumulada);
    printf("    - Tiempo Salida: %.2f\n", vehiculo->tiempo_salida_vehiculo);
    printf("    - Tiempo Llegada: %.2f\n", vehiculo->tiempo_llegada_vehiculo);
    printf("    - Ventana Inicial: %.2f\n", vehiculo->vt_inicial);
    printf("    - Ventana Final: %.2f\n", vehiculo->vt_final);
    printf("    - Número de clientes: %d\n", vehiculo->clientes_contados);
    printf("    - Fitness del vehículo: %.2f\n", vehiculo->fitness_vehiculo);

    // Imprime la ruta del vehículo
    imprimir_ruta(vehiculo->ruta, vehiculo->id_vehiculo);
}

// Función para imprimir la flota de vehículos de una hormiga
void imprimir_flota(struct lista_vehiculos *flota)
{
    printf("  Flota:\n");

    struct nodo_vehiculo *actual = flota->cabeza; // Apunta al primer vehículo de la lista
    int contador = 1;

    while (actual != NULL)
    {                                        // Recorre todos los vehículos
        imprimir_vehiculo(actual->vehiculo); // Imprime los detalles del vehículo
        actual = actual->siguiente;          // Avanza al siguiente nodo
        contador++;
    }
}

// Función para imprimir la información de la lista tabú
void imprimir_tabu(int *tabu, int num_clientes)
{
    printf("  Tabu: [");
    for (int i = 0; i < num_clientes; i++)
    {
        printf("%d", tabu[i]);
        if (i < num_clientes - 1)
            printf(","); // Imprime una coma entre valores
    }
    printf("]\n");
}

// Función para imprimir los parámetros de un individuo
void imprimir_individuo(individuo *ind)
{
    printf("Individuo: ");
    printf("\n  Alpha: %.2lf, Beta: %.2lf, Gamma: %.2lf, Rho: %.2lf, Hormigas: %d, Iteraciones ACO: %d\n  Temperatura Inicial: %.2lf, Temperatura Final: %.2lf, Factor Enfriamiento: %.2lf, Factor Control: %.2lf, Iteraciones SA: %d\n",
           ind->alpha, ind->beta, ind->gamma, ind->rho, ind->numHormigas, ind->numIteracionesACO, ind->temperatura_inicial, ind->temperatura_final, ind->factor_enfriamiento, ind->factor_control, ind->numIteracionesSA);
}

// Función principal para imprimir toda la información de las hormigas
void imprimir_hormigas(struct hormiga *hormigas, struct vrp_configuracion *vrp, struct individuo *ind)
{
    printf("=================================================\n");
    printf("INFORMACIÓN DE HORMIGAS Y SUS RUTAS\n");
    printf("=================================================\n");

    // Imprime los parámetros del individuo
    imprimir_individuo(ind);

    for (int i = 0; i < ind->numHormigas; i++)
    { // Recorre todas las hormigas
        printf("\nHORMIGA (ID: %d)\n", hormigas[i].id_hormiga);
        printf("  Vehículos contados: %d/%d\n", hormigas[i].vehiculos_necesarios, hormigas[i].vehiculos_maximos);
        printf("  Fitness global: %.2f\n", hormigas[i].fitness_global);

        // Imprime la lista tabú de la hormiga
        imprimir_tabu(hormigas[i].tabu, vrp->num_clientes);

        // Imprime la flota de vehículos de la hormiga
        imprimir_flota(hormigas[i].flota);

        printf("-------------------------------------------------\n");
    }
    printf("=================================================\n");
}

// Función para imprimir la información de la mejor hormiga encontrada
void imprimir_mejor_hormiga(struct hormiga *hormiga, struct individuo *ind)
{
    printf("=================================================\n");
    printf("INFORMACIÓN DE MEJOR HORMIGA Y RUTA\n");
    printf("=================================================\n");

    // Imprime los parámetros del individuo asociado a la hormiga
    imprimir_individuo(ind);

    printf("  Vehículos contados: %d/%d\n", hormiga->vehiculos_necesarios, hormiga->vehiculos_maximos);
    printf("  Fitness global: %.2f\n", hormiga->fitness_global);

    // Imprime la flota de vehículos de la mejor hormiga
    imprimir_flota(hormiga->flota);

    printf("=================================================\n");
}
