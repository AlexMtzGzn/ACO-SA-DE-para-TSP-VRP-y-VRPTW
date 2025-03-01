// #include <stdio.h>
// #include <stdlib.h>

// #define MAX_LINE 1024  // Tamaño máximo de línea

// int main() {
//     FILE *file = fopen("/home/amg/Documentos/Proyecto-Terminal-Green-VRP/VRP-TW/Instances/C101.csv", "r");
//     if (!file) {
//         perror("Error al abrir el archivo");
//         return EXIT_FAILURE;
//     }

//     char line[MAX_LINE]; // Buffer para leer cada línea
//     int numVehiculos, capacidad;

//     // Omitir la primera línea (nombre del dataset)
//     fgets(line, MAX_LINE, file);

//     // Leer la segunda línea con número de vehículos y capacidad
//     if (fgets(line, MAX_LINE, file)) {
//         sscanf(line, "%d,%d", &numVehiculos, &capacidad);
//         printf("Número de vehículos: %d\nCapacidad: %d\n", numVehiculos, capacidad);
//     }

//     // Leer las demás líneas con la información de clientes
//     printf("\nID\tX\tY\tDemanda\tInicio\tFin\tServicio\n");
//     while (fgets(line, MAX_LINE, file)) {
//         int id, x, y, demanda, inicio, fin, servicio;
//         if (sscanf(line, "%d,%d,%d,%d,%d,%d,%d", &id, &x, &y, &demanda, &inicio, &fin, &servicio) == 7) {
//             printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\n", id, x, y, demanda, inicio, fin, servicio);
//         }
//     }

//     fclose(file);
//     return EXIT_SUCCESS;
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main() {
    FILE *file = fopen("/home/amg/Documentos/Proyecto-Terminal-Green-VRP/VRP-TW/VRP_Solomon/C101.txt", "r");
    if (!file) {
        perror("Error al abrir el archivo");
        return 1;
    }

    char buffer[256];
    char ultima_linea[256];
    int linea_contador = 0;

    // Leer todas las líneas y almacenar la última y la quinta línea
    while (fgets(buffer, sizeof(buffer), file)) {
        linea_contador++;

        // Extraer la quinta línea
        if (linea_contador == 5) {
            printf("Quinta línea: %s", buffer);
            char *token = strtok(buffer, " ");  // Separar por espacios

            // Obtener el primer valor (vehículos)
            if (token) {
                int vehiculos = atoi(token);
                printf("Primer valor de la quinta línea: %d\n", vehiculos);
            }

            // Obtener el segundo valor (capacidad)
            token = strtok(NULL, " ");  // Obtener el siguiente token
            if (token) {
                int capacidad = atoi(token);
                printf("Segundo valor de la quinta línea: %d\n", capacidad);
            }
        }

        // Guardar la última línea
        snprintf(ultima_linea, sizeof(ultima_linea), "%s", buffer);
    }

    fclose(file);

    // Mostrar la última línea
    printf("\nÚltima línea: %s", ultima_linea);

    // Obtener el primer valor de la última línea
    char *token = strtok(ultima_linea, " ");  // Separar por espacios
    if (token) {
        int valor = atoi(token);  // Convertir a entero
        printf("Primer valor de la última línea: %d\n", valor);
    }

    return 0;
}
