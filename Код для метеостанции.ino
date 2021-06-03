#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#include <SFE_BMP180.h>
#include <Wire.h>

SFE_BMP180 pressure;

#include <dht11.h>       // Добавляем библиотеку DHT11
dht11 DHT;               // Объявление переменной класса dht11
#define DHT11_PIN 2     // Датчик DHT11 подключен к цифровому пину номер 4

  const char* ssid = "planeta322927"; // Имя сети wifi
  const char* password = "F754579t";  // Пароль от сети wifi

// Инициализация Telegram бота
#define BOTtoken "1834747810:AAFdUWkW_AoqnN39SZubFt_QxxS4qYdY_2Q"  // Токен бота


#define chatid "1712997246"                                   // ID чата

long timeNotify;

long checkTelegramDueTime;
int checkTelegramDelay = 1000;
int timeLast = 0;

long ldrDueTime;
int checkLDRDelay = 250;

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

void handleNewMessages(int numNewMessages) {
  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text == "/temperatureInsideHouse") {
      bot.sendMessage(chatid, (String)"Температура снаружи: "+DHT.temperature);
    }

    if (text == "/temperatureOutsideHouse") {
      bot.sendMessage(chatid, (String)"Температура снаружи: "+getTemperature());
    }
    
    if (text == "/humidityInsideHouse") {
      bot.sendMessage(chatid, (String)"Влажность в доме: "+DHT.humidity);
    }

    if (text == "/atmospherePressure") {
      bot.sendMessage(chatid, (String)"Атмосферное давление: "+getPressure());
    }

    if (text == "/options") {
      String keyboardJson = "[[\"/temperatureInsideHouse\", \"/temperatureOutsideHouse\", \"/humidityInsideHouse\", \"/atmospherePressure\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, "Что вы хотите узнать?", "", keyboardJson, true);
    }

    if (text == "/start" || text == "/help") {
      String welcome = "Привет, я домашняя метеостанция. Рада знакомству, " + from_name + ".\n";
      welcome += "Доступные функции:\n\n";
      welcome += "/temperatureInsideHouse : Узнать температуру внутри дома в градусах цельсия\n";
      welcome += "/temperatureOutsideHouse : Узнать температуру снаружи дома в градусах цельсия\n";
      welcome += "/humidityInsideHouse : Узнать влажность внутри дома в процентах(%)\n";
      welcome += "/humidityOutsideHouse : Узнать влажность снаружи дома в процентах(%)\n";
      welcome += "/atmospherePressure : Узнать атмосферное давление в мм.рт.ст\n";
      welcome += "/options : returns a custom reply keyboard\n";
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
    bot.sendMessage(chatid," +timeNotify+ ");
      timeLast = timeNotify;
  }
   
}

double getPressure(){
    char status;
    double T,P,p0,a;

    status = pressure.startTemperature();
    if (status != 0){
        // ожидание замера температуры
        delay(status);
        status = pressure.getTemperature(T);
        if (status != 0){
            status = pressure.startPressure(3);
            if (status != 0){
                // ожидание замера давления
                delay(status);
                status = pressure.getPressure(P,T);
                if (status != 0){
                    return(P);
                }
            }
        }
    }
}

double getTemperature(){
    char status;
    double T,P,p0,a;

    status = pressure.startTemperature();
    if (status != 0){
        // ожидание замера температуры
        delay(status);
        status = pressure.getTemperature(T);
        if (status != 0){
          return(T);
        }
    }
}
