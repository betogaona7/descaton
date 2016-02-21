# Lectura de datos desde un Arduino
#
# Basado en el ejemplo AnalogInOutSerial de Arduino (CC-BY-SA 3.0)
# http://arduino.cc/en/Tutorial/AnalogInOutSerial
#
# 2014 Juan Luis Cano <juanlu001@gmail.com>

 #!/usr/bin/python
 # -*- coding: ascii -*-
import os
import sys
import time
import serial
import io
import mysql.connector

try:
    arduino = serial.Serial('COM9', baudrate=9600, timeout=2.0)

    # Nota: provocamos un reseteo manual de la placa para leer desde
    # el principio, ver http://stackoverflow.com/a/21082531/554319
    arduino.setDTR(False)
    time.sleep(2)
    arduino.flushInput()
    arduino.setDTR(True)
    print('ok')

except (ImportError, serial.SerialException):
    #import io

    class FakeArduino(io.RawIOBase):
        """Clase para representar un "falso Arduino"
        """
        def readline(self):
            time.sleep(0.1)
            return b'sensor = 0\toutput = 0\r\n'

    arduino = FakeArduino()

with arduino:
    while True:
        try:
            matricula = ""
            line = arduino.readline()
            end = ""
            #print(line.decode('ascii', errors='replace'), end)
            #print line
            for letter in line:
                if(letter == '*'):
                    matricula = line[1:9]
                    cnx = mysql.connector.connect(user="root", password="", host="127.0.0.1", database="hack")
                    cursor = cnx.cursor()

                    query = ("SELECT discapacitado FROM conadis WHERE matricula = '%s'")
                    query2 = ("UPDATE conadis values multas=sum(multas,1) where matricula = '%s'")

                    cursor.execute(query, matricula)

                    for(discapacitado) in cursor:
                     print("Discapacitado = %s", discapacitado)
                     cursor.close()
                     cnx.close()
                    break
            print matricula
            matricula = ""
#Start mysql connection
            


        except KeyboardInterrupt:
            print("Exiting")
            break