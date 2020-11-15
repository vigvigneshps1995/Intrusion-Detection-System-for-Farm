# Animal Intrusion Detection System - 2017
######  Note: This project is an end-to-end prototype of the actual idea.

#### Objective

   To detect an intrusion in a farm and to notify the relevant personnel with the live geographic location and the visuals of the intruder that provides the person to react in time and prevent any damage caused to the farm.

The proposed approach uses a set of motion sensors, whose count depends upon the area of the farm, which triggers the selective cameras when the sensors detect a motion of an animal or a human. The camera captures the visuals of the moving animal which and tracks the moving object into the farm. Additionally, it transmits the visuals to MQTT server for image processing to identify the object of intrusion. Once the moving object is identified to be an animal, the intruder (animal) along with it’s geographic location (WITHOUT EMPLOYING A GPS SENSOR) in the farm is sent from server to the output display that is developed over OpenGL library.

#### Architecture Diagram
![Architecture.png](System Architecture.png)
![Figure 1.png](Figure 1.png)

#### Sequence Diagram
![sequence_diagram.png](sequence_diagram.png)

#### Flow Chart
![Flow Chart.png](Flow Chart.png) 

#### Image Processing 
![Image processing pipeline.png](Image processing pipeline.png)


#### Advantages
The system is power efficient wherein, the cameras are triggered only when the sensors identify a motion within a farm.
This cost-efficient system is capable of efficiently capturing any intrusion irrespective of it’s location within the farm with limited, desired and calculated number of motion sensors and cameras.
It provides precise location of the object within the farm without usage of any GPS sensors.
It is based on (OpenCV) computer-vision, which helps in identifying an animal thereby disregarding tiny and harmless movements within the farm.
The system tracks the path of the animal intruding.

#### Hardware

1.      NodeMCU – Microcontroller that has an inbuilt WIFI shield
2.      Motion Sensor – Doppler motion Sensor or any sensor that uses Infrared rays to detect motion
3.      Web Camera – To record and track the visuals that is compatible with the laptop

#### Software (tools/languages/libraries)

1.      Coding language – Embedded C
2.      Arduino IDE
3.      OpenGL and OpenCV
4.      Ubuntu OS to set up MQTT terminals

#### Sample visual capture

The system was tested in two different settings, farms and office surveillance. The results of the system can be found in the Results-Farm and Results-surveillance directories. 

###### Note: The screenshots of the OpenGL graphic display aren't included

#### Code definition

1.   Run.sh - shell code that orchestrates the entire process
2.   publisher.c - code to publish the data to MQTT server
3.   subscriber.c - code to receive the data from MQTT server
4.   Data.h -  file containing all the used data structures
5.   Pipe.h - code to initialize a pipe communication between the device and MQTT server
6.   nocvp.cpp - code to capture and analyze the video feed received from the cameras
7.   main.c - code to control and manipulate the camera  movements
8.   Embedded Code - Code for the embedded MCUs for the motion sensor node and the camera node. 
