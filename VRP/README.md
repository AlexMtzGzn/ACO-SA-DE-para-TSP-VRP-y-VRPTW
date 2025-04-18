# 🚚 Resolución del VRP con ACO optimizado por DE

Este proyecto implementa una solución híbrida para el Problema de Rutas de Vehículos (VRP), utilizando el algoritmo Ant Colony Optimization (ACO) optimizado automáticamente mediante el Algoritmo Evolutivo Diferencial (DE).

## 🧩 ¿Qué es el VRP?

El Problema de Rutas de Vehículos (VRP, por sus siglas en inglés "Vehicle Routing Problem") es una extensión del clásico Problema del Viajante (TSP) y representa uno de los problemas más importantes en logística y distribución.

Consiste en encontrar las rutas óptimas para una flota de vehículos que deben visitar un conjunto de clientes, considerando restricciones como la capacidad de carga de cada vehículo y la demanda de cada cliente, con el objetivo de minimizar la distancia total recorrida.

## 🐜 ¿Qué es ACO (Ant Colony Optimization)?

ACO (Ant Colony Optimization) es una metaheurística inspirada en el comportamiento colectivo de las colonias de hormigas.

En la naturaleza, las hormigas encuentran caminos cortos entre su nido y las fuentes de comida dejando feromonas. Cuanto mejor sea el camino (más corto), más feromonas se acumulan, y más probable es que otras hormigas lo sigan, reforzando así la solución.

En el VRP, simulamos este comportamiento:

- Cada "hormiga" construye una solución recorriendo clientes.
- Las decisiones se toman con base en:
  - **Cantidad de feromona** (lo aprendido)
  - **Visibilidad** (inverso de la distancia)
- Después de cada iteración, se actualizan las feromonas, favoreciendo los caminos más cortos.
- Se respetan las **restricciones de capacidad** de cada vehículo.

## 🧬 ¿Qué es el Algoritmo Evolutivo Diferencial (DE)?

DE es una técnica de optimización basada en poblaciones. Ideal para problemas continuos y para ajustar parámetros automáticamente.

📌 Se basa en tres operadores:

Mutación – Combinación de soluciones existentes.

Cruzamiento (recombinación) – Mezcla de individuo mutado y original.

Selección – Se elige el más apto entre ambos.

## 🧠 ¿Cómo se resolvió el VRP?

El enfoque fue **híbrido**:

- **ACO** resuelve el VRP directamente, respetando las restricciones de capacidad.
- **DE** encuentra los mejores parámetros para ACO.
- **Selección Greedy** de vehículos cuando se alcanza la capacidad máxima.

🎯 **Parámetros optimizados por DE**:
El algoritmo Evolutivo Diferencial (DE) se utilizó para calibrar los parámetros del algoritmo ACO. Los siguientes rangos fueron considerados para cada parámetro:

- **α (alpha)**: Influencia de la feromona. Ajustado entre **1.0 y 2.5**.
- **β (beta)**: Influencia de la visibilidad (heurística). Ajustado entre **1.0 y 2.5**.
- **ρ (rho)**: Tasa de evaporación de feromonas. Ajustado entre **0.1 y 0.9**.
- **Número de hormigas**: Ajustado entre **20 y 100**.
- **Número de iteraciones**: Ajustado entre **50 y 200**.

### 📊 ¿Cómo se generaron los parámetros?

Durante la ejecución de DE, cada parámetro fue generado aleatoriamente dentro de los siguientes intervalos:

- **α (alpha)**: Se generó entre **1.0 y 2.5**.
- **β (beta)**: Se generó entre **1.0 y 2.5**.
- **ρ (rho)**: Se generó entre **0.1 y 0.9**.
- **Número de hormigas**: Se generó entre **20 y 100**.
- **Número de iteraciones**: Se generó entre **50 y 200**.

Esto permitió ajustar de manera eficiente los parámetros para obtener la mejor solución en el VRP sin necesidad de hacerlo manualmente.

🔁 **Proceso combinado**:

1. DE genera una población de parámetros.
2. Cada conjunto se evalúa ejecutando ACO.
3. Se obtiene la distancia total de todas las rutas generadas.
4. DE evoluciona los parámetros para minimizar la distancia total.

📈 Así se optimiza el rendimiento de ACO **sin ajustar nada manualmente**.

## 🚛 Gestión de Vehículos y Capacidad

La principal diferencia con respecto al TSP es la incorporación de:

- **Restricciones de capacidad**: Cada vehículo tiene una capacidad máxima.
- **Demanda de clientes**: Cada cliente requiere cierta cantidad de producto.
- **Selección greedy de vehículos**: Cuando un vehículo alcanza su capacidad máxima, se selecciona el siguiente vehículo disponible.

El algoritmo construye las rutas considerando estas restricciones:

1. Comienza desde el depósito.
2. Selecciona el siguiente cliente basándose en feromonas y visibilidad.
3. Verifica si la capacidad del vehículo permite atender al cliente.
4. Si no es posible, regresa al depósito y utiliza el siguiente vehículo.
5. Continúa hasta que todos los clientes sean atendidos.

## 🎯 Resultados Esperados

El objetivo principal de este proyecto es encontrar las mejores rutas para el **Problema de Rutas de Vehículos (VRP)** con restricciones de capacidad, mediante el uso combinado del algoritmo **ACO** y el algoritmo **DE** para optimizar los parámetros.

### 🔍 ¿Qué se espera como salida?

1. **Las mejores rutas encontradas**: Las rutas óptimas para cada vehículo, que minimizan la distancia total recorrida.
2. **Tiempo de ejecución total**: El tiempo total que tomó ejecutar el proceso de optimización y encontrar las mejores rutas.
3. **Utilización de vehículos**: Cantidad de vehículos utilizados y su nivel de ocupación.

### 📦 Resultados Generados

3. **Archivo JSON**

   - Se genera un archivo `.json` que contiene todos los parámetros utilizados en la ejecución, tales como:
     - Nombre del archivo de entrada
     - Tiempo de ejecución en minutos
     - α (alpha)
     - β (beta)
     - ρ (rho)
     - Número de hormigas
     - Número de iteraciones
     - Valor de fitness de la solución
     - Rutas generadas para cada vehículo (listas de clientes)
     - Capacidad utilizada por cada vehículo

4. **Imagen simulada**

   - Se genera una imagen estática (`.png`) que representa visualmente las **rutas generadas** por el algoritmo ACO.
   - Las rutas de diferentes vehículos se representan con colores distintos.

   Ejemplo de visualización:
   ![Imagen Ruta](Recursos_Readme/Ejemplo_png.png)

5. **GIF simulado**

   - Se crea un **GIF animado** que simula el proceso de construcción de las rutas, mostrando cómo los vehículos recorren los clientes a lo largo del tiempo.

   Ejemplo de animación:
   ![Simulador Ruta](Recursos_Readme/Ejemplo_gif.gif)

### 💾 Ejemplo de archivo JSON

El archivo `JSON` generado tendrá la siguiente estructura:
```json
{
  "Archivo": "C100_(25)",
  "Tiempo Ejecucion en Minutos": 132,
  "Alpha": 2.29598500407114,
  "Beta": 2.2105640269399451,
  "Rho": 0.575674044189823,
  "Numero Hormigas": 29,
  "Numero Iteraciones": 116,
  "Fitness Global": 142.35069377207884,
  "Rutas Vehiculos": [
    {
      "Vehiculo": 1,
      "Capacidad": 200,
      "Carga": 192,
      "Ruta": [0, 5, 3, 7, 8, 10, 11, 0]
    },
    {
      "Vehiculo": 2,
      "Capacidad": 200,
      "Carga": 187,
      "Ruta": [0, 14, 16, 15, 19, 18, 17, 13, 0]
    },
    {
      "Vehiculo": 3,
      "Capacidad": 200,
      "Carga": 195,
      "Ruta": [0, 21, 22, 23, 24, 25, 20, 0]
    },
    {
      "Vehiculo": 4,
      "Capacidad": 200,
      "Carga": 176,
      "Ruta": [0, 1, 2, 12, 4, 6, 9, 0]
    }
  ]
}
```

## Requisitos

Para ejecutar este proyecto, asegúrate de tener lo siguiente:

### 🧑‍💻 C Compiler

Es necesario tener un compilador de C instalado (como gcc) para compilar el código fuente.

### Librería `cJSON`:  
  Este proyecto requiere la librería `cJSON` para trabajar con archivos JSON en C.  
  Puedes encontrarla y consultar cómo instalarla en su repositorio oficial:
    
  👉 [https://github.com/DaveGamble/cJSON](https://github.com/DaveGamble/cJSON)

### 📦 Python

Asegúrate de tener Python instalado junto con las siguientes bibliotecas:

- json
- os
- sys
- matplotlib
- numpy

## Compilación y Ejecución

### 1. **Compilación**

Para compilar el proyecto, usa el siguiente comando:

```bash
make
```

Este comando compilará el código en modo release por defecto (optimizado). Si prefieres compilar en modo debug para facilitar la depuración, puedes usar:

```bash
make debug
```

### 2. Ejecutar el Programa

Una vez compilado el proyecto, puedes ejecutar el ejecutable generado (llamado main) con los siguientes parámetros:

```bash
./main <poblacion> <generaciones> <archivo> <numero_de_clientes> <capacidad_vehiculo>
```

Ejemplo:
```bash
./main 50 100 C100 25 200
```

- poblacion: el tamaño de la población para el algoritmo.
- generaciones: el número de generaciones que el algoritmo debe ejecutar.
- archivo: el archivo de entrada.
- numero_de_clientes: el número de clientes a considerar en el VRP.
- capacidad_vehiculo: la capacidad máxima de cada vehículo.

### 3. Limpieza

Si deseas limpiar los archivos generados (archivos objeto, ejecutables, etc.), puedes usar:

```bash
make clean
```

## 📁 Estructura del Proyecto

```bash
.
├── build/                     # Archivos objetos y dependencias generados por el compilador
├── include/                  # Archivos de cabecera (.h)
│   ├── aed.h
│   ├── configuracion_json.h
│   ├── configuracion_vrp.h    # Modificado para VRP
│   ├── control_memoria.h
│   ├── estructuras.h
│   ├── lista_flota.h
│   ├── lista_ruta.h
│   ├── salida_datos.h
│   └── vrp_aco.h
├── Instancias/               # Instancias CSV utilizadas en la ejecución
│   ├── Instancias_25/        
│   ├── Instancias_50/
│   └── Instancias_100/
├── main                      # Ejecutable generado tras compilar
├── makefile                  # Makefile para compilar el proyecto
├── README.md                 # Archivo de documentación
├── Resultados/               # Salidas generadas por la ejecución
│   ├── Resultados_25/
│   │   ├── Gifs/
│   │   ├── Imagenes/
│   │   └── Json/
│   ├── Resultados_50/
│   │   ├── Gifs/
│   │   ├── Imagenes/
│   │   └── Json/
│   └── Resultados_100/
│       ├── Gifs/
│       ├── Imagenes/
│       └── Json/
├── src/                      # Código fuente del proyecto en C y Python
│   ├── aed.c
│   ├── configuracion_json.c
│   ├── configuracion_vrp.c   # Modificado para VRP
│   ├── control_memoria.c
│   ├── lista_flota.c
│   ├── lista_ruta.c
│   ├── main.c
│   ├── salida_datos.c
│   ├── vrp_aco.c
│   └── Simulador_VRP/        # Modificado para VRP
│       └── simulador_vrp.py
└── VRP_Solomon/              # Instancias del benchmark Solomon
    ├── VRP_Solomon_25/
    │   ├── C100_(25).txt
    │   ├── C200_(25).txt
    │   ├── R100_(25).txt
    │   ├── R200_(25).txt
    │   ├── RC100_(25).txt
    │   └── RC200_(25).txt
    ├── VRP_Solomon_50/
    │   ├── C100_(50).txt
    │   ├── C200_(50).txt
    │   ├── R100_(50).txt
    │   ├── R200_(50).txt
    │   ├── RC100_(50).txt
    │   └── RC200_(50).txt
    └── VRP_Solomon_100/
        ├── C100_(100).txt
        ├── C200_(100).txt
        ├── R100_(100).txt
        ├── R200_(100).txt
        └── RC100_(100).txt
```

### ✅ Consideraciones finales

Este trabajo busca contribuir al estudio y solución del problema VRP mediante la implementación de algoritmos bioinspirados. La principal diferencia respecto a la versión TSP es la incorporación de restricciones de capacidad y la selección greedy de vehículos. Se invita a la comunidad a explorar, reutilizar y mejorar el código según sus necesidades.

## 👥 Contribuciones

- 🧑‍🏫 **Dr. Edwin Montes Orozco**  
  Director de PT y responsable del acompañamiento académico durante el desarrollo del proyecto.

- 👨‍💻 **Alejandro Martínez Guzmán**  
  Autor del proyecto. Encargado del diseño, implementación y documentación del sistema de optimización.

- 🧪 **Jaime López Lara**  
  Ayudante en la ejecución del código y recolección de resultados.

## 📝 Licencia

Este proyecto está licenciado bajo los términos de la licencia MIT.  
Consulta el archivo [LICENSE](./LICENSE) para más detalles.