// Referencas a Bibliotecas
#include <Servo.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"

// Pick analog outputs, for the UNO these three work well
// use ~560  ohm resistor between Red & Blue, ~1K for green (its brighter)
#define redpin 3
#define greenpin 5
#define bluepin 6
// set to false if using a common cathode LED
#define commonAnode true

//colores de clasificación 
#define AZUL 1
#define ROJO 2
#define AMARILLO 3
#define VERDE 4
#define ROSA 5
#define NARANJA 6


// our RGB -> eye-recognized gamma color
byte gammatable[256];
  Servo pistonrojo;
  Servo pistonamarillo;
  Servo pistonverde;
  Servo pistonazul;
  Servo tolva;

  
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// Variables globales
int estado=0;
int colorDelUsuario=0;
int cantidadDelUsuario=0;
 

void setup()
{
  // Tareas de configuración 
  pistonrojo.attach(13);
  pistonamarillo.attach(5);
  pistonverde.attach(7);
  pistonazul.attach(4);
  tolva.attach(8);
  
  //Conexion con el usuario
  Serial.begin(9600);
  while(!Serial) { ; }
  

  if (tcs.begin()) {
    //Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }

  // use these three pins to drive an LED
/*#if defined(ARDUINO_ARCH_ESP32)
  ledcAttachPin(redpin, 1);
  ledcSetup(1, 12000, 8);
  ledcAttachPin(greenpin, 2);
  ledcSetup(2, 12000, 8);
  ledcAttachPin(bluepin, 3);
  ledcSetup(3, 12000, 8);
#else
  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
#endif*/

  // thanks PhilB for this gamma table!
  // it helps convert RGB colors to what humans see
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;

    if (commonAnode) {
      gammatable[i] = 255 - x;
    } else {
      gammatable[i] = x;
    }
    
  }
}

#define LEERDATOS 0
#define RESETHW 1
#define PROCESARCUENTA 2
#define DESPLAZARCINTA 3

void loop()
{
  int color;
  int numCuentasSelecionadas;
  switch(estado){
  case LEERDATOS: 
    colorDelUsuario=leerDatoDelUsuario();
    cantidadDelUsuario=leerDatoDelUsuario();
    numCuentasSelecionadas=0;
    estado=RESETHW;
    break;
  case RESETHW:
    pistonazul.write(0);
    pistonamarillo.write(0);
    pistonrojo.write(0);
    pistonverde.write(0);
    tova.write(180);
    //void MotorDetenido(void);
    estado=PROCESARCUENTA;
    break;
  caso PROCESARCUENTA:
    tolva.write(148);
    color=0;
    color=readColor();
    if (color==colorDelUsuario){
      numCuentasSelecionadas++;
      switch (color) {
      case 1:
        pistonrojo.write(180);
        pistonverde.write(0);
        pistonamarillo.write(0);
        pistonazul.write(0);
        break;
      case 2:
        pistonrojo.write(0);
        pistonverde.write(0);
        pistonamarillo.write(180);
        pistonazul.write(0);
        break;
      case 3:
        pistonrojo.write(0);
        pistonverde.write(180);
        pistonamarillo.write(0);
        pistonazul.write(0);
        break;
      case 4:
        pistonrojo.write(0);
        pistonverde.write(0);
        pistonamarillo.write(0);
        pistonazul.write(180);
        break;
      }
      delay(4000);
    }
    estado=DESPLAZARCINTA;
    break;
  case DESPLAZARCINTA:
    //MotorEncendido();
    tolva.write(45);
    delay(2000);
    delay(2000);
    delay(2000);
    if (numCuentasSelecionadas < cantidadDelUsuario){
      estado=RESETHW;
    } else {
      estado=LEERDATOS;
    }
  } 
}
 
int leerDatoDelUsuario(){
  int dato;
  while( Serial.available()<= 0) {
    delay(20);
  } 
  dato=(int)Serial.read();
  return dato;
}

int readColor() {
  float red, green, blue;
  int colorr;
  
   //float red, green, blue;

    tcs.setInterrupt(false);  // turn on LED

    delay(60);  // takes 50ms to read

    tcs.getRGB(&red, &green, &blue);

    tcs.setInterrupt(true);  // turn off LED

    Serial.print("R:\t"); Serial.print(int(red));
    Serial.print("\tG:\t"); Serial.print(int(green));
    Serial.print("\tB:\t"); Serial.print(int(blue));

    //  Serial.print("\t");
    //  Serial.print((int)red, HEX); Serial.print((int)green, HEX); Serial.print((int)blue, HEX);
    Serial.print("\n");

    /* uint16_t red, green, blue, clear;
    
    tcs.setInterrupt(false);  // turn on LED
    
    delay(60);  // takes 50ms to read

    tcs.getRawData(&red, &green, &blue, &clear);
    tcs.setInterrupt(true);  // turn off LED
    Serial.print("C:\t"); Serial.print(int(clear)); 
    Serial.print("R:\t"); Serial.print(int(red)); 
    Serial.print("\tG:\t"); Serial.print(int(green)); 
    Serial.print("\tB:\t"); Serial.print(int(blue));
    Serial.println(); */

#if defined(ARDUINO_ARCH_ESP32)
    ledcWrite(1, gammatable[(int)red]);
    ledcWrite(2, gammatable[(int)green]);
    ledcWrite(3, gammatable[(int)blue]);

#else
    analogWrite(redpin, gammatable[(int)red]);
    analogWrite(greenpin, gammatable[(int)green]);
    analogWrite(bluepin, gammatable[(int)blue]);

#endif
        int r, g, b,colorr;
        r = int(red);
        g = int(green);
        b = int(blue);
        
         if (abs(r-64)<5 && abs(g-90)<5 && abs(b-110)<10) {//bola azul
            Serial.print("La bola es de color azul.");  
            colorr=AZUL;      
        }else if (abs(r-170)<20 && abs(g-50)<10 && abs(b-40)<10) {//bola roja
            Serial.print("La bola es de color rojo.");
            colorr=ROJO;
        } else if (abs(r-110)<10 && abs(g-100)<5 && abs(b-40)<5) {//bola amarilla
            Serial.print("La bola es de color amarilla.");
            colorr=AMARILLO;
        } else if (abs(r-70)<15 && abs(g-112)<15 && abs(b-70)<10) {//bola verde
            Serial.print("La bola es de color verde.");
            colorr=VERDE;
        } else if (abs(r-160)<20 && abs(g-40)<10 && abs(b-70)<5) {//bola rosa
            Serial.print("La bola es de color rosa.");
            colorr=ROSA;
        } else if (abs(r-140)<10 && abs(g-80)<10 && abs(b-50)<10) {//bola naranja
            Serial.print("La bola de color naranja.");
            colorr=NARANJA;
        } else {
            Serial.print("La bola es de un color no registrado.");
        }
  return colorr;  
}
