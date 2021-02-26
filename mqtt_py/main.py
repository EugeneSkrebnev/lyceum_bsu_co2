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
    x = 100
    while True:
        x = x + randint(-10, 10)
        client.publish("sensors", ('sensors,site=room1 value=' + str(x)))
        time.sleep(1)




