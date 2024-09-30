# Problema del Agente Viajante (TSP) utilizando el Algoritmo Evolutivo Diferencial (AED) y Sistemas de Optimización por Colonia de Hormigas (ACO).

Este proyecto implementa un sistema de optimización para el **Problema del Agente Viajante** (TSP) utilizando el **Algoritmo Evolutivo Diferenciale** (AED) y **Sistemas de Optimización por Colonia de Hormigas** (ACO).

## Tabla de Contenidos
1. [Descripción](#descripción)
2. [Instalación](#instalación)
3. [Uso](#uso)
4. [Estructura del proyecto](#estructura-del-proyecto)
5. [Licencia](#licencia)

## Descripción

El objetivo del proyecto es resolver el **Problema del Viajante** (TSP) combinando dos técnicas de optimización metaheurísticas:

1. **ACO (Optimización por Colonias de Hormigas):** Simula el comportamiento de las hormigas en busca de rutas óptimas a través de la utilización de feromonas.
2. **AED (Algoritmos Evolutivos Diferenciales):** Una técnica de optimización que busca mejorar parámetros específicos (alpha, beta, rho, número de hormigas e iteraciones) para minimizar el costo de la ruta en cada generación.

Este proyecto permite probar con diferentes instancias y comparar configuraciones de parámetros.

## Instalación

### Requisitos Previos
Antes de comenzar, asegúrate de tener instalados los siguientes componentes:

- **GCC** (Compilador de C)
- **Git** (Para clonar el repositorio)
- **Linux** (Entorno de ejecución)

### Pasos de Instalación y Ejecución
1. Clona este repositorio:
   ```bash
   git clone https://github.com/AlexMtzGzn/Proyecto-Terminal-Green-VRP.git

2. Navega al directorio del proyecto:
    ```bash
    cd Proyecto-Terminal-Green-VRP/TSP_ANT/src

3. Compila el proyecto usando el archivo Makefile:
    ```bash
    make

4. Ejecuta el archivo principal:
    ```bash
    ./main [población] [generaciones] [archivo_instancia]

## Uso

### Ejecución básica

Para ejecutar el programa, utiliza el siguiente comando:

    ./main 20 100 8 instancia8.txt



- 20: Tamaño de la población.
- 100: Número de generaciones.
- 8: Tamaño de la instancia.
- instancia8.txt: Archivo de entrada con la matriz de distancias para el problema.

El programa evaluará las rutas óptimas para la instancia proporcionada.

### Opciones de personalización
Puedes ajustar los parámetros de alpha, beta, rho, el número de hormigas y el número de iteraciones para modificar el comportamiento del ACO.

## Estructura del Proyecto

- instancias/: Contiene archivos de entrada con matrices de distancias para el problema del TSP.

    + instancia4.txt: Matriz para la instancia de tamaño 4.
    + instancia6.txt: Matriz para la instancia de tamaño 6.
    + instancia8.txt: Matriz para la instancia de tamaño 8.
    + instancia10.txt: Matriz para la instancia de tamaño 10.
    + instancia51.txt: Matriz para la instancia de tamaño 51.
    + instancia130.txt: Matriz para la instancia de tamaño 130.

- src/: Contiene los archivos fuente del proyecto.

    + algoritmo_evolutivo_diferencial.c: Implementación del algoritmo evolutivo diferencial.
    + algoritmo_evolutivo_diferencial.h: Archivo de cabecera para el algoritmo evolutivo diferencial.
    + main.c: Archivo principal que ejecuta el programa.
    + makefile: Script para compilar el proyecto.
    + tsp_ant_system.c: Implementación del sistema de optimización por colonias de hormigas.
    + tsp_ant_system.h: Archivo de cabecera para el sistema de optimización por colonias de hormigas.

