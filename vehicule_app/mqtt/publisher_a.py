import paho.mqtt.client as paho
from time import sleep 

broker="test.mosquitto.org"
port=1883
def on_publish(client,userdata,result):             #create function for callback
    print("data published \n")
    pass

client1= paho.Client("control1")                           #create client object
client1.on_publish = on_publish                          #assign function to callback
client1.connect(broker,port)  
r = 1
#establish connection
while True:
    ret = client1.publish("CoreElectronics/temp",str(r)) 
    ret = client1.publish("CoreElectronics/sun","hot") 
    ret = client1.publish("real_unique_topic_3","hot") 
    r += 1
    sleep(4)

