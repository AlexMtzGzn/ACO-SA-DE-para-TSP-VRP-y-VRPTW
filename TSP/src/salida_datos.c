#include <stdio.h>
#include <stdlib.h>

#include "../include/estructuras.h"

// Función para imprimir un mensaje de error
void imprimir_mensaje(char *texto_instancia)
{
    fprintf(stderr, "%s\n", texto_instancia);
}

// Función para imprimir una matriz de instancia tsp
void imprimir_instancia(double **matriz_instancia, struct tsp_configuracion *tsp, char *texto_instancia)
{
    printf("\n\n%s\n\n", texto_instancia);
    for (int i = 0; i < tsp->num_clientes; i++)
    {                                                  // Recorre las filas
        for (int j = 0; j < tsp->num_clientes; j++)    // Recorre las columnas
            printf(" %.2lf ", matriz_instancia[i][j]); // Imprime cada valor con 2 decimales
        printf("\n");                                  // Salto de línea después de cada fila
    }
}

// Función para imprimir una ruta de un vehículo (lista enlazada de clientes)
void imprimir_ruta(struct lista_ruta *ruta)
{

    printf("  Ruta: ");
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
    printf("\n  Alpha: %.2lf, Beta: %.2lf,Rho: %.2lf, Hormigas: %d, Iteraciones ACO: %d\n  Temperatura Inicial: %.2lf, Temperatura Final: %.2lf, Factor Enfriamiento: %.2lf, Factor Control %.2lf, Iteraciones SA: %d\n",
           ind->alpha, ind->beta, ind->rho, ind->numHormigas, ind->numIteracionesACO, ind->temperatura_inicial, ind->temperatura_final, ind->factor_enfriamiento, ind->factor_control, ind->numIteracionesSA);
}

// Función principal para imprimir toda la información de las hormigas
void imprimir_hormigas(struct hormiga *hormigas, struct tsp_configuracion *tsp, struct individuo *ind)
{
    printf("\n=================================================\n");
    printf("INFORMACIÓN DE HORMIGAS Y SUS RUTAS\n");
    printf("=================================================\n");

    // Imprime los parámetros del individuo
    imprimir_individuo(ind);

    for (int i = 0; i < ind->numHormigas; i++)
    { // Recorre todas las hormigas
        printf("\nHORMIGA (ID: %d)\n", hormigas[i].id_hormiga);
        printf("  Fitness global: %.2f\n", hormigas[i].fitness_global);
        // Imprime la lista tabú de la hormiga
        imprimir_tabu(hormigas[i].tabu, tsp->num_clientes);
        struct lista_ruta *ruta = hormigas[i].ruta;
        // Imprime la flota de vehículos de la hormiga
        imprimir_ruta(ruta);
        printf("-------------------------------------------------\n");
    }
    printf("=================================================\n");
}

// Función para imprimir la información de la mejor hormiga encontrada
void imprimir_mejor_hormiga(struct hormiga *hormiga, struct individuo *ind)
{
    printf("\n=================================================\n");
    printf("INFORMACIÓN DE MEJOR HORMIGA Y RUTA\n");
    printf("=================================================\n");

    // Imprime los parámetros del individuo asociado a la hormiga
    imprimir_individuo(ind);
    printf("  Fitness global: %.2f\n", hormiga->fitness_global);
    imprimir_ruta(hormiga->ruta);
    printf("=================================================\n");
}
