#! /bin/sh

#test.mosquitto.org:1883
#broker.hivemq.com:1883
MQTTBROKER="broker.hivemq.com:1883"
TOPICS="Exit Reset Intrusion  MotionSensor/# CameraSensor/+/IntrusionPosition"

cd Source/temp
rm * 
cd ..

echo "\nCompiling Files..."
	g++ Pipe.h -o Binary/Pipe -w
	g++ Data.h -o Binary/Data -w
	g++ main.c -o Binary/main -w
	gcc publisher.c -o Binary/pub -l paho-mqtt3c 
	gcc subscriber.c -o Binary/sub -l paho-mqtt3c
	g++ ngp.cpp -o Binary/graphic -lGL -lGLU -lglut -w
	g++ nocvp.cpp -o Binary/camera `pkg-config --libs opencv` `pkg-config --cflags opencv`

echo Compilation Done...

echo "Opening..."
gnome-terminal  --tab -t "Main" -e "./Binary/main" --tab -t "Pub" -e "./Binary/pub $MQTTBROKER" --tab -t "Sub" -e "./Binary/sub $MQTTBROKER $TOPICS" --tab -t "Graphics" -e "./Binary/graphic" --tab -t "Camera" -e "./Binary/camera"

	#sleep 1
	#gnome-terminal --tab -e "./Binary/pub $MQTTBROKER" 
	#sleep 1
	#gnome-terminal -e "./Binary/sub $MQTTBROKER Exit Reset Intrusion  MotionSensor/# CameraSensor/#"
	#sleep 1
	#gnome-terminal -e "./Binary/graphic"  
	#sleep 1
echo "DONE"

