#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/control_memoria.h"
#include "../include/configuracion_vrp.h"
#include "../include/estructuras.h"
#include "../include/salida_datos.h"

// Función para leer un archivo CSV con los datos del VRP
void leemos_csv(struct vrp_configuracion *vrp, char *archivo_instancia, int tamanio_instancia)
{
    char ruta[100];
    snprintf(ruta, sizeof(ruta), "Instancias/Instancias_%d/%s.csv", tamanio_instancia, archivo_instancia);

    // Intentamos abrir el archivo CSV
    FILE *archivo = fopen(ruta, "r");
    if (archivo == NULL)
    {
        imprimir_mensaje("Error al abrir el archivo CSV para lectura.");
        return;
    }

    char buffer[256];
    fgets(buffer, sizeof(buffer), archivo); // Leemos la primera línea (encabezado)

    int num_vehiculos, num_capacidad, num_clientes;
    // Leemos los parámetros de configuración del archivo CSV
    if (fscanf(archivo, "%d, %d, %d\n", &num_vehiculos, &num_capacidad, &num_clientes) != 3)
    {
        imprimir_mensaje("Error al leer los parámetros de configuración del archivo CSV.");
        fclose(archivo);
        return;
    }

    vrp->num_vehiculos = num_vehiculos;
    vrp->num_capacidad = num_capacidad;
    vrp->num_clientes = num_clientes;

    // Asignamos memoria para los clientes
    vrp->clientes = asignar_memoria_clientes(vrp);
    if (vrp->clientes == NULL)
    {
        fclose(archivo);
        return;
    }

    // Leemos la información de los clientes
    int cliente_index = 0;
    while (fgets(buffer, sizeof(buffer), archivo) && cliente_index < vrp->num_clientes)
    {
        int id;
        double x, y, demanda;

        // Si encontramos datos válidos, los guardamos en la estructura
        if (sscanf(buffer, "%d, %lf, %lf, %lf",
                   &id, &x, &y, &demanda) == 4)
        {
            vrp->clientes[cliente_index].id_cliente = id;
            vrp->clientes[cliente_index].coordenada_x = x;
            vrp->clientes[cliente_index].coordenada_y = y;
            vrp->clientes[cliente_index].demanda_capacidad = demanda;

            cliente_index++;
        }
    }

    fclose(archivo); // Cerramos el archivo
}

// Función para crear un archivo CSV con los datos del VRP
void creamos_csv(struct vrp_configuracion *vrp, char *archivo_instancia, int tamanio_instancia)
{
    char ruta[100];
    snprintf(ruta, sizeof(ruta), "Instancias/Instancias_%d/%s.csv", tamanio_instancia, archivo_instancia);

    // Abrimos el archivo en modo escritura
    FILE *archivo = fopen(ruta, "w");

    if (archivo == NULL)
    {
        imprimir_mensaje("Error al abrir el archivo CSV para escritura.");
        return;
    }

    // Escribimos la primera línea con los parámetros de configuración
    fprintf(archivo, "%s\n%d, %d, %d\n",
            archivo_instancia,
            vrp->num_vehiculos,
            vrp->num_capacidad,
            vrp->num_clientes);

    // Escribimos los datos de cada cliente
    for (int i = 0; i < vrp->num_clientes; i++)
    {
        fprintf(archivo, "%d, %.2lf, %.2lf, %.2lf\n",
                vrp->clientes[i].id_cliente,
                vrp->clientes[i].coordenada_x,
                vrp->clientes[i].coordenada_y,
                vrp->clientes[i].demanda_capacidad);
    }

    fclose(archivo); // Cerramos el archivo
}

// Función para leer un archivo TXT con los datos del VRP
void leemos_txt(struct vrp_configuracion *vrp, char *ruta)
{
    FILE *file = fopen(ruta, "r");
    if (!file)
    {
        perror("Error al abrir el archivo");
        return;
    }

    char buffer[256];

    // Leemos la configuración de vehículos y capacidad
    while (fgets(buffer, sizeof(buffer), file))
    {
        if (strstr(buffer, "NUMBER") && strstr(buffer, "CAPACITY"))
        {
            if (fgets(buffer, sizeof(buffer), file))
            {
                char *ptr = buffer;
                while (*ptr == ' ' && *ptr != '\0')
                    ptr++;
                sscanf(ptr, "%d %d", &vrp->num_vehiculos, &vrp->num_capacidad);
                break;
            }
        }
    }

    // Leemos la información de los clientes
    while (fgets(buffer, sizeof(buffer), file))
    {
        if (strstr(buffer, "CUST NO."))
        {
            fgets(buffer, sizeof(buffer), file); // Salto de línea
            break;
        }
    }

    // Contamos los clientes
    int num_clientes = 0;
    long posicion_inicial = ftell(file);
    while (fgets(buffer, sizeof(buffer), file))
    {
        int id;
        if (sscanf(buffer, "%d", &id) == 1)
        {
            num_clientes++;
        }
    }

    vrp->num_clientes = num_clientes;

    // Asignamos memoria para los clientes
    vrp->clientes = asignar_memoria_clientes(vrp);
    if (vrp->clientes == NULL)
    {
        fclose(file);
        return;
    }

    // Regresamos al inicio de la sección de clientes
    fseek(file, posicion_inicial, SEEK_SET);

    // Leemos la información de cada cliente
    int cliente_index = 0;
    while (fgets(buffer, sizeof(buffer), file) && cliente_index < vrp->num_clientes)
    {
        int id;
        double x, y, demanda;

        if (sscanf(buffer, "%d %lf %lf %lf",
                   &id, &x, &y, &demanda) == 4)
        {
            vrp->clientes[cliente_index].id_cliente = id;
            vrp->clientes[cliente_index].coordenada_x = x;
            vrp->clientes[cliente_index].coordenada_y = y;
            vrp->clientes[cliente_index].demanda_capacidad = demanda;

            cliente_index++;
        }
    }

    fclose(file); // Cerramos el archivo
}

// Función para leer una instancia desde archivo CSV o TXT
#include <stdlib.h>  // Necesario para la función exit()

// Función para leer una instancia desde archivo CSV o TXT
struct vrp_configuracion *leer_instancia(char *archivo_instancia, int tamanio_instancia)
{
    char ruta[100];
    struct vrp_configuracion *vrp = asignar_memoria_vrp_configuracion(); // Asignamos memoria para la estructura vrp_configuracion

    vrp->num_vehiculos = 0; // Inicializamos número de vehículos en 0
    vrp->num_capacidad = 0; // Inicializamos la capacidad del vehículo en 0
    vrp->num_clientes = 0;  // Inicializamos número de clientes en 0
    vrp->clientes = NULL;   // Inicializamos la estructura vrp_clientes en NULL

    // Intentamos leer el archivo CSV
    snprintf(ruta, sizeof(ruta), "Instancias/Instancias_%d/%s.csv", tamanio_instancia, archivo_instancia);
    FILE *archivo = fopen(ruta, "r");

    if (archivo)
    {
        leemos_csv(vrp, archivo_instancia, tamanio_instancia);
        fclose(archivo);
        if (vrp == NULL || vrp->clientes == NULL) {
            liberar_memoria_vrp_configuracion(vrp); // Liberamos la memoria del vrp
            exit(EXIT_FAILURE); // Finaliza el programa con un código de error
        }
        return vrp; // Retornamos vrp si la lectura fue exitosa
    }

    // Si no existe el CSV, intentamos con el TXT
    snprintf(ruta, sizeof(ruta), "VRP_Solomon/VRP_Solomon_%d/%s.txt", tamanio_instancia, archivo_instancia);
    archivo = fopen(ruta, "r");

    if (archivo)
    {

        leemos_txt(vrp, ruta);
        creamos_csv(vrp, archivo_instancia, tamanio_instancia); // Crear el CSV si el TXT fue leído correctamente
        fclose(archivo);
        if (vrp == NULL || vrp->clientes == NULL) {
            liberar_memoria_vrp_configuracion(vrp); // Liberamos la memoria del vrp
            exit(EXIT_FAILURE); // Finaliza el programa con un código de error
        }
        return vrp; // Retornamos vrp si la lectura fue exitosa
    }

    exit(EXIT_FAILURE); // Finaliza el programa con un código de error
}
