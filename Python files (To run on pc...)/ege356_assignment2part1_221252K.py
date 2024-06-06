"""
'location.py'
Example of sending GPS data points
to an Adafruit IO Feed using the API
Author(s): Brent Rubell, Todd Treece
"""
# Import standard python modules
import time
import paho.mqtt.client as mqtt

# Import Adafruit IO REST client.
from Adafruit_IO import Client, Feed, RequestError

global client
global gdata1
# global gdata2
global mqtt_ipsvr

gdata1 = 0.0
# gdata2 = "Normal"
mqtt_ipsvr = "192.168.23.1"

# Set to your Adafruit IO key.
# Remember, your key is a secret,
# so make sure not to publish it when you publish this code!
ADAFRUIT_IO_KEY = ''

# Set to your Adafruit IO username.
# (go to https://accounts.adafruit.com to find your username)
ADAFRUIT_IO_USERNAME = 'Studentwastaken'

# Create an instance of the REST client.
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

# limit feed updates to every 3 seconds, avoid IO throttle
loop_delay = 2.1

# We don't have a GPS hooked up, but let's fake it for the example/test:
# (replace this data with values from a GPS hardware module)
lat = 1.3136292
lon = 103.8156587
# elevation above sea level (meters)
ele = 41


def on_connect(client, userdata, flags, rc):
    # client.subscribe("test/#")
    # client.subscribe("test/sens1")
    client.subscribe("test/temp1")


def on_message(client, userdata, msg):
    # Do something
    global gdata1
    # global gdata2
    #print( msg.topic, msg.payload)
    print("Message Received: {},{}\n".format(msg.topic, msg.payload.decode("utf-8")))
    if msg.topic == "test/temp1":
        gdata1 = str(msg.payload.decode("utf-8"))
        print("message temp1: {}".format(gdata1))
        aio.send_data(temperature.key, gdata1)
        print('\nTemperature sent: {0}'.format(gdata1))
        data = aio.receive(temperature.key)
        print('Temperature received: {0}\n'.format(data.value))


def clientconn():
    global gdata1
    global client
    global mqtt_ipsvr
    client = mqtt.Client("hostname1")
    client.on_connect = on_connect
    client.on_message = on_message
    print("mqtt_ipsvr addr={}".format(mqtt_ipsvr))
    client.connect(mqtt_ipsvr, 1883, 60)


# Assign a location feed, if one exists already
try:
    location = aio.feeds('location')
except RequestError:
    # Doesn't exist, create a new feed
    feed = Feed(name="location")
    location = aio.create_feed(feed)

try:
    temperature = aio.feeds('mqtt')
except RequestError:
    feed = Feed(name="mqtt")
    temperature = aio.create_feed(feed)

print('\nSending GPS Data to location feed...\n')
print('Lat: ', lat)
print('Lon: ', lon)
print('Ele: ', ele)
# Send location data to Adafruit IO
metadata = {'lat': lat, 'lon': lon, 'ele': ele, 'created_at': time.asctime(time.gmtime())}
aio.send_data(location.key, 0, metadata)

# Read the location data back from IO
print('\nData Received by Adafruit IO Feed:\n')
data = aio.receive(location.key)
print('Lat: {0}\nLon: {1}\nEle: {2}\n'.format(data.lat, data.lon, data.ele))


if __name__ == '__main__':
    clientconn()
    time.sleep(loop_delay)
    client.loop_forever()

# use client.loop_forever()?
'''
while True:
    print('\nTemperature sent: {0}'.format(value))
    value = random.randint(40, 70)
    aio.send_data(temperature.key, value)

    Read the location data back from IO
    data = aio.receive(temperature.key)
    print('Temperature received: {0}\n'.format(data.value))
    wait loop_delay seconds to avoid api throttle
    time.sleep(loop_delay)
'''
