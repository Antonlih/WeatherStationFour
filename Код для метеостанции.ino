#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266TelegramBOT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

WiFiClientSecure client;
// Initialize Wifi connection to the router
char ssid[] = "Redmi";              
char password[] = "12345678";            
int status = 0;


// Initialize Telegram BOT

#define BOTtoken "1834747810:AAFdUWkW_AoqnN39SZubFt_QxxS4qYdY_2Q"  //Токен бота полученного от @BotFather
#define BOTname "weather station" // Имя бота
#define BOTusername "Project_Weather_Station_Beta_bot" // Логин бота
TelegramBOT bot(BOTtoken, BOTname, BOTusername);

int Bot_mtbs = 1000; //среднее время между сканированием сообщений
long Bot_lasttime;   

bool Start = false;

void Bot_Messages() {
  for (int i = 1; i < bot.message[0][0].toInt() + 1; i++)      {
    bot.sendMessage(bot.message[i][4], bot.message[i][5], "");
    bot.message[i][5]=bot.message[i][5].substring(1,bot.message[i][5].length());
       
    if (bot.message[i][5] == "start") {
      String wellcome = "Привет. Я домашняя метеостанция";
      bot.sendMessage(bot.message[i][4], wellcome, "");
      Start = true;
    }
  }
  bot.message[0][0] = "";   // All messages have been replied - reset new messages
}

void setup() {
  
  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
 
  Serial.println(ip);
  client.setInsecure();
  bot.begin();      // Включаем бота
}



void loop() { 
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    bot.getUpdates(bot.message[0][1]);   // Включаем API и получаем новые сообщения    
    if (getTemperature() < 23){
    Serial.print("ALARM: "); // Проверяем,если температура ниже указанной, тогда шлем сообщение
    bot.sendMessage("216473175", (String)"Температура: "+bme.readTemperature()+", "");
    bot.sendMessage("216473175", (String)"Влажность: "+bme.readPressure() / 100.0F+", "");
    bot.sendMessage("216473175", (String)"Давление(%): "+bme.readHumidity()+", "");
  }    
    Bot_Messages();
    Bot_lasttime = millis();
  }
}
