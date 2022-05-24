//Proyecto para NodeMCU y Aplicacion Blynk

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//Codigo de autenticacion de BLynk
char auth[] = "e05b08f4e7e542a9b40b46b885948aa6";

//Configuracion Wifi
char ssid[] = "FT MARTU04";
char pass[] = "01420289243";

//Salidas
int pinRojo = 14;   //LED Rojo,   Pin D5
int pinVerde = 2;  //LED Verde, Pin D4
int pinAzul = 0;  //LED Azul,  Pin D3

//Arrays colores
int negro[3]  = { 0, 0, 0 };
int blanco[3]  = { 100, 100, 100 };
int rojo[3]    = { 100, 0, 0 };
int verde[3]  = { 0, 100, 0 };
int azul[3]   = { 0, 0, 100 };
int amarillo[3] = { 40, 95, 0 };
int dimBlanco[3] = { 30, 30, 30 };
int violeta[3] = { 60, 0, 100 };
int naranja[3] = { 100, 35, 0 };
int rosa[3] = { 100, 0, 40 };

//Inicializacion de colores
int valorRojo = negro[0];
int valorVerde = negro[1];
int valorAzul = negro[2];

int wait = 10;
int hold = 0;
int DEBUG = 1;
int contadorLoop = 60;
int repetir = 999999999;
int j = 0;

//Variables extras
int pinColorFade = 16; //Pin D0
int pinVelocidadRandom = A0; //Pin A0
int pinColorRandom = 4; //Pin D2
int pinColorRandomRapido = 12; //Pin D6
int pinColorRandomLento = 3; //Pin D9
int pinBlancoTenue = 13; //Pin D7
int pinLuzTecho = 15; //Pin D8

int colorFade = LOW;
int blancoTenue = LOW;
int luzTecho = LOW;
int colorRandom = LOW;
int colorRandomRapido = LOW;
int colorRandomLento = LOW;
int velocidad_random = 0;

int banderaColorFade = 0;
int banderaBlancoTenue = 0;
int banderaLuzTecho = 0;
int banderaColorRandom = 0;
int i = 0;

//Inicializacion de variables de colores
int prevRojo = valorRojo;
int prev_verde = valorVerde;
int prevAzul = valorAzul;

void setup()
{
  pinMode(pinRojo, OUTPUT);
  pinMode(pinVerde, OUTPUT);
  pinMode(pinAzul, OUTPUT);

  if(DEBUG)
  {
    Serial.begin(9600);
  }
  
  Blynk.begin(auth, ssid, pass);
  //Se puede especificar un servidor
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,60), 80);

  
  //Los virtual pin se pueden usar tanto de lectura como escritura
  //En este caso se esta usando de escritura para poder escribir
  //en una terminal de la aplicacion Blynk
}

void loop()
{
  //Toma los valores de los pines
  //Estos son modificados desde la aplicacion Blynk
  luzTecho = digitalRead(pinLuzTecho);
  blancoTenue = digitalRead(pinBlancoTenue);
  colorFade = digitalRead(pinColorFade);
  colorRandom = digitalRead(pinColorRandom);
  colorRandomRapido = digitalRead(pinColorRandomRapido);
  colorRandomLento = digitalRead(pinColorRandomLento);

  //velocidad_random = analogRead(pinLuzTecho);

  //Esperara cada valor de las variables de los pines en este caso digitales
  //TODO: pasar todo a pines virtuales
  banderaLuzTecho = luzTecho_f(banderaLuzTecho);
  banderaBlancoTenue = blancoTenue_f(banderaBlancoTenue, banderaColorFade);
  banderaColorFade = colorFade_f(banderaColorFade);
  banderaColorRandom = fnColoresRandom(banderaColorRandom);

  //Serial.println(velocidad_random);
  
  Blynk.run();
  
  delay(100);
}

int luzTecho_f(int bandera)
{
  if(!luzTecho && !bandera)
  {
    Serial.print("Luz Apagada - ");
      
    Serial.print("Pin luzTecho: ");
    Serial.println(luzTecho);
    
    bandera = 1;
  }
  else
  {
    if(luzTecho && bandera)
    {
      Serial.print("Luz Prendida - ");
      
      Serial.print("Pin luzTecho: ");
      Serial.println(luzTecho);

      bandera = 0;
    }
  }
  
  return bandera;
}

int blancoTenue_f(int bandera1, int bandera2)
{
  if(!blancoTenue && !bandera1 && bandera2)
  {
    digitalWrite(pinRojo, LOW);
    digitalWrite(pinVerde, LOW);
    digitalWrite(pinAzul, LOW);
    
    bandera1 = 1;

    Serial.print("Blanco Tenue Apagado - ");

    Serial.print("Pin blancoTenue: ");
    Serial.println(blancoTenue);
  }
  else if(blancoTenue && bandera1)
  {
    analogWrite(pinRojo, 100);
    analogWrite(pinVerde, 100);
    analogWrite(pinAzul, 100);
    
    bandera1 = 0;
    
    Serial.print("Blanco Tenue Prendido - ");

    Serial.print("Pin blancoTenue: ");
    Serial.println(blancoTenue);
  }
  
  return bandera1;
}

int colorFade_f(int bandera)
{
  if(!colorFade && !bandera)
  {
    Serial.print("Color Fade Apagado - ");
    
    Serial.print("Pin colorFade: ");
    Serial.println(colorFade);
    
    digitalWrite(pinRojo, LOW);
    digitalWrite(pinVerde, LOW);
    digitalWrite(pinAzul, LOW);

    bandera = 1;    
  }
  else if(colorFade && bandera)
  {
    if(repetir)
    {
      j = j + 1;
      
      Serial.print("Repeticion Color Fade: ");
      Serial.println(j);
      
      if (j >= repetir)
      {
        bandera = 0;

        digitalWrite(pinRojo, LOW);
        digitalWrite(pinVerde, LOW);
        digitalWrite(pinAzul, LOW);
        
        exit(j);
      }
    }

    Serial.print("Color Fade Prendido - ");
    
    Serial.print("Pin colorFade: ");
    Serial.println(colorFade);

    bandera = 1;

    Serial.println("BLANCO");
    crossFade(blanco);
    Serial.println("VIOLETA");
    crossFade(violeta);
    Serial.println("AZUL");
    crossFade(azul);
    //Serial.println("ROSA");
    //crossFade(rosa);
    Serial.println("ROJO");
    crossFade(rojo);
    Serial.println("NARANJA");
    crossFade(naranja);
    Serial.println("VERDE");
    crossFade(verde);
  }
  
  return bandera;
}

int fnColoresRandom(int bandera)
{
  int tiempo = 1000;
  
  if(!colorRandom && !bandera)
  {
    Serial.print("Color Random Apagado - ");

    Serial.print("Pin colorRandom: ");
    Serial.println(colorRandom);
    
    bandera = 1;
    
    digitalWrite(pinRojo, LOW);
    digitalWrite(pinVerde, LOW);
    digitalWrite(pinAzul, LOW);
  }
  else if(colorRandom && bandera)
  {
    Serial.print("Color Random Prendido - ");

    Serial.print("Pin colorRandom: ");
    Serial.println(colorRandom);

    for(i = 0; i < repetir; i++)
    {
      colorRandom = digitalRead(pinColorRandom);
      colorRandomRapido = digitalRead(pinColorRandomRapido);
      colorRandomLento = digitalRead(pinColorRandomLento);

      if(colorRandomLento)
      {
        tiempo = 450;
      }
      else if(colorRandomRapido)
      {
        tiempo = 100;
      }
      

      Blynk.run();

      if(!colorRandom)
      {
        digitalWrite(pinRojo, LOW);
        digitalWrite(pinVerde, LOW);
        digitalWrite(pinAzul, LOW);
        
        break;
      }
      else
      {
        int numero1 = random(100,1024);
        int numero2 = random(100,1024);
        int numero3 = random(100,1024);
                  
        analogWrite(pinRojo, numero1);
        analogWrite(pinVerde, numero2);
        analogWrite(pinAzul, numero3);
  
        Serial.print("Loop/RGB: #");
        Serial.print(i);
        Serial.print(" | ");
        Serial.print(numero1);
        Serial.print(" / ");
        Serial.print(numero2);
        Serial.print(" / ");  
        Serial.println(numero3);
      }
      
      delay(tiempo);
    }
    
    bandera = 0;
  }

  return bandera;
}

int calcularStep(int valorPrevio, int valorFinal)
{
  int step = valorFinal - valorPrevio;

  if(step)
  {
    step = 1020 / step;
  }
  
  return step;
}

int calcularValor(int step, int valor, int i)
{
  if((step) && i % step == 0)
  {
    if(step > 0)
    {
      valor = valor + 1;
    }
    else if(step < 0)
    {
      valor = valor - 1;
    }
  }

  if(valor > 255)
  {
    valor = 255;
  }
  else if(valor < 0)
  {
    valor = 0;
  }
  return valor;
}

void crossFade(int color[3])
{
  // Conversor de 0 a 255
  int rojo = (color[0] * 255) / 100;
  int verde = (color[1] * 255) / 100;
  int azul = (color[2] * 255) / 100;

  int stepRojo = calcularStep(prevRojo, rojo);
  int stepVerde = calcularStep(prev_verde, verde);
  int stepAzul = calcularStep(prevAzul, azul);

  for (int i = 0; i <= 1020; i++)
  {
    colorFade = digitalRead(pinColorFade);

    Blynk.run();

    if(!colorFade)
    {
      digitalWrite(pinRojo, LOW);
      digitalWrite(pinVerde, LOW);
      digitalWrite(pinAzul, LOW);

      break;
    }
    
    valorRojo = calcularValor(stepRojo, valorRojo, i);
    valorVerde = calcularValor(stepVerde, valorVerde, i);
    valorAzul = calcularValor(stepAzul, valorAzul, i);

    analogWrite(pinRojo, valorRojo);
    analogWrite(pinVerde, valorVerde);
    analogWrite(pinAzul, valorAzul);

    delay(wait);

    if(DEBUG)
    {
      if(i == 0 or i % contadorLoop == 0)
      {
        Serial.print("Loop/RGB: #");
        Serial.print(i);
        Serial.print(" | ");
        Serial.print(valorRojo);
        Serial.print(" / ");
        Serial.print(valorVerde);
        Serial.print(" / ");
        Serial.println(valorAzul);
      }

      DEBUG = DEBUG + 1;
    }
  }

  prevRojo = valorRojo;
  prev_verde = valorVerde;
  prevAzul = valorAzul;

  delay(hold);
}
