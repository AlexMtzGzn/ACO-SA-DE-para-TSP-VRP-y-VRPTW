# 🚚 "Resolución del Problema de Rutas Vehiculares con Ventanas de Tiempo Mediante un Algoritmo Híbrido Entre Colonia de Hormigas y Recocido Simulado."

Este proyecto implementa una solución híbrida para el Problema de Rutas de Vehículos con Ventanas de Tiempo (VRPTW), combinando el algoritmo de Optimización por Colonias de Hormigas (**ACO**) con Recocido Simulado (**SA**) como refinador local, y ajustando automáticamente sus parámetros mediante un Algoritmo Evolutivo Diferencial (**DE**).

---

## 🧩 ¿Qué es el VRPTW?

El Problema de Rutas de Vehículos con Ventanas de Tiempo (VRPTW, por sus siglas en inglés _Vehicle Routing Problem with Time Windows_) es una extensión del Problema de Rutas de Vehículos (_VRP_) clásico que añade restricciones temporales.

Consiste en encontrar las rutas óptimas para una flota de vehículos que deben visitar un conjunto de clientes dentro de intervalos de tiempo específicos (ventanas de tiempo), considerando también restricciones de capacidad de carga y la demanda de cada cliente, con el objetivo de minimizar la distancia total recorrida.

---

## 🕒 Restricciones de Ventanas de Tiempo

Cada cliente debe ser visitado dentro de una ventana de tiempo específica:

- **Tiempo más temprano** (earliest time): El vehículo no puede comenzar el servicio antes de este tiempo.
- **Tiempo más tardío** (latest time): El vehículo debe comenzar el servicio a más tardar en este tiempo.

Si un vehículo llega antes del tiempo más temprano, debe esperar. Si no puede llegar antes del tiempo más tardío, la solución no es factible.

---

## 🐜 ¿Qué es ACO (Ant Colony Optimization)?

ACO (Ant Colony Optimization) es una metaheurística inspirada en el comportamiento colectivo de las colonias de hormigas.

En la naturaleza, las hormigas encuentran caminos cortos entre su nido y las fuentes de comida dejando feromona. Cuanto mejor sea el camino (más corto), más feromona se acumulan, y más probable es que otras hormigas lo sigan, reforzando así la solución.

En el VRPTW, simulamos este comportamiento:

- Cada "hormiga" construye una solución recorriendo clientes.
- Las decisiones se toman con base en:
  - **Cantidad de feromona** (lo aprendido)
  - **Visibilidad** (inverso de la distancia)
  - **Factibilidad temporal** (ventanas de tiempo)
- Después de cada iteración, se actualizan las feromonas, favoreciendo los caminos más cortos y temporalmente viables.
- Se respetan las **restricciones de capacidad y tiempo** de cada vehículo.

## 🔥 ¿Qué es el Recocido Simulado (SA)?

El Recocido Simulado (_Simulated Annealing_, SA) es una metaheurística inspirada en el proceso metalúrgico de recocido, donde un metal se calienta y luego se enfría de forma controlada para modificar sus propiedades físicas.

En optimización:

- Inicialmente, **acepta soluciones peores con alta probabilidad** (cuando la temperatura es alta).
- Gradualmente, **se vuelve más selectivo** a medida que la temperatura disminuye (_enfriamiento_).
- Este enfoque permite escapar de óptimos locales y explorar ampliamente el espacio de soluciones.

🔧 En nuestro sistema, **SA toma las rutas generadas por ACO y las refina** mediante pequeñas modificaciones, aceptando temporalmente algunas soluciones subóptimas para potencialmente encontrar mejores soluciones globales.

---

### ⚙️ Descripción de Operadores Utilizados en el Recocido Simulado (SA)

Este algoritmo aplica distintos operadores de vecindad para explorar nuevas soluciones del problema de ruteo de vehículos (VRP_TW). A continuación, se describen los operadores utilizados:

### Operador 1: `opt_2`
- **Descripción:** Elimina dos aristas y reconecta las rutas invirtiendo el segmento intermedio.
- **Ejemplo:**  
  Ruta: `[0, 2, 4, 5, 3, 1, 0]`  
  → Elimina `(4-5)` y `(3-1)`  
  → Resultado: `[0, 2, 4, 3, 5, 1, 0]`



### Operador 2: `or_opt`
- **Descripción:** Mueve uno o más clientes consecutivos a otra posición en la misma o en otra ruta.
- **Ejemplo:**  
  De: `[1, 3, 4, 5]`  
  Mueve `[3,4]` detrás de 5 → `[1, 5, 3, 4]`



### Operador 3: `swap_intra`
- **Descripción:** Intercambia dos clientes dentro de la misma ruta.
- **Ejemplo:**  
  `[0, 1, 2, 3, 0]` → Intercambia 1 y 3 → `[0, 3, 2, 1, 0]`



### Operador 4: `swap_inter`
- **Descripción:** Intercambia clientes entre dos rutas distintas.
- **Ejemplo:**  
  Ruta 1: `[0, 2, 4, 0]`  
  Ruta 2: `[0, 3, 5, 0]`  
  Intercambia 4 y 5 →  
  Ruta 1: `[0, 2, 5, 0]`  
  Ruta 2: `[0, 3, 4, 0]`


### Operador 5: `reinsercion_intra_inter`
- **Descripción:** Reubica un cliente en otra posición dentro de la misma ruta o en otra.
- **Ejemplo:**  
  Ruta 1: `[0, 1, 4, 0]`  
  Ruta 2: `[0, 2, 3, 0]`  
  Mueve 4 a Ruta 2 →  
  Ruta 1: `[0, 1, 0]`  
  Ruta 2: `[0, 2, 3, 4, 0]`


### Operador 6: `opt_2_5`
- **Descripción:** Variante extendida del 2-opt que permite múltiples cortes y reconexiones.
- **Ejemplo:**  
  `[0, 1, 2, 3, 4, 0]` → `[0, 1, 4, 3, 2, 0]`


### Operador 7: `cross_exchange`
- **Descripción:** Intercambia subsecuencias entre dos rutas.
- **Ejemplo:**  
  Ruta 1: `[0, 1, 2, 0]`  
  Ruta 2: `[0, 3, 4, 0]`  
  Intercambia `[1,2]` con `[3,4]` →  
  Ruta 1: `[0, 3, 4, 0]`  
  Ruta 2: `[0, 1, 2, 0]`


### Operador 8: `relocate_chain`
- **Descripción:** Mueve una cadena de clientes (por ejemplo: `[2,3,4]`) a otra posición.
- **Ejemplo:**  
  Ruta 1: `[0, 1, 2, 3, 4, 0]`  
  Ruta 2: `[0, 5, 6, 0]`  
  Mueve `[2,3,4]` a Ruta 2 →  
  Ruta 1: `[0, 1, 0]`  
  Ruta 2: `[0, 5, 6, 2, 3, 4, 0]`


### Observaciones
- Los operadores **1, 2, 3 y 6** son **intra-ruta** (actúan dentro de una sola ruta).
- Los operadores **4, 5, 7 y 8** son **inter-ruta** (actúan entre distintas rutas).
- La elección del operador se adapta dinámicamente según:
  - Tamaño de la instancia (`num_clientes`)
  - Número de vehículos en la solución actual

---

## 🧬 ¿Qué es el Algoritmo Evolutivo Diferencial (DE)?

DE es una técnica de optimización basada en poblaciones. Ideal para problemas continuos y para ajustar parámetros automáticamente.

📌 Se basa en tres operadores:

- **Mutación**: Combinación de soluciones existentes para generar un vector perturbado.

- **Cruzamiento (recombinación)**: Mezcla del individuo mutado con el original.
- **Selección**: Se elige entre el original y el mutado, conservando el que tenga mejor desempeño.

En este proyecto, **DE ajusta automáticamente los parámetros de ACO** (como α, β, ρ, número de hormigas, etc.) para minimizar la distancia total recorrida por los vehículos.

---

## 🧠 ¿Cómo se resolvió el VRPTW?

El enfoque fue **híbrido**, utilizando tres algoritmos colaborativos:

- **ACO** construye rutas factibles para una flota de vehículos.
- **SA** refina las rutas generadas por ACO.
- **DE** ajusta automáticamente los parámetros de ambos algoritmos para mejorar el rendimiento global.

---

## ⚙️ Rango de Parámetros Adaptativos según el Tamaño del Problema

Para lograr una **mejor calibración** de los algoritmos ACO (Ant Colony Optimization) y SA (Simulated Annealing), se definieron **rangos de parámetros adaptativos** en función del número de clientes en la instancia del VRPTW.

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
| `alpha`                  | 2.0    | 3.5    |
| `beta`                   | 2.5    | 4.0    |
| `gamma`                  | 2.5    | 1.0    |
| `rho`                    | 0.3    | 0.5    |
| `número de hormigas`     | 6      | 6      |
| `porcentaje hormigas`    | 0.30   | 0.60   |
| `iteraciones ACO`        | 60     | 80     |
| `temperatura inicial`    | 1200.0 | 1800.0 |
| `temperatura final`      | 0.01   | 0.1    |
| `factor de enfriamiento` | 0.95   | 0.98   |
| `iteraciones SA`         | 80     | 120    |


#### 🔸 Instancia Mediana (`27–51 clientes`)

| Parámetro                | Mínimo | Máximo |
| ------------------------ | ------ | ------ |
| `alpha`                  | 2.5    | 4.0    |
| `beta`                   | 3.5    | 5.0    |
| `gamma`                  | 1.5    | 3.0    |
| `rho`                    | 0.25   | 0.45   |
| `número de hormigas`     | 6      | 12     |
| `porcentaje hormigas`    | 0.20   | 0.50   |
| `iteraciones ACO`        | 100    | 150    |
| `temperatura inicial`    | 1600.0 | 2200.0 |
| `temperatura final`      | 0.005  | 0.05   |
| `factor de enfriamiento` | 0.97   | 0.99   |
| `iteraciones SA`         | 120    | 200    |


#### 🔸 Instancia Grande (`52–101 clientes`)

| Parámetro                | Mínimo | Máximo |
| ------------------------ | ------ | ------ |
| `alpha`                  | 3.0    | 5.0    |
| `beta`                   | 4.0    | 6.0    |
| `gamma`                  | 2.0    | 4.0    |
| `rho`                    | 0.10   | 0.30   |
| `número de hormigas`     | 10     | 20     |
| `porcentaje hormigas`    | 0.15   | 0.35   |
| `iteraciones ACO`        | 120    | 180    |
| `temperatura inicial`    | 2500.0 | 4000.0 |
| `temperatura final`      | 0.001  | 0.01   |
| `factor de enfriamiento` | 0.97   | 0.995  |
| `iteraciones SA`         | 200    | 300    |


### 🧠 ¿Por qué definir rangos diferentes?

Esto permite que el algoritmo DE explore soluciones **más ajustadas al tamaño del problema**, evitando usar configuraciones demasiado pequeñas para instancias grandes, o demasiado costosas para instancias pequeñas. Así se logra un **balance entre calidad de la solución y tiempo de cómputo**.

---

## 🔁 Proceso de Optimización Híbrida (DE + ACO + SA) para VRP

1. **Inicialización con DE**:  
   Se genera aleatoriamente una población inicial de posibles soluciones, donde cada individuo representa un conjunto de parámetros para el algoritmo **ACO** (por ejemplo: α, β, ρ, número de hormigas, número de iteraciones, etc.).

2. **Evaluación de Individuos**:  
   Cada conjunto de parámetros se evalúa ejecutando el algoritmo **ACO** para resolver el **VRPTW**, construyendo rutas factibles que respetan la capacidad de los vehículos y las ventanas de tiempo.

3. **Optimización Local**:  
   Después de que **ACO** genera una solución (rutas), se aplica **Recocido Simulado (SA)** como optimizador local. Este paso consiste en realizar pequeños ajustes en las rutas generadas por **ACO** para mejorar su calidad. **SA** se encarga de explorar soluciones vecinas a la actual (cercanas en el espacio de soluciones) para encontrar una mejor solución local. Durante este proceso, **SA** acepta temporalmente soluciones peores con una probabilidad que disminuye gradualmente a medida que "enfría" su temperatura, permitiendo escapar de óptimos locales.

4. **Cálculo del Fitness**:  
   Se calcula la **distancia total recorrida por todos los vehículos**. Este valor se utiliza como el **fitness** del individuo, penalizando soluciones que excedan la capacidad o que tengan vehículos mal distribuidos.

5. **Evolución con DE**:  
   El algoritmo **DE** utiliza los valores de fitness para evolucionar la población, generando nuevos conjuntos de parámetros con el objetivo de **minimizar la distancia total de la solución del VRPTW**.

6. **Criterio de Paro**:  
   El proceso se repite durante un número máximo de generaciones.

Este proceso permite **optimizar automáticamente** el rendimiento del algoritmo ACO (y SA), **evitando el ajuste manual** de parámetros y encontrando de manera más eficiente soluciones de alta calidad para el **Problema de Ruteo de Vehículos con Ventanas de Tiempo (VRPTW)**.

---

## 🚛 Gestión de Vehículos, Capacidad y Tiempo

La principal diferencia con respecto al VRP clásico es la incorporación de:

- **Restricciones de capacidad**: Cada vehículo tiene una capacidad máxima.
- **Demanda de clientes**: Cada cliente requiere cierta cantidad de producto.
- **Ventanas de tiempo**: Cada cliente debe ser visitado dentro de un intervalo de tiempo específico.
- **Tiempo de servicio**: Cada cliente requiere un tiempo de servicio determinado.
- **Tiempos de viaje**: El tiempo necesario para viajar entre localizaciones.
- **Selección greedy de vehículos**: Cuando un vehículo alcanza su capacidad máxima o no puede cumplir con las ventanas de tiempo restantes, se selecciona el siguiente vehículo disponible.

El algoritmo construye las rutas considerando estas restricciones:

1. **Inicio en el depósito**: Comienza desde el depósito con tiempo inicial 0.

2. **Selección del siguiente cliente**: Basada en una combinación de **feromonas** (experiencia acumulada),**visibilidad** (inverso de la distancia) y **ventana tiempo** (inverso de la ventana final del cliente).

3. **Verificación de capacidad**: Se comprueba si el vehículo puede atender al cliente sin exceder su capacidad.

4. **Verificación de la ventana de tiempo**: Se comprueba si el vehículo puede llegar dentro del intervalo [aᵢ, bᵢ] definido para el cliente, donde:

   - **aᵢ** es el tiempo más temprano para comenzar el servicio, y

   - **bᵢ** es el tiempo más tardío aceptado.

5. **Actualiza el tiempo de llegada**: Si el vehículo llega antes de aᵢ, deberá esperar. Luego se actualiza el tiempo actual sumando el tiempo de espera y el tiempo de servicio del cliente.

6. **Regreso al depósito si no es posible atender**: Si no se puede visitar al cliente actual (por violar la capacidad o la ventana de tiempo), el vehículo regresa al depósito, y se intenta asignar el cliente al siguiente vehículo disponible.

7. **Continuación del proceso**: El procedimiento se repite, seleccionando nuevos clientes factibles hasta que no queden más clientes por atender.

8. **Cambio de vehículo**: Si el cliente no puede ser atendido, el vehículo regresa al depósito y se asigna el siguiente vehículo disponible.

9. **Cobertura total**: El proceso continúa hasta que todos los clientes han sido asignados a una ruta factible.

Este enfoque garantiza que todas las restricciones del problema sean respetadas, generando soluciones viables y eficientes para el VRPTW.

---

## 🎯 Resultados Esperados

El objetivo principal de este proyecto es encontrar las mejores rutas para el **Problema de Rutas de Vehículos con Ventanas de Tiempo (VRPTW)** mediante el uso combinado del algoritmo **ACO** y el algoritmo **DE** para optimizar los parámetros.

### 🔍 ¿Qué se espera como salida?

. **Conjunto de rutas optimizadas**  
   Una solución factible donde todos los clientes son atendidos, respetando las restricciones de capacidad, tiempo y con una distancia total mínima.

2. **Distancia total recorrida**  
   Suma de las distancias de todas las rutas generadas por los vehículos.

3. **Tiempo de ejecución total**: El tiempo total que tomó ejecutar el proceso de optimización y encontrar las mejores rutas.

3. **Utilización de vehículos**: Cantidad de vehículos utilizados y su nivel de ocupación.

5. **Planificación temporal**: Tiempos de llegada, espera y salida para cada cliente.

### 📦 Resultados Generados

3. **Archivo JSON**

   - Se genera un archivo `.json` que contiene todos los parámetros utilizados en la ejecución, tales como:
     - Nombre del archivo de entrada
     - Tiempo de ejecución en minutos
     - Poblacion y Generaciones
     - α (alpha), β (beta), γ (gamma), ρ (rho) 
     - Número de hormigas
     - Número de iteraciones
     - T.ini, T.fin, Factor enfriamiento, Numero de iteraciones
     - Valor de fitness de la solución
     - Flota:
       - ID_Vehiculo
       - Velocidad
       - Capacidad Maxima
       - Capacidad Acumulada
       - Tiempo Salida
       - Tiempo Llegada
       - Venta Inicial
       - Ventana Final
       - Numero Clientes
       - Fitness Vehiculo
       - Rutas generada(listas de clientes)
       - Rutas coordenadas(listas de coordenadas clientes)
       - Tiempos de clientes

4. **Imagen simulada**

   - Se genera una imagen estática (`.png`) que representa visualmente las **rutas generadas** por el algoritmo ACO.
   - Las rutas de diferentes vehículos se representan con colores distintos.
   - Incluye información sobre las ventanas de tiempo de cada cliente.

   Ejemplo de visualización:
   ![Imagen_VRPTW](Recursos_Readme/Ejemplo_png.png)

5. **GIF simulado**

   - Se crea un **GIF animado** que simula el proceso de construcción de las rutas en el tiempo, mostrando cómo los vehículos recorren los clientes respetando las ventanas de tiempo.

   Ejemplo de animación:
   ![Simulacion VRPTW](Recursos_Readme/Ejemplo_gif.gif)

### 💾 Ejemplo de archivo JSON

El archivo `JSON` generado tendrá la siguiente estructura:

```json
{
	"Archivo":	"C104_(25)",
	"Tiempo Ejecucion en Minutos":	2,
	"Poblacion":	3,
	"Generaciones":	5,
	"Alpha":	2.8141023790529474,
	"Beta":	3.8692335087662717,
	"Gamma":	1.247727090375371,
	"Rho":	0.477554173198321,
	"Numero Hormigas":	4,
	"Porcentaje Hormigas":	0.31966146059318512,
	"Numero Iteraciones ACO":	71,
	"Temperatura Inicial":	1783.5621627902437,
	"Temperatura Final":	0.01050384173193194,
	"Factor de Enfriamiento":	0.9645347308574872,
	"Numero Iteraciones SA":	105,
	"Fitness Global":	187.44945538505974,
	"Flota":	[{
			"Id_vehiculo":	1,
			"Capacidad Maxima":	200,
			"Capacidad Acumulada":	150,
			"Tiempo Salida":	0,
			"Tiempo Llegada":	1202.3438485013496,
			"Ventana Inicial":	0,
			"Ventana Final":	1236,
			"Numero Clientes":	10,
			"Fitness Vehiculo":	54.968966901432061,
			"Ruta Clientes":	[0, 7, 8, 11, 9, 6, 4, 2, 1, 3, 5, 0],
			"Ruta Coordenadas":	[{
					"X":	40,
					"Y":	50
				}, {
					"X":	40,
					"Y":	66
				}, {
					"X":	38,
					"Y":	68
				}, {
					"X":	35,
					"Y":	69
				}, {
					"X":	38,
					"Y":	70
				}, {
					"X":	40,
					"Y":	69
				}, {
					"X":	42,
					"Y":	68
				}, {
					"X":	45,
					"Y":	70
				}, {
					"X":	45,
					"Y":	68
				}, {
					"X":	42,
					"Y":	66
				}, {
					"X":	42,
					"Y":	65
				}, {
					"X":	40,
					"Y":	50
				}],
			"Detalles Cliente":	[{
					"Cliente":	"Depósito",
					"Demanda":	0,
					"Ventana Inicial":	0,
					"Ventana Final":	1236,
					"Llegada":	0,
					"Espera":	0,
					"Inicio Servicio":	0,
					"Duración":	0,
					"Salida":	0
				}, {
					"Cliente":	7,
					"Demanda":	20,
					"Ventana Inicial":	0,
					"Ventana Final":	1130,
					"Llegada":	16,
					"Espera":	0,
					"Inicio Servicio":	16,
					"Duración":	90,
					"Salida":	106
				}, {
					"Cliente":	8,
					"Demanda":	20,
					"Ventana Inicial":	255,
					"Ventana Final":	324,
					"Llegada":	108.82842712474618,
					"Espera":	146.17157287525382,
					"Inicio Servicio":	255,
					"Duración":	90,
					"Salida":	345
				}, {
					"Cliente":	11,
					"Demanda":	10,
					"Ventana Inicial":	448,
					"Ventana Final":	505,
					"Llegada":	348.1622776601684,
					"Espera":	99.8377223398316,
					"Inicio Servicio":	448,
					"Duración":	90,
					"Salida":	538
				}, {
					"Cliente":	9,
					"Demanda":	10,
					"Ventana Inicial":	534,
					"Ventana Final":	605,
					"Llegada":	541.16227766016834,
					"Espera":	0,
					"Inicio Servicio":	541.16227766016834,
					"Duración":	90,
					"Salida":	631.16227766016834
				}, {
					"Cliente":	6,
					"Demanda":	20,
					"Ventana Inicial":	0,
					"Ventana Final":	1127,
					"Llegada":	633.39834563766817,
					"Espera":	0,
					"Inicio Servicio":	633.39834563766817,
					"Duración":	90,
					"Salida":	723.39834563766817
				}, {
					"Cliente":	4,
					"Demanda":	10,
					"Ventana Inicial":	727,
					"Ventana Final":	782,
					"Llegada":	725.634413615168,
					"Espera":	1.3655863848319996,
					"Inicio Servicio":	727,
					"Duración":	90,
					"Salida":	817
				}, {
					"Cliente":	2,
					"Demanda":	30,
					"Ventana Inicial":	0,
					"Ventana Final":	1125,
					"Llegada":	820.605551275464,
					"Espera":	0,
					"Inicio Servicio":	820.605551275464,
					"Duración":	90,
					"Salida":	910.605551275464
				}, {
					"Cliente":	1,
					"Demanda":	10,
					"Ventana Inicial":	0,
					"Ventana Final":	1127,
					"Llegada":	912.605551275464,
					"Espera":	0,
					"Inicio Servicio":	912.605551275464,
					"Duración":	90,
					"Salida":	1002.605551275464
				}, {
					"Cliente":	3,
					"Demanda":	10,
					"Ventana Inicial":	0,
					"Ventana Final":	1129,
					"Llegada":	1006.211102550928,
					"Espera":	0,
					"Inicio Servicio":	1006.211102550928,
					"Duración":	90,
					"Salida":	1096.211102550928
				}, {
					"Cliente":	5,
					"Demanda":	10,
					"Ventana Inicial":	0,
					"Ventana Final":	1130,
					"Llegada":	1097.211102550928,
					"Espera":	0,
					"Inicio Servicio":	1097.211102550928,
					"Duración":	90,
					"Salida":	1187.211102550928
				}, {
					"Cliente":	"Depósito",
					"Demanda":	0,
					"Ventana Inicial":	0,
					"Ventana Final":	1236,
					"Llegada":	1202.3438485013496,
					"Espera":	0,
					"Inicio Servicio":	1202.3438485013496,
					"Duración":	0,
					"Salida":	1202.3438485013496
				}]
		}, {
			"Id_vehiculo":	2,
			"Capacidad Maxima":	200,
			"Capacidad Acumulada":	200,
			"Tiempo Salida":	0,
			"Tiempo Llegada":	906.0398087693228,
			"Ventana Inicial":	0,
			"Ventana Final":	1236,
			"Numero Clientes":	9,
			"Fitness Vehiculo":	96.039808769322832,
			"Ruta Clientes":	[0, 13, 17, 18, 19, 15, 16, 14, 12, 10, 0],
			"Ruta Coordenadas":	[{
					"X":	40,
					"Y":	50
				}, {
					"X":	22,
					"Y":	75
				}, {
					"X":	18,
					"Y":	75
				}, {
					"X":	15,
					"Y":	75
				}, {
					"X":	15,
					"Y":	80
				}, {
					"X":	20,
					"Y":	80
				}, {
					"X":	20,
					"Y":	85
				}, {
					"X":	22,
					"Y":	85
				}, {
					"X":	25,
					"Y":	85
				}, {
					"X":	35,
					"Y":	66
				}, {
					"X":	40,
					"Y":	50
				}],
			"Detalles Cliente":	[{
					"Cliente":	"Depósito",
					"Demanda":	0,
					"Ventana Inicial":	0,
					"Ventana Final":	1236,
					"Llegada":	0,
					"Espera":	0,
					"Inicio Servicio":	0,
					"Duración":	0,
					"Salida":	0
				}, {
					"Cliente":	13,
					"Demanda":	30,
					"Ventana Inicial":	30,
					"Ventana Final":	92,
					"Llegada":	30.805843601498726,
					"Espera":	0,
					"Inicio Servicio":	30.805843601498726,
					"Duración":	90,
					"Salida":	120.80584360149872
				}, {
					"Cliente":	17,
					"Demanda":	20,
					"Ventana Inicial":	0,
					"Ventana Final":	1112,
					"Llegada":	124.80584360149872,
					"Espera":	0,
					"Inicio Servicio":	124.80584360149872,
					"Duración":	90,
					"Salida":	214.80584360149874
				}, {
					"Cliente":	18,
					"Demanda":	20,
					"Ventana Inicial":	0,
					"Ventana Final":	1110,
					"Llegada":	217.80584360149874,
					"Espera":	0,
					"Inicio Servicio":	217.80584360149874,
					"Duración":	90,
					"Salida":	307.80584360149874
				}, {
					"Cliente":	19,
					"Demanda":	10,
					"Ventana Inicial":	0,
					"Ventana Final":	1106,
					"Llegada":	312.80584360149874,
					"Espera":	0,
					"Inicio Servicio":	312.80584360149874,
					"Duración":	90,
					"Salida":	402.80584360149874
				}, {
					"Cliente":	15,
					"Demanda":	40,
					"Ventana Inicial":	384,
					"Ventana Final":	429,
					"Llegada":	407.80584360149874,
					"Espera":	0,
					"Inicio Servicio":	407.80584360149874,
					"Duración":	90,
					"Salida":	497.80584360149874
				}, {
					"Cliente":	16,
					"Demanda":	40,
					"Ventana Inicial":	0,
					"Ventana Final":	1105,
					"Llegada":	502.80584360149874,
					"Espera":	0,
					"Inicio Servicio":	502.80584360149874,
					"Duración":	90,
					"Salida":	592.80584360149874
				}, {
					"Cliente":	14,
					"Demanda":	10,
					"Ventana Inicial":	0,
					"Ventana Final":	1106,
					"Llegada":	594.80584360149874,
					"Espera":	0,
					"Inicio Servicio":	594.80584360149874,
					"Duración":	90,
					"Salida":	684.80584360149874
				}, {
					"Cliente":	12,
					"Demanda":	20,
					"Ventana Inicial":	0,
					"Ventana Final":	1107,
					"Llegada":	687.80584360149874,
					"Espera":	0,
					"Inicio Servicio":	687.80584360149874,
					"Duración":	90,
					"Salida":	777.80584360149874
				}, {
					"Cliente":	10,
					"Demanda":	10,
					"Ventana Inicial":	0,
					"Ventana Final":	1129,
					"Llegada":	799.27675415508259,
					"Espera":	0,
					"Inicio Servicio":	799.27675415508259,
					"Duración":	90,
					"Salida":	889.27675415508259
				}, {
					"Cliente":	"Depósito",
					"Demanda":	0,
					"Ventana Inicial":	0,
					"Ventana Final":	1236,
					"Llegada":	906.0398087693228,
					"Espera":	0,
					"Inicio Servicio":	906.0398087693228,
					"Duración":	0,
					"Salida":	906.0398087693228
				}]
		}, {
			"Id_vehiculo":	3,
			"Capacidad Maxima":	200,
			"Capacidad Acumulada":	110,
			"Tiempo Salida":	0,
			"Tiempo Llegada":	1017.1980390271856,
			"Ventana Inicial":	0,
			"Ventana Final":	1236,
			"Numero Clientes":	6,
			"Fitness Vehiculo":	36.440679714304849,
			"Ruta Clientes":	[0, 20, 24, 25, 23, 22, 21, 0],
			"Ruta Coordenadas":	[{
					"X":	40,
					"Y":	50
				}, {
					"X":	30,
					"Y":	50
				}, {
					"X":	25,
					"Y":	50
				}, {
					"X":	25,
					"Y":	52
				}, {
					"X":	28,
					"Y":	55
				}, {
					"X":	28,
					"Y":	52
				}, {
					"X":	30,
					"Y":	52
				}, {
					"X":	40,
					"Y":	50
				}],
			"Detalles Cliente":	[{
					"Cliente":	"Depósito",
					"Demanda":	0,
					"Ventana Inicial":	0,
					"Ventana Final":	1236,
					"Llegada":	0,
					"Espera":	0,
					"Inicio Servicio":	0,
					"Duración":	0,
					"Salida":	0
				}, {
					"Cliente":	20,
					"Demanda":	10,
					"Ventana Inicial":	0,
					"Ventana Final":	1136,
					"Llegada":	10,
					"Espera":	0,
					"Inicio Servicio":	10,
					"Duración":	90,
					"Salida":	100
				}, {
					"Cliente":	24,
					"Demanda":	10,
					"Ventana Inicial":	0,
					"Ventana Final":	1131,
					"Llegada":	105,
					"Espera":	0,
					"Inicio Servicio":	105,
					"Duración":	90,
					"Salida":	195
				}, {
					"Cliente":	25,
					"Demanda":	40,
					"Ventana Inicial":	169,
					"Ventana Final":	224,
					"Llegada":	197,
					"Espera":	0,
					"Inicio Servicio":	197,
					"Duración":	90,
					"Salida":	287
				}, {
					"Cliente":	23,
					"Demanda":	10,
					"Ventana Inicial":	732,
					"Ventana Final":	777,
					"Llegada":	291.24264068711926,
					"Espera":	440.75735931288074,
					"Inicio Servicio":	732,
					"Duración":	90,
					"Salida":	822
				}, {
					"Cliente":	22,
					"Demanda":	20,
					"Ventana Inicial":	0,
					"Ventana Final":	1133,
					"Llegada":	825,
					"Espera":	0,
					"Inicio Servicio":	825,
					"Duración":	90,
					"Salida":	915
				}, {
					"Cliente":	21,
					"Demanda":	20,
					"Ventana Inicial":	0,
					"Ventana Final":	1135,
					"Llegada":	917,
					"Espera":	0,
					"Inicio Servicio":	917,
					"Duración":	90,
					"Salida":	1007
				}, {
					"Cliente":	"Depósito",
					"Demanda":	0,
					"Ventana Inicial":	0,
					"Ventana Final":	1236,
					"Llegada":	1017.1980390271856,
					"Espera":	0,
					"Inicio Servicio":	1017.1980390271856,
					"Duración":	0,
					"Salida":	1017.1980390271856
				}]
		}]
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

👉 [https://github.com/DaveGamble/cJSON](https://github.com/DaveGamble/cJSON)

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
./main 50 100 C101 25
```

- poblacion: el tamaño de la población para el algoritmo.
- generaciones: el número de generaciones que el algoritmo debe ejecutar.
- archivo: el archivo de entrada.
- numero_de_clientes: el número de clientes a considerar en el VRPTW.

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
│   ├── configuracion_vrp_tw.h  
│   ├── control_memoria.h
│   ├── estructuras.h
│   ├── lista_flota.h
│   ├── lista_ruta.h
│   ├── movimientos_sa.h
│   ├── salida_datos.h
│   ├── vrp_tw_aco.h
│   └── vrp_tw_sa.h           
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
│   ├── configuracion_vrp_tw.c  
│   ├── control_memoria.c
│   ├── lista_flota.c
│   ├── lista_ruta.c
│   ├── main.c
│   ├── movimientos_sa.c
│   ├── salida_datos.c
│   ├── vrp_tw_aco.c
│   ├── vrp_tw_sa.c          
│   └── Simulador_VRP_TW/      
│       └── simulador_vrp_tw.py
└── VRP_Solomon/              # Instancias del benchmark Solomon para VRPTW
    ├── VRP_Solomon_25/
    │   ├── C101_(25).txt
    │   ├── C201_(25).txt
    │   ├── R101_(25).txt
    │   ├── R201_(25).txt
    │   ├── RC101_(25).txt
    │   └── RC201_(25).txt
    ├── VRP_Solomon_50/
    │   ├── C101_(50).txt
    │   ├── C201_(50).txt
    │   ├── R101_(50).txt
    │   ├── R201_(50).txt
    │   ├── RC101_(50).txt
    │   └── RC201_(50).txt
    └── VRP_Solomon_100/
        ├── C101_(100).txt
        ├── C201_(100).txt
        ├── R101_(100).txt
        ├── R201_(100).txt
        └── RC101_(100).txt
```
---
## ✅ Consideraciones finales

Este trabajo busca contribuir al estudio y solución del problema VRPTW mediante la implementación de algoritmos bioinspirados. La principal diferencia respecto a la versión VRP clásica es la incorporación de restricciones de ventanas de tiempo, lo que añade una capa de complejidad al problema y requiere una gestión temporal durante la construcción de las rutas. Se invita a la comunidad a explorar, reutilizar y mejorar el código según sus necesidades.

---
## 👥 Contribuciones

- 🧑‍🏫 **Dr. Edwin Montes Orozco**  
  Director de PT y responsable del acompañamiento académico durante el desarrollo del proyecto.

- 👨‍💻 **Alejandro Martínez Guzmán**  
  Autor del proyecto. Encargado del diseño, implementación y documentación del sistema de optimización.

- 🧪 **Jaime López Lara**  
  Ayudante en la ejecución del código y recolección de resultados.

---
## 📝 Licencia

Este proyecto está licenciado bajo los términos de la licencia MIT.  
Consulta el archivo [LICENSE](./LICENSE) para más detalles.
