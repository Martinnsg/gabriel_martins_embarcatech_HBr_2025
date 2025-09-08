import sys
import math
from collections import deque
from PyQt5.QtWidgets import (
    QApplication, QWidget, QVBoxLayout, QHBoxLayout, QLabel,
    QGroupBox, QFormLayout, QSpinBox, QPushButton
)
from PyQt5.QtGui import QPainter, QColor, QFont, QPen, QBrush, QRadialGradient
from PyQt5.QtCore import QPointF, QRectF, Qt, QTimer, QThread, pyqtSignal, pyqtSlot, QObject
import serial
import pyqtgraph as pg

SERIAL_PORT = "/dev/ttyUSB0"
BAUDRATE = 115200
MAX_HISTORY = 100


# ---------------------------
# Worker para leitura e escrita serial
# ---------------------------
class SerialWorker(QObject):
    data_received = pyqtSignal(dict)
    connection_lost = pyqtSignal()
    finished = pyqtSignal()

    def __init__(self, port, baudrate=115200):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.ser = None
        self._running = True
        self.buffer = ""

    def run(self):
        """Este método contém o loop principal de leitura."""
        try:
            self.ser = serial.Serial(self.port, self.baudrate, timeout=1)
            print(f"[OK] Worker conectado na {self.port} ({self.baudrate} baud)")
        except Exception as e:
            print(f"[ERRO] Worker não conseguiu abrir porta {self.port}: {e}")
            self._running = False

        while self._running:
            try:
                if not self.ser.is_open:
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
                    if line:
                        print(f"[RX] {line}")
                        data = self.parse_data(line)
                        if data:
                            self.data_received.emit(data)

            except serial.serialutil.SerialException as e:
                if self._running:
                    print(f"[ERRO] Conexão serial perdida: {e}")
                    self.connection_lost.emit()
                break
            except Exception as e:
                print(f"Erro de leitura inesperado: {e}")
                break

        if self.ser and self.ser.is_open:
            self.ser.close()
        print("[INFO] Worker encerrou o loop e fechou a serial.")
        self.finished.emit()

    def parse_data(self, line):
        try:
            line = line.strip()
            if line.startswith("$"):
                parts = line[1:].split(":")
                if len(parts) == 2:
                    key_raw, val_raw = parts
                    key_map = {"LUM": "LUX", "TEMP": "TEMP", "HUM": "HUM", "SOIL": "SOIL"}
                    key = key_map.get(key_raw.upper())
                    if key:
                        value = float(val_raw)
                        return {key: value}
            return None
        except Exception as e:
            print("Erro parseando linha:", e)
            return None

    @pyqtSlot()
    def stop(self):
        """Slot para parar o loop de forma segura."""
        print("[INFO] Sinal de parada recebido pelo Worker.")
        self._running = False

    @pyqtSlot(str)
    def write_data(self, msg_str):
        """Slot para escrever dados na serial (executado na thread do worker)."""
        if self.ser and self.ser.is_open:
            try:
                self.ser.write(msg_str.encode())
                print(f"[TX] {msg_str.strip()}")
            except Exception as e:
                print(f"[ERRO] Falha ao escrever na serial: {e}")
        else:
            print("[AVISO] Tentativa de escrita com a porta serial fechada.")


# ---------------------------
# Gauge profissional
# ---------------------------
class ProfessionalGauge(QWidget):
    def __init__(self, title="SENSOR", min_val=0, max_val=100):
        super().__init__()
        self.title = title
        self.min_val = min_val
        self.max_val = max_val
        self.value = min_val
        self.display_value = min_val
        self.ideal_min = 20
        self.ideal_max = 80
        self.alert = False
        self.blink_state = False
        self.setMinimumSize(250, 250)
        self.blink_timer = QTimer()
        self.blink_timer.timeout.connect(self.toggle_blink)
        self.blink_timer.start(500)
        self.ani_timer = QTimer()
        self.ani_timer.timeout.connect(self.animate_pointer)
        self.ani_timer.start(20)

    def toggle_blink(self):
        if self.alert:
            self.blink_state = not self.blink_state
            self.update()

    def animate_pointer(self):
        diff = self.value - self.display_value
        step = diff * 0.1
        if abs(step) < 0.01:
            self.display_value = self.value
        else:
            self.display_value += step
        self.update()

    def setValue(self, val):
        self.value = val
        self.update_alert()

    def setIdealRange(self, min_ideal, max_ideal):
        self.ideal_min = min_ideal
        self.ideal_max = max_ideal
        self.update_alert()

    def update_alert(self):
        self.alert = not (self.ideal_min <= self.value <= self.ideal_max)

    def get_color_for_value(self):
        if self.ideal_min <= self.value <= self.ideal_max:
            return QColor(0, 255, 0)
        intervalo = self.ideal_max - self.ideal_min
        margem = intervalo * 0.5
        if (self.ideal_min - margem) <= self.value < self.ideal_min or \
           self.ideal_max < self.value <= (self.ideal_max + margem):
            return QColor(255, 255, 0)
        return QColor(255, 0, 0)

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        rect = QRectF(10, 10, self.width()-20, self.height()-20)
        cx, cy = rect.center().x(), rect.center().y()
        radius = rect.width()/2 - 20
        gradient = QRadialGradient(cx, cy, radius)
        gradient.setColorAt(0, QColor("#222222"))
        gradient.setColorAt(1, QColor("#444444"))
        painter.setBrush(QBrush(gradient))
        painter.setPen(Qt.NoPen)
        painter.drawEllipse(rect)
        angle = 270 * (self.display_value - self.min_val)/(self.max_val - self.min_val) - 135
        rad = math.radians(angle)
        needle_color = self.get_color_for_value()
        if self.alert and self.blink_state:
            needle_color = QColor("red")
        painter.setPen(QPen(needle_color, 6))
        x = cx + (radius-20) * math.cos(rad)
        y = cy + (radius-20) * math.sin(rad)
        painter.drawLine(QPointF(cx, cy), QPointF(x, y))
        painter.setPen(QColor("white"))
        painter.setFont(QFont("Arial", 18, QFont.Bold))
        painter.drawText(rect, Qt.AlignCenter, f"{self.title}\n{self.display_value:.1f}")


# ---------------------------
# GUI Principal
# ---------------------------
class SmartFarmGUI(QWidget):
    trigger_write = pyqtSignal(str)

    def __init__(self, port):
        super().__init__()
        self.setWindowTitle("Smart Farm - Dashboard Extremo")
        self.resize(1400, 800)
        self.setStyleSheet("background-color: #121212; color: white;")

        # --- CONFIGURAÇÃO DA THREAD (MÉTODO RECOMENDADO) ---
        self.thread = QThread()
        self.worker = SerialWorker(port, BAUDRATE)

        self.worker.moveToThread(self.thread)

        self.thread.started.connect(self.worker.run)
        self.worker.finished.connect(self.thread.quit)
        self.worker.finished.connect(self.worker.deleteLater)
        self.thread.finished.connect(self.thread.deleteLater)

        self.worker.data_received.connect(self.update_data)
        self.worker.connection_lost.connect(self.handle_connection_lost)

        self.trigger_write.connect(self.worker.write_data)
        self.thread.start()
        # --- FIM DA CONFIGURAÇÃO DA THREAD ---

        self.data_history = {}
        self.gauges = {}
        self.curves = {}
        self.regions = {}

        main_layout = QHBoxLayout()
        self.setLayout(main_layout)

        sensors = ["TEMP", "HUM", "LUX", "SOIL"]
        gauge_layout = QVBoxLayout()
        for k in sensors:
            gauge = ProfessionalGauge(title=k, min_val=0, max_val=100)
            gauge_layout.addWidget(gauge)
            self.gauges[k] = gauge
            self.data_history[k] = deque(maxlen=MAX_HISTORY)
        main_layout.addLayout(gauge_layout)

        plot_layout = QVBoxLayout()
        for k in sensors:
            plot_widget = pg.PlotWidget(title=k)
            plot_widget.setBackground("#121212")
            plot_widget.showGrid(x=True, y=True)
            curve = plot_widget.plot(pen=pg.mkPen('#00FFFF', width=2))
            region = pg.LinearRegionItem([20, 80], orientation=pg.LinearRegionItem.Horizontal,
                                         brush=pg.mkBrush(0, 255, 0, 50))
            plot_widget.addItem(region)
            self.curves[k] = curve
            self.regions[k] = region
            plot_layout.addWidget(plot_widget)
        main_layout.addLayout(plot_layout)

        control_box = QGroupBox("Configurar Intervalo Ideal")
        form = QFormLayout()
        self.ideal_min_spins = {}
        self.ideal_max_spins = {}
        self.send_btn = QPushButton("Enviar Limiar")
        self.send_btn.clicked.connect(self.send_thresholds)
        for k in sensors:
            min_spin = QSpinBox()
            min_spin.setRange(-1000, 1000)
            min_spin.setValue(20)
            max_spin = QSpinBox()
            max_spin.setRange(-1000, 1000)
            max_spin.setValue(80)
            self.ideal_min_spins[k] = min_spin
            self.ideal_max_spins[k] = max_spin
            min_spin.valueChanged.connect(lambda val, s=k: self.update_ideal_range(s))
            max_spin.valueChanged.connect(lambda val, s=k: self.update_ideal_range(s))
            container = QHBoxLayout()
            container.addWidget(QLabel("Ideal Min:"))
            container.addWidget(min_spin)
            container.addWidget(QLabel("Ideal Max:"))
            container.addWidget(max_spin)
            form.addRow(QLabel(k), container)
        form.addRow(self.send_btn)
        self.confirm_label = QLabel("")
        form.addRow(self.confirm_label)
        control_box.setLayout(form)
        main_layout.addWidget(control_box)


    def handle_connection_lost(self):
        print("[AVISO] Conexão com o dispositivo perdida. Gráficos parados.")
        self.confirm_label.setText("Erro: Conexão com o dispositivo perdida! 🔌")
        self.send_btn.setEnabled(False)
        for k in self.gauges.keys():
            self.gauges[k].setValue(0)
        QTimer.singleShot(5000, lambda: self.confirm_label.setText(""))

    def update_ideal_range(self, sensor_key):
        min_val = self.ideal_min_spins[sensor_key].value()
        max_val = self.ideal_max_spins[sensor_key].value()
        self.gauges[sensor_key].setIdealRange(min_val, max_val)
        region = self.regions[sensor_key]
        region.setRegion([min_val, max_val])

    def update_data(self, data):
        for k, v in data.items():
            if k in self.gauges:
                self.gauges[k].setValue(float(v))
                self.data_history[k].append(float(v))
                self.curves[k].setData(list(self.data_history[k]))

    def send_thresholds(self):
        thresholds = {k: (self.ideal_min_spins[k].value(), self.ideal_max_spins[k].value())
                      for k in self.gauges.keys()}
        print("Limiar pronto para ser enviado:", thresholds)
        msg = "$THRESH,{},{},{},{},{},{},{},{}\n".format(
            thresholds['TEMP'][0], thresholds['TEMP'][1],
            thresholds['HUM'][0], thresholds['HUM'][1],
            thresholds['LUX'][0], thresholds['LUX'][1],
            thresholds['SOIL'][0], thresholds['SOIL'][1]
        )
        self.trigger_write.emit(msg)
        self.confirm_label.setText("Comando de limiar enviado!")
        QTimer.singleShot(2000, lambda: self.confirm_label.setText(""))

    def closeEvent(self, event):
        print("[INFO] Fechando a aplicação...")
        if self.thread.isRunning():
            print("[INFO] Solicitando parada do Worker...")
            self.worker.stop()
            self.thread.quit()
            self.thread.wait()
            print("[INFO] Thread encerrada com sucesso.")
        super().closeEvent(event)


# ---------------------------
# Executar GUI
# ---------------------------
if __name__ == "__main__":
    app = QApplication(sys.argv)
    gui = SmartFarmGUI(SERIAL_PORT)
    gui.show()
    sys.exit(app.exec_())