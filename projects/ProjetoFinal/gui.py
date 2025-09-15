import sys
import math
from collections import deque
from functools import partial
from PyQt5.QtWidgets import (
    QApplication, QWidget, QVBoxLayout, QHBoxLayout, QLabel,
    QGroupBox, QFormLayout, QSpinBox, QPushButton,
    QMainWindow, QAction, QComboBox, QSizePolicy, QStackedWidget, QGridLayout, QListWidget, QListWidgetItem, QPlainTextEdit, QDialog, QSizePolicy
)
from PyQt5.QtGui import QPainter, QColor, QFont, QPen, QBrush, QRadialGradient, QPolygonF
from PyQt5.QtCore import QPointF, QRectF, Qt, QTimer, QThread, pyqtSignal, pyqtSlot, QObject
import serial
import pyqtgraph as pg
import warnings

from datetime import datetime


warnings.filterwarnings("ignore", category=DeprecationWarning)

# ---------------------------
# Configurações
# ---------------------------
BAUDRATE = 115200
MAX_HISTORY = 100

# <<< ALTERAÇÃO 1: Definir uma única porta serial para toda a aplicação >>>
SERIAL_PORT = "/dev/ttyUSB0"  # Mude para a sua porta serial principal

# <<< ALTERAÇÃO 2: Mapear nomes de zona para seus identificadores de protocolo (P1, P2, etc.) >>>
ZONE_IDS = {
    "Zona 1": "P1",
    "Zona 2": "P2",
    "Zona 3": "P3",
    "Zona 4": "P4",
    "Zona 5": "P5",
    "Zona 6": "P6",
    "Zona 7": "P7",
    "Zona 8": "P8",
    "Zona 9": "P9",
    "Zona 10": "P10",
    "Zona 11": "P11",
    "Zona 12": "P12",
}

# Mapeamento reverso para facilitar a busca do nome da zona a partir do ID
ID_TO_ZONE_NAME = {v: k for k, v in ZONE_IDS.items()}


# Escalas reais dos sensores
SENSOR_RANGES = {
    "TEMP": (10, 40),      # °C
    "HUM": (0, 100),       # %
    "LUX": (0, 600),    # lux
    "SOIL": (0, 100),      # %
}

# Coloque no topo do seu arquivo, com as outras configurações
SENSOR_UNITS = {
    "TEMP": "°C", "HUM": "%", "LUX": "lux", "SOIL": "%",
}

# Perfis de plantação com limiares ideais (valores reais)
PLANT_PROFILES = {
    "Café":      {"TEMP": (18, 24), "HUM": (60, 80), "LUX": (30, 10000), "SOIL": (40, 70)},
    "Tomate":    {"TEMP": (20, 30), "HUM": (50, 70), "LUX": (30, 10000), "SOIL": (50, 80)},
    "Alface":    {"TEMP": (15, 22), "HUM": (55, 75), "LUX": (30, 10000), "SOIL": (50, 70)},
    "Morango":   {"TEMP": (18, 25), "HUM": (60, 80), "LUX": (30, 10000), "SOIL": (40, 60)},
    "Milho":     {"TEMP": (20, 32), "HUM": (50, 70), "LUX": (30, 10000), "SOIL": (45, 75)},
    "Soja":      {"TEMP": (20, 30), "HUM": (50, 70), "LUX": (30, 10000), "SOIL": (40, 70)},
    "Banana":    {"TEMP": (24, 30), "HUM": (70, 90), "LUX": (30, 10000), "SOIL": (60, 80)},
    "Alfarroba": {"TEMP": (18, 28), "HUM": (40, 60), "LUX": (30, 10000), "SOIL": (30, 60)},
    "Cenoura":   {"TEMP": (16, 22), "HUM": (55, 75), "LUX": (30, 10000), "SOIL": (50, 70)},
    "Couve":     {"TEMP": (15, 22), "HUM": (55, 75), "LUX": (30, 10000), "SOIL": (50, 70)},
    "Pepino":    {"TEMP": (20, 28), "HUM": (60, 80), "LUX": (30, 10000), "SOIL": (50, 75)},
    "Pimentão":  {"TEMP": (20, 30), "HUM": (60, 80), "LUX": (30, 10000), "SOIL": (50, 80)},
    "Uva":       {"TEMP": (18, 28), "HUM": (50, 70), "LUX": (30, 10000), "SOIL": (40, 70)},
    "Manga":     {"TEMP": (24, 32), "HUM": (60, 80), "LUX": (30, 10000), "SOIL": (50, 80)},
    "Laranja":   {"TEMP": (20, 30), "HUM": (60, 80), "LUX": (30, 10000), "SOIL": (50, 80)},
    "Abacaxi":   {"TEMP": (22, 30), "HUM": (60, 80), "LUX": (30, 10000), "SOIL": (45, 70)},
    "Feijão":    {"TEMP": (18, 30), "HUM": (50, 70), "LUX": (30, 10000), "SOIL": (40, 70)},
    "Ervilha":   {"TEMP": (15, 25), "HUM": (55, 75), "LUX": (30, 10000), "SOIL": (50, 70)},
    "Beterraba": {"TEMP": (15, 25), "HUM": (55, 75), "LUX": (30, 10000), "SOIL": (50, 70)},
    "Espinafre": {"TEMP": (15, 22), "HUM": (55, 75), "LUX": (30, 10000), "SOIL": (50, 70)},
    "Brócolis":  {"TEMP": (15, 22), "HUM": (55, 75), "LUX": (30, 10000), "SOIL": (50, 70)},
    "Repolho":   {"TEMP": (15, 22), "HUM": (55, 75), "LUX": (30, 10000), "SOIL": (50, 70)},
    "Abóbora":   {"TEMP": (20, 30), "HUM": (50, 70), "LUX": (30, 10000), "SOIL": (50, 80)},
    "Melancia":  {"TEMP": (22, 32), "HUM": (50, 70), "LUX": (30, 10000), "SOIL": (45, 75)},
    "Melão":     {"TEMP": (22, 32), "HUM": (50, 70), "LUX": (30, 10000), "SOIL": (45, 75)},
    "Goiaba":    {"TEMP": (22, 30), "HUM": (60, 80), "LUX": (30, 10000), "SOIL": (50, 80)},
    "Pera":      {"TEMP": (18, 28), "HUM": (50, 70), "LUX": (30, 10000), "SOIL": (40, 70)},
    "Maçã":      {"TEMP": (18, 28), "HUM": (50, 70), "LUX": (30, 10000), "SOIL": (40, 70)},
    "Caju":      {"TEMP": (24, 32), "HUM": (60, 80), "LUX": (30, 10000), "SOIL": (50, 80)},
    "Mamão":     {"TEMP": (22, 30), "HUM": (60, 80), "LUX": (30, 10000), "SOIL": (50, 80)},
    "Acerola":   {"TEMP": (22, 30), "HUM": (60, 80), "LUX": (30, 10000), "SOIL": (50, 80)},
    "Maracujá":  {"TEMP": (22, 30), "HUM": (60, 80), "LUX": (30, 10000), "SOIL": (50, 80)},
    "Cacau":     {"TEMP": (22, 30), "HUM": (70, 90), "LUX": (30, 10000), "SOIL": (60, 80)},
    "Hortelã":   {"TEMP": (18, 25), "HUM": (55, 75), "LUX": (30, 10000), "SOIL": (50, 70)},
    "Manjericão":{"TEMP": (18, 28), "HUM": (55, 75), "LUX": (30, 10000), "SOIL": (50, 70)},
    "Alecrim":   {"TEMP": (20, 30), "HUM": (40, 60), "LUX": (30, 10000), "SOIL": (30, 60)},
    "Orégano":   {"TEMP": (18, 28), "HUM": (40, 60), "LUX": (30, 10000), "SOIL": (30, 60)},
    "Salsa":     {"TEMP": (15, 22), "HUM": (55, 75), "LUX": (30, 10000), "SOIL": (50, 70)},
    "Cebolinha": {"TEMP": (15, 22), "HUM": (55, 75), "LUX": (30, 10000), "SOIL": (50, 70)},
    "Alho":      {"TEMP": (18, 28), "HUM": (50, 70), "LUX": (30, 10000), "SOIL": (40, 70)},
    "Batata":    {"TEMP": (15, 25), "HUM": (55, 75), "LUX": (30, 10000), "SOIL": (50, 70)},
    "Inhame":    {"TEMP": (22, 30), "HUM": (60, 80), "LUX": (30, 10000), "SOIL": (50, 80)},
    "Rúcula":    {"TEMP": (15, 22), "HUM": (55, 75), "LUX": (30, 10000), "SOIL": (50, 70)},
    "Alho-poró": {"TEMP": (15, 22), "HUM": (55, 75), "LUX": (30, 10000), "SOIL": (50, 70)},
    "Chuchu":    {"TEMP": (20, 30), "HUM": (50, 70), "LUX": (30, 10000), "SOIL": (50, 80)}
}


# ---------------------------
# Worker Serial
# ---------------------------
class SerialWorker(QObject):
    data_received = pyqtSignal(dict)
    connection_lost = pyqtSignal()
    finished = pyqtSignal()
    
    # <<< NOVO SINAL: Para enviar linhas de debug completas >>>
    debug_data_received = pyqtSignal(str)

    def __init__(self, port, baudrate=115200):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.ser = None
        self._running = True
        self.buffer = ""

    def run(self):
        try:
            self.ser = serial.Serial(self.port, self.baudrate, timeout=1)
            print(f"[OK] Worker conectado na {self.port} ({self.baudrate} baud)")
            # Emite a mensagem de conexão bem-sucedida para o log de debug
            self.debug_data_received.emit(f"[RX] Conexão estabelecida com {self.port}")
        except Exception as e:
            print(f"[ERRO] Worker não conseguiu abrir porta {self.port}: {e}")
            self._running = False
            self.connection_lost.emit()
            # Emite a mensagem de erro para o log de debug
            self.debug_data_received.emit(f"[ERRO] Falha ao abrir porta {self.port}: {e}")
            return # Sai da função se a conexão falhar

        while self._running:
            try:
                if not self.ser or not self.ser.is_open:
                    if self._running:
                        print("[ERRO] A porta serial foi fechada inesperadamente.")
                        self.connection_lost.emit()
                    break
                
                raw_bytes = self.ser.readline()
                if not raw_bytes:
                    continue
                
                line_part = raw_bytes.decode("utf-8", errors="ignore")
                self.buffer += line_part

                while "\n" in self.buffer:
                    line, self.buffer = self.buffer.split("\n", 1)
                    line = line.strip()
                    
                    # <<< ADIÇÃO: Emite a linha completa para o log de debug ANTES de ser interpretada >>>
                    if line:
                        self.debug_data_received.emit(f"[RX] {line}")
                    
                    if line.startswith("$P"): # Apenas tenta fazer o parse se a linha for do protocolo
                        data = self.parse_data(line)
                        if data:
                            self.data_received.emit(data)

            except serial.serialutil.SerialException as e:
                if self._running:
                    print(f"[ERRO] Conexão serial perdida: {e}")
                    self.connection_lost.emit()
                    self.debug_data_received.emit(f"[ERRO] Conexão serial perdida: {e}")
                break
            except Exception as e:
                print(f"Erro de leitura inesperado: {e}")
                self.debug_data_received.emit(f"[ERRO] Erro de leitura inesperado: {e}")
                break

        if self.ser and self.ser.is_open:
            self.ser.close()
        self.finished.emit()

    def parse_data(self, line):
        """
        Interpreta uma linha como "$P1:TEMP:25.5"
        Retorna um dicionário como: {'zone': 'Zona 1', 'data': {'TEMP': 25.5}}
        """
        try:
            if line.startswith("$P"):
                parts = line[1:].split(":")
                if len(parts) == 3:
                    zone_id, key_raw, val_raw = parts
                    
                    zone_name = ID_TO_ZONE_NAME.get(zone_id)
                    if not zone_name:
                        return None # ID de zona desconhecido

                    key_map = {"LUM": "LUX", "TEMP": "TEMP", "HUM": "HUM", "SOIL": "SOIL"}
                    key = key_map.get(key_raw.upper())
                    
                    if key:
                        return {"zone": zone_name, "data": {key: float(val_raw)}}
            return None
        except Exception as e:
            # Emite a linha de erro para o log de debug
            self.debug_data_received.emit(f"[ERRO] Falha ao interpretar '{line}': {e}")
            return None

    @pyqtSlot()
    def stop(self):
        self._running = False

    @pyqtSlot(str)
    def write_data(self, msg_str):
        if self.ser and self.ser.is_open:
            try:
                self.ser.write(msg_str.encode())
                print(f"Enviado: {msg_str.strip()}")
                self.debug_data_received.emit(f"[TX] {msg_str.strip()}")
            except Exception as e:
                print(f"[ERRO] Falha ao escrever na serial: {e}")
                self.debug_data_received.emit(f"[ERRO] Falha ao escrever na serial: {e}")

# ---------------------------
# Gauge Profissional
# ---------------------------
class ProfessionalGauge(QWidget):
    def __init__(self, title="SENSOR", min_val=0, max_val=100):
        super().__init__()
        self.title = title
        self.min_val = min_val
        self.max_val = max_val
        self.value = min_val
        self.display_value = min_val
        self.ideal_min = min_val
        self.ideal_max = max_val
        self.setMinimumSize(150, 150)
        
        self.ani_timer = QTimer(self)
        self.ani_timer.timeout.connect(self.animate_pointer)
        self.ani_timer.start(20)

    def animate_pointer(self):
        diff = self.value - self.display_value
        if abs(diff) < 0.1:
            self.display_value = self.value
        else:
            self.display_value += diff * 0.1
        self.update()

    def setValue(self, val):
        self.value = max(self.min_val, min(val, self.max_val))

    def setIdealRange(self, min_ideal, max_ideal):
        if min_ideal > max_ideal:
            min_ideal, max_ideal = max_ideal, min_ideal
        self.ideal_min = min_ideal
        self.ideal_max = max_ideal
        self.update()

    def value_to_angle(self, value):
        start_angle = -135
        total_angle_span = 270
        if self.max_val == self.min_val:
            return start_angle
        value_clamped = max(self.min_val, min(value, self.max_val))
        proportion = (value_clamped - self.min_val) / (self.max_val - self.min_val)
        return start_angle + proportion * total_angle_span

    def get_pointer_color(self):
        if self.ideal_min <= self.value <= self.ideal_max:
            return QColor(46, 204, 113) # Verde

        ideal_zone_width = self.ideal_max - self.ideal_min
        margin = max(ideal_zone_width, 0.1) * 0.10 

        if (self.ideal_min - margin) <= self.value < self.ideal_min or \
           self.ideal_max < self.value <= (self.ideal_max + margin):
            return QColor(241, 196, 15) # Amarelo

        return QColor(231, 76, 60) # Vermelho

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)

        side = min(self.width(), self.height())
        rect = QRectF((self.width() - side) / 2, (self.height() - side) / 2, side, side)
        
        painter.translate(rect.center())
        radius = side / 2 - 15
        
        value_font_size = max(10, int(radius / 6)) 

        # 1. --- FUNDO ---
        bg_gradient = QRadialGradient(QPointF(0, 0), radius)
        bg_gradient.setColorAt(0, QColor(50, 50, 50))
        bg_gradient.setColorAt(1, QColor(20, 20, 20))
        painter.setBrush(QBrush(bg_gradient))
        painter.setPen(Qt.NoPen)
        painter.drawEllipse(QPointF(0, 0), radius + 10, radius + 10)

        # <<< ALTERAÇÃO: ARCO ÚNICO DE COR NEUTRA >>>
        arc_rect = QRectF(-radius, -radius, radius * 2, radius * 2)
        arc_width = max(5, int(radius / 8))
        
        pen = QPen(QColor(80, 80, 80), float(arc_width))
        pen.setCapStyle(Qt.FlatCap)
        painter.setPen(pen)
        
        start_angle = -135 * 16
        span_angle = 270 * 16
        painter.drawArc(arc_rect, start_angle, span_angle)

        # 3. --- MARCADORES E NÚMEROS ---
        tick_font_size = max(7, int(radius / 12))
        painter.setPen(QPen(Qt.white, 2))
        num_ticks = 10
        for i in range(11):
            angle = self.value_to_angle(self.min_val + (self.max_val - self.min_val) * i / num_ticks)
            angle_rad = math.radians(angle)
            p1 = QPointF(math.cos(angle_rad) * (radius), math.sin(angle_rad) * (radius))
            p2 = QPointF(math.cos(angle_rad) * (radius - arc_width + 1), math.sin(angle_rad) * (radius - arc_width + 1))
            painter.drawLine(p1, p2)
            if i % 2 == 0:
                
                val = self.min_val + (self.max_val - self.min_val) * i / num_ticks
                text_pos = QPointF(math.cos(angle_rad) * (radius - arc_width*2), math.sin(angle_rad) * (radius - arc_width*2))
                font = QFont("Arial", tick_font_size)
                painter.setFont(font)
                text = f"{val/1000:.0f}k" if val >= 1000 else f"{val:.0f}"
                painter.drawText(QRectF(text_pos.x() - 15, text_pos.y() - 8, 30, 16), Qt.AlignCenter, text)

        # 4. --- PONTEIRO ---
        painter.save()
        angle = self.value_to_angle(self.display_value)
        painter.rotate(angle)
        pointer_color = self.get_pointer_color()
        painter.setPen(QPen(pointer_color.darker(120), 1))
        painter.setBrush(QBrush(pointer_color))
        points = [QPointF(0, -radius*0.05), QPointF(0, radius*0.05), QPointF(radius - arc_width, 0)]
        polygon = QPolygonF(points)
        painter.drawPolygon(polygon)
        painter.restore()

        # 5. --- PIVÔ E TEXTO ---
        pivot_brush = QBrush(QColor(50, 50, 50))
        painter.setBrush(pivot_brush)
        painter.setPen(QPen(Qt.white, 2))
        painter.drawEllipse(QPointF(0, 0), max(5, radius*0.1), max(5, radius*0.1))
        
        painter.setPen(Qt.white)
        
        unit = SENSOR_UNITS.get(self.title, "")
        value_text = f"{self.display_value:.1f} {unit}"
        painter.setFont(QFont("Arial", value_font_size, QFont.Bold))
        painter.drawText(QRectF(-radius, 0, radius * 2, radius), Qt.AlignCenter, value_text)
# ---------------------------
# SmartFarmGUI (por zona)
# ---------------------------
class SmartFarmGUI(QWidget):
    send_command_to_zone = pyqtSignal(str, str)

    def __init__(self, zone_name, serial_worker):
        super().__init__()
        self.zone_name = zone_name
        self.serial_worker = serial_worker
        self.setWindowTitle(f"Smart Farm - {self.zone_name}")
        self.resize(1600, 900)
        self.setStyleSheet("""
            QListWidget { 
                background-color: #21262d; 
                color: #c9d1d9; 
                border: 1px solid #30363d;
                border-radius: 6px;
                font-size: 14px;
            }
            QGroupBox {
                font-size: 16px;
                font-weight: bold;
            }
            QWidget {
                background-color: #121212; 
                color: white;
            }
        """)

        self.data_history = {}
        self.gauges = {}
        self.curves = {}
        self.regions = {}
        self.ideal_min_spins = {}
        self.ideal_max_spins = {}
        self.sensor_states = {}

        main_layout = QHBoxLayout(self)
        
        content_layout = QVBoxLayout()
        sensors = ["TEMP", "HUM", "LUX", "SOIL"]

        for sensor_key in sensors:
            self.sensor_states[sensor_key] = 'IN'
            
            row_layout = QHBoxLayout()
            gauge = ProfessionalGauge(title=sensor_key, min_val=SENSOR_RANGES[sensor_key][0], max_val=SENSOR_RANGES[sensor_key][1])
            self.gauges[sensor_key] = gauge
            self.data_history[sensor_key] = deque(maxlen=MAX_HISTORY)
            
            plot_widget = pg.PlotWidget(title=f"Histórico de {sensor_key}")
            plot_widget.setBackground("#121212")
            plot_widget.showGrid(x=True, y=True)
            plot_widget.setMinimumHeight(200)
            curve = plot_widget.plot(pen=pg.mkPen('#00FFFF', width=2))
            region = pg.LinearRegionItem([SENSOR_RANGES[sensor_key][0], SENSOR_RANGES[sensor_key][1]], orientation=pg.LinearRegionItem.Horizontal, brush=pg.mkBrush(0, 255, 0, 50))
            plot_widget.addItem(region)
            self.curves[sensor_key] = curve
            self.regions[sensor_key] = region
            
            row_layout.addWidget(gauge)
            row_layout.addWidget(plot_widget)
            row_layout.setStretch(0, 1)
            row_layout.setStretch(1, 3)
            content_layout.addLayout(row_layout)

        content_container = QWidget()
        content_container.setLayout(content_layout)

        right_panel_layout = QVBoxLayout()

        control_box = QGroupBox("Configurar Intervalo Ideal")
        form = QFormLayout()
        self.profile_cb = QComboBox()
        self.profile_cb.addItems(list(PLANT_PROFILES.keys()))
        self.profile_cb.currentTextChanged.connect(self.apply_plant_profile)
        form.addRow(QLabel("Perfil da Plantação:"), self.profile_cb)
        for k in sensors:
            min_spin = QSpinBox(); max_spin = QSpinBox()
            min_spin.setRange(int(SENSOR_RANGES[k][0]), int(SENSOR_RANGES[k][1]))
            max_spin.setRange(int(SENSOR_RANGES[k][0]), int(SENSOR_RANGES[k][1]))
            self.ideal_min_spins[k] = min_spin; self.ideal_max_spins[k] = max_spin
            min_spin.valueChanged.connect(lambda val, s=k: self.__update_ideal_range(s))
            max_spin.valueChanged.connect(lambda val, s=k: self.__update_ideal_range(s))
            spin_layout = QHBoxLayout()
            spin_layout.addWidget(QLabel("Min:")); spin_layout.addWidget(min_spin)
            spin_layout.addWidget(QLabel("Max:")); spin_layout.addWidget(max_spin)
            form.addRow(QLabel(f"<b>{k}</b>"), spin_layout)
        self.send_btn = QPushButton("Enviar Limiar para Dispositivo")
        self.send_btn.clicked.connect(self.send_thresholds)
        form.addRow(self.send_btn)
        self.confirm_label = QLabel(""); self.confirm_label.setAlignment(Qt.AlignCenter)
        form.addRow(self.confirm_label)
        control_box.setLayout(form)
        
        log_box = QGroupBox("Registro de Eventos")
        log_layout = QVBoxLayout()
        self.log_widget = QListWidget()
        log_layout.addWidget(self.log_widget)
        log_box.setLayout(log_layout)

        right_panel_layout.addWidget(control_box)
        right_panel_layout.addWidget(log_box)
        right_panel_layout.setStretch(0, 1)
        right_panel_layout.setStretch(1, 1)

        right_panel_container = QWidget()
        right_panel_container.setLayout(right_panel_layout)
        right_panel_container.setFixedWidth(400)

        main_layout.addWidget(content_container)
        main_layout.addWidget(right_panel_container)

        self.apply_plant_profile(self.profile_cb.currentText())

    def add_log_message(self, message, color):
        timestamp = datetime.now().strftime("%d/%m %H:%M:%S")
        log_item = QListWidgetItem(f"[{timestamp}] {message}")
        log_item.setForeground(QColor(color))
        self.log_widget.addItem(log_item)
        self.log_widget.scrollToBottom()

    def check_thresholds_and_log(self, sensor_key, current_value):
        ideal_min = self.ideal_min_spins[sensor_key].value()
        ideal_max = self.ideal_max_spins[sensor_key].value()
        
        is_in_range = ideal_min <= current_value <= ideal_max
        current_state = 'IN' if is_in_range else 'OUT'
        
        last_state = self.sensor_states[sensor_key]
        
        if current_state != last_state:
            if current_state == 'OUT':
                msg = f"ALERTA: {sensor_key} fora do ideal ({current_value:.1f})"
                self.add_log_message(msg, "orange")
            else: # Se voltou para 'IN'
                msg = f"OK: {sensor_key} voltou ao normal ({current_value:.1f})"
                self.add_log_message(msg, "#55aaff")
            
            self.sensor_states[sensor_key] = current_state

    @pyqtSlot(dict)
    def update_data(self, data):
        for k, v in data.items():
            if k in self.gauges:
                self.gauges[k].setValue(float(v))
                self.data_history[k].append(float(v))
                self.curves[k].setData(list(self.data_history[k]))
                self.check_thresholds_and_log(k, float(v))

    def __update_ideal_range(self, sensor_key):
        min_val = self.ideal_min_spins[sensor_key].value()
        max_val = self.ideal_max_spins[sensor_key].value()
        self.gauges[sensor_key].setIdealRange(min_val, max_val)
        self.regions[sensor_key].setRegion([min_val, max_val])

    def apply_plant_profile(self, profile_name):
        profile = PLANT_PROFILES[profile_name]
        for sensor, (min_val, max_val) in profile.items():
            if sensor in self.gauges:
                self.ideal_min_spins[sensor].blockSignals(True)
                self.ideal_max_spins[sensor].blockSignals(True)
                self.ideal_min_spins[sensor].setValue(int(min_val))
                self.ideal_max_spins[sensor].setValue(int(max_val))
                self.ideal_min_spins[sensor].blockSignals(False)
                self.ideal_max_spins[sensor].blockSignals(False)
                self.__update_ideal_range(sensor)
                self.sensor_states[sensor] = 'IN'
        self.log_widget.clear()
        self.add_log_message(f"Perfil '{profile_name}' aplicado.", "white")

    @pyqtSlot()
    def handle_connection_lost(self):
        self.confirm_label.setText("Erro: Conexão perdida! 🔌")
        self.send_btn.setEnabled(False)
        for k in self.gauges.keys(): self.gauges[k].setValue(0)
        QTimer.singleShot(5000, lambda: self.confirm_label.setText(""))

    def send_thresholds(self):
        thresholds = {k: (self.ideal_min_spins[k].value(), self.ideal_max_spins[k].value()) for k in self.gauges.keys()}
        zone_id = ZONE_IDS.get(self.zone_name, "P1")
        msg = f"${zone_id}:THRESH:{thresholds['TEMP'][0]},{thresholds['TEMP'][1]},{thresholds['HUM'][0]},{thresholds['HUM'][1]},{thresholds['LUX'][0]},{thresholds['LUX'][1]},{thresholds['SOIL'][0]},{thresholds['SOIL'][1]}\n"
        # Envia pela serial
        if self.serial_worker.ser and self.serial_worker.ser.is_open:
            self.serial_worker.ser.write(msg.encode("utf-8"))
            print(f"[DEBUG] Enviado pela serial: {msg.strip()}")
            self.confirm_label.setText("Comando de limiar enviado!")
        else:
            print("[ERRO] Serial não está aberta!")
            self.confirm_label.setText("Erro: Serial não está aberta!")
        QTimer.singleShot(2000, lambda: self.confirm_label.setText(""))

# <<< NOVO WIDGET: A janela de debug >>>
class DebugWindow(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Debug Serial")
        self.resize(800, 600)
        self.setWindowFlags(self.windowFlags() | Qt.Window) # Permite ser movida livremente
        
        layout = QVBoxLayout(self)
        self.log_area = QPlainTextEdit()
        self.log_area.setReadOnly(True)
        self.log_area.setStyleSheet("""
            background-color: #161b22;
            color: #8b949e;
            font-family: monospace;
            font-size: 10pt;
            border: 1px solid #30363d;
        """)
        
        layout.addWidget(self.log_area)
        self.setModal(False) # Não bloqueia a janela principal

    @pyqtSlot(str)
    def update_log(self, message):
        """ Adiciona uma nova linha ao log, com rolagem automática. """
        self.log_area.appendPlainText(message)
        self.log_area.verticalScrollBar().setValue(self.log_area.verticalScrollBar().maximum())

# ---------------------------
# ZoneSelector (Menu Principal e Gerenciador)
# ---------------------------
class ZoneSelector(QMainWindow):
    trigger_write = pyqtSignal(str)

    def __init__(self):
        super().__init__()
        self.setWindowTitle("🌱 Smart Farm - Escolha a área de plantação")
        self.resize(1600, 900)

        self.setStyleSheet("""
            QMainWindow { background-color: #0d1117; }
            QLabel#title { font-size: 36px; font-weight: bold; color: #58a6ff; }
            QLabel#subtitle { font-size: 12px; color: #8b949e; }
        """)

        self.stack = QStackedWidget()
        self.setCentralWidget(self.stack)

        menu_widget = QWidget()
        menu_layout = QVBoxLayout(menu_widget)
        menu_layout.setAlignment(Qt.AlignCenter)
        
        title = QLabel("🌱 Smart Farm")
        title.setObjectName("title")
        title.setAlignment(Qt.AlignCenter)
        menu_layout.addWidget(title)
        
        subtitle = QLabel("Selecione a zona da plantação para monitorar")
        subtitle.setObjectName("subtitle")
        subtitle.setAlignment(Qt.AlignCenter)
        menu_layout.addWidget(subtitle)

        btn_layout = QGridLayout()
        btn_layout.setSpacing(20)
        zone_list = list(ZONE_IDS.keys())
        cols = 4

        for idx, zone_name in enumerate(zone_list):
            row = idx // cols
            col = idx % cols
            btn = QPushButton(zone_name)
            btn.setFixedSize(300, 200)
            btn.setStyleSheet("""
                QPushButton { font-size: 24px; border-radius: 15px; background-color: #21262d; color: white; border: 2px solid #30363d; }
                QPushButton:hover { background-color: #238636; border: 2px solid #2ea043; }
                QPushButton:pressed { background-color: #196c2e; }
            """)
            btn.clicked.connect(partial(self.open_zone, zone_name))
            btn_layout.addWidget(btn, row, col, alignment=Qt.AlignCenter)
        
        menu_layout.addLayout(btn_layout)
        
        # <<< NOVO BOTÃO: Para abrir a janela de debug >>>
        debug_btn = QPushButton("Abrir Debug Serial")
        debug_btn.setMinimumHeight(40)
        debug_btn.setStyleSheet("""
            QPushButton { font-size: 14px; border-radius: 8px; background-color: #555555; color: white; border: 1px solid #777777; }
            QPushButton:hover { background-color: #777777; }
        """)
        debug_btn.clicked.connect(self.show_debug_window)
        menu_layout.addWidget(debug_btn, alignment=Qt.AlignCenter)
        
        menu_layout.addStretch()
        
        self.menu_widget = menu_widget
        self.stack.addWidget(menu_widget)

        self.zone_widgets = {}

        self.toolbar = self.addToolBar("Navegação")
        self.toolbar.setMovable(False)
        self.toolbar.setStyleSheet("""
            QToolBar { background-color: #161b22; border-bottom: 1px solid #30363d; padding: 5px; }
            QToolButton { font-size: 16px; color: white; padding: 8px 16px; border-radius: 8px; }
            QToolButton:hover { background-color: #238636; }
        """)
        back_action = QAction("🔙 Voltar ao Menu", self)
        back_action.triggered.connect(self.show_menu)
        self.toolbar.addAction(back_action)
        
        # <<< NOVO BOTÃO NA TOOLBAR: Para o debug >>>
        debug_action = QAction("🐛 Debug Serial", self)
        debug_action.triggered.connect(self.show_debug_window)
        self.toolbar.addAction(debug_action)

        self.toolbar.hide()

        self.debug_window = DebugWindow()
        self.setup_serial_worker()

    def setup_serial_worker(self):
        self.thread = QThread()
        self.worker = SerialWorker(SERIAL_PORT, BAUDRATE)
        self.worker.moveToThread(self.thread)

        self.thread.started.connect(self.worker.run)
        self.worker.finished.connect(self.thread.quit)
        self.worker.finished.connect(self.worker.deleteLater)
        self.thread.finished.connect(self.thread.deleteLater)
        
        self.worker.data_received.connect(self.distribute_data)
        self.worker.connection_lost.connect(self.handle_global_connection_lost)
        
        self.trigger_write.connect(self.worker.write_data)
        
        # <<< CONEXÃO DO SINAL DE DEBUG >>>
        self.worker.debug_data_received.connect(self.debug_window.update_log)
        
        self.thread.start()

    @pyqtSlot()
    def show_debug_window(self):
        self.debug_window.show()

    @pyqtSlot(dict)
    def distribute_data(self, data_packet):
        zone_name = data_packet.get("zone")
        sensor_data = data_packet.get("data")
        
        if zone_name and zone_name in self.zone_widgets:
            self.zone_widgets[zone_name].update_data(sensor_data)

    @pyqtSlot(str, str)
    def handle_zone_command(self, zone_name, command):
        zone_id = ZONE_IDS.get(zone_name)
        if zone_id:
            # Garante o formato correto: $P2:THRESH,...
            formatted_msg = f"${zone_id}:{command}"
            self.trigger_write.emit(formatted_msg)

    @pyqtSlot()
    def handle_global_connection_lost(self):
        print("Notificando todas as janelas sobre a perda de conexão.")
        for gui in self.zone_widgets.values():
            gui.handle_connection_lost()

    def open_zone(self, zone_name):
        if zone_name not in self.zone_widgets:
            gui = SmartFarmGUI(zone_name, self.worker)  # Passe o worker!
            self.zone_widgets[zone_name] = gui
            self.stack.addWidget(gui)
        self.stack.setCurrentWidget(self.zone_widgets[zone_name])
        self.toolbar.show()

    def show_menu(self):
        self.stack.setCurrentWidget(self.menu_widget)
        self.toolbar.hide()

    def closeEvent(self, event):
        print("Fechando aplicação... Parando worker serial.")
        if self.thread.isRunning():
            self.worker.stop()
            self.thread.quit()
            self.thread.wait()
        super().closeEvent(event)

# ---------------------------
# Executar GUI
# ---------------------------
if __name__ == "__main__":
    app = QApplication(sys.argv)
    selector = ZoneSelector()
    selector.show()
    sys.exit(app.exec_())