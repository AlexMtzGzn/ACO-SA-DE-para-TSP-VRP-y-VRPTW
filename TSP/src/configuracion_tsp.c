#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/control_memoria.h"
#include "../include/configuracion_tsp.h"
#include "../include/estructuras.h"
#include "../include/salida_datos.h"

// Función para leer un archivo CSV con los datos del tsp
void leemos_csv(struct tsp_configuracion *tsp, char *archivo_instancia, int tamanio_instancia)
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

    int num_clientes;
    // Leemos los parámetros de configuración del archivo CSV
    if (fscanf(archivo, "%d\n",&num_clientes) != 2)
    {
        imprimir_mensaje("Error al leer los parámetros de configuración del archivo CSV.");
        fclose(archivo);
        return;
    }

    tsp->num_clientes = num_clientes;

    // Asignamos memoria para los clientes
    tsp->clientes = asignar_memoria_clientes(tsp);
    if (tsp->clientes == NULL)
    {
        fclose(archivo);
        return;
    }

    // Leemos la información de los clientes
    int cliente_index = 0;
    while (fgets(buffer, sizeof(buffer), archivo) && cliente_index < tsp->num_clientes)
    {
        int id;
        double x, y;

        // Si encontramos datos válidos, los guardamos en la estructura
        if (sscanf(buffer, "%d, %lf, %lf",
                   &id, &x, &y) == 3)
        {
            tsp->clientes[cliente_index].id_cliente = id;
            tsp->clientes[cliente_index].coordenada_x = x;
            tsp->clientes[cliente_index].coordenada_y = y;

            cliente_index++;
        }
    }

    fclose(archivo); // Cerramos el archivo
}

// Función para crear un archivo CSV con los datos del tsp
void creamos_csv(struct tsp_configuracion *tsp, char *archivo_instancia, int tamanio_instancia)
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
    fprintf(archivo, "%s\n,%d\n",
            archivo_instancia,
            tsp->num_clientes);

    // Escribimos los datos de cada cliente
    for (int i = 0; i < tsp->num_clientes; i++)
    {
        fprintf(archivo, "%d, %.2lf, %.2lf\n",
                tsp->clientes[i].id_cliente,
                tsp->clientes[i].coordenada_x,
                tsp->clientes[i].coordenada_y);
    }

    fclose(archivo); // Cerramos el archivo
}

// Función para leer un archivo TXT con los datos del tsp
void leemos_txt(struct tsp_configuracion *tsp, char *ruta)
{
    FILE *file = fopen(ruta, "r");
    if (!file)
    {
        perror("Error al abrir el archivo");
        return;
    }

    char buffer[256];

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

    tsp->num_clientes = num_clientes;

    // Asignamos memoria para los clientes
    tsp->clientes = asignar_memoria_clientes(tsp);
    if (tsp->clientes == NULL)
    {
        fclose(file);
        return;
    }

    // Regresamos al inicio de la sección de clientes
    fseek(file, posicion_inicial, SEEK_SET);

    // Leemos la información de cada cliente
    int cliente_index = 0;
    while (fgets(buffer, sizeof(buffer), file) && cliente_index < tsp->num_clientes)
    {
        int id;
        double x, y;

        if (sscanf(buffer, "%d %lf %lf",
                   &id, &x, &y) == 3)
        {
            tsp->clientes[cliente_index].id_cliente = id;
            tsp->clientes[cliente_index].coordenada_x = x;
            tsp->clientes[cliente_index].coordenada_y = y;
            cliente_index++;
        }
    }

    fclose(file); // Cerramos el archivo
}

// Función para leer una instancia desde archivo CSV o TXT
struct tsp_configuracion *leer_instancia(char *archivo_instancia, int tamanio_instancia)
{
    char ruta[100];
    struct tsp_configuracion *tsp = asignar_memoria_tsp_configuracion(); // Asignamos memoria para la estructura tsp_configuracion

    tsp->num_clientes = 0;  // Inicializamos número de clientes en 0
    tsp->clientes = NULL;   // Inicializamos la estructura tsp_clientes en NULL

    // Intentamos leer el archivo CSV
    snprintf(ruta, sizeof(ruta), "Instancias/Instancias_%d/%s.csv", tamanio_instancia, archivo_instancia);
    FILE *archivo = fopen(ruta, "r");

    if (archivo)
    {
        leemos_csv(tsp, archivo_instancia, tamanio_instancia);
        fclose(archivo);
        if (tsp == NULL || tsp->clientes == NULL) {
            liberar_memoria_tsp_configuracion(tsp); // Liberamos la memoria del tsp
            exit(EXIT_FAILURE); // Finaliza el programa con un código de error
        }
        return tsp; // Retornamos tsp si la lectura fue exitosa
    }

    // Si no existe el CSV, intentamos con el TXT
    snprintf(ruta, sizeof(ruta), "tsp_Solomon/tsp_Solomon_%d/%s.txt", tamanio_instancia, archivo_instancia);
    archivo = fopen(ruta, "r");

    if (archivo)
    {

        leemos_txt(tsp, ruta);
        creamos_csv(tsp, archivo_instancia, tamanio_instancia); // Crear el CSV si el TXT fue leído correctamente
        fclose(archivo);
        if (tsp == NULL || tsp->clientes == NULL) {
            liberar_memoria_tsp_configuracion(tsp); // Liberamos la memoria del tsp
            exit(EXIT_FAILURE); // Finaliza el programa con un código de error
        }
        return tsp; // Retornamos tsp si la lectura fue exitosa
    }

    exit(EXIT_FAILURE); // Finaliza el programa con un código de error
}
