#include <stdio.h>
#include <stdlib.h>

#include "../include/estructuras.h"

// Imprime un mensaje de error en stderr
void imprimir_mensaje(char *texto_instancia) {
    fprintf(stderr, "\n[ERROR] %s\n", texto_instancia);
}

// Imprime una matriz (instancia VRP) con un encabezado descriptivo
void imprimir_instancia(double **matriz_instancia, struct vrp_configuracion *vrp, char *texto_instancia) {
    printf("\n=================================================\n");
    printf(">>> MATRIZ: %s\n", texto_instancia);
    printf("=================================================\n");

    for (int i = 0; i < vrp->num_clientes; i++) {
        printf("  "); // Sangría para mejor alineación visual
        for (int j = 0; j < vrp->num_clientes; j++) {
            printf("%6.2lf ", matriz_instancia[i][j]);
        }
        printf("\n");
    }

    printf("=================================================\n\n");
}

// Imprime los datos detallados de cada cliente atendido por un vehículo
void imprimir_datos_cliente(struct vehiculo *vehiculo) {
    printf("\n  >> Detalles de clientes atendidos:\n");
    printf("  %-10s %-10s %-12s %-12s %-15s %-15s %-15s %-12s %-12s\n",
           "Cliente", "Demanda", "Vent. Ini", "Vent. Fin", "Llegada", "Espera", "Inicio Serv.", "Duración", "Salida");
    printf("  %-10s %-10s %-12s %-12s %-15s %-15s %-15s %-12s %-12s\n",
           "-------", "-------", "---------", "---------", "--------", "------", "-------------", "--------", "------");

    for (int i = 0; i < vehiculo->clientes_contados + 2; i++) {
        if (vehiculo->datos_cliente[i].cliente == 0)
            printf("  %-10s ", "Depósito");
        else
            printf("  %-10d ", vehiculo->datos_cliente[i].cliente);

        printf("%-10.2f %-12.2f %-12.2f %-15.2f %-15.2f %-15.2f %-12.2f %-12.2f\n",
               vehiculo->datos_cliente[i].demanda_capacidad,
               vehiculo->datos_cliente[i].ventana_inicial,
               vehiculo->datos_cliente[i].ventana_final,
               vehiculo->datos_cliente[i].tiempo_llegada,
               vehiculo->datos_cliente[i].tiempo_espera,
               vehiculo->datos_cliente[i].inicio_servicio,
               vehiculo->datos_cliente[i].duracion_servicio,
               vehiculo->datos_cliente[i].tiempo_salida);
    }
    printf("\n");
}

// Imprime la ruta del vehículo en coordenadas
void imprimir_ruta_cordenadas(struct lista_ruta *ruta, int vehiculo_id, struct vrp_configuracion *vrp) {
    printf("    - Ruta en coordenadas: Depósito -> ");

    struct nodo_ruta *actual = ruta->cabeza;
    while (actual != NULL) {
        printf("(%.2lf, %.2lf)", vrp->clientes[actual->cliente].coordenada_x,
                                  vrp->clientes[actual->cliente].coordenada_y);

        if (actual->siguiente != NULL)
            printf(" -> ");

        actual = actual->siguiente;
    }

    printf(" -> (%.2lf, %.2lf)\n", vrp->clientes[0].coordenada_x, vrp->clientes[0].coordenada_y);
}

// Imprime la ruta del vehículo en formato de clientes
void imprimir_ruta(struct lista_ruta *ruta, int vehiculo_id) {
    printf("    - Ruta (clientes): Depósito -> ");

    struct nodo_ruta *actual = ruta->cabeza;
    while (actual != NULL) {
        if (actual->cliente != 0)
            printf("Cliente %d", actual->cliente);

        if (actual->siguiente != NULL && actual->cliente != 0)
            printf(" -> ");

        actual = actual->siguiente;
    }

    printf(" -> Depósito\n");
}

// Imprime la información completa de un vehículo
void imprimir_vehiculo(struct vehiculo *vehiculo) {
    printf("  + Vehículo ID: %d\n", vehiculo->id_vehiculo);
    printf("    - Capacidad máxima     : %.2f\n", vehiculo->capacidad_maxima);
    printf("    - Capacidad acumulada  : %.2f\n", vehiculo->capacidad_acumulada);
    printf("    - Tiempo salida        : %.2lf\n", vehiculo->tiempo_salida_vehiculo);
    printf("    - Tiempo llegada       : %.2lf\n", vehiculo->tiempo_llegada_vehiculo);
    printf("    - Ventana inicial      : %.2f\n", vehiculo->vt_inicial);
    printf("    - Ventana final        : %.2f\n", vehiculo->vt_final);
    printf("    - Clientes atendidos   : %d\n", vehiculo->clientes_contados);
    printf("    - Fitness del vehículo : %.2f\n", vehiculo->fitness_vehiculo);

    imprimir_ruta(vehiculo->ruta, vehiculo->id_vehiculo);
    imprimir_datos_cliente(vehiculo);
}

// Imprime la flota completa de un conjunto de vehículos
void imprimir_flota(struct lista_vehiculos *flota) {
    printf("  >> Flota:\n");

    struct nodo_vehiculo *actual = flota->cabeza;
    int contador = 1;

    while (actual != NULL) {
        imprimir_vehiculo(actual->vehiculo);
        actual = actual->siguiente;
        contador++;
    }
}

// Imprime la lista tabú de una hormiga
void imprimir_tabu(int *tabu, int num_clientes) {
    printf("  Tabú: [");
    for (int i = 0; i < num_clientes; i++) {
        printf("%d", tabu[i]);
        if (i < num_clientes - 1)
            printf(", ");
    }
    printf("]\n");
}

// Imprime los parámetros del individuo (configuración)
void imprimir_individuo(individuo *ind) {
    printf(">> Individuo:\n");
    printf("  ACO:    α=%.2lf, β=%.2lf, γ=%.2lf, ρ=%.2lf, Hormigas=%d, Iteraciones=%d\n",
           ind->alpha, ind->beta, ind->gamma, ind->rho,
           ind->numHormigas, ind->numIteracionesACO);
    printf("  SA:     T_ini=%.2lf, T_fin=%.2lf, Enfriamiento=%.2lf, Iteraciones=%d\n",
           ind->temperatura_inicial, ind->temperatura_final,
           ind->factor_enfriamiento, ind->numIteracionesSA);
}

// Imprime toda la información de todas las hormigas
void imprimir_hormigas(struct hormiga *hormigas, struct vrp_configuracion *vrp, struct individuo *ind) {
    printf("=================================================\n");
    printf(">>> INFORMACIÓN DE HORMIGAS Y SUS RUTAS\n");
    printf("=================================================\n");

    imprimir_individuo(ind);

    for (int i = 0; i < ind->numHormigas; i++) {
        printf("\n[Hormiga ID: %d]\n", hormigas[i].id_hormiga);
        printf("  Vehículos usados : %d/%d\n", hormigas[i].vehiculos_necesarios, hormigas[i].vehiculos_maximos);
        printf("  Fitness global   : %.2f\n", hormigas[i].fitness_global);

        imprimir_tabu(hormigas[i].tabu, vrp->num_clientes);
        imprimir_flota(hormigas[i].flota);

        printf("-------------------------------------------------\n");
    }

    printf("=================================================\n");
}

// Imprime la información de la mejor hormiga encontrada
void imprimir_mejor_hormiga(struct hormiga *hormiga, struct individuo *ind) {
    printf("=================================================\n");
    printf(">>> MEJOR HORMIGA ENCONTRADA\n");
    printf("=================================================\n");

    imprimir_individuo(ind);
    printf("  Vehículos usados : %d/%d\n", hormiga->vehiculos_necesarios, hormiga->vehiculos_maximos);
    printf("  Fitness global   : %.2f\n", hormiga->fitness_global);

    imprimir_flota(hormiga->flota);

    printf("=================================================\n");
}
