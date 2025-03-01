#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vrp_tw_setting.h"



void abrimostxt_creamosxcvs(int *num_clientes, int *num_vehiculos, int *capacidad_vehiculos, char *ruta) {
    if (!num_vehiculos || !capacidad_vehiculos) {
        printf("Error: puntero a num_vehiculos o capacidad_vehiculos es NULL.\n");
        return;
    }

    FILE *file = fopen(ruta, "r");
    if (!file) {
        perror("Error al abrir el archivo");
        return;  // Termina la función si no se puede abrir el archivo
    }

    char buffer[256];
    int linea_contador = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        linea_contador++;

        // Solo procesar la quinta línea
        if (linea_contador == 5) {
            printf("Quinta línea: %s", buffer);
            
            // Usamos strtok para dividir la línea por espacios
            char *token = strtok(buffer, " ");  // Primer valor (vehiculos)
            if (token) {
                *num_vehiculos = atoi(token);  // Asignamos el valor a num_vehiculos
                printf("Primer valor de la quinta línea (num_vehiculos): %d\n", *num_vehiculos);
            }

            token = strtok(NULL, " ");  // Segundo valor (capacidad)
            if (token) {
                *capacidad_vehiculos = atoi(token);  // Asignamos el valor a capacidad_vehiculos
                printf("Segundo valor de la quinta línea (capacidad_vehiculos): %d\n", *capacidad_vehiculos);
            }
        }
    }

    fclose(file);  // Cerramos el archivo
}

void leer_instancia(int *num_clientes, int *num_vehiculos, int *capacidad_vehiculos, char *archivo_instancia) {
    char ruta[100];

    // Ruta del archivo CSV
    snprintf(ruta, sizeof(ruta), "../Instancias/%s.csv", archivo_instancia);
    FILE *archivo = fopen(ruta, "r");

    if (archivo) {
        printf("Archivo CSV abierto exitosamente.\n");
        fclose(archivo);  // Cerrar archivo CSV si existe
    } else {
        printf("No se encontró el archivo CSV, buscando el TXT...\n");
        // Ruta del archivo TXT
        snprintf(ruta, sizeof(ruta), "../VRP_Solomon/%s.txt", archivo_instancia);
        abrimostxt_creamosxcvs(num_clientes, num_vehiculos, capacidad_vehiculos, ruta);
    }
}

