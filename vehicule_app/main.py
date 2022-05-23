from PyQt5.QtWidgets import QApplication, QMainWindow, QStackedWidget, QWidget
from PyQt5 import uic, QtGui
import sys
import paho.mqtt.client as paho
from PyQt5.QtCore import Qt, QThread, pyqtSignal
from time import sleep


class MqttApp(QThread):
    sun = pyqtSignal(str)
    temp = pyqtSignal(str)
    slot_statut = pyqtSignal(str)

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
        self.client.subscribe("CoreElectronics/slot_statut")

    def on_message(self, client, userdata, msg):
        topic, message = msg.topic, msg.payload.decode("utf-8")
        print(topic + " -> " + str(message))
        if topic == "CoreElectronics/temp":
            self.temp.emit(str(message))
        elif topic == "CoreElectronics/sun":
            self.sun.emit(message)
        elif topic == "CoreElectronics/slot_statut":
            self.slot_statut.emit(message)

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
        self.langage.activated[str].connect(self.set_langage)

    def trigged_buttons(self):
        self.button_start.clicked.connect(lambda: interfaces.setCurrentWidget(parking))

    def set_langage(self, langage):
        if langage == "Francais":
            self.title.setText("IoT Intelligent Parking Solution")
            self.button_start.setText("Réserver Maintenant")
            self.button_about.setText("About Nous")
            parking.title.setText("IoT Intelligent Parking Solution")
        elif langage == "Anglais":
            self.title.setText("Smart Parking IoT Solution")
            parking.title.setText("Smart Parking IoT Solution")
            self.button_start.setText("Get Started")
            self.button_about.setText("About US")
        else:
            self.title.setText("حلول إنترنت الأشياء لمواقف السيارات")
            parking.title.setText("حلول إنترنت الأشياء لمواقف السيارات")
            self.button_start.setText("البدء")
            self.button_about.setText("معلومات عنا")

        self.title.setStyleSheet(
            " color:orange;font-size:14pt; font-weight:550; font-style:italic;"
        )


class Parking(QWidget):
    def __init__(self):
        super().__init__()
        uic.loadUi("./ui/parking.ui", self)
        self.set_ui()
        self.start_subscribing()
        self.trigged_buttons()
        self.slot1_available = 1
        self.slot2_available = 0
        self.slot3_available = 0

    def set_ui(self):
        self.setWindowTitle("Automobile app")
        self.setGeometry(39, 30, 554, 405)
        self.setFixedSize(554, 405)

    def slot_statut(self, msg):
        if msg == "slot1_1":
            self.slot1.setPixmap(QtGui.QPixmap("./images/park1_on.png"))
        elif msg == "slot1_0":
            self.slot1.setPixmap(QtGui.QPixmap("./images/park1_off.png"))
        if msg == "slot2_1":
            self.slot2.setPixmap(QtGui.QPixmap("./images/park2_on.png"))
        elif msg == "slot2_0":
            self.slot2.setPixmap(QtGui.QPixmap("./images/park2_off.png"))
        elif msg == "slot3_1":
            self.slot3.setPixmap(QtGui.QPixmap("./images/park3_on.png"))
        elif msg == "slot3_0":
            self.slot3.setPixmap(QtGui.QPixmap("./images/park3_off.png"))

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
        self.thread.slot_statut.connect(self.slot_statut)
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

    interfaces.setWindowTitle("Automobile app")
    interfaces.setGeometry(39, 30, 554, 405)
    interfaces.setFixedSize(554, 405)

    try:
        app.exec_()
    except:
        print("closing...")
