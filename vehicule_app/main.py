from PyQt5.QtWidgets import QApplication, QMainWindow, QStackedWidget, QWidget
from PyQt5 import uic, QtGui
import sys
import paho.mqtt.client as paho
from PyQt5.QtCore import Qt, QThread, pyqtSignal
from time import sleep


class MqttApp(QThread):
    sun = pyqtSignal(str)
    temp = pyqtSignal(str)

    def run(self):
        self.client = paho.Client()
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        self.client.on_publish = self.on_publish
        self.client.connect("test.mosquitto.org", 1883, 60)
        self.client.loop_forever()

    def on_connect(self, client, userdata, flags, rc):
        self.client.subscribe("CoreElectronics/temp")
        self.client.subscribe("CoreElectronics/sun")

    def on_message(self, client, userdata, msg):
        topic, message = msg.topic, msg.payload.decode("utf-8")
        print(topic + " -> " + str(message))
        if topic == "CoreElectronics/temp":
            self.temp.emit(str(message))
        elif topic == "CoreElectronics/sun":
            self.sun.emit(message)

    def on_publish(self, client, userdata, result):
        print("data published")
        pass

    def publish_msg(self, topic, message):
        ret = self.client.publish(topic, message)


class Main(QMainWindow):
    def __init__(self):
        super().__init__()
        self.set_ui()
        self.trigged_buttons()

    def set_ui(self):
        self.setWindowTitle("Automobile app")
        self.setGeometry(39, 30, 554, 405)
        self.setFixedSize(554, 405)
        uic.loadUi("./ui/main.ui", self)

    def trigged_buttons(self):
        self.button_start.clicked.connect(lambda: interfaces.setCurrentWidget(parking))


class Parking(QWidget):
    def __init__(self):
        super().__init__()
        uic.loadUi("./ui/parking.ui", self)
        self.set_ui()
        self.start_subscribing()
        self.trigged_buttons()

    def set_ui(self):
        self.setWindowTitle("Automobile app")
        self.setGeometry(39, 30, 554, 405)
        self.setFixedSize(554, 405)

    def trigged_buttons(self):
        # reserve buttons
        self.button_reserve_1.clicked.connect(
            lambda: self.thread.publish_msg("CoreElectronics/reserve", "1")
        )
        self.button_reserve_2.clicked.connect(
            lambda: self.thread.publish_msg("CoreElectronics/reserve", "2")
        )
        self.button_reserve_3.clicked.connect(
            lambda: self.thread.publish_msg("CoreElectronics/reserve", "3")
        )
        # unreserve buttons
        self.button_unreserve_1.clicked.connect(
            lambda: self.thread.publish_msg("CoreElectronics/unreserve", "1")
        )
        self.button_unreserve_2.clicked.connect(
            lambda: self.thread.publish_msg("CoreElectronics/unreserve", "2")
        )
        self.button_unreserve_3.clicked.connect(
            lambda: self.thread.publish_msg("CoreElectronics/unreserve", "3")
        )

    def start_subscribing(self):
        self.thread = MqttApp()
        self.thread.temp.connect(self.set_temp)
        self.thread.sun.connect(self.set_sun)
        self.thread.start()

    def set_temp(self, temp):
        self.temperature.setText(str(temp))

    def set_sun(self, sun):
        self.sun_statut.setText(str(sun))


if __name__ == "__main__":
    app = QApplication(sys.argv)
    interfaces = QStackedWidget()
    main_window = Main()
    parking = Parking()

    interfaces.addWidget(main_window)
    interfaces.addWidget(parking)

    interfaces.show()

    try:
        app.exec_()
    except:
        print("closing...")