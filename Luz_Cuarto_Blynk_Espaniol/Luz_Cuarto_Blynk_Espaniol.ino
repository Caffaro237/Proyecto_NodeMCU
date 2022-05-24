//Proyecto para NodeMCU y Aplicacion Blynk

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//Codigo de autenticacion de BLynk
char auth[] = "e05b08f4e7e542a9b40b46b885948aa6";

//Configuracion Wifi
char ssid[] = "FT MARTU04";
char pass[] = "01420289243";

//Salidas
int pin_rojo = 14;   //LED Rojo,   Pin D5
int pin_verde = 2;  //LED Verde, Pin D4
int pin_azul = 0;  //LED Azul,  Pin D3

//Arrays colores
int negro[3]  = { 0, 0, 0 };
int blanco[3]  = { 100, 100, 100 };
int rojo[3]    = { 100, 0, 0 };
int verde[3]  = { 0, 100, 0 };
int azul[3]   = { 0, 0, 100 };
int amarillo[3] = { 40, 95, 0 };
int dim_blanco[3] = { 30, 30, 30 };
int violeta[3] = { 60, 0, 100 };
int naranja[3] = { 100, 35, 0 };
int rosa[3] = { 100, 0, 40 };

//Inicializacion de colores
int valor_rojo = negro[0];
int valor_verde = negro[1];
int valor_azul = negro[2];

int wait = 10;
int hold = 0;
int DEBUG = 1;
int contador_loop = 60;
int repetir = 999999999;
int j = 0;

//Variables extras
int pin_color_fade = 16; //Pin D0
int pin_velocidad_random = A0; //Pin A0
int pin_color_random = 4; //Pin D2
int pin_color_random_rapido = 12; //Pin D6
int pin_color_random_lento = 3; //Pin D9
int pin_blanco_tenue = 13; //Pin D7
int pin_luz_techo = 15; //Pin D8

int color_fade = LOW;
int blanco_tenue = LOW;
int luz_techo = LOW;
int color_random = LOW;
int color_random_rapido = LOW;
int color_random_lento = LOW;
int velocidad_random = 0;

int bandera_color_fade = 0;
int bandera_blanco_tenue = 0;
int bandera_luz_techo = 0;
int bandera_color_random = 0;
int i = 0;

//Inicializacion de variables de colores
int prev_rojo = valor_rojo;
int prev_verde = valor_verde;
int prev_azul = valor_azul;


void setup()
{
  pinMode(pin_rojo, OUTPUT);
  pinMode(pin_verde, OUTPUT);
  pinMode(pin_azul, OUTPUT);

  if(DEBUG)
  {
    Serial.begin(9600);
  }
  
  Blynk.begin(auth, ssid, pass);
  //Se puede especificar un servidor
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,60), 80);
}

void loop()
{
  luz_techo = digitalRead(pin_luz_techo);
  blanco_tenue = digitalRead(pin_blanco_tenue);
  color_fade = digitalRead(pin_color_fade);
  color_random = digitalRead(pin_color_random);
  color_random_rapido = digitalRead(pin_color_random_rapido);
  color_random_lento = digitalRead(pin_color_random_lento);

  //velocidad_random = analogRead(pin_luz_techo);

  bandera_luz_techo = luz_techo_f(bandera_luz_techo);
  bandera_blanco_tenue = blanco_tenue_f(bandera_blanco_tenue, bandera_color_fade);
  bandera_color_fade = color_fade_f(bandera_color_fade);
  bandera_color_random = colores_random_f(bandera_color_random);

  //Serial.println(velocidad_random);
  
  Blynk.run();
  
  delay(100);
}

int luz_techo_f(int bandera)
{
  if(!luz_techo && !bandera)
  {
    Serial.print("Luz Apagada - ");
      
    Serial.print("Pin luz_techo: ");
    Serial.println(luz_techo);
    
    bandera = 1;
  }
  else
  {
    if(luz_techo && bandera)
    {
      Serial.print("Luz Prendida - ");
      
      Serial.print("Pin luz_techo: ");
      Serial.println(luz_techo);

      bandera = 0;
    }
  }
  
  return bandera;
}

int blanco_tenue_f(int bandera_1, int bandera_2)
{
  if(!blanco_tenue && !bandera_1 && bandera_2)
  {
    digitalWrite(pin_rojo, LOW);
    digitalWrite(pin_verde, LOW);
    digitalWrite(pin_azul, LOW);
    
    bandera_1 = 1;

    Serial.print("Blanco Tenue Apagado - ");

    Serial.print("Pin blanco_tenue: ");
    Serial.println(blanco_tenue);
  }
  else if(blanco_tenue && bandera_1)
  {
    analogWrite(pin_rojo, 100);
    analogWrite(pin_verde, 100);
    analogWrite(pin_azul, 100);
    
    bandera_1 = 0;
    
    Serial.print("Blanco Tenue Prendido - ");

    Serial.print("Pin blanco_tenue: ");
    Serial.println(blanco_tenue);
  }
  
  return bandera_1;
}

int color_fade_f(int bandera)
{
  if(!color_fade && !bandera)
  {
    Serial.print("Color Fade Apagado - ");
    
    Serial.print("Pin color_fade: ");
    Serial.println(color_fade);
    
    digitalWrite(pin_rojo, LOW);
    digitalWrite(pin_verde, LOW);
    digitalWrite(pin_azul, LOW);

    bandera = 1;    
  }
  else if(color_fade && bandera)
  {
    if(repetir)
    {
      j = j + 1;
      
      Serial.print("Repeticion Color Fade: ");
      Serial.println(j);
      
      if (j >= repetir)
      {
        bandera = 0;

        digitalWrite(pin_rojo, LOW);
        digitalWrite(pin_verde, LOW);
        digitalWrite(pin_azul, LOW);
        
        exit(j);
      }
    }

    Serial.print("Color Fade Prendido - ");
    
    Serial.print("Pin color_fade: ");
    Serial.println(color_fade);

    bandera = 1;

    Serial.println("BLANCO");
    cross_fade(blanco);
    Serial.println("VIOLETA");
    cross_fade(violeta);
    Serial.println("AZUL");
    cross_fade(azul);
    //Serial.println("ROSA");
    //cross_fade(rosa);
    Serial.println("ROJO");
    cross_fade(rojo);
    Serial.println("NARANJA");
    cross_fade(naranja);
    Serial.println("VERDE");
    cross_fade(verde);
  }
  
  return bandera;
}

int colores_random_f(int bandera)
{
  int tiempo = 1000;
  
  if(!color_random && !bandera)
  {
    Serial.print("Color Random Apagado - ");

    Serial.print("Pin color_random: ");
    Serial.println(color_random);
    
    bandera = 1;
    
    digitalWrite(pin_rojo, LOW);
    digitalWrite(pin_verde, LOW);
    digitalWrite(pin_azul, LOW);
  }
  else if(color_random && bandera)
  {
    Serial.print("Color Random Prendido - ");

    Serial.print("Pin color_random: ");
    Serial.println(color_random);

    for(i = 0; i < repetir; i++)
    {
      color_random = digitalRead(pin_color_random);
      color_random_rapido = digitalRead(pin_color_random_rapido);
      color_random_lento = digitalRead(pin_color_random_lento);

      if(color_random_lento)
      {
        tiempo = 450;
      }
      else if(color_random_rapido)
      {
        tiempo = 100;
      }
      

      Blynk.run();

      if(!color_random)
      {
        digitalWrite(pin_rojo, LOW);
        digitalWrite(pin_verde, LOW);
        digitalWrite(pin_azul, LOW);
        
        break;
      }
      else
      {
        int numero_1 = random(100,1024);
        int numero_2 = random(100,1024);
        int numero_3 = random(100,1024);
                  
        analogWrite(pin_rojo, numero_1);
        analogWrite(pin_verde, numero_2);
        analogWrite(pin_azul, numero_3);
  
        Serial.print("Loop/RGB: #");
        Serial.print(i);
        Serial.print(" | ");
        Serial.print(numero_1);
        Serial.print(" / ");
        Serial.print(numero_2);
        Serial.print(" / ");  
        Serial.println(numero_3);
      }
      
      delay(tiempo);
    }
    
    bandera = 0;
  }

  return bandera;
}

int calcular_step(int valor_previo, int valor_final)
{
  int step = valor_final - valor_previo;

  if(step)
  {
    step = 1020 / step;
  }
  
  return step;
}

int calcular_valor(int step, int valor, int i)
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

void cross_fade(int color[3])
{
  // Conversor de 0 a 255
  int rojo = (color[0] * 255) / 100;
  int verde = (color[1] * 255) / 100;
  int azul = (color[2] * 255) / 100;

  int step_rojo = calcular_step(prev_rojo, rojo);
  int step_verde = calcular_step(prev_verde, verde);
  int step_azul = calcular_step(prev_azul, azul);

  for (int i = 0; i <= 1020; i++)
  {
    color_fade = digitalRead(pin_color_fade);

    Blynk.run();

    if(!color_fade)
    {
      digitalWrite(pin_rojo, LOW);
      digitalWrite(pin_verde, LOW);
      digitalWrite(pin_azul, LOW);

      break;
    }
    
    valor_rojo = calcular_valor(step_rojo, valor_rojo, i);
    valor_verde = calcular_valor(step_verde, valor_verde, i);
    valor_azul = calcular_valor(step_azul, valor_azul, i);

    analogWrite(pin_rojo, valor_rojo);
    analogWrite(pin_verde, valor_verde);
    analogWrite(pin_azul, valor_azul);

    delay(wait);

    if(DEBUG)
    {
      if(i == 0 or i % contador_loop == 0)
      {
        Serial.print("Loop/RGB: #");
        Serial.print(i);
        Serial.print(" | ");
        Serial.print(valor_rojo);
        Serial.print(" / ");
        Serial.print(valor_verde);
        Serial.print(" / ");
        Serial.println(valor_azul);
      }

      DEBUG = DEBUG + 1;
    }
  }

  prev_rojo = valor_rojo;
  prev_verde = valor_verde;
  prev_azul = valor_azul;

  delay(hold);
}
