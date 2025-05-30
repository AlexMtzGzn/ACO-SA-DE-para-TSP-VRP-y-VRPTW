# 🚚 "Optimización del Problema de Ruteo de Vehículos (VRP) mediante una Metaheurística Híbrida ACO-SA con Calibración de Parámetros por Evolución Diferencial"

Este proyecto implementa una solución híbrida para el Problema de Ruteo de Vehículos (**VRP**), combinando el algoritmo de Optimización por Colonias de Hormigas (**ACO**) con Recocido Simulado (**SA**) como refinador local, y ajustando automáticamente sus parámetros mediante un Algoritmo Evolutivo Diferencial (**DE**).

---

## 🧩 ¿Qué es el VRP?

El Problema de Ruteo de Vehículos (**VRP**, por sus siglas en inglés _Vehicle Routing Problem_) es una extensión del clásico Problema del Agente Viajero (_TSP_), y representa uno de los desafíos más relevantes en logística y distribución.

El objetivo es encontrar las rutas óptimas para una flota de vehículos que deben atender a un conjunto de clientes, considerando restricciones como la capacidad de carga de cada vehículo y la demanda de cada cliente. La meta principal es **minimizar la distancia total recorrida**.

---

## 🐜 ¿Qué es ACO (Ant Colony Optimization)?

**ACO** (_Ant Colony Optimization_) es una metaheurística inspirada en el comportamiento colectivo de las colonias de hormigas.

En la naturaleza, las hormigas encuentran caminos cortos entre su nido y las fuentes de alimento dejando feromona en el trayecto. Cuanto mejor es el camino (más corto), más feromona se acumulan, lo que aumenta la probabilidad de que otras hormigas lo sigan, reforzando así la solución.

En el VRP, simulamos este comportamiento:

- Cada _hormiga_ construye una solución recorriendo los clientes.
- Las decisiones se toman en función de:
  - **Cantidad de feromona** (conocimiento aprendido).
  - **Visibilidad** (inverso de la distancia entre nodos).
- Después de cada iteración, se actualizan las feromonas favoreciendo los caminos más prometedores.
- Se respetan las **restricciones de capacidad** de cada vehículo.

---

## 🔥 ¿Qué es el Recocido Simulado (SA)?

El Recocido Simulado (_Simulated Annealing_, **SA**) es una metaheurística inspirada en el proceso metalúrgico de recocido, donde un metal se calienta y luego se enfría de forma controlada para modificar sus propiedades físicas.

En optimización:

- Inicialmente, **acepta soluciones peores con alta probabilidad** (cuando la temperatura es alta).
- Gradualmente, **se vuelve más selectivo** a medida que la temperatura disminuye (_enfriamiento_).
- Este enfoque permite escapar de óptimos locales y explorar ampliamente el espacio de soluciones.

🔧 En nuestro sistema, **SA toma las rutas generadas por ACO y las refina** mediante pequeñas modificaciones, aceptando temporalmente algunas soluciones subóptimas para potencialmente encontrar mejores soluciones globales.

---

## 🔄 Movimientos de Vecindad del Recocido Simulado (SA)

Durante la optimización local con **SA**, se generan **soluciones vecinas** a partir de la solución actual mediante uno de los siguientes tres movimientos aleatorios:

1. **Mover un cliente entre vehículos:**  
   Se selecciona un cliente de un vehículo y se lo asigna a otro vehículo (si es factible en capacidad). Este movimiento permite explorar cambios importantes en la asignación general de la flota.

2. **Inversión de un segmento de ruta:**  
   Similar al movimiento clásico de 2-opt. Se invierte el orden de visita entre dos puntos dentro de una misma ruta, buscando reducir cruces o trayectos ineficientes.

3. **Mover dos clientes entre vehículos:**  
   Se seleccionan dos clientes en rutas distintas y se intercambian entre sí, siempre que las restricciones de capacidad lo permitan. Este movimiento puede generar reconfiguraciones más grandes y efectivas.

La elección del movimiento se realiza aleatoriamente con igual probabilidad, usando el siguiente criterio:

```bash
if (prob < factor / 3.0)
    aceptado = mover_cliente_vehiculo(...);
else if (prob < 2.0 * factor / 3.0)
    aceptado = invertir__segmento_ruta(...);
else
    aceptado = mover_dos_clientes_vehiculos(...);
```

Donde prob es un número aleatorio entre 0 y 1, y factor es calibrado por **DE**.

Este conjunto de movimientos permite que **SA** explore diversas configuraciones vecinas, ayudando a escapar de óptimos locales y mejorando la calidad de las rutas generadas por **ACO**.

---

## 🧬 ¿Qué es el Algoritmo Evolutivo Diferencial (DE)?

DE es una técnica de optimización basada en poblaciones, ideal para problemas de parámetros continuos y para el ajuste automático de hiperparámetros.

📌 Se basa en tres operadores clave:

- **Mutación**: Combinación de soluciones existentes para generar un vector perturbado.
- **Cruzamiento (recombinación)**: Mezcla del individuo mutado con el original.
- **Selección**: Se elige entre el original y el mutado, conservando el que tenga mejor desempeño.

En este proyecto, **DE ajusta automáticamente los parámetros de ACO** (como α, β, ρ, número de hormigas, etc.) para minimizar la distancia total recorrida por los vehículos.

---

## 🧠 ¿Cómo se resolvió el VRP?

El enfoque fue **híbrido**, utilizando tres algoritmos colaborativos:

- **ACO** construye rutas factibles para una flota de vehículos.
- **SA** refina las rutas generadas por ACO.
- **DE** ajusta automáticamente los parámetros de ambos algoritmos para mejorar el rendimiento global.

---

## ⚙️ Rango de Parámetros Adaptativos según el Tamaño del Problema

Para lograr una **mejor calibración** de los algoritmos ACO (Ant Colony Optimization) y SA (Simulated Annealing), se definieron **rangos de parámetros adaptativos** en función del número de clientes en la instancia del **VRP**.

Esto permite que los algoritmos se ajusten de forma dinámica, dependiendo de la **complejidad del problema** (tamaño de la instancia).

### 🔢 Tamaños de instancia considerados

| Tamaño del problema | Número de clientes (`vrp->num_clientes`) |
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
| `rho`                    | 0.4    | 0.6    |
| `número de hormigas`     | 10     | 25     |
| `iteraciones ACO`        | 50     | 100    |
| `temperatura inicial`    | 1000.0 | 2000.0 |
| `temperatura final`      | 0.1    | 0.3    |
| `factor de enfriamiento` | 0.95   | 0.999  |
| `factor de control`      | 0.7    | 0.5    |
| `iteraciones SA`         | 100    | 150    |

#### 🔸 Instancia Mediana (`50 clientes`)

| Parámetro                | Mínimo | Máximo |
| ------------------------ | ------ | ------ |
| `alpha`                  | 2.0    | 4.0    |
| `beta`                   | 3.0    | 5.0    |
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
| `factor de control`      | 0.7    | 0.9    |
| `iteraciones SA`         | 200    | 300    |

### 🧠 ¿Por qué definir rangos diferentes?

Esto permite que el algoritmo DE explore soluciones **más ajustadas al tamaño del problema**, evitando usar configuraciones demasiado pequeñas para instancias grandes, o demasiado costosas para instancias pequeñas. Así se logra un **balance entre calidad de la solución y tiempo de cómputo**.

---

## 🔁 Proceso de Optimización Híbrida (DE + ACO + SA) para VRP

1. **Inicialización con DE**:  
   Se genera aleatoriamente una población inicial de posibles soluciones, donde cada individuo representa un conjunto de parámetros para el algoritmo **ACO** (por ejemplo: α, β, ρ, número de hormigas, número de iteraciones, etc.).

2. **Evaluación de Individuos**:  
   Cada conjunto de parámetros se evalúa ejecutando el algoritmo **ACO** para resolver el **VRP**, construyendo rutas factibles que respetan la capacidad de los vehículos.

3. **Optimización Local**:  
   Después de que **ACO** genera una solución (rutas), se aplica **Recocido Simulado (SA)** como optimizador local. Este paso consiste en realizar pequeños ajustes en las rutas generadas por **ACO** para mejorar su calidad. **SA** se encarga de explorar soluciones vecinas a la actual (cercanas en el espacio de soluciones) para encontrar una mejor solución local. Durante este proceso, **SA** acepta temporalmente soluciones peores con una probabilidad que disminuye gradualmente a medida que "enfría" su temperatura, permitiendo escapar de óptimos locales.

4. **Cálculo del Fitness**:  
   Se calcula la **distancia total recorrida por todos los vehículos**. Este valor se utiliza como el **fitness** del individuo, penalizando soluciones que excedan la capacidad o que tengan vehículos mal distribuidos.

5. **Evolución con DE**:  
   El algoritmo **DE** utiliza los valores de fitness para evolucionar la población, generando nuevos conjuntos de parámetros con el objetivo de **minimizar la distancia total de la solución del VRP**.

6. **Criterio de Paro**:  
   El proceso se repite durante un número máximo de generaciones.

Este proceso permite **optimizar automáticamente** el rendimiento del algoritmo **ACO** y **SA**, **evitando el ajuste manual** de parámetros y encontrando de manera más eficiente soluciones de alta calidad para el **Problema de Ruteo de Vehículos (VRP)**.

---

## 🚛 Gestión de Vehículos y Capacidad en el VRP

A diferencia del **TSP**, el **VRP** introduce restricciones adicionales que hacen más compleja la construcción de rutas:

- **Capacidad de los vehículos**: Cada vehículo tiene una capacidad máxima que no puede ser superada.
- **Demanda de los clientes**: Cada cliente requiere una cantidad específica de producto.
- **Gestión secuencial de vehículos**: Al agotarse la capacidad de un vehículo, se selecciona el siguiente disponible de manera **greedy**.

El algoritmo construye rutas de la siguiente forma:

1. **Inicio en el depósito**: Cada vehículo parte desde el depósito.
2. **Selección del siguiente cliente**: Basada en una combinación de **feromonas** (experiencia acumulada) y **visibilidad** (inverso de la distancia).
3. **Verificación de capacidad**: Se comprueba si el vehículo puede atender al cliente sin exceder su capacidad.
4. **Cambio de vehículo**: Si el cliente no puede ser atendido, el vehículo regresa al depósito y se asigna el siguiente vehículo disponible.
5. **Cobertura total**: El proceso continúa hasta que todos los clientes han sido asignados a una ruta factible.

Este enfoque garantiza que todas las restricciones del problema sean respetadas, generando soluciones viables y eficientes para el **VRP**.

---

## 🎯 Resultados Esperados

El objetivo principal de este proyecto es encontrar la mejor solución al **Problema de Ruteo de Vehículos (VRP)** utilizando un enfoque híbrido con los algoritmos **ACO**, **SA** y **DE**.  
El algoritmo **DE** se encarga de optimizar automáticamente los parámetros de **ACO** y **SA**, adaptándose al tamaño y complejidad de la instancia.

### 🔍 ¿Qué se espera como salida?

1. **Conjunto de rutas optimizadas**  
   Una solución factible donde todos los clientes son atendidos, respetando las restricciones de capacidad, y con una distancia total mínima.

2. **Distancia total recorrida**  
   Suma de las distancias de todas las rutas generadas por los vehículos.

3. **Tiempo de ejecución total**  
   Tiempo que toma ejecutar el proceso completo de optimización, incluyendo la calibración de parámetros y construcción/refinamiento de rutas.

4. **Parámetros óptimos encontrados**  
   Valores de α, β, ρ, temperatura, número de hormigas, iteraciones, etc., que generaron la mejor solución en la instancia evaluada.

### 📦 Resultados Generados

3. **Archivo JSON**

   - Se genera un archivo `.json` que contiene todos los **parámetros optimizados automáticamente** durante la ejecución, incluyendo:
     - Nombre del archivo de entrada
     - Tiempo de ejecución en minutos
     - Tamaño de población y número de generaciones del DE
     - Parámetros de **ACO** (`α`, `β`, `ρ`, número de hormigas, iteraciones ACO)
     - Parámetros de **SA** (temperatura inicial, final, factor de enfriamiento, factor de control, iteraciones **SA**)
     - Valor de fitness de la solución (distancia total recorrida)
     - Conjunto de rutas generadas (lista de clientes visitados por cada vehículo)

4. **GIF simulado**

   - Se crea un **GIF animado** que simula el proceso de construcción de las rutas, mostrando cómo cada vehículo va atendiendo clientes, según el proceso de decisión de la hormiga.

   Ejemplo de animación:  
   ![Simulador Ruta](Recursos_Readme/Ejemplo_gif.gif)

5. **Imagen simulada**

   - Se genera una imagen estática (`.png`) que representa visualmente el **conjunto de rutas** recorridas por los vehículos, partiendo y regresando al depósito.

   Ejemplo de visualización:  
   ![Imagen Ruta](Recursos_Readme/Ejemplo_png.png)



### 💾 Ejemplo de archivo JSON

El archivo `JSON` generado tendrá la siguiente estructura:

```json
{
  "Archivo": "C100_(25)",
  "Tiempo Ejecucion en Minutos": 3,
  "Poblacion: ": 10,
  "Generaciones: ": 10,
  "Alpha": 1.5352719139937645,
  "Beta": 2.0948502659308028,
  "Rho": 0.4791237080838176,
  "Numero Hormigas": 16,
  "Numero Iteraciones ACO": 77,
  "Temperatura Inicial: ": 1554.6596225139963,
  "Temperatura Final: ": 0.14210512518980778,
  "Factor de Enfriamiento: ": 0.998101131701889,
  "Factor de Control: ": 0.57017857575331743,
  "Numero Iteraciones SA: ": 130,
  "Fitness Global": 187.44945538505971,
  "Vehiculos Necesarios": "3/25",
  "flota": [
    {
      "Id_vehiculo": 1,
      "Capacidad Maxima": 200,
      "Capacidad Acumulada": 110,
      "Numero Clientes": 6,
      "Fitness Vehiculo": 36.440679714304849,
      "Ruta Clientes": [0, 20, 24, 25, 23, 22, 21, 0],
      "Ruta Coordenadas": [
        {
          "Capacidad": 0,
          "X": 40,
          "Y": 50
        },
        {
          "Capacidad": 10,
          "X": 30,
          "Y": 50
        },
        {
          "Capacidad": 10,
          "X": 25,
          "Y": 50
        },
        {
          "Capacidad": 40,
          "X": 25,
          "Y": 52
        },
        {
          "Capacidad": 10,
          "X": 28,
          "Y": 55
        },
        {
          "Capacidad": 20,
          "X": 28,
          "Y": 52
        },
        {
          "Capacidad": 20,
          "X": 30,
          "Y": 52
        },
        {
          "Capacidad": 0,
          "X": 40,
          "Y": 50
        }
      ]
    },
    {
      "Id_vehiculo": 2,
      "Capacidad Maxima": 200,
      "Capacidad Acumulada": 150,
      "Numero Clientes": 10,
      "Fitness Vehiculo": 54.968966901432061,
      "Ruta Clientes": [0, 7, 8, 11, 9, 6, 4, 2, 1, 3, 5, 0],
      "Ruta Coordenadas": [
        {
          "Capacidad": 0,
          "X": 40,
          "Y": 50
        },
        {
          "Capacidad": 20,
          "X": 40,
          "Y": 66
        },
        {
          "Capacidad": 20,
          "X": 38,
          "Y": 68
        },
        {
          "Capacidad": 10,
          "X": 35,
          "Y": 69
        },
        {
          "Capacidad": 10,
          "X": 38,
          "Y": 70
        },
        {
          "Capacidad": 20,
          "X": 40,
          "Y": 69
        },
        {
          "Capacidad": 10,
          "X": 42,
          "Y": 68
        },
        {
          "Capacidad": 30,
          "X": 45,
          "Y": 70
        },
        {
          "Capacidad": 10,
          "X": 45,
          "Y": 68
        },
        {
          "Capacidad": 10,
          "X": 42,
          "Y": 66
        },
        {
          "Capacidad": 10,
          "X": 42,
          "Y": 65
        },
        {
          "Capacidad": 0,
          "X": 40,
          "Y": 50
        }
      ]
    },
    {
      "Id_vehiculo": 3,
      "Capacidad Maxima": 200,
      "Capacidad Acumulada": 200,
      "Numero Clientes": 9,
      "Fitness Vehiculo": 96.0398087693228,
      "Ruta Clientes": [0, 10, 12, 14, 16, 15, 19, 18, 17, 13, 0],
      "Ruta Coordenadas": [
        {
          "Capacidad": 0,
          "X": 40,
          "Y": 50
        },
        {
          "Capacidad": 10,
          "X": 35,
          "Y": 66
        },
        {
          "Capacidad": 20,
          "X": 25,
          "Y": 85
        },
        {
          "Capacidad": 10,
          "X": 22,
          "Y": 85
        },
        {
          "Capacidad": 40,
          "X": 20,
          "Y": 85
        },
        {
          "Capacidad": 40,
          "X": 20,
          "Y": 80
        },
        {
          "Capacidad": 10,
          "X": 15,
          "Y": 80
        },
        {
          "Capacidad": 20,
          "X": 15,
          "Y": 75
        },
        {
          "Capacidad": 20,
          "X": 18,
          "Y": 75
        },
        {
          "Capacidad": 30,
          "X": 22,
          "Y": 75
        },
        {
          "Capacidad": 0,
          "X": 40,
          "Y": 50
        }
      ]
    }
  ]
}
```

---

## Requisitos

Para ejecutar este proyecto, asegúrate de tener lo siguiente:

### 🧑‍💻 C Compiler

Es necesario tener un compilador de C instalado (como gcc) para compilar el código fuente.

### Librería `cJSON`:

Este proyecto requiere la librería `cJSON` para trabajar con archivos JSON en C.  
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

Este comando compilará el código en modo release por defecto (optimizado). Si prefieres compilar en modo debug para facilitar la depuración, puedes usar:

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
./main 50 100 RC100 25
```

- poblacion: el tamaño de la población para el algoritmo.
- generaciones: el número de generaciones que el algoritmo debe ejecutar.
- archivo: el archivo de entrada.
- numero_de_clientes: el número de clientes a considerar en el VRP.

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
│   ├── configuracion_vrp.h
│   ├── control_memoria.h
│   ├── estructuras.h
│   ├── lista_flota.h
│   ├── lista_ruta.h
│   ├── salida_datos.h
│   ├── vrp_aco.h
│   └── vrp_sa.h
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
│   ├── configuracion_vrp.c
│   ├── control_memoria.c
│   ├── lista_flota.c
│   ├── lista_ruta.c
│   ├── main.c
│   ├── salida_datos.c
│   ├── vrp_aco.c
│   ├── vrp_sa.c
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

---

## ✅ Conclusión

Este proyecto presentó una solución híbrida al Problema de Ruteo de Vehículos (**VRP**), integrando las fortalezas de tres algoritmos metaheurísticos: **ACO** para la construcción de rutas, SA como optimizador local y DE como calibrador automático de parámetros. La combinación permitió generar rutas eficientes que respetan las restricciones del problema, al mismo tiempo que se optimizaban automáticamente los hiperparámetros involucrados.

Gracias al uso de rangos adaptativos de parámetros según el tamaño del problema, se logró un equilibrio entre calidad de la solución y eficiencia computacional, permitiendo que el sistema sea escalable a distintas instancias del **VRP**.

Los resultados obtenidos evidencian que la integración de **ACO** con **SA** mejora la calidad de las rutas mediante refinamiento local, mientras que DE contribuye significativamente a la exploración del espacio de configuraciones óptimas, reduciendo la necesidad de ajuste manual.

En conjunto, este enfoque demostró ser una alternativa robusta y flexible para abordar problemas de ruteo complejos en logística, con potencial de ser aplicado o extendido a otras variantes del **VRP** o a escenarios reales.

---

## 🚀 Trabajo futuro

Como línea futura de trabajo, se propone la integración de otros enfoques metaheurísticos híbridos que puedan mejorar la calidad de las soluciones encontradas y reducir el tiempo de cómputo. También sería interesante evaluar el rendimiento del algoritmo propuesto con diferentes tipos de instancias del problema, incluyendo aquellas con restricciones más complejas como ventanas de tiempo o múltiples depósitos.

Además, se podría explorar la paralelización del algoritmo utilizando técnicas de programación concurrente o programación paralela, con el fin de acelerar el proceso de optimización en instancias de mayor tamaño.

---

## ✅ Consideraciones finales

Este trabajo busca contribuir al estudio y solución del problema **VRP** mediante la implementación de algoritmos bioinspirados. La principal diferencia respecto a la versión **TSP** es la incorporación de restricciones de capacidad y la selección greedy de vehículos. Se invita a la comunidad a explorar, reutilizar y mejorar el código según sus necesidades.

---

## 👥 Contribuciones

- 🧑‍🏫 **Dr. Edwin Montes Orozco**  
  Director de PT y responsable del acompañamiento académico durante el desarrollo del proyecto.

- 👨‍💻 **Alejandro Martínez Guzmán**  
  Autor del proyecto. Encargado del diseño, implementación y documentación del sistema de optimización.

- 🧪 **Jaime López Lara**  
  Colaborador en la ejecución del código y recolección de resultados.

---

## 📝 Licencia

Este proyecto está licenciado bajo los términos de la licencia MIT.  
Consulta el archivo [LICENSE](./LICENSE) para más detalles.
