#include <stdio.h>
#include <stdlib.h>

#include "../includes/estructuras.h"

void imprimir_instancia(double **matriz_instancia, struct vrp_configuracion *vrp, char *texto_instancia)
{
    printf("\n\n%s\n\n", texto_instancia);
    for (int i = 0; i < vrp->num_clientes; i++)
    {
        for (int j = 0; j < vrp->num_clientes; j++)
            printf(" %.2lf ", matriz_instancia[i][j]);
        printf("\n");
    }
}
// Función para imprimir la información de una ruta (lista de clientes)
void imprimir_ruta(struct lista_ruta *ruta, int vehiculo_id)
{
    printf("    - Ruta del vehículo: ");

    struct nodo_ruta *actual = ruta->cabeza;
    printf("Depósito -> ");
    while (actual != NULL)
    {
        if (actual->cliente != 0)
            printf("Cliente %d", actual->cliente);

        if (actual->siguiente != NULL && actual->cliente != 0)
            printf(" -> ");

        actual = actual->siguiente;
    }
    printf("Depósito\n");
}

// Función para imprimir la información de un vehículo
void imprimir_vehiculo(struct vehiculo *vehiculo)
{
    printf("  + Vehículo ID: %d\n", vehiculo->id_vehiculo);
    printf("    - Capacidad máxima: %.2f\n", vehiculo->capacidad_maxima);
    printf("    - Capacidad acumulada: %.2f\n", vehiculo->capacidad_acumulada);
    printf("    - Tiempo consumido: %.2f\n", (vehiculo->vt_actual));
    printf("    - Tiempo máximo: %.2f\n", vehiculo->vt_final);
    printf("    - Número de clientes: %d\n", vehiculo->clientes_contados);
    printf("    - Fitness del vehículo: %.2f\n", vehiculo->fitness_vehiculo);
    imprimir_ruta(vehiculo->ruta, vehiculo->id_vehiculo);
}

// Función para imprimir la flota de vehículos de una hormiga
void imprimir_flota(struct lista_vehiculos *flota)
{
    printf("  Flota:\n");

    struct nodo_vehiculo *actual = flota->cabeza;
    int contador = 1;

    while (actual != NULL)
    {
        printf("  Vehículo #%d:\n", contador);
        imprimir_vehiculo(actual->vehiculo);
        actual = actual->siguiente;
        contador++;
    }
}

// Función para imprimir la información del array de tabu
void imprimir_tabu(int *tabu, int num_clientes)
{
    printf("  Tabu: [");
    for (int i = 0; i < num_clientes; i++)
    {
        printf("%d", tabu[i]);
        if (i < num_clientes - 1)
            printf(",");
    }
    printf("]\n");
}

void imprimir_individuo(individuo *ind)
{

    printf("Individuo: ");
    printf("Alpha: %.2lf, Beta: %.2lf, Gamma: %.2lf, Rho: %.2lf, Hormigas: %d, Iteraciones: %d\n\n", ind->alpha, ind->beta, ind->gamma, ind->rho, ind->numHormigas, ind->numIteraciones);
}

// Función principal para imprimir toda la información de las hormigas
void imprimir_hormigas(struct hormiga *hormigas, struct vrp_configuracion *vrp, struct individuo * ind)
{
    printf("=================================================\n");
    printf("INFORMACIÓN DE HORMIGAS Y SUS RUTAS\n");
    printf("=================================================\n");
    imprimir_individuo(ind);
    for (int i = 0; i < ind->numHormigas; i++)
    {
        printf("\nHORMIGA (ID: %d)\n", hormigas[i].id_hormiga);
        printf("  Vehículos contados: %d/%d\n", hormigas[i].vehiculos_necesarios, hormigas[i].vehiculos_maximos);
        printf("  Fitness global: %.2f\n", hormigas[i].fitness_global);
        imprimir_tabu(hormigas[i].tabu, vrp->num_clientes);
        imprimir_flota(hormigas[i].flota);
        printf("-------------------------------------------------\n");
    }
    printf("=================================================\n");
}

void imprimir_mejor_hormiga(struct hormiga *hormiga, struct individuo *ind)
{
    printf("=================================================\n");
    printf("INFORMACÍON DE MEJOR HORMIGA Y RUTA\n");
    printf("=================================================\n");
    imprimir_individuo(ind);
    printf("  Vehículos contados: %d/%d\n", hormiga->vehiculos_necesarios, hormiga->vehiculos_maximos);
    printf("  Fitness global: %.2f\n", hormiga->fitness_global);
    imprimir_flota(hormiga->flota);
    printf("=================================================\n");
}