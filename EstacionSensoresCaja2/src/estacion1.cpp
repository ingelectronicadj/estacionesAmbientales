/* *********************************************************************
 *                      LIBRERIAS y CONSTANTES
 * *********************************************************************/
 #include <Arduino.h>

 /****** MODULO RELOJ ******/
  #include <Wire.h>
  #include <Time.h>
  #include "RTClib.h"
  RTC_DS1307 RTC;


/***** LCD ******/
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);  // LiquidCrystal(rs, enable, d4, d5, d6, d7)


/***** MODULO GPS ******/
#include <SoftwareSerial.h>
SoftwareSerial mySerial(13, 31); // RX, TX
#include "TinyGPS.h"
TinyGPS gps;
#define GPS_TX_DIGITAL_OUT_PIN 13
#define GPS_RX_DIGITAL_OUT_PIN 31
#define MAX_BUFFER 500
unsigned long momento_anterior=0;
unsigned long bytes_recibidos=0;
long startMillis;
long secondsToFirstLocation = 0;
float latitude = 0.0;
float longitude = 0.0;
char latit[12];
char longi[12];

/****** INDICADORES LED ******/
int circ=22, heat=23, alert=6;


/****** HUMEDAD Y TEMPERATURA ******/
#include "DHT.h"
#define DHTPIN 6
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);
float h;
float t;


/****** UV ******/
float uvindex=0;

/****** GASES ******/
float CO, COV;

/****** dustVal ******/
int dustPin=0;
int dustVal=0;

int ledPower=3;
int delayTime=280;
int delayTime2=40;
float offTime=9680;


/****** CONTROL DE LECTURAS ******/
unsigned long time;
int alertaUV;

/****** CONTADOR SET POINT *****/
const int boton = 2; // Interrupcion 0
int valor=0;
volatile  int contador=0;
int estadoanteriorboton=0;
int lastInt = millis();
char archivo[80];
char ndata[5]="dato";
char fdata[5]="sen";
char edata[5]=".csv";

 /****** MODULO microSD ******/
#include <SD.h>
 File datos;

/***************FUNCIONES ADICIONALES AL SETUP**********/
 /********************Arreglo Set Point ****************/
void SetPoint() {
   if((millis()-lastInt)>500) {
   // GetSD();
    contador++;
    lastInt = millis();
    }
 }


/* *********************************************************************
*                   FUNCION DE CONFIGURACION INICIAL
* *********************************************************************/

void setup(){
   Wire.begin();
   RTC.begin();
   //Si quitamos el comentario de la linea siguiente, se ajusta la hora y la fecha con la del ordenador
   RTC.adjust(DateTime(__DATE__, __TIME__));

   Serial.begin(9600);
   Serial.println("Iniciando..");
   mySerial.begin(9600);
   pinMode(53, OUTPUT);
   dht.begin();
   pinMode(circ, OUTPUT);
   pinMode(heat, OUTPUT);
   pinMode(alert, OUTPUT);
   pinMode(GPS_TX_DIGITAL_OUT_PIN, INPUT);
   pinMode(GPS_RX_DIGITAL_OUT_PIN, INPUT);
   lcd.begin(16, 2);
    //Antirebote para pulsador por software
   attachInterrupt(0, SetPoint, FALLING);
   pinMode(boton,INPUT);

   ///////////////////////////////////////////////////////
    if (!SD.begin(53)) {
        Serial.println("Fallo microSD!");
        return;
      }
    Serial.println("Inicializacion microSD lista.");

}

/**********FUNCIONES ADICIONALES A LA FUNCION LOOP*******************/

void  alertas(){
  digitalWrite(alert, LOW);
  digitalWrite(circ, LOW);
  analogWrite(heat, 245);
  delay(14);
  analogWrite(heat, 0);
  delay(981);
  digitalWrite(circ, HIGH);
  delay(3);
}


/* *********************************************************************
*                     FUNCIONES ADICIONALES
* *********************************************************************/

void GetGPS() {
  bool newData = false;
  unsigned long chars = 0;
  unsigned short sentences, failed;

  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (mySerial.available())
    {
      int c = mySerial.read();
      ++chars;
      if (gps.encode(c))
        newData = true;
    }
  }

  if (newData) {
    if(secondsToFirstLocation == 0){
    secondsToFirstLocation = (millis() - startMillis) / 1000;
    }
    unsigned long age;
    gps.f_get_position(&latitude, &longitude, &age);
  }

  dtostrf(latitude, 6, 6, latit);
  dtostrf(longitude, 6, 6, longi);
}


void GetHT() {
  h = dht.readHumidity();
  t = dht.readTemperature();
    //Serial.print("Humedad: ");
    //Serial.print(h);
    //Serial.print("% ");
  }


 void GetUV() {
   unsigned int ADCValue;
   double Voltage;
   ADCValue = analogRead(2);
   Voltage = (ADCValue / 1023.0) * 5.0;
   uvindex = Voltage * 1000;
   //Serial.print("UV:");
   //Serial.println(uvindex);
 }

void GetCO() {
  float val1 = analogRead(A3);
  CO = val1 * (5.0 / 1023.0);
}

void GetCOV() {
  float val3 = analogRead(A4);
  COV = val3 * (5.0 / 1023.0);
}

void GetdustVal() {
  digitalWrite(ledPower,LOW); // Enciende LEd infrarojo
  delayMicroseconds(delayTime);
  dustVal=analogRead(dustPin); // Lee el valor analogica de la reflexion de las particulas de polvo
  delayMicroseconds(delayTime2);
  digitalWrite(ledPower,HIGH); // Apaga LEd infrarojo
  delayMicroseconds(offTime);
  delay(3000);
  Serial.println(dustVal);
 }


void GetSD() {
//  sprintf(archivo,"%s%s%s ",ndata,fdata,edata);
  datos = SD.open("set.csv", FILE_WRITE); //datos = SD.open(archivo, FILE_WRITE);
  // if the file opened okay, write to it:
  if (datos) {
    DateTime now = RTC.now();
    Serial.print("Guardando datos en SD en locacion 1.txt...");
    datos.print(now.day(), DEC);
    datos.print("/");
    datos.print(now.month(), DEC);
    datos.print("/");
    datos.print(now.year(), DEC);
    datos.print(",");
    datos.print(now.hour(), DEC);
    datos.print(":");
    datos.print(now.minute(), DEC);
    datos.print(":");
    datos.print(now.second(), DEC);
    datos.print(" ");
    datos.print(",");
    datos.print(h);
    datos.print(",");
    datos.print(t);
    datos.print(",");
    datos.print(uvindex);
    datos.print(",");
    datos.print(CO);
    datos.print(",");
    datos.print(COV);
    datos.print(",");
    datos.print(dustVal);
    datos.print(",");
    datos.print(latit);
    datos.print(",");
    datos.println(longi);
    datos.close();
    // close the file:
    Serial.println("Datos enviados  Localizacion 1");
  } else {
    // if the file didn't open, print an error:
    Serial.println("Error de almacenamiento");
  }

// re-open the file for reading:
  datos = SD.open("set.csv");
  if (datos) {
    Serial.println("set.csv");

    // read from the file until there's nothing else in it:
    while (datos.available()) {
      Serial.write(datos.read());
    }
    // close the file:
    datos.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening data.txt");
  }

}

void GetSD2() {
//  sprintf(archivo,"%s%s%s ",ndata,fdata,edata);
  datos = SD.open("set2.csv", FILE_WRITE);//datos = SD.open(archivo, FILE_WRITE);
  // if the file opened okay, write to it:
  if (datos) {
    DateTime now = RTC.now();
    Serial.print("Guardando datos en SD en locacion 2 ...");
    datos.print(now.day(), DEC);
    datos.print("/");
    datos.print(now.month(), DEC);
    datos.print("/");
    datos.print(now.year(), DEC);
    datos.print(",");
    datos.print(now.hour(), DEC);
    datos.print(":");
    datos.print(now.minute(), DEC);
    datos.print(":");
    datos.print(now.second(), DEC);
    datos.print(" ");
    datos.print(",");
    datos.print(h);
    datos.print(",");
    datos.print(t);
    datos.print(",");
    datos.print(uvindex);
    datos.print(",");
    datos.print(CO);
    datos.print(",");
    datos.print(COV);
    datos.print(",");
    datos.print(dustVal);
    datos.print(",");
    datos.print(latit);
    datos.print(",");
    datos.println(longi);
    datos.close();
    // close the file:
    Serial.println("Datos enviados  Localizacion 2");
  } else {
    // if the file didn't open, print an error:
    Serial.println("Error de almacenamiento");
  }

// re-open the file for reading:
  datos = SD.open("set2.csv");
  if (datos) {
    Serial.println("set2.csv");
    // read from the file until there's nothing else in it:
    while (datos.available()) {
      Serial.write(datos.read());
    }
    // close the file:
    datos.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening data.txt");
  }
}

void GetSD3() {
//  sprintf(archivo,"%s%s%s ",ndata,fdata,edata);
  datos = SD.open("set3.csv", FILE_WRITE); //datos = SD.open(archivo, FILE_WRITE);
  // if the file opened okay, write to it:
  if (datos) {
    DateTime now = RTC.now();
    Serial.print("Guardando datos en SD en locacion**.txt...");
    datos.print(now.day(), DEC);
    datos.print("/");
    datos.print(now.month(), DEC);
    datos.print("/");
    datos.print(now.year(), DEC);
    datos.print(",");
    datos.print(now.hour(), DEC);
    datos.print(":");
    datos.print(now.minute(), DEC);
    datos.print(":");
    datos.print(now.second(), DEC);
    datos.print(" ");
    datos.print(",");
    datos.print(h);
    datos.print(",");
    datos.print(t);
    datos.print(",");
    datos.print(uvindex);
    datos.print(",");
    datos.print(CO);
    datos.print(",");
    datos.print(COV);
    datos.print(",");
    datos.print(dustVal);
    datos.print(",");
    datos.print(latit);
    datos.print(",");
    datos.println(longi);
    datos.close();
    // close the file:
    Serial.println("Datos enviados Localizacion 3");
    } else {
    // if the file didn't open, print an error:
    Serial.println("Error de almacenamiento");
    }
// re-open the file for reading:
  datos = SD.open("set3.csv");
  if (datos) {
    Serial.println("set3.csv");
    // read from the file until there's nothing else in it:
    while (datos.available()) {
      Serial.write(datos.read());
    }
    // close the file:
    datos.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening data.txt");
  }
}

void GetSD4() {
//  sprintf(archivo,"%s%s%s ",ndata,fdata,edata);
  datos = SD.open("set4.csv", FILE_WRITE); //datos = SD.open(archivo, FILE_WRITE);
  // if the file opened okay, write to it:
  if (datos) {
    DateTime now = RTC.now();
    Serial.print("Guardando datos en SD en locacion 4.txt...");
    datos.print(now.day(), DEC);
    datos.print("/");
    datos.print(now.month(), DEC);
    datos.print("/");
    datos.print(now.year(), DEC);
    datos.print(",");
    datos.print(now.hour(), DEC);
    datos.print(":");
    datos.print(now.minute(), DEC);
    datos.print(":");
    datos.print(now.second(), DEC);
    datos.print(" ");
    datos.print(",");
    datos.print(h);
    datos.print(",");
    datos.print(t);
    datos.print(",");
    datos.print(uvindex);
    datos.print(",");
    datos.print(CO);
    datos.print(",");
    datos.print(COV);
    datos.print(",");
    datos.print(dustVal);
    datos.print(",");
    datos.print(latit);
    datos.print(",");
    datos.println(longi);
    datos.close();
    // close the file:
    Serial.println("Datos enviados Localizacion 4");
    } else {
    // if the file didn't open, print an error:
    Serial.println("Error de almacenamiento");
  }

  // re-open the file for reading:
  datos = SD.open("set4.csv");
  if (datos) {
    Serial.println("set4.csv");
    // read from the file until there's nothing else in it:
    while (datos.available()) {
      Serial.write(datos.read());
    }
    // close the file:
    datos.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening data.txt");
  }
}

void GetSD5() {
//  sprintf(archivo,"%s%s%s ",ndata,fdata,edata);
  datos = SD.open("set5.csv", FILE_WRITE);  //datos = SD.open(archivo, FILE_WRITE);
  // if the file opened okay, write to it:
  if (datos) {
    DateTime now = RTC.now();
    Serial.print("Guardando datos en SD en locacion 5.txt...");
    datos.print(now.day(), DEC);
    datos.print("/");
    datos.print(now.month(), DEC);
    datos.print("/");
    datos.print(now.year(), DEC);
    datos.print(",");
    datos.print(now.hour(), DEC);
    datos.print(":");
    datos.print(now.minute(), DEC);
    datos.print(":");
    datos.print(now.second(), DEC);
    datos.print(" ");
    datos.print(",");
    datos.print(h);
    datos.print(",");
    datos.print(t);
    datos.print(",");
    datos.print(uvindex);
    datos.print(",");
    datos.print(CO);
    datos.print(",");
    datos.print(COV);
    datos.print(",");
    datos.print(dustVal);
    datos.print(",");
    datos.print(latit);
    datos.print(",");
    datos.println(longi);
    datos.close();
    // close the file:
    Serial.println("Datos enviados Localizacion 5");
  } else {
    // if the file didn't open, print an error:
    Serial.println("Error de almacenamiento");
  }
  // re-open the file for reading:
  datos = SD.open("set5.csv");
  if (datos) {
    Serial.println("set5.csv");
    // read from the file until there's nothing else in it:
    while (datos.available()) {
      Serial.write(datos.read());
    }
    // close the file:
    datos.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening data.txt");
  }
}

void Imprime_registros() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("H:");
  lcd.print(h);
  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print(t);
  lcd.setCursor(9, 0);
  lcd.print("UV:");
  lcd.print(uvindex);
  lcd.setCursor(9, 1);// Arreglo Set Point
  lcd.print("Punto:");
  lcd.print(contador);
  delay(3000);
}

void Imprime_registros2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CO: ");
  lcd.print(CO);
  lcd.setCursor(0, 1);
  lcd.print("COV:");
  lcd.print(COV);
  lcd.setCursor(9, 0);
  lcd.print("MP:");
  lcd.print(dustVal);
  delay(3000);
}

void Imprime_registros3() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("La:");
  lcd.print(latit);
  lcd.setCursor(0, 1);
  lcd.print("Lo:");
  lcd.print(longi);
  delay(3000);
}


void Imprime_alertauv() {
  lcd.clear();
  lcd.setCursor(0, 0);//lcd.setCursor(col, row)
  lcd.print("ALERTA X UV: ");
  lcd.print(alertaUV);
  delay(3000);
}


void GetFecha() {
  DateTime now = RTC.now();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FECHA:");
  lcd.print(now.day());
  lcd.print("/");
  lcd.print(now.month());
  lcd.print("/");
  lcd.print(now.year());
  lcd.setCursor(0, 1);
  lcd.print("HORA:");
  lcd.print(now.hour());
  lcd.print(":");
  lcd.print(now.minute());
  lcd.print(":");
  lcd.print(now.second());
}

/* *********************************************************************
*                         FUNCION PRINCIPAL
* *********************************************************************/

void loop() {
  alertas();
  GetGPS();
  GetHT();
  GetUV();
  GetCO();
  GetCOV();
  GetdustVal();
  Imprime_registros();
  Imprime_registros2();
  Imprime_registros3();
  GetFecha();
    switch (contador) {
      case 1:
            GetSD();
            break;
      case 2:
            GetSD2();
            break;

      case 3:
            GetSD3();
            break;
      case 4:
            GetSD4();
            break;
      case 5:
            GetSD5();
            break;
      case 6:
            contador=0;
            break;
      default:
      // if nothing else matches, do the default
      // default is optional
      break;
    }
//Enviamos datos wirless cada segundo
delay(1000);
}
