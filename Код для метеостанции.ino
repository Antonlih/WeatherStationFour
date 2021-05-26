#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

  const char* ssid = "VISH1"; // Имя сети wifi
  const char* password = "1q2w3e4r5t";  // Пароль от сети wifi

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
      bot.sendMessage(chatid, "30 gradusov");
    }

    if (text == "/temperatureOutsideHouse") {
      bot.sendMessage(chatid, "35 gradusov");
    }
    
    if (text == "/humidityInsideHouse") {
      bot.sendMessage(chatid, "80%");
    }

    if (text == "/humidityOutsideHouse") {
      bot.sendMessage(chatid, "60%");
    }

    if (text == "/atmospherePressure") {
      bot.sendMessage(chatid, "776 mm");
    }

    if (text == "/options") {
      String keyboardJson = "[[\"/temperatureInsideHouse\", \"/temperatureOutsideHouse\", \"/humidityInsideHouse\", \"/humidityOutsideHouse\", \"/atmospherePressure\"]]";
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
  if (timeNotify - timeLast >= 300000)   //Отправка сообщения каждые 5 минут
  {
    bot.sendMessage(chatid," +timeNotify+ ");
      timeLast = timeNotify;
  }
  

  
  }
