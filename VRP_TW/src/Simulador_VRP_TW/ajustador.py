import json
import os
import sys
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
from matplotlib.patches import Circle

class VRPSimulator:
    def __init__(self, instancia, numero_instancia, numero_clientes):
        """
        Args:
            instancia: Nombre de la instancia (ej: "C101")
            numero_instancia: Número de la instancia (ej: 4)
            numero_clientes: Número de clientes (ej: 100)
        """
        self.instancia = instancia
        self.numero_instancia = numero_instancia
        self.numero_clientes = numero_clientes
        
        # Construir el nombre del archivo y la ruta
        self.directorio = instancia  # Solo el nombre de la instancia (ej: "C101")
        self.filename = f"{instancia}_({numero_clientes})_{numero_instancia}"
        self.ruta_archivo = f"../../Resultados/Resultados_{numero_clientes}/Json/{self.directorio}/{self.filename}.json"
        
        # Verificar si el archivo existe
        if not os.path.exists(self.ruta_archivo):
            raise FileNotFoundError(f"No se encontró el archivo: {self.ruta_archivo}")
        
        # Cargar los datos desde el archivo JSON
        with open(self.ruta_archivo, 'r') as file:
            self.data = json.load(file)
        
        print(f"✓ Archivo cargado: {self.ruta_archivo}")
        
        # Colores para cada ruta
        self.colors = plt.cm.tab10(np.linspace(0, 1, len(self.data['Flota'])))
    
    def plot_static_routes(self):
        """Dibuja todas las rutas de forma estática"""
        fig, ax = plt.subplots(figsize=(12, 10))
        
        # Dibujar cada ruta
        for i, vehiculo in enumerate(self.data['Flota']):
            color = self.colors[i]
            ruta = vehiculo['Ruta Coordenadas']
            
            # Extraer coordenadas
            x = [punto['X'] for punto in ruta]
            y = [punto['Y'] for punto in ruta]
            
            # Dibujar ruta
            ax.plot(x, y, '-o', color=color, markersize=5, label=f"Vehículo {vehiculo['Id_vehiculo']}")
            
            # Dibujar el depósito con un símbolo especial
            ax.plot(ruta[0]['X'], ruta[0]['Y'], 'ks', markersize=8)
        
        # Dibujar todos los clientes como puntos y etiquetas
        clientes_x = []
        clientes_y = []
        clientes_id = []
        cliente_counter = 1
        
        for vehiculo in self.data['Flota']:
            ruta = vehiculo['Ruta Coordenadas']
            for j, punto in enumerate(ruta[1:-1]):  # Excluir el depósito y retorno
                clientes_x.append(punto['X'])
                clientes_y.append(punto['Y'])
                if 'Ruta Clientes' in vehiculo and j + 1 < len(vehiculo['Ruta Clientes']):
                    clientes_id.append(vehiculo['Ruta Clientes'][j+1])  # +1 porque el índice 0 es el depósito
                else:
                    clientes_id.append(cliente_counter)
                    cliente_counter += 1
        
        # Dibujar puntos de clientes
        ax.scatter(clientes_x, clientes_y, c='black', s=20, alpha=0.5)
        
        # Añadir etiquetas de cliente
        for i in range(len(clientes_x)):
            ax.annotate(str(clientes_id[i]), (clientes_x[i], clientes_y[i]), 
                        xytext=(3, 3), textcoords='offset points', fontsize=8)
        
        # Añadir etiqueta al depósito
        depot_x = self.data['Flota'][0]['Ruta Coordenadas'][0]['X']
        depot_y = self.data['Flota'][0]['Ruta Coordenadas'][0]['Y']
        ax.annotate('Depósito', (depot_x, depot_y), 
                    xytext=(5, 5), textcoords='offset points', fontsize=12)
        
        # Configurar el gráfico con el nombre del archivo en el título
        ax.set_xlabel('Coordenada X')
        ax.set_ylabel('Coordenada Y')
        ax.set_title(f'Rutas de los Vehículos - {self.filename} ({self.numero_clientes} clientes)')
        ax.legend(loc='upper right', bbox_to_anchor=(1.1, 1))
        ax.grid(True)
        
        # Guardar el gráfico
        plt.tight_layout()
        output_dir = f'../../Resultados/Resultados_{self.numero_clientes}/Imagenes/{self.directorio}'
        os.makedirs(output_dir, exist_ok=True)
        output_path = f'{output_dir}/{self.filename}.png'
        plt.savefig(output_path)
        print(f"✓ Imagen guardada: {output_path}")
        plt.close()
    
    def animate_routes(self):
        """Anima la simulación de rutas"""
        fig, ax = plt.subplots(figsize=(12, 10))
        
        # Almacena las líneas y vehículos para la animación
        route_lines = []
        vehicles = []
        
        # Información de cada ruta
        max_steps = 0
        all_routes = []
        
        # Preparar los datos para la animación
        for i, vehiculo in enumerate(self.data['Flota']):
            ruta = vehiculo['Ruta Coordenadas']
            
            # Extraer coordenadas
            x = [punto['X'] for punto in ruta]
            y = [punto['Y'] for punto in ruta]
            
            # Crear línea para la ruta
            line, = ax.plot([], [], '-', color=self.colors[i], alpha=0.6, 
                          label=f"Vehículo {vehiculo['Id_vehiculo']}")
            route_lines.append(line)
            
            # Crear marcador para el vehículo
            vehicle = Circle((x[0], y[0]), 1.5, color=self.colors[i], zorder=3)
            ax.add_patch(vehicle)
            vehicles.append(vehicle)
            
            # Guardar la ruta
            all_routes.append((x, y))
            
            # Actualizar el máximo número de pasos
            max_steps = max(max_steps, len(x))
        
        # Dibujar el depósito
        depot_x = self.data['Flota'][0]['Ruta Coordenadas'][0]['X']
        depot_y = self.data['Flota'][0]['Ruta Coordenadas'][0]['Y']
        ax.plot(depot_x, depot_y, 'ks', markersize=8)
        ax.annotate('Depósito', (depot_x, depot_y), 
                    xytext=(5, 5), textcoords='offset points', fontsize=12)
        
        # Dibujar todos los puntos de clientes
        clientes_x = []
        clientes_y = []
        clientes_id = []
        cliente_counter = 1
        
        for vehiculo in self.data['Flota']:
            for j, punto in enumerate(vehiculo['Ruta Coordenadas'][1:-1]):  # Excluir el depósito y el punto final
                clientes_x.append(punto['X'])
                clientes_y.append(punto['Y'])
                # Aquí asumimos que necesitamos asignar IDs a los clientes
                # Usamos el valor de Ruta Clientes o un contador si no está disponible
                if 'Ruta Clientes' in vehiculo and j < len(vehiculo['Ruta Clientes']) - 2:
                    clientes_id.append(vehiculo['Ruta Clientes'][j+1])  # +1 porque el depósito es el índice 0
                else:
                    clientes_id.append(cliente_counter)
                    cliente_counter += 1
        
        ax.scatter(clientes_x, clientes_y, c='black', s=20, alpha=0.5)
        
        # Añadir etiquetas a los clientes
        for i in range(len(clientes_x)):
            ax.annotate(str(clientes_id[i]), (clientes_x[i], clientes_y[i]), 
                       xytext=(3, 3), textcoords='offset points', fontsize=8)
        
        # Configuración del gráfico con el nombre del archivo en el título
        ax.set_xlabel('Coordenada X')
        ax.set_ylabel('Coordenada Y')
        ax.set_title(f'Simulación de Rutas de Vehículos TW - {self.filename} ({self.numero_clientes} clientes)')
        
        # Determinar los límites del gráfico
        x_min = min(min(clientes_x), depot_x) - 5 if clientes_x else depot_x - 5
        x_max = max(max(clientes_x), depot_x) + 5 if clientes_x else depot_x + 5
        y_min = min(min(clientes_y), depot_y) - 5 if clientes_y else depot_y - 5
        y_max = max(max(clientes_y), depot_y) + 5 if clientes_y else depot_y + 5
        
        ax.set_xlim(x_min, x_max)
        ax.set_ylim(y_min, y_max)
        ax.grid(True)
        ax.legend(loc='upper right', bbox_to_anchor=(1.1, 1))
        
        # Información de la simulación
        info_text = ax.text(0.02, 0.98, '', transform=ax.transAxes, fontsize=9,
                          verticalalignment='top', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
        
        # Función para inicializar la animación
        def init():
            for line in route_lines:
                line.set_data([], [])
            return route_lines + vehicles + [info_text]
        
        # Función para actualizar la animación en cada paso
        def update(frame):
            frame_info = f"Paso: {frame+1}/{max_steps}\n"
            
            for i, ((x, y), line, vehicle) in enumerate(zip(all_routes, route_lines, vehicles)):
                if frame < len(x):
                    # Actualizar la línea
                    line.set_data(x[:frame+1], y[:frame+1])
                    
                    # Actualizar la posición del vehículo
                    vehicle.center = (x[frame], y[frame])
                    
                    # Añadir información del vehículo
                    vehiculo = self.data['Flota'][i]
                    if frame < len(x) - 1:
                        # Intentar obtener información del cliente actual y siguiente
                        if 'Ruta Clientes' in vehiculo and frame < len(vehiculo['Ruta Clientes']):
                            cliente_actual = vehiculo['Ruta Clientes'][frame]
                            cliente_siguiente = vehiculo['Ruta Clientes'][min(frame+1, len(vehiculo['Ruta Clientes'])-1)]
                        else:
                            # Si no tenemos Ruta Clientes, usamos índices
                            cliente_actual = frame
                            cliente_siguiente = min(frame+1, len(x)-1)
                        
                        frame_info += f"Vehículo {vehiculo['Id_vehiculo']}: Cliente {cliente_actual} → {cliente_siguiente}\n"
            
            info_text.set_text(frame_info)
            return route_lines + vehicles + [info_text]
        
        # Crear la animación
        ani = animation.FuncAnimation(fig, update, frames=max_steps,
                                     init_func=init, blit=True, interval=200)
        
        plt.tight_layout()
        
        # Guardar la animación como GIF
        output_dir = f'../../Resultados/Resultados_{self.numero_clientes}/Gifs/{self.directorio}'
        os.makedirs(output_dir, exist_ok=True)
        output_path = f'{output_dir}/{self.filename}.gif'
        ani.save(output_path, writer='pillow', fps=5)
        print(f"✓ GIF guardado: {output_path}")
        plt.close()

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Uso: python simulador_vrp_tw.py <instancia> <numero_instancia> <numero_clientes>")
        print("Ejemplo: python simulador_vrp_tw.py C101 4 100")
        sys.exit(1)
    
    instancia = sys.argv[1]           # ej: "C101"
    numero_instancia = sys.argv[2]    # ej: "4"
    numero_clientes = sys.argv[3]     # ej: "100"
    
    try:
        # Crear simulador
        simulator = VRPSimulator(instancia, numero_instancia, numero_clientes)
        
        print(f"\n🚀 Generando visualizaciones para {instancia}_({numero_clientes})_{numero_instancia}...\n")
        
        # Dibujar las rutas estáticas
        simulator.plot_static_routes()
        
        # Animar las rutas
        simulator.animate_routes()
        
        print(f"\n✅ Proceso completado exitosamente!")
        
    except FileNotFoundError as e:
        print(f"❌ Error: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"❌ Error inesperado: {e}")
        sys.exit(1)