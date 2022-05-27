//Proyecto para NodeMCU y Aplicacion Blynk
//Version 1.1.3

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <DHT.h>

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

int colorFade = LOW;
int blancoTenue = LOW;
int luzTecho = LOW;
int colorRandom = LOW;
int colorRandomRapido = LOW;
int colorRandomLento = LOW;
int resetMCU = LOW;

int banderaColorFade = 0;
int banderaBlancoTenue = 0;
int banderaLuzTecho = 0;
int banderaColorRandom = 0;
int i = 0;

//Inicializacion de variables de colores
int prevRojo = valorRojo;
int prev_verde = valorVerde;
int prevAzul = valorAzul;

//Variables para la fecha con NTPClient
const long utcOffsetInSeconds = -10800;

char daysOfTheWeek[7][12] = 
  {
    "Domingo", 
    "Lunes", 
    "Martes", 
    "Miercoles", 
    "Jueves", 
    "Viernes", 
    "Sabado"
  };

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "south-america.pool.ntp.org", utcOffsetInSeconds);
String concatStrDate = "";

//Variables para el Sensor de temperatura y Humedad

#define DHTPIN 15 //Pin Entrada, Pin D8
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

//Variables Virtuales Vx pudiendo setear hasta 255
int terminal = 0; //Pin V0

//Toma los valores de las variables virtuales
//Estos son modificados desde la aplicacion Blynk
BLYNK_WRITE(V3)  //Pin V3
{
  blancoTenue = param.asInt();
}

BLYNK_WRITE(V4)  //Pin V4
{
  colorRandom = param.asInt();
}

BLYNK_WRITE(V5)  //Pin V5
{
  colorRandomRapido = param.asInt();
}

BLYNK_WRITE(V6)  //Pin V6
{
  colorRandomLento = param.asInt();
}

BLYNK_WRITE(V7)  //Pin V7
{
  colorFade = param.asInt();
}

BLYNK_WRITE(V8)  //Pin V8
{
  luzTecho = param.asInt();
}

BLYNK_WRITE(V9)  //Pin V9
{
  resetMCU = param.asInt();
}

void setup()
{
  pinMode(pinRojo, OUTPUT);
  pinMode(pinVerde, OUTPUT);
  pinMode(pinAzul, OUTPUT);

  if(DEBUG)
  {
    Serial.begin(9600);
  }
  
  dht.begin();
  
  Blynk.begin(auth, ssid, pass);
  //Se puede especificar un servidor
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,60), 80);

  
  //Los virtual pin se pueden usar tanto de lectura como escritura
  //En este caso se esta usando de escritura para poder escribir
  //en una terminal de la aplicacion Blynk
  Blynk.virtualWrite(terminal, "clr");
  Blynk.virtualWrite(terminal, "Conectado al cuarto de Facu\n");

  //Inicia la conexion con la red para recibir la hora y mostrarla por la terminal
  timeClient.begin();
  timeClient.update();
  
  //Concatena en un string toda la informacion para mostrar el dia y la hora
  concatStrDate += daysOfTheWeek[timeClient.getDay()];
  concatStrDate += " - ";
  concatStrDate += timeClient.getHours();
  concatStrDate += ":";
  concatStrDate += timeClient.getMinutes();
  concatStrDate += ":";
  concatStrDate += timeClient.getSeconds();
  concatStrDate += "\n";

  Blynk.virtualWrite(terminal, "Ultimo dia y hora de ejecucion: ");
  Blynk.virtualWrite(terminal, concatStrDate);
}

void loop()
{
  if(resetMCU)
  {
    ESP.reset();
  }

  //Esperara cada valor de las variables de los pines en este caso digitales
  //TODO: pasar todo a pines virtuales
  banderaLuzTecho = fnLuzTecho(banderaLuzTecho);
  banderaBlancoTenue = fnBlancoTenue(banderaBlancoTenue, banderaColorFade);
  banderaColorFade = fnColorFade(banderaColorFade);
  banderaColorRandom = fnColoresRandom(banderaColorRandom);
  
  
  //TODO: Comprar un DHT11 y descomentar codigo
  /*float temp = fnGetTempHum();
  Blynk.virtualWrite(10, temp);
  Blynk.virtualWrite(terminal, temp);
  Blynk.virtualWrite(terminal, " C\n");*/ 
  
  Blynk.run();
  
  delay(100);
}

int fnLuzTecho(int bandera)
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

int fnBlancoTenue(int bandera1, int bandera2)
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

float fnGetTempHum()
{
  int h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)) 
  {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    Blynk.virtualWrite(terminal, "Error obteniendo los datos del sensor DHT11\n");
    delay(1000);
  }

  return t;
}

int fnColorFade(int bandera)
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
    fnCrossFade(blanco);
    Serial.println("VIOLETA");
    fnCrossFade(violeta);
    Serial.println("AZUL");
    fnCrossFade(azul);
    Serial.println("ROJO");
    fnCrossFade(rojo);
    Serial.println("NARANJA");
    fnCrossFade(naranja);
    Serial.println("VERDE");
    fnCrossFade(verde);
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

int fnCalcularStep(int valorPrevio, int valorFinal)
{
  int step = valorFinal - valorPrevio;

  if(step)
  {
    step = 1020 / step;
  }
  
  return step;
}

int fnCalcularValor(int step, int valor, int i)
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

void fnCrossFade(int color[3])
{
  // Conversor de 0 a 255
  int rojo = (color[0] * 255) / 100;
  int verde = (color[1] * 255) / 100;
  int azul = (color[2] * 255) / 100;

  int stepRojo = fnCalcularStep(prevRojo, rojo);
  int stepVerde = fnCalcularStep(prev_verde, verde);
  int stepAzul = fnCalcularStep(prevAzul, azul);

  for (int i = 0; i <= 1020; i++)
  {
    Blynk.run();

    if(!colorFade)
    {
      digitalWrite(pinRojo, LOW);
      digitalWrite(pinVerde, LOW);
      digitalWrite(pinAzul, LOW);
      
      valorRojo = 0;
      valorVerde = 0;
      valorAzul = 0;

      break;
    }
    
    valorRojo = fnCalcularValor(stepRojo, valorRojo, i);
    valorVerde = fnCalcularValor(stepVerde, valorVerde, i);
    valorAzul = fnCalcularValor(stepAzul, valorAzul, i);

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
