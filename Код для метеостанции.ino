#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#include <SFE_BMP180.h>
#include <Wire.h>

SFE_BMP180 pressure;

#include "DHT.h"
#define DHTPIN 13
DHT dht(DHTPIN, DHT11); 

  const char* ssid = "planeta322927"; // Имя сети wifi
  const char* password = "F754579t";  // Пароль от сети wifi

// Инициализация Telegram бота
#define BOTtoken "1834747810:AAFdUWkW_AoqnN39SZubFt_QxxS4qYdY_2Q"  // Токен бота


#define chatid "1712997246"                                   // ID чата

long timeNotify;

  char status;
  double T,P; 
  
long checkTelegramDueTime;
int checkTelegramDelay = 1000;
int timeLast = 0;

long ldrDueTime;
int checkLDRDelay = 250;

double pressure_last;
double pressure_now;


WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

void handleNewMessages(int numNewMessages) {

 
  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text == "/temInHouse") {
      float temperature = dht.readTemperature();
      bot.sendMessage(chatid, (String)"Температура внутри дома: " + temperature + " C");
    }

    if (text == "/temOutHouse") {

      status = pressure.startTemperature();
      if (status != 0)
      {
      delay(status);
      status = pressure.getTemperature(T);
      if (status != 0)
      { 
       bot.sendMessage(chatid, (String)"Температура снаружи дома: " + T + " C");
      }
      }
    }
    
    if (text == "/humInHouse") {
      float h = dht.readHumidity();
      bot.sendMessage(chatid, (String)"Влажность в доме: " + h + " %");
    }

    if (text == "/wForecast") {
     
      if(pressure_now - pressure_last  > 2 && pressure_last != 0){
      bot.sendMessage(chatid, (String)"Погода будет хорошей и ясной. Без осадков");
      }

      if(pressure_last - pressure_now  > 2){
      bot.sendMessage(chatid, (String)"Ожидается ухудшение погоды");
      }

      if(pressure_last - pressure_now  <= 2 && pressure_now - pressure_last  <= 2){
      bot.sendMessage(chatid, (String)"Погода не изменится");
      }

      if(pressure_last == 0){
      bot.sendMessage(chatid, (String)"Недостаточно данных для прогнозирования погоды. Подождите");
      }
      
    }

    if (text == "/atmPressure") {
      P = GetPressure();
      bot.sendMessage(chatid, (String)"Атмосферное давление: " + P + " мм.рт.ст");
      
      }

    if (text == "/options") {
      String keyboardJson = "[[\"/temInHouse\", \"/temOutHouse\", \"/humInHouse\", \"/atmPressure\", \"/wForecast\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, "Что вы хотите узнать?", "", keyboardJson, true);
    }

    if (text == "/start" || text == "/help") {
      String welcome = "Привет, я домашняя метеостанция. Рада знакомству, " + from_name + ".\n";
      welcome += "Доступные функции:\n\n";
      welcome += "/temInHouse : Узнать температуру внутри дома в градусах цельсия\n";
      welcome += "/temOutHouse : Узнать температуру снаружи дома в градусах цельсия\n";
      welcome += "/humInHouse : Узнать влажность внутри дома в процентах(%)\n";
      welcome += "/atmPressure : Узнать атмосферное давление в мм.рт.ст\n";
      welcome += "/wForecast : Получить прогноз погоды, основанный на перепадах атмосферного давления\n";
      welcome += "/options : Показать кнопки для быстрого ввода команд\n";
      welcome += "/help : Показать это сообщение снова\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}


void setup() {
  Serial.begin(115200);

  pressure.begin();
  
  client.setInsecure();

  
  // Попытка подключения к сети
  Serial.print("Соединение с Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi соединение установлено");
  Serial.print("IP адрес: ");
  Serial.println(WiFi.localIP());

  

      
      P = GetPressure();
      pressure_now = P;
      


    bot.sendMessage(chatid, "Бот стартовал", "");
      
      
}

void loop() {
    
    long now = millis();
  if(now >= checkTelegramDueTime) {
    Serial.println("---- Checking Telegram -----");
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while(numNewMessages) {
      Serial.println("Bot recieved a message");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    checkTelegramDueTime = now + checkTelegramDelay;
  }
  now = millis();

  timeNotify =  millis();
  if (timeNotify - timeLast >= 300000)   //Проверка показаний с датчиков и, влзможно, отправка сообщения каждые 5 минут
  {
      float h = dht.readHumidity();
      if(h < 30){
        bot.sendMessage(chatid, (String)"Влажность в доме слишком низкая (" + h + "%)");
      }
      if(h > 60){
        bot.sendMessage(chatid, (String)"Влажность в доме слишком высокая (" + h + "%)");
      }

      float temperature = dht.readTemperature();
      if(temperature >= 30){
        bot.sendMessage(chatid, (String)"Дома сликом жарко (" + temperature + " C)");
      }
      if(temperature <= 18){
        bot.sendMessage(chatid, (String)"Дома сликом холодно (" + temperature + " C)");
      }
      
      timeLast = timeNotify;
  }


  timeNotify =  millis();
  if (timeNotify - timeLast >= 10800000) // каждые 3 часа  10800000
  {
      P = GetPressure();
      pressure_last = pressure_now;
      pressure_now = P;

      
      timeLast = timeNotify;
  }
}


double GetPressure() {

  status = pressure.startTemperature();
      if (status != 0)
      {
      delay(status);
      status = pressure.getTemperature(T);
      }

      status = pressure.startPressure(3);
      if (status != 0)
      {
      delay(status);
      status = pressure.getPressure(P,T);
      if (status != 0)
      {
       P = P*0.750064;
      }
      }

      return P;
  
}
   
