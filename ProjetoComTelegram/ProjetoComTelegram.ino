#include <WiFi.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>
#include <Ultrasonic.h>


const char* ssid = "SATC IOT";
const char* password = "IOT2024@#";
const char* botToken = "7749822141:AAECm6LSk1tpGpaZ-_vADpEphv0Q1Y-rTNw";

#define CHAT_ID "6861124036"

WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

String chat_id;

const int trig = 5;
const int echo = 4;
const int luzquarto = 13;    
const int luzsala = 12;    
const int som = 15;   
const int botaoQuarto = 17;
const int botaoSala = 18;


bool estadoLuzQuarto = false;
bool estadoLuzSala = false;
bool alarmeAtivo = false; 
bool sensorAtivo = false;  

unsigned long lastDebounceTimeQuarto = 0;  
unsigned long lastDebounceTimeSala = 0;     
unsigned long debounceDelay = 300;   

unsigned long tempoAnterior = 0;

Ultrasonic ultrasonic(echo, trig);

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi!");

  client.setInsecure();

  pinMode(luzquarto, OUTPUT);
  pinMode(luzsala, OUTPUT);
  pinMode(som, OUTPUT);
  pinMode(botaoQuarto, INPUT_PULLUP);
  pinMode(botaoSala, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(botaoQuarto), interruptQuarto, FALLING); 
  attachInterrupt(digitalPinToInterrupt(botaoSala), interruptSala, FALLING); 
}

void loop() {

  if (millis() - tempoAnterior >= 1000) {
    tempoAnterior = millis();
    int novasmensagens = bot.getUpdates(bot.last_message_received + 1);

    while (novasmensagens) {
      Serial.println("Nova mensagem recebida");
      handleNewMessages(novasmensagens);
      novasmensagens = bot.getUpdates(bot.last_message_received + 1);
    }
  }
  Serial.println(ultrasonic.read());
  if (alarmeAtivo && ultrasonic.read() <= 25) {
    Serial.println("Alarme Ativo");
    digitalWrite(som, HIGH);  
    bot.sendMessage(chat_id, "Alarme ativado! Verifique sua casa.", "");
  }

}


void handleNewMessages(int novasmensagens) {
  for (int i = 0; i < novasmensagens; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    if (text == "?"){
      bot.sendMessage(chat_id, "Os comandos são:\n1 - Acender Luz Quarto\n2 - Apagar Luz quarto\n3 - Acender Luz Sala\n4 - Apagar Luz Sala\n5 - Ativar Alarme\n6 - Desativar Alarme", "");
    }

    else if (text == "1") {
      estadoLuzQuarto = true;
      digitalWrite(luzquarto, HIGH);
      bot.sendMessage(chat_id, "Luz do quarto acesa.", "");
    }
    else if (text == "2") {
      estadoLuzQuarto = false;
      digitalWrite(luzquarto, LOW);
      bot.sendMessage(chat_id, "Luz do quarto apagada.", "");
    }
    else if (text == "3") {
      estadoLuzSala = true;
      digitalWrite(luzsala, HIGH);
      bot.sendMessage(chat_id, "Luz da sala acesa.", "");
    }
    else if (text == "4") {
      estadoLuzSala = false;
      digitalWrite(luzsala, LOW);
      bot.sendMessage(chat_id, "Luz da sala apagada.", "");
    }
    else if (text == "5") {
      alarmeAtivo = true;
      bot.sendMessage(chat_id, "Alarme ativado. Sensor de movimento ativo.", "");
    }
    else if (text == "6") {
      alarmeAtivo = false;
      digitalWrite(som, LOW); 
      bot.sendMessage(chat_id, "Alarme e sensor de movimento desativados.", "");
    }
    else {
      bot.sendMessage(chat_id, "Comando não reconhecido.", "");
    }
  }
}

void interruptQuarto() {
  if ((millis() - lastDebounceTimeQuarto) > debounceDelay) {
    estadoLuzQuarto = !estadoLuzQuarto;
    digitalWrite(luzquarto, estadoLuzQuarto);

    lastDebounceTimeQuarto = millis();
  }
}

void interruptSala() {
  if ((millis() - lastDebounceTimeSala) > debounceDelay) {
    estadoLuzSala = !estadoLuzSala;
    digitalWrite(luzsala, estadoLuzSala);

    lastDebounceTimeSala = millis();
  }
}
