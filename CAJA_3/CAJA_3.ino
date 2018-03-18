      
  /* *********************************************************************
   *                      LIBRERIAS y CONSTANTES
   * *********************************************************************/    
   
  /***** LCD ******/  
  
  #include <LiquidCrystal.h>  
  LiquidCrystal lcd(12, 11, 5, 4, 3, 2);            
  
  
  /***** MODULO GPS ******/ 
  
  #include <SoftwareSerial.h>
  SoftwareSerial mySerial(51, 38); // RX, TX
  
  #include "TinyGPS.h"
  TinyGPS gps;
  
  #define GPS_TX_DIGITAL_OUT_PIN 31
  #define GPS_RX_DIGITAL_OUT_PIN 43
  
  #define MAX_BUFFER 500
 
  unsigned long momento_anterior=0;
  unsigned long bytes_recibidos=0;
  
  long startMillis;
  long secondsToFirstLocation = 0;
  
  float latitude = 0.0;
  float longitude = 0.0;

  char latit[12];
  char longi[12];
  
  
  /****** MODULO RELOJ ******/
  
  #include <Wire.h>
  #include "RTClib.h"
  RTC_DS1307 RTC; 

  /****** MODULO SONIDO ******/  
  
  float audio;
       
  
  /****** LEDS ******/
  
  
  int circ=45, heat=33, alert=35;
  
  
  /****** HUMEDAD Y TEMPERATURA ******/
  
  #include "DHT.h" //cargamos la librerÃ­a DHT
  #define DHTPIN 7 //Seleccionamos el pin en el que se //conectarÃ¡ el sensor
  #define DHTTYPE DHT11 //Se selecciona el DHT11 (hay //otros DHT)
  DHT dht(DHTPIN, DHTTYPE); //Se inicia una variable que serÃ¡ usada por Arduino para comunicarse con el sensor
  float h;
  float t;
 
  
  /****** GASES ******/ 
  float MQ136;     
  float MQ137;
  float MQ2; 
  float MG8; 
  float MQ7;
  float MQ138;
  
  /******  LUZ ******/
//      #include <Wire.h>
//      #include <BH1750.h>
//      BH1750 lightMeter;  
     
     /****** PRESION BAROMETRICA ******/
      // Se importan las librerÃ­as
//      #include <SFE_BMP180.h>
//      #include <Wire.h>
//      
      //Se declara una instancia de la librerÃ­a
   //   SFE_BMP180 pressure;
      
      //Se declaran las variables. Es necesario tomar en cuenta una presiÃ³n inicial
      //esta serÃ¡ la presiÃ³n que se tome en cuenta en el cÃ¡lculo de la diferencia de altura
      double PresionBase;
      
      
      //Leeremos presiÃ³n y temperatura. Calcularemos la diferencia de altura
      double Presion = 0;
      double Altura = 0;
      double Temperatura = 0;
      
      char status;
      
       /****** CONTADOR SET POINT *****/
    const int boton = 2; // Interrupcion 0
    volatile  int contador=0;
    int lastInt = millis();
    char archivo[80];
    char ndata[5]="dato";
    char fdata[5]="sen";
    char edata[5]=".csv";
    
       /****** MODULO SD ******/
       #include <SD.h>     
       File datos;   
      
    
    
/* *********************************************************************
*                   FUNCION DE CONFIGURACION INICIAL
* *********************************************************************/
    void setup()  {
      
       Serial.begin(9600);  
       Serial.println("Iniciando..");   
       mySerial.begin(9600);
       lcd.begin(20, 4);
       dht.begin(); //Se inicia el sensor
       
       pinMode(circ, OUTPUT);
       pinMode(heat, OUTPUT); 
       pinMode(alert, OUTPUT);       
  
       pinMode(GPS_TX_DIGITAL_OUT_PIN, INPUT);
       pinMode(GPS_RX_DIGITAL_OUT_PIN, INPUT);          
       
        Serial.begin(9600); // Establece la velocidad de datos del puerto serie
        //
       attachInterrupt(4, SetPoint, FALLING);
       
        ///////////////////////////////////////////////////
        
       Wire.begin();
       RTC.begin();
       RTC.adjust(DateTime(__DATE__, __TIME__)); 
       //RTC.adjust(DateTime(2015, 10, 30, 9, 32, 0));
     //  lightMeter.begin();
    //   SensorStart();
        
        
        Serial.print("Initializing SD card...");
      
         pinMode(53, OUTPUT);
         
        if (!SD.begin(53)) {
          Serial.println("initialization failed!");
          return;
        }
        Serial.println("initialization done.");  
     }
    
    
    /* *********************************************************************
      *                         FUNCION PRINCIPAL
     * *********************************************************************/
       void SetPoint() {         
           if((millis()-lastInt)>500) {   
         // GetSD();
          contador++;      
     
          lastInt = millis();
       }
       
       }
    
    void loop() {
            
      
      GetHT();      
      GetMQ136(); 
      GetMQ137(); 
      GetMQ2();
      GetMG8();
      GetMQ7();
      GetMQ138();
      GetAUDIO();
      Getluz();
      GetPresion();     
      GetTiempo();    
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
      contador=0;
    break;
  }
      
       
    
      //////////////////////////////////////////////////////    
      
  //Enviamos por el puerto serie cada 3s
     delay(1000); 
          
   
      }       
      
  
     
  
    /* *********************************************************************
     *                     FUNCIONES ADICIONALES
     * *********************************************************************/

     void GetSD() {   
     
       datos = SD.open("databox1.csv", FILE_WRITE);
        
        // if the file opened okay, write to it:
        if (datos) {
          Serial.print("Writing to test.txt..."); 
            DateTime now = RTC.now();
          datos.print(contador); 
           datos.print(",");         
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
          datos.print(",");        
          datos.print(MQ136);
          datos.print(","); 
          datos.print(MQ7);
          datos.print(",");  
          datos.print(MQ2);
          datos.print(",");
          datos.print(MQ138);
          datos.print(",");
          datos.print(MG8);
          datos.print(","); 
          datos.print(MQ137);
          datos.print(",");
          datos.print(audio);
//          datos.print(",");
//          datos.print(lightMeter.readLightLevel());
          datos.print(",");  
          datos.print(Presion);                 
                
          datos.close();
          Serial.println("Hecho.");
      	// close the file:
          datos.close();
          Serial.println("done.");
        } else {
          // if the file didn't open, print an error:
          Serial.println("error opening test.txt");
        }
        
        // re-open the file for reading:
        datos = SD.open("databox1.csv");
        if (datos) {
          Serial.println("databox1.csv");
          
          // read from the file until there's nothing else in it:
          while (datos.available()) {
          	Serial.write(datos.read());
          }
          // close the file:
          datos.close();
        } else {
        	// if the file didn't open, print an error:
          Serial.println("error opening test.txt");
        }
      
     
     
     }
     
     
     void GetSD2() {   
     
       datos = SD.open("databox2.csv", FILE_WRITE);
        
        // if the file opened okay, write to it:
        if (datos) {
          Serial.print("Writing to test.txt..."); 
            DateTime now = RTC.now();
          datos.print(contador); 
           datos.print(",");         
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
          datos.print(",");        
          datos.print(MQ136);
          datos.print(","); 
          datos.print(MQ7);
          datos.print(",");  
          datos.print(MQ2);
          datos.print(",");
          datos.print(MQ138);
          datos.print(",");
          datos.print(MG8);
          datos.print(","); 
          datos.print(MQ137);
          datos.print(",");
          datos.print(audio);
//          datos.print(",");
//          datos.print(lightMeter.readLightLevel());
          datos.print(",");  
          datos.print(Presion);                 
                
          datos.close();
          Serial.println("Hecho.");
      	// close the file:
          datos.close();
          Serial.println("done.");
        } else {
          // if the file didn't open, print an error:
          Serial.println("error opening test.txt");
        }
        
        // re-open the file for reading:
        datos = SD.open("databox2.csv");
        if (datos) {
          Serial.println("databox2.csv");
          
          // read from the file until there's nothing else in it:
          while (datos.available()) {
          	Serial.write(datos.read());
          }
          // close the file:
          datos.close();
        } else {
        	// if the file didn't open, print an error:
          Serial.println("error opening test.txt");
        }
      
     
     
     }
     
     void GetSD3() {   
     
       datos = SD.open("databox3.csv", FILE_WRITE);
        
        // if the file opened okay, write to it:
        if (datos) {
          Serial.print("Writing to test.txt..."); 
            DateTime now = RTC.now();
          datos.print(contador); 
           datos.print(",");         
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
          datos.print(",");        
          datos.print(MQ136);
          datos.print(","); 
          datos.print(MQ7);
          datos.print(",");  
          datos.print(MQ2);
          datos.print(",");
          datos.print(MQ138);
          datos.print(",");
          datos.print(MG8);
          datos.print(","); 
          datos.print(MQ137);
          datos.print(",");
          datos.print(audio);
//          datos.print(",");
//          datos.print(lightMeter.readLightLevel());
          datos.print(",");  
          datos.print(Presion);                 
                
          datos.close();
          Serial.println("Hecho.");
      	// close the file:
          datos.close();
          Serial.println("done.");
        } else {
          // if the file didn't open, print an error:
          Serial.println("error opening test.txt");
        }
        
        // re-open the file for reading:
        datos = SD.open("databox3.csv");
        if (datos) {
          Serial.println("databox3.csv");
          
          // read from the file until there's nothing else in it:
          while (datos.available()) {
          	Serial.write(datos.read());
          }
          // close the file:
          datos.close();
        } else {
        	// if the file didn't open, print an error:
          Serial.println("error opening test.txt");
        }
      
     
     
     }
     
     void GetSD4() {   
     
       datos = SD.open("databox4.csv", FILE_WRITE);
        
        // if the file opened okay, write to it:
        if (datos) {
          Serial.print("Writing to test.txt..."); 
            DateTime now = RTC.now();
          datos.print(contador); 
           datos.print(",");         
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
          datos.print(",");        
          datos.print(MQ136);
          datos.print(","); 
          datos.print(MQ7);
          datos.print(",");  
          datos.print(MQ2);
          datos.print(",");
          datos.print(MQ138);
          datos.print(",");
          datos.print(MG8);
          datos.print(","); 
          datos.print(MQ137);
          datos.print(",");
          datos.print(audio);
//          datos.print(",");
//          datos.print(lightMeter.readLightLevel());
          datos.print(",");  
          datos.print(Presion);                 
                
          datos.close();
          Serial.println("Hecho.");
      	// close the file:
          datos.close();
          Serial.println("done.");
        } else {
          // if the file didn't open, print an error:
          Serial.println("error opening test.txt");
        }
        
        // re-open the file for reading:
        datos = SD.open("databox4.csv");
        if (datos) {
          Serial.println("databox4.csv");
          
          // read from the file until there's nothing else in it:
          while (datos.available()) {
          	Serial.write(datos.read());
          }
          // close the file:
          datos.close();
        } else {
        	// if the file didn't open, print an error:
          Serial.println("error opening test.txt");
        }
      
     
     
     }
     
     void GetSD5() {   
     
       datos = SD.open("databox5.csv", FILE_WRITE);
        
        // if the file opened okay, write to it:
        if (datos) {
          Serial.print("Writing to test.txt..."); 
            DateTime now = RTC.now();
          datos.print(contador); 
           datos.print(",");         
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
          datos.print(",");        
          datos.print(MQ136);
          datos.print(","); 
          datos.print(MQ7);
          datos.print(",");  
          datos.print(MQ2);
          datos.print(",");
          datos.print(MQ138);
          datos.print(",");
          datos.print(MG8);
          datos.print(","); 
          datos.print(MQ137);
          datos.print(",");
          datos.print(audio);
//          datos.print(",");
//          datos.print(lightMeter.readLightLevel());
          datos.print(",");  
          datos.print(Presion);                 
                
          datos.close();
          Serial.println("Hecho.");
      	// close the file:
          datos.close();
          Serial.println("done.");
        } else {
          // if the file didn't open, print an error:
          Serial.println("error opening test.txt");
        }
        
        // re-open the file for reading:
        datos = SD.open("databox5.csv");
        if (datos) {
          Serial.println("databox5.csv");
          
          // read from the file until there's nothing else in it:
          while (datos.available()) {
          	Serial.write(datos.read());
          }
          // close the file:
          datos.close();
        } else {
        	// if the file didn't open, print an error:
          Serial.println("error opening test.txt");
        }
      
     
     
     }
       
        void GetHT() {
          
                    h = dht.readHumidity(); //Se lee la humedad
                    t = dht.readTemperature(); //Se lee la temperatura
                    //Se imprimen las variables
                    lcd.clear();
                    Serial.println("Humedad: "); 
                    Serial.println(h);
                    Serial.println("Temperatura: ");
                    Serial.println(t);
                    delay(2000); //Se espera 2 segundos para seguir leyendo //datos       
                    lcd.setCursor(0, 0);
                    lcd.print("H:"); 
                    lcd.print(h);
                    lcd.print(" ");                     
                    lcd.print("T:");                     
                    lcd.print(t);  
                    lcd.setCursor(0, 1);
                    lcd.print("M2:"); 
                    lcd.print(MQ2);
                    lcd.print(" ");                
                    lcd.print("M136:"); 
                    lcd.print(MQ136);
                    lcd.setCursor(0, 2);
                    lcd.print("M137:"); 
                    lcd.print(MQ137); 
                    lcd.setCursor(10, 2);
                    lcd.print("MG8:"); 
                    lcd.print(MG8); 
                    lcd.setCursor(0, 3);
                    lcd.print("M7:"); 
                    lcd.print(MQ7);
                    lcd.print(" M138:"); 
                    lcd.print(MQ138);                    
                    delay(2000);            
       
          }
       
       
       void GetMQ136() {
         int sensorM136 = analogRead(A3);
         // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
         MQ136 = sensorM136 * (5.0 / 1023.0);
         // print out the value you read:
         //Serial.println(MQ136);
         //delay(1000);
       
       }
       
         void GetMQ137() {
         int sensorM137 = analogRead(A7);
         // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
         MQ137 = sensorM137 * (5.0 / 1023.0);
         // print out the value you read:
         //Serial.println(MQ137);
         //delay(1000);
       }
       
        void GetMQ2() {
         int sensorM2 = analogRead(A1);
         // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
         MQ2 = sensorM2 * (5.0 / 1023.0);
         // print out the value you read:
         //Serial.println(MQ2);
         //delay(1000);
       
       }
       
       
      void GetMG8() {
         int sensorM8 = analogRead(A6);
         // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
         MG8 = sensorM8 * (5.0 / 1023.0);
         // print out the value you read:
         //Serial.println(MG8);
         //delay(1000);
       
       }
       
      void GetMQ7() {
         int sensorM7 = analogRead(A0);
         // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
         MQ7 = sensorM7 * (5.0 / 1023.0);
         // print out the value you read:
         //Serial.println(MQ7);
         //delay(1000);
       
       }
     
       
       
       void GetMQ138() {
         int sensorM138 = analogRead(A2);
         // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
         MQ138 = sensorM138 * (5.0 / 1023.0);
         // print out the value you read:
         //Serial.println(MQ138);
         //delay(1000);
       
       }
       
       
       void GetAUDIO() {
         int sensorValue = analogRead(A7);
         // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
         audio = sensorValue * (5.0 / 1023.0);
         // print out the value you read:
         //Serial.println(audio);
         //delay(1000);
       
       }
       
       void Getluz() {
        //uint16_t lux = lightMeter.readLightLevel();
        //Serial.print("Light: ");
        //Serial.print(lux);
        //Serial.println("lx");
        //delay(1000);
       
       }
       
       void GetPresion() {
            //Se hace lectura del sensor
            //  ReadSensor();
              //Se imprimen las variables
              //Serial.println(" ////// ");
              //Serial.print("Temperatura: ");
              //Serial.print(Temperatura);
              //Serial.println(" grados C");
            
              //Serial.print("Presion: ");
              //Serial.print(Presion);
              //Serial.println(" milibares");
            
              //Serial.print("Altura relativa: ");
              //Serial.print(Altura);
              //Serial.println(" metros");
            
              //delay(2000);
             
             }
             
             
      //       void SensorStart() {
      //
      //  //Secuencia de inicio del sensor
      //
      //  if (pressure.begin())
      //    Serial.println("BMP180 init success");
      //  else
      //  {
      //
      //    //Serial.println("BMP180 init fail (disconnected?)\n\n");
      //    while (1);
      //  }
      //
      //  //Se inicia la lectura de temperatura
      //  status = pressure.startTemperature();
      //  if (status != 0)  {
      //    delay(status);
      //    //Se lee una temperatura inicial
      //    status = pressure.getTemperature(Temperatura);
      //    if (status != 0)    {
      //      //Se inicia la lectura de presiones
      //      status = pressure.startPressure(3);
      //      if (status != 0)      {
      //        delay(status);
      //        //Se lee la presiÃ³n inicial incidente sobre el sensor en la primera ejecuciÃ³n
      //        status = pressure.getPressure(PresionBase, Temperatura);
      //      }
      //    }
      //  }
      //}
      //       
      //       
        
      //void ReadSensor() {
      //  //En este mÃ©todo se hacen las lecturas de presiÃ³n y temperatura y se calcula la altura
      //
      //  //Se inicia la lectura de temperatura
      //  status = pressure.startTemperature();
      //  if (status != 0)
      //  {
      //    delay(status);
      //    //Se realiza la lectura de temperatura
      //    status = pressure.getTemperature(Temperatura);
      //    if (status != 0)
      //    {
      //      //Se inicia la lectura de presiÃ³n
      //      status = pressure.startPressure(3);
      //      if (status != 0)
      //      {
      //        delay(status);
      //        //Se lleva a cabo la lectura de presiÃ³n,
      //        //considerando la temperatura que afecta el desempeÃ±o del sensor
      //
      //        status = pressure.getPressure(Presion, Temperatura);
      //        if (status != 0)
      //        {
      //          //Se hace el cÃ¡lculo de la altura en base a la presiÃ³n leÃ­da en el Setup
      //          Altura = pressure.altitude(Presion, PresionBase);
      //        }
      //        else Serial.println("error en la lectura de presion\n");
      //      }
      //      else Serial.println("error iniciando la lectura de presion\n");
      //    }
      //    else Serial.println("error en la lectura de temperatura\n");
      //  }
      //  else Serial.println("error iniciando la lectura de temperatura\n");
      //
      //}     
      //                  
   void GetTiempo() {

        DateTime now = RTC.now();       
        Serial.print(now.day(), DEC);
        Serial.print('/');      
        Serial.print(now.month(), DEC);
        Serial.print('/');      
        Serial.print(now.year(), DEC);
        Serial.print(' ');     
        Serial.print(now.hour(), DEC);
        Serial.print(':');        
        Serial.print(now.minute(), DEC);
        Serial.print(':');        
        Serial.print(now.second(), DEC);
        Serial.println();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Fecha:");
        lcd.print(now.day(), DEC);
        lcd.print("/"); 
        lcd.print(now.month(), DEC);
        lcd.print("/");
        lcd.print(now.year(), DEC);
        lcd.setCursor(0, 1);  
        lcd.print("Hora:");      
        lcd.print(now.hour(), DEC);
        lcd.print(":");  
        lcd.print(now.minute(), DEC); 
        lcd.print(":");
        lcd.print(now.second(), DEC);  
        lcd.setCursor(0, 2);
        lcd.print("AU:"); 
        lcd.print(audio);
//        lcd.setCursor(8, 2);
//        lcd.print("LUX:"); 
//        lcd.print(lightMeter.readLightLevel());
        lcd.setCursor(0, 3);
        lcd.print("PB:");
        lcd.print(Presion);
        delay(3000);
        lcd.setCursor(11, 2);// Arreglo Set Point
        lcd.print("Punto:"); 
        lcd.print(contador);
        
     
    }
                   
      
