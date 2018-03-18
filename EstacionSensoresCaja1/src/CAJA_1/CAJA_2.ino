      
      /* *********************************************************************
       *                      LIBRERIAS y CONSTANTES
       * *********************************************************************/    
       
      /***** LCD ******/  
      
      #include <LiquidCrystal.h>  
      LiquidCrystal lcd(6, 5, 10, 9, 8, 3);// LiquidCrystal(rs, enable, d4, d5, d6, d7)       
      
      
      /***** MODULO GPS ******/ 
      
      #include <SoftwareSerial.h>
      SoftwareSerial mySerial(53, 31); // RX, TX
      
      #include "TinyGPS.h"
      TinyGPS gps;
      
      #define GPS_TX_DIGITAL_OUT_PIN 53
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
      
      
      /****** MODULO RELOJ ******/
      
      #include <Wire.h>
      #include "RTClib.h"
      RTC_DS1307 RTC; 
    
      /****** MODULO SONIDO ******/  
      
      int audio, lectura;
      
           /****** MODULO OZNO******/  
      
      int ozono, lecturaozono;
    
      
      /****** LEDS ******/
      
      
      int circ=37, heat=33, alert=35;
      
      
      /****** HUMEDAD Y TEMPERATURA ******/
      
      #include "DHT.h" //cargamos la librerÃ­a DHT
      #define DHTPIN 7 //Seleccionamos el pin en el que se conecta el sensor
      #define DHTTYPE DHT11 //Se selecciona el DHT11 (hay //otros DHT)
      DHT dht(DHTPIN, DHTTYPE); //Se inicia una variable que sera usada por Arduino para comunicarse con el sensor
      float h;
      float t;
     
      
      /****** GASES ******/ 
      float CO;     
      
      /****** AUDIO ******/ 
      float AU;  
      
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
      
       /****** MODULO RTC ******/
    
   #include <virtuabotixRTC.h> //	
// Creation of the Real Time Clock Object	
//SCLK -> 6, I/O -> 7, CE -> 8	
virtuabotixRTC myRTC(21, 20, 19);
    
    
     /* *********************************************************************
      *                   FUNCION DE CONFIGURACION INICIAL
      * *********************************************************************/
     
    void setup()  {
     //  Wire.begin();
   //    RTC.begin();
      // RTC.adjust(DateTime(__DATE__, __TIME__)); //Para ajustar con la hora del programador
     //  RTC.adjust(DateTime(2016, 01, 23, 11, 00, 0)); //Para ajustar de manera manual
       // seconds, minutes, hours, day of the week, day of the month, month, year
	
        myRTC.setDS1302Time(00, 21, 16, 6, 23, 1, 2016);
       
       
       Serial.begin(9600); // Establece la velocidad del puerto serial 
       Serial.println("Iniciando..");   
       mySerial.begin(9600);
       pinMode(53, OUTPUT);
       dht.begin(); //Se inicia el sensor
       pinMode(circ, OUTPUT);
       pinMode(heat, OUTPUT); 
       pinMode(alert, OUTPUT);       
       pinMode(GPS_TX_DIGITAL_OUT_PIN, INPUT);
       pinMode(GPS_RX_DIGITAL_OUT_PIN, INPUT);
       lcd.begin(16, 2);
        //Antirebote para pulsador por software  
       attachInterrupt(0, SetPoint, FALLING);
       pinMode(boton,INPUT);         
  
      /////////////////////////////////////////////////////////////////////////////////// 
        if (!SD.begin(53)) {
          Serial.println("Fallo microSD!");
          return;
        }
        Serial.println("Inicializacion microSD lista.");    
     }
    
    
    /* *********************************************************************
      *                         FUNCION PRINCIPAL
     * *********************************************************************/
    
    void loop() {
      
      // Start printing elements as individuals 
     myRTC.updateTime();	
Serial.print("Current Date / Time: "); 	
Serial.print(myRTC.dayofmonth); 	
Serial.print("/"); 	
Serial.print(myRTC.month); 	
Serial.print("/");	
Serial.print(myRTC.year);	
Serial.print(" ");	
Serial.print(myRTC.hours);	
Serial.print(":");	
Serial.print(myRTC.minutes);	
Serial.print(":");	
Serial.println(myRTC.seconds);	
// Delay so the program doesn't print non-stop
	
delay( 5000); 
            
   
      GetCO(); 
      GetGPS(); 
      GetOZONO();
      GetTiempo();
      GetHT();
      GetAudio();   
      Imprime_registros();
      Imprime_registros2();
      Imprime_coordenadas();
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
      
       
        
      }       
      
               
     
  
    /* *********************************************************************
     *                     FUNCIONES ADICIONALES
     * *********************************************************************/
     
   void GetSD() {            
      //  sprintf(archivo,"%s%s%s ",ndata,fdata,edata);        
        datos = SD.open("set.csv", FILE_WRITE);
          //datos = SD.open(archivo, FILE_WRITE);
        
        // if the file opened okay, write to it:
        if (datos) {
          
         
          Serial.print("Guardando datos en SD en locacion 1.txt...");
          datos.print(myRTC.dayofmonth, DEC);
          datos.print("/"); 
          datos.print(myRTC.month, DEC);
          datos.print("/"); 
          datos.print(myRTC.year, DEC);
          datos.print(","); 
          datos.print(myRTC.hours, DEC);
          datos.print(":");
          datos.print(myRTC.minutes, DEC);
          datos.print(":");
          datos.print(myRTC.seconds, DEC);
          datos.print(" ");
          datos.print(",");          
          datos.print(h);
          datos.print(",");
          datos.print(t);
          datos.print(",");
          datos.print(CO);
          datos.print(",");
          datos.print(audio);
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
     
       datos = SD.open("databox2.csv", FILE_WRITE);
        
        // if the file opened okay, write to it:
        if (datos) {
          
          Serial.print("Guardando datos en SD en locacion 1.txt...");
          datos.print(myRTC.dayofmonth, DEC);
          datos.print("/"); 
          datos.print(myRTC.month, DEC);
          datos.print("/"); 
          datos.print(myRTC.year, DEC);
          datos.print(","); 
          datos.print(myRTC.hours, DEC);
          datos.print(":");
          datos.print(myRTC.minutes, DEC);
          datos.print(":");
          datos.print(myRTC.seconds, DEC);
          datos.print(" ");
          datos.print(",");          
          datos.print(h);
          datos.print(",");
          datos.print(t);
          datos.print(",");
          datos.print(CO);
          datos.print(",");
          datos.print(audio);
          datos.print(",");
          datos.print(latit);
          datos.print(",");
          datos.println(longi);               
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
         
          Serial.print("Guardando datos en SD en locacion 1.txt...");
          datos.print(myRTC.dayofmonth, DEC);
          datos.print("/"); 
          datos.print(myRTC.month, DEC);
          datos.print("/"); 
          datos.print(myRTC.year, DEC);
          datos.print(","); 
          datos.print(myRTC.hours, DEC);
          datos.print(":");
          datos.print(myRTC.minutes, DEC);
          datos.print(":");
          datos.print(myRTC.seconds, DEC);
          datos.print(" ");
          datos.print(",");          
          datos.print(h);
          datos.print(",");
          datos.print(t);
          datos.print(",");
          datos.print(CO);
          datos.print(",");
          datos.print(audio);
          datos.print(",");
          datos.print(latit);
          datos.print(",");
          datos.println(longi); 
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
         
          Serial.print("Guardando datos en SD en locacion 1.txt...");
          datos.print(myRTC.dayofmonth, DEC);
          datos.print("/"); 
          datos.print(myRTC.month, DEC);
          datos.print("/"); 
          datos.print(myRTC.year, DEC);
          datos.print(","); 
          datos.print(myRTC.hours, DEC);
          datos.print(":");
          datos.print(myRTC.minutes, DEC);
          datos.print(":");
          datos.print(myRTC.seconds, DEC);
          datos.print(" ");
          datos.print(",");          
          datos.print(h);
          datos.print(",");
          datos.print(t);
          datos.print(",");
          datos.print(CO);
          datos.print(",");
          datos.print(audio);
          datos.print(",");
          datos.print(latit);
          datos.print(",");
          datos.println(longi);             
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
      
          Serial.print("Guardando datos en SD en locacion 1.txt...");
          datos.print(myRTC.dayofmonth, DEC);
          datos.print("/"); 
          datos.print(myRTC.month, DEC);
          datos.print("/"); 
          datos.print(myRTC.year, DEC);
          datos.print(","); 
          datos.print(myRTC.hours, DEC);
          datos.print(":");
          datos.print(myRTC.minutes, DEC);
          datos.print(":");
          datos.print(myRTC.seconds, DEC);
          datos.print(" ");
          datos.print(",");          
          datos.print(h);
          datos.print(",");
          datos.print(t);
          datos.print(",");
          datos.print(CO);
          datos.print(",");
          datos.print(audio);
          datos.print(",");
          datos.print(latit);
          datos.print(",");
          datos.println(longi); 
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

            
   void GetTiempo() {
                   
                  //  DateTime now = RTC.now();
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("FECHA:"); 
                    lcd.print(myRTC.dayofmonth); 
                    lcd.print("/");
                    lcd.print(myRTC.year); 
                    lcd.print("/");
                    lcd.print(myRTC.year);               
                    lcd.setCursor(0, 1);
                    lcd.print("HORA:"); 
                    lcd.print(myRTC.hours); 
                    lcd.print(":");                    
                    lcd.print(myRTC.minutes);
                    lcd.print(":");
                    lcd.print(myRTC.seconds); 
                    delay(2000);     
                                
        } 


        void GetHT() {
          
          h = dht.readHumidity();
          t = dht.readTemperature();                             
         
          }
       
       
         void GetCO() {
      
         int sensorValue = analogRead(A3);
         // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
         CO = sensorValue * (5.0 / 1023.0);
      
      }
       
         void GetAudio() {
      
         lectura = analogRead(A1);
         audio =  lectura * (5.0 / 1023.0);
               
       }
       
     void    GetOZONO(){
      
         lecturaozono = analogRead(A5);
         ozono =  lectura * (5.0 / 1023.0);
               
       }
  /********************Arreglo Set Point ****************/
       void SetPoint() {         
           if((millis()-lastInt)>500) {   
          contador++;      
          lastInt = millis();
           }
       }              
                                     
                                      
/********************Imprime en LCD ****************/

         void Imprime_coordenadas()  {
            
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("La:");
                    lcd.print(latit);
                    lcd.setCursor(0, 1);
                    lcd.print("Lo:");  
                    lcd.print(longi);
                    lcd.setCursor(12, 1);// Arreglo Set Point
                    lcd.print("SP:"); 
                    lcd.print(contador); 
                    delay(3000);               
        } 
        
        void Imprime_registros()  {
            
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("H:"); 
                    lcd.print(h); 
                    lcd.setCursor(0, 1);
                    lcd.print("T:");                     
                    lcd.print(t);  
                    lcd.setCursor(8, 0);
                    lcd.print("CO:"); 
                    lcd.print(CO); 
                    lcd.setCursor(8, 1);
                    lcd.print("AU:"); 
                    lcd.print(audio);                                     
                    delay(3000);        
        
        }
        
         void Imprime_registros2()  {
            lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("H:"); 
                    lcd.print(h); 
                    lcd.setCursor(0, 1);
                    lcd.print("T:");                     
                    lcd.print(t);    
                    lcd.setCursor(9, 0);
                    lcd.print("CO:"); 
                    lcd.print(CO);
                    lcd.setCursor(9, 1);// Arreglo Set Point
                    lcd.print("Oz:"); 
                    lcd.print(ozono);                    
                    delay(3000);        
         }
