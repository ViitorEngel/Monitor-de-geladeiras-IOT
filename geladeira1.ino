//Incluimos as bibliotecas necessárias para realizar a leitura da temperatura e a conexão com o MQTT
#include <OneWire.h>
#include <DallasTemperature.h>
#include "EspMQTTClient.h"

//configurações da conexão MQTT
EspMQTTClient client
(
  "AndroidAP", //Nome da sua rede Wi-Fi
  "svdk5335", //Senha da sua rede Wi-Fi
  "broker.emqx.io",  //MQTT Broker 
  "Default",   //Username
  "",   //Código do Token 
  "geladeiramedicao",     //Nome do client
  1883              //Porta MQTT
);
//variáveis que pegarão o valor da temperatura após convertida para string
char charsomm[15]; //Temperatira perdida média
char chartempp[15]; //Temperatura perdida
char tempgel[15]; //Temperatura real da geladeira

int estadoPorta; //Variável que monitora o estado da geladeira (Aberta/Fechada)

unsigned long momentoFechado = 0.00; //Momento quando a geladeira foi fechada
unsigned long momentoAberto = 0.00; //Momento quando a geladeira foi aberta
unsigned long tempoAberto = 0.00; //Tempo que a geladeira ficou aberta

float temperaturaFechado; //Temperatura no momento que a geladeira é fechada
float temperaturaAberto; //Temperatura no momento que a geladeira é aberta
float temperaturaPerdida; //A temperatura perdida enquanto a geladeira ficou aberta
float somaTemperatura; //Variáves utilizadas para tirar a média da temperatura
float temperaturaSomada;

bool ctrl = 0; //Variável de controle que auxilia na lógica da medição dos dados da geladeira
const int oneWireBus = 17; //Configuração da leitura do sensor
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

int count = 1; //Variável que conta quanto tempo a geladeira foi aberta
float somaTempo; //Variáveis envolvidas no cálculo do tempo médio que a geladeira permanece aberta
float tempoSomado;

void setup() {
  Serial.begin(9600); //Iniciamos comunicação serial com baud rate de 9600
  pinMode(19, INPUT_PULLUP); //Realizamos as configurações dos pinos dos leds e sensores
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  sensors.begin(); //Iniciamos o sensor de temperatura
}

  //Função obrigatória de ser chamada quando for realizada conexão
  void onConnectionEstablished(){}

void loop() {
  sensors.requestTemperatures(); //Pedimos o valor da temperatura
  estadoPorta = digitalRead(19); //Lemos o estado da porta
  if(estadoPorta == HIGH){ //Se estiver em HIGH, então a porta está aberta
    momentoAberto = millis(); //Pegamos o momento em que a porta abriu
    tempoAberto = momentoAberto - momentoFechado; //Calculamos o tempo que a porta permaneceu aberta
    ctrl = 1; //Setamos a variável de controle para 1
    Serial.println("ABRIU"); //Printamos no terminal serial
    client.publish("geladeira01/estado", "ABERTA"); //Publicamos no tópico MQTT desta geladeira a informação de que ela está aberta
    temperaturaAberto = sensors.getTempCByIndex(0); //Pegamos a temperatura da geladeira
    dtostrf(temperaturaAberto, 1, 2, tempgel); //Convertemos de um float para uma string, para podermos enviar via MQTT
    client.publish("geladeira01/tempgel",tempgel); //Publicamos no tópico da geladeira via MQTT sua temperatura
  
    if (tempoAberto <= 4000 && tempoAberto != 0){ //Se a geladeira ficou pouco tempo aberta, ligamos somente o primeiro led
      digitalWrite(12, HIGH);
      digitalWrite(13, LOW);
      digitalWrite(14, LOW);
      return;    
    }
    if (tempoAberto > 4000 && tempoAberto < 10000){ //Se a geladeira ficou por um período de tempo intermediário aberta, ligamos somente o segundo led
      digitalWrite(12, LOW);
      digitalWrite(13, HIGH);
      digitalWrite(14, LOW);
      return;
    }
    // Caso seja superior a 10 segundos -> Caso ela tenha ficado tempo demais aberta,ligamos somente o último led 
     digitalWrite(12, LOW);
     digitalWrite(13, LOW);
     digitalWrite(14, HIGH); 
  
   } else { //Se não está aberta, está fechada
    Serial.println("FECHOU"); //Printamos essa informação no terminal
    client.publish("geladeira01/estado", "FECHADA"); //Publicamos no seu tópico o seu estado via MQTT
    dtostrf(temperaturaFechado, 1, 2, tempgel); //Convertemos a temperatura da geladeira de float para string
    client.publish("geladeira01/tempgel",tempgel);  //Publicamos via MQTT a temperatura da geladeira

    if(ctrl == 1){ //Se a variável de controle for 1
      temperaturaPerdida = temperaturaAberto - temperaturaFechado; //Calculamos a temperatura perdida
      
      somaTempo += tempoAberto; //Somamos ao tempo da geladeira o tempo que ela ficou aberta
      tempoSomado = somaTempo / count; //Tiramos a média do tempo que ela ficou aberta

      somaTemperatura += temperaturaPerdida; //Somamos a perda da temperatura
      temperaturaSomada = somaTemperatura / count; //Tiramos a média da perda de temperatura

      Serial.println(temperaturaSomada); //Printamos a média da temperatura
      dtostrf(temperaturaSomada, 1, 2, charsomm); //Convertemos a média da temperatura de float para string
      client.publish("geladeira01/tempsom",charsomm); //Publicamos via MQTT a média da temperatura perdida
      Serial.println(temperaturaPerdida); //Printamos a temperatura perdida
      dtostrf(temperaturaPerdida, 1, 2, chartempp); //Convertemos a temperatura perdida de float para string
      client.publish("geladeira01/tempp",chartempp); //Publicamos a temperatura perdida via MQTT

      count++; //Incrementamos a variável que conta quantas vezes a geladeira abriu, para podermos continuar tirando a média
     
      ctrl=0; //Setamos a variável de controle para zero
    }
    
    temperaturaFechado = sensors.getTempCByIndex(0); //Pegamos a temperatura da geladeira quando fecha
    Serial.println(temperaturaFechado); //Printamos o valor da temperatura quando fecha
    momentoFechado = millis(); //Pegamos o momento em que a geladeira fechou
   } 
  client.loop(); //Função que mantém a conexão sempre ativa
}
