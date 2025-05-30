# 🚚 "Optimización del Problema del Agente Viajero (TSP) mediante una Metaheurística Híbrida ACO-SA con Calibración de Parámetros por Evolución Diferencial"

Este proyecto implementa una solución híbrida para el Problema del Agente Viajero (**TSP**), utilizando el algoritmo Ant Colony Optimization (**ACO**) para generar rutas iniciales, el Recocido Simulado (**SA**) para refinarlas, y todo el proceso optimizado automáticamente mediante el Algoritmo Evolutivo Diferencial (**DE**).

---

## 🧩 ¿Qué es el TSP?

El Problema del Agente Viajero (**TSP**, por sus siglas en inglés _Traveling Salesman Problem_), es uno de los problemas clásicos más conocidos en optimización combinatoria.

Consiste en encontrar la ruta más corta posible, que permita a un viajero visitar una serie de ciudades o clientes **exactamente una vez** y regresar al punto de partida. La meta principal es **minimizar la distancia total recorrida por los vehiculos**.

---

## 🐜 ¿Qué es ACO (Ant Colony Optimization)?

**ACO** (Ant Colony Optimization) es una metaheurística inspirada en el comportamiento colectivo de las colonias de hormigas.

En la naturaleza, las hormigas encuentran caminos cortos entre su nido y las fuentes de comida dejando feromonas, cuanto mejor sea el camino (más corto), más feromonas se acumulan, y hay más probabilidad de que otras hormigas sigan el camino, reforzando así la solución.

En el **TSP**, simulamos este comportamiento:

- Cada "hormiga" construye una solución recorriendo ciudades o clientes.
- Las decisiones se toman basadas en:
  - **Cantidad de feromona** (lo aprendido)
  - **Visibilidad** (inverso de la distancia)
- Después de cada iteración, se actualizan las feromonas, favoreciendo los caminos más cortos.

---

## 🔥 ¿Qué es el Recocido Simulado (SA)?

El Recocido Simulado (_Simulated Annealing_, **SA**) es una metaheurística inspirada en el proceso metalúrgico de Recocido, donde un metal se calienta y luego se enfría de forma controlada para modificar sus propiedades físicas.

En optimización:

- Inicialmente, **acepta soluciones peores con alta probabilidad** (cuando la temperatura es alta).
- Gradualmente, **se vuelve más selectivo** a medida que la temperatura disminuye (_enfriamiento_).
- Este enfoque permite escapar de óptimos locales y explorar ampliamente el espacio de soluciones.

🔧 En nuestro sistema, **SA** toma las rutas generadas por **ACO** y las refina mediante pequeñas modificaciones, aceptando temporalmente algunas soluciones subóptimas para potencialmente encontrar mejores soluciones globales.

---

## 🔄 Movimientos de Vecindad del Recocido Simulado (SA)

Durante la optimización local con **SA**, se generan **soluciones vecinas** a partir de la solución actual mediante uno de los siguientes tres movimientos aleatorios:

1. **Inversión de un segmento de ruta:**  
   Se selecciona una ruta y se invierte el orden de visita de un segmento entre dos clientes, este cambio puede reducir la distancia total si existen trayectos cruzados o ineficientes.

2. **Intercambio de dos clientes:**  
   Se eligen dos clientes (dentro de una misma ruta o entre rutas diferentes) y se intercambian sus posiciones, esto puede modificar significativamente la estructura del recorrido.

3. **Reubicación de un cliente dentro de una ruta:**  
   Se toma un cliente y se lo mueve a otra posición dentro de la misma ruta, es útil para ajustes finos sin alterar mucho la composición de la ruta.

La elección del movimiento se realiza aleatoriamente con igual probabilidad, usando el siguiente criterio:

```bash
if (prob < factor / 3.0)
    aceptado = invertir_segmento_ruta(...);
else if (prob < 2.0 * factor / 3.0)
    aceptado = intercambiar_cliente_ruta(...);
else
    aceptado = mover_cliente_dentro_ruta(...);
```

Donde prob es un número aleatorio entre 0 y 1, y factor es calibrado por **DE**.

Este conjunto de movimientos permite que **SA** explore diversas configuraciones vecinas, ayudando a escapar de óptimos locales y mejorando la calidad de las rutas generadas por **ACO**.

---

## 🧬 ¿Qué es el Algoritmo Evolutivo Diferencial (DE)?

DE es una técnica de optimización basada en poblaciones, ideal para problemas continuos y para ajustar parámetros automáticamente.

📌 Se basa en tres operadores:

- **Mutación**: Combinación de soluciones existentes para generar un vector perturbado.
- **Cruzamiento (recombinación)**: Mezcla del individuo mutado con el original.
- **Selección**: Se elige entre el original y el mutado, conservando el que tenga mejor desempeño.

En este proyecto, **DE ajusta automáticamente los parámetros de ACO** (como α, β, ρ, número de hormigas, etc.) para minimizar la distancia total recorrida por los vehículos.

---

## 🧠 ¿Cómo se resolvió el TSP?

El enfoque fue **híbrido** con tres algortimos:

- **ACO** resuelve el **TSP** directamente.
- **SA** refina las rutas generadas por **ACO**.
- **DE** encuentra los mejores parámetros para ambos algoritmos.

---

## ⚙️ Rango de Parámetros Adaptativos según el Tamaño del Problema

Para lograr una **mejor calibración** de los algoritmos **ACO** (Ant Colony Optimization) y **SA** (Simulated Annealing), se definieron **rangos de parámetros adaptativos** en función del número de clientes en la instancia del **TSP**.

Esto permite que los algoritmos se ajusten de forma dinámica, dependiendo de la complejidad del problema (tamaño de la instancia).

### 🔢 Tamaños de instancia considerados

| Tamaño del problema | Número de clientes (`tsp->num_clientes`) |
| ------------------- | ---------------------------------------- |
| **Pequeña**         | `25`                                     |
| **Mediana**         | `50`                                     |
| **Grande**          | `100`                                    |

### 📐 Rangos de Parámetros por Tamaño

#### 🔸 Instancia Pequeña (`25 clientes`)

| Parámetro                | Mínimo | Máximo |
| ------------------------ | ------ | ------ |
| `alpha`                  | 1.0    | 3.0    |
| `beta`                   | 1.0    | 3.0    |
| `rho`                    | 0.5    | 0.7    |
| `número de hormigas`     | 10     | 25     |
| `iteraciones ACO`        | 50     | 100    |
| `temperatura inicial`    | 1000.0 | 2000.0 |
| `temperatura final`      | 0.1    | 0.5    |
| `factor de enfriamiento` | 0.99   | 0.999  |
| `factor de control`      | 0.7    | 0.5    |
| `iteraciones SA`         | 100    | 150    |

#### 🔸 Instancia Mediana (`50 clientes`)

| Parámetro                | Mínimo | Máximo |
| ------------------------ | ------ | ------ |
| `alpha`                  | 2.0    | 4.0    |
| `beta`                   | 2.0    | 4.0    |
| `rho`                    | 0.4    | 0.6    |
| `número de hormigas`     | 25     | 40     |
| `iteraciones ACO`        | 100    | 150    |
| `temperatura inicial`    | 1500.0 | 2500.0 |
| `temperatura final`      | 0.1    | 0.3    |
| `factor de enfriamiento` | 0.95   | 0.999  |
| `factor de control`      | 0.6    | 0.8    |
| `iteraciones SA`         | 150    | 200    |

#### 🔸 Instancia Grande (`100 clientes`)

| Parámetro                | Mínimo | Máximo |
| ------------------------ | ------ | ------ |
| `alpha`                  | 3.0    | 5.0    |
| `beta`                   | 3.0    | 5.0    |
| `rho`                    | 0.3    | 0.5    |
| `número de hormigas`     | 40     | 50     |
| `iteraciones ACO`        | 150    | 200    |
| `temperatura inicial`    | 2000.0 | 3000.0 |
| `temperatura final`      | 0.1    | 0.2    |
| `factor de enfriamiento` | 0.95   | 0.999  |
| `factor de control`      | 0.6    | 0.9    |
| `iteraciones SA`         | 200    | 300    |

### 🧠 ¿Por qué definir rangos diferentes?

Esto permite que el algoritmo **DE** explore soluciones **más ajustadas al tamaño del problema**, evitando usar configuraciones demasiado pequeñas para instancias grandes, o demasiado costosas para instancias pequeñas,de esta manera se logra un **balance entre calidad de la solución y tiempo de cómputo.**

---

## 🔁 Proceso de Optimización Híbrida (DE + ACO + SA) para TSP

1. **Inicialización con DE**:  
   Se genera aleatoriamente una población inicial de posibles soluciones, donde cada individuo representa un conjunto de parámetros para el algoritmo **ACO** y **SA**.

2. **Evaluación de Individuos**:  
   Cada conjunto de parámetros se evalúa ejecutando el algoritmo **ACO** y **SA** con dichos valores.

3. **Optimización Local**:  
   Después de que **ACO** genera una solución (ruta), se aplica **Recocido Simulado (SA)** como optimizador local, este paso consiste en realizar pequeños ajustes en la ruta generada por **ACO** para mejorar su calidad. **SA** se encarga de explorar soluciones vecinas a la actual (cercanas en el espacio de soluciones) para encontrar una mejor solución local. Durante este proceso, **SA** acepta temporalmente soluciones peores con una probabilidad que disminuye gradualmente a medida que "enfría" su temperatura, permitiendo escapar de óptimos locales.

4. **Cálculo del Fitness**:  
   Se obtiene la **distancia total de la mejor ruta** generada por **ACO** y refinada con **SA**. Esta distancia se utiliza como el valor de fitness del individuo.

5. **Evolución con DE**:  
   El algoritmo **DE** utiliza los valores de fitness para evolucionar la población, generando nuevos conjuntos de parámetros con el objetivo de **minimizar la distancia total**.

6. **Criterio de Paro**:  
   El proceso se repite durante un número máximo de generaciones.

Este proceso permite **optimizar automáticamente** el rendimiento del algoritmo **ACO** y **SA**, **evitando el ajuste manual** de parámetros y encontrando de manera más eficiente soluciones de alta calidad para el **Problema del Agente Viajero (TSP)**.

---

## 🎯 Resultados Esperados

El objetivo principal de este proyecto es encontrar la mejor ruta para el **Problema del Agente Viajero(TSP)** mediante el uso combinado del algoritmo **ACO** y el algoritmo **DE**, el cual optimiza automáticamente los parámetros del **ACO** y del **SA**.

### 🔍 ¿Qué se espera como salida?

1. **La mejor ruta encontrada**  
   La ruta óptima, que minimiza la distancia total recorrida.

2. **Distancia total recorrida**  
   La distancia total de la ruta generada.

3. **Tiempo de ejecución total**  
   El tiempo total que tomó ejecutar el proceso de optimización y encontrar la mejor ruta.

4. **Parámetros óptimos encontrados**  
   Valores de α, β, ρ, temperatura, número de hormigas, iteraciones, etc., que generaron la mejor solución en la instancia evaluada.

### 📦 Resultados Generados

3. **Archivo JSON**

   - Se genera un archivo `.json` que contiene todos los **parámetros optimizados automáticamente** durante la ejecución, tales como:
     - Nombre del archivo de entrada
     - Tiempo de ejecución en minutos
     - Población y generaciones del DE
     - Parámetros de **ACO** (`α`, `β`, `ρ`, número de hormigas, iteraciones **ACO**)
     - Parámetros de **SA** (temperatura inicial, final, factor de enfriamiento, factor de control, iteraciones **SA**)
     - Valor de fitness de la solución
     - Ruta generada (lista de ciudades o clientes visitados)

4. **GIF simulado**

   - Se crea un **GIF animado** que simula el proceso de construcción de la ruta, mostrando cómo la mejor hormiga recorre las ciudades o clientes a lo largo del tiempo.

   Ejemplo de animación:
   ![Simulador Ruta](Recursos_Readme/Ejemplo_gif.gif)

5. **Imagen simulada**

   - Se genera una imagen estática (`.png`) que representa visualmente la **ruta generada** por el algoritmo **ACO** + **SA**.

   Ejemplo de visualización:
   ![Imagen Ruta](Recursos_Readme/Ejemplo_png.png)

### 💾 Ejemplo de archivo JSON

El archivo `JSON` generado tendrá una estructura como la siguiente:

```json
{
  "Archivo": "C100_(25)",
  "Tiempo Ejecucion en Minutos": 2,
  "Poblacion: ": 10,
  "Generaciones: ": 10,
  "Alpha": 1.7343517992339805,
  "Beta": 2.72667057426957,
  "Rho": 0.58487435303855417,
  "Numero Hormigas": 22,
  "Numero Iteraciones ACO": 85,
  "Temperatura Inicial: ": 1389.69581871745,
  "Temperatura Final: ": 0.42306764699661537,
  "Factor de Enfriamiento: ": 0.997994885530786,
  "Factor de Control: ": 0.630100765978033,
  "Numero Iteraciones SA: ": 131,
  "Fitness Global": 132.12162500340892,
  "Ruta Clientes": [
    0, 20, 21, 22, 24, 25, 23, 13, 17, 18, 19, 15, 16, 14, 12, 11, 10, 8, 9, 6,
    4, 2, 1, 3, 5, 7, 0
  ],
  "Ruta Coordenadas": [
    {
      "X": 40,
      "Y": 50
    },
    {
      "X": 30,
      "Y": 50
    },
    {
      "X": 30,
      "Y": 52
    },
    {
      "X": 28,
      "Y": 52
    },
    {
      "X": 25,
      "Y": 50
    },
    {
      "X": 25,
      "Y": 52
    },
    {
      "X": 28,
      "Y": 55
    },
    {
      "X": 22,
      "Y": 75
    },
    {
      "X": 18,
      "Y": 75
    },
    {
      "X": 15,
      "Y": 75
    },
    {
      "X": 15,
      "Y": 80
    },
    {
      "X": 20,
      "Y": 80
    },
    {
      "X": 20,
      "Y": 85
    },
    {
      "X": 22,
      "Y": 85
    },
    {
      "X": 25,
      "Y": 85
    },
    {
      "X": 35,
      "Y": 69
    },
    {
      "X": 35,
      "Y": 66
    },
    {
      "X": 38,
      "Y": 68
    },
    {
      "X": 38,
      "Y": 70
    },
    {
      "X": 40,
      "Y": 69
    },
    {
      "X": 42,
      "Y": 68
    },
    {
      "X": 45,
      "Y": 70
    },
    {
      "X": 45,
      "Y": 68
    },
    {
      "X": 42,
      "Y": 66
    },
    {
      "X": 42,
      "Y": 65
    },
    {
      "X": 40,
      "Y": 66
    },
    {
      "X": 40,
      "Y": 50
    }
  ]
}
```

---

## Requisitos

Para ejecutar este proyecto, asegúrate de tener lo siguiente:

### 🧑‍💻 C Compiler

Es necesario tener un compilador de C instalado (como gcc) para compilar el código fuente.

### Bibleoteca `cJSON`:

Este proyecto requiere la bibleoteca `cJSON` para trabajar con archivos JSON en C.  
 Puedes encontrarla y consultar cómo instalarla en su repositorio oficial:

👉 [https://github.com/DaveGamble/cJSON]

### 📦 Python

Asegúrate de tener Python instalado junto con las siguientes bibliotecas:

- json
- os
- sys
- matplotlib
- numpy

---

## Compilación y Ejecución

### 1. **Compilación**

Para compilar el proyecto, usa el siguiente comando:

```bash
make
```

Este comando compilará el código en modo release por defecto (optimizado), si prefieres compilar en modo debug para facilitar la depuración, puedes usar:

```bash
make debug
```

### 2. Ejecutar el Programa

Una vez compilado el proyecto, puedes ejecutar el ejecutable generado (llamado main) con los siguientes parámetros:

```bash
./main <poblacion> <generaciones> <archivo> <numero_de_clientes>
```

Ejemplo:

```bash
./main 50 100 C100 25
```

- poblacion: el tamaño de la población para el algoritmo.

- generaciones: el número de generaciones que el algoritmo debe ejecutar.

- archivo: el archivo de entrada.

- numero_de_clientes: el número de clientes o ciudades a considerar en el **TSP**.

### 3. Limpieza

Si deseas limpiar los archivos generados (archivos objeto, ejecutables, etc.), puedes usar:

```bash
make clean
```

---

## 📁 Estructura del Proyecto

```bash
.
├── build/                     # Archivos objetos y dependencias generados por el compilador
├── include/                  # Archivos de cabecera (.h)
│   ├── aed.h
│   ├── configuracion_json.h
│   ├── configuracion_tsp.h
│   ├── control_memoria.h
│   ├── estructuras.h
│   ├── lista_flota.h
│   ├── lista_ruta.h
│   ├── salida_datos.h
│   ├── tsp_sa.h
│   └── tsp_aco.h
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
│   ├── configuracion_tsp.c
│   ├── control_memoria.c
│   ├── lista_flota.c
│   ├── lista_ruta.c
│   ├── main.c
│   ├── salida_datos.c
│   ├── tsp_sa.c
│   ├── tsp_aco.c
│   └── Simulador_TSP/
│       └── simulador_tsp.py
└── TSP_Solomon/              # Instancias del benchmark Solomon
    ├── TSP_Solomon_25/
    │   ├── C100_(25).txt
    │   ├── C200_(25).txt
    │   ├── R100_(25).txt
    │   ├── RC100_(25).txt
    ├── TSP_Solomon_50/
    │   ├── C100_(50).txt
    │   ├── C200_(50).txt
    │   ├── R100_(50).txt
    │   ├── RC100_(50).txt
    └── TSP_Solomon_100/
        ├── C100_(100).txt
        ├── C200_(100).txt
        ├── R100_(100).txt
        └── RC100_(100).txt
```

---

## ✅ Conclusión

El desarrollo de una metaheurística híbrida basada en Ant Colony Optimization (**ACO**) y Recocido Simulado (**SA**), calibrada automáticamente mediante un Algoritmo Evolutivo Diferencial (DE), demostró ser una estrategia efectiva para resolver el Problema del Agente Viajero (**TSP**).

El uso de **ACO** permitió generar soluciones iniciales de alta calidad inspiradas en el comportamiento de las hormigas, mientras que **SA** refinó estas soluciones para escapar de óptimos locales y explorar regiones más prometedoras del espacio de búsqueda. La incorporación del DE automatizó por completo el ajuste de parámetros, adaptando la configuración de los algoritmos en función del tamaño y complejidad del problema.

Gracias a este enfoque híbrido, se obtuvieron rutas más cortas y eficientes con menor intervención manual, haciendo el sistema escalable y versátil para distintas instancias del **TSP**. Además, el uso de rangos adaptativos por tamaño del problema garantizó un equilibrio entre precisión y eficiencia computacional.

---

## 🚀 Trabajo futuro

Como línea futura de trabajo, se propone la integración de otros enfoques metaheurísticos híbridos que puedan mejorar la calidad de las soluciones encontradas y reducir el tiempo de cómputo. También sería interesante evaluar el rendimiento del algoritmo propuesto con diferentes tipos de instancias del problema.

Además, se podría explorar la paralelización del algoritmo utilizando técnicas de programación concurrente o programación paralela, con el fin de acelerar el proceso de optimización en instancias de mayor tamaño.

---

## ✅ Consideraciones finales

Este trabajo busca contribuir al estudio y solución del problema **TSP** mediante la implementación de algoritmos bioinspirados.

Se invita a la comunidad a explorar, reutilizar y mejorar el código según sus necesidades.

---

## 👥 Contribuciones

- 🧑‍🏫 **Dr. Edwin Montes Orozco**  
  Director de PT y responsable del acompañamiento académico durante el desarrollo del proyecto.

- 👨‍💻 **Alejandro Martínez Guzmán**  
  Autor del proyecto, encargado del diseño, implementación y documentación del sistema de optimización.

- 🧪 **Jaime López Lara**  
  Colaborador en la ejecución del código y recolección de resultados.

---

## 📝 Licencia

Este proyecto está licenciado bajo los términos de la licencia MIT.  
Consulta el archivo [LICENSE](./LICENSE) para más detalles.
