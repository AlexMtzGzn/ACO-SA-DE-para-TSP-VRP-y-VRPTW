#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/control_memoria.h"
#include "../include/configuracion_tsp.h"
#include "../include/estructuras.h"
#include "../include/salida_datos.h"

// Función para leer un archivo CSV con los datos del TSP
void leemos_csv(struct tsp_configuracion *tsp, char *archivo_instancia, int tamanio_instancia)
{
    char ruta[100];
    // Construimos la ruta al archivo CSV con base en el tamaño de la instancia y el nombre del archivo
    snprintf(ruta, sizeof(ruta), "Instancias/Instancias_%d/%s.csv", tamanio_instancia, archivo_instancia);

    FILE *archivo = fopen(ruta, "r");
    if (archivo == NULL)
    {
        imprimir_mensaje("Error al abrir el archivo CSV para lectura.");
        return;
    }

    char buffer[256];
    fgets(buffer, sizeof(buffer), archivo); // Leemos la primera línea (encabezado)

    int num_clientes;
    // Leemos el número de clientes desde la siguiente línea del archivo
    if (fscanf(archivo, "%d\n", &num_clientes) != 1)
    {
        imprimir_mensaje("Error al leer los parámetros de configuración del archivo CSV.");
        fclose(archivo);
        return;
    }

    tsp->num_clientes = num_clientes; // Guardamos el número de clientes en la estructura

    // Asignamos memoria para los clientes
    tsp->clientes = asignar_memoria_clientes(tsp);
    if (tsp->clientes == NULL)
    {
        fclose(archivo);
        return;
    }

    // Leemos los datos de cada cliente del archivo
    int cliente_index = 0;
    while (fgets(buffer, sizeof(buffer), archivo) && cliente_index < tsp->num_clientes)
    {
        int id;
        double x, y;

        // Si se encuentran datos válidos, se almacenan en la estructura
        if (sscanf(buffer, "%d, %lf, %lf", &id, &x, &y) == 3)
        {
            tsp->clientes[cliente_index].id_cliente = id;
            tsp->clientes[cliente_index].coordenada_x = x;
            tsp->clientes[cliente_index].coordenada_y = y;
            cliente_index++;
        }
    }

    fclose(archivo); // Cerramos el archivo

    // Verificamos si se leyeron todos los clientes esperados
    if (cliente_index < tsp->num_clientes)
        imprimir_mensaje("Advertencia: No se pudieron leer todos los clientes del archivo CSV.");
}

// Función para crear un archivo CSV con los datos del TSP
void creamos_csv(struct tsp_configuracion *tsp, char *archivo_instancia, int tamanio_instancia)
{
    char ruta[100];
    // Generamos la ruta del archivo CSV donde se guardará la información
    snprintf(ruta, sizeof(ruta), "Instancias/Instancias_%d/%s.csv", tamanio_instancia, archivo_instancia);

    FILE *archivo = fopen(ruta, "w");
    if (archivo == NULL)
    {
        imprimir_mensaje("Error al abrir el archivo CSV para escritura.");
        return;
    }

    // Escribimos el encabezado con nombre del archivo y número de clientes
    fprintf(archivo, "%s\n%d\n", archivo_instancia, tsp->num_clientes);

    // Escribimos los datos de cada cliente en el archivo CSV
    for (int i = 0; i < tsp->num_clientes; i++)
    {
        fprintf(archivo, "%d, %.2lf, %.2lf\n",
                tsp->clientes[i].id_cliente,
                tsp->clientes[i].coordenada_x,
                tsp->clientes[i].coordenada_y);
    }

    fclose(archivo); // Cerramos el archivo
}

// Función para leer un archivo TXT con los datos del TSP
void leemos_txt(struct tsp_configuracion *tsp, char *ruta)
{
    FILE *file = fopen(ruta, "r");
    if (!file)
    {
        imprimir_mensaje("Error al abrir el archivo TXT para lectura.");
        return;
    }

    char buffer[256];

    // Avanzamos hasta encontrar la sección de clientes
    while (fgets(buffer, sizeof(buffer), file))
    {
        if (strstr(buffer, "CUST NO."))
        {
            fgets(buffer, sizeof(buffer), file); // Saltamos la línea siguiente al encabezado
            break;
        }
    }

    // Guardamos la posición actual para regresar después
    long posicion_inicial = ftell(file);

    // Contamos la cantidad de clientes
    int num_clientes = 0;
    while (fgets(buffer, sizeof(buffer), file))
    {
        int id;
        double x, y;
        if (sscanf(buffer, "%d %lf %lf", &id, &x, &y) == 3)
            num_clientes++;
    }

    tsp->num_clientes = num_clientes; // Guardamos el número total de clientes

    if (num_clientes == 0)
    {
        imprimir_mensaje("Error: No se encontraron clientes en el archivo TXT.");
        fclose(file);
        return;
    }

    // Asignamos memoria para los clientes
    tsp->clientes = asignar_memoria_clientes(tsp);
    if (tsp->clientes == NULL)
    {
        fclose(file);
        return;
    }

    // Regresamos a la posición donde empieza la sección de clientes
    fseek(file, posicion_inicial, SEEK_SET);

    // Leemos los datos de los clientes y los almacenamos en la estructura
    int cliente_index = 0;
    while (fgets(buffer, sizeof(buffer), file) && cliente_index < tsp->num_clientes)
    {
        int id;
        double x, y;

        if (sscanf(buffer, "%d %lf %lf", &id, &x, &y) == 3)
        {
            tsp->clientes[cliente_index].id_cliente = id;
            tsp->clientes[cliente_index].coordenada_x = x;
            tsp->clientes[cliente_index].coordenada_y = y;
            cliente_index++;
        }
    }

    fclose(file); // Cerramos el archivo

    // Verificamos si se leyeron todos los clientes correctamente
    if (cliente_index < tsp->num_clientes)
        imprimir_mensaje("Advertencia: No se pudieron leer todos los clientes del archivo TXT.");
}

// Función general que intenta leer una instancia desde un archivo CSV o TXT
struct tsp_configuracion *leer_instancia(char *archivo_instancia, int tamanio_instancia)
{
    // Asignamos memoria para la configuración TSP
    struct tsp_configuracion *tsp = asignar_memoria_tsp_configuracion();

    if (tsp == NULL)
    {
        imprimir_mensaje("Error: No se pudo asignar memoria para la configuración tsp.");
        exit(EXIT_FAILURE);
    }

    tsp->num_clientes = 0;
    tsp->clientes = NULL;

    char ruta_csv[100];
    char ruta_txt[100];

    // Construimos las rutas para ambos tipos de archivos
    snprintf(ruta_csv, sizeof(ruta_csv), "Instancias/Instancias_%d/%s.csv", tamanio_instancia, archivo_instancia);
    snprintf(ruta_txt, sizeof(ruta_txt), "TSP_Solomon/TSP_Solomon_%d/%s.txt", tamanio_instancia, archivo_instancia);

    // Intentamos abrir y leer el archivo CSV primero
    FILE *archivo_csv = fopen(ruta_csv, "r");
    if (archivo_csv)
    {
        fclose(archivo_csv);
        leemos_csv(tsp, archivo_instancia, tamanio_instancia);

        // Si no se pudo leer bien, liberamos memoria y salimos
        if (tsp->num_clientes == 0 || tsp->clientes == NULL)
        {
            imprimir_mensaje("Error: Falló la lectura del archivo CSV.");
            liberar_memoria_tsp_configuracion(tsp);
            exit(EXIT_FAILURE);
        }

        return tsp;
    }

    // Si no hay CSV, intentamos leer el TXT
    FILE *archivo_txt = fopen(ruta_txt, "r");
    if (archivo_txt)
    {
        fclose(archivo_txt);
        leemos_txt(tsp, ruta_txt);

        // Validamos la lectura
        if (tsp->num_clientes == 0 || tsp->clientes == NULL)
        {
            imprimir_mensaje("Error: Falló la lectura del archivo TXT.");
            liberar_memoria_tsp_configuracion(tsp);
            exit(EXIT_FAILURE);
        }

        // Creamos el CSV automáticamente si el TXT se leyó bien
        creamos_csv(tsp, archivo_instancia, tamanio_instancia);
        return tsp;
    }

    // Si no se encontró ningún archivo válido
    imprimir_mensaje("Error: No se encontró ningún archivo de instancia válido.");
    liberar_memoria_tsp_configuracion(tsp);
    exit(EXIT_FAILURE);
}
