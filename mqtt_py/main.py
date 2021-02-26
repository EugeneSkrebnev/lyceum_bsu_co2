# This is a sample Python script.


import paho.mqtt.client as mqtt
from random import randint
import time


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    client_name = 'test_values'
    client = mqtt.Client(client_name)
# put your mosquitto adr here
    host = '192.168.1.84'
    client.connect(host)
    while True:
        value = randint(0, 100)
        client.publish("sensors/room112", value)
        time.sleep(3)




