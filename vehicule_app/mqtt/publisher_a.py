from turtle import delay
import paho.mqtt.client as paho
from time import sleep

broker = "test.mosquitto.org"
port = 1883


def on_publish(client, userdata, result):  # create function for callback
    print("data published \n")
    pass


client1 = paho.Client("control1")  # create client object
client1.on_publish = on_publish  # assign function to callback
client1.connect(broker, port)
r = 1

while True:
    ret = client1.publish("CoreElectronics/slot_statut", "slot1_0")
    ret = client1.publish("CoreElectronics/slot_statut", "slot2_0")
    ret = client1.publish("CoreElectronics/slot_statut", "slot3_1")
    sleep(2)
    ret = client1.publish("CoreElectronics/slot_statut", "slot1_1")
    ret = client1.publish("CoreElectronics/slot_statut", "slot2_1")
    ret = client1.publish("CoreElectronics/slot_statut", "slot3_0")
    sleep(2)
