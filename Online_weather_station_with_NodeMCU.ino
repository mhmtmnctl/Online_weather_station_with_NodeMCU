#include <LcdI2cTUR.h>
LcdI2cTUR lcd(0x27);
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
const char* ssid     = "T50B_2.4G";
const char* password = "635453Rr522.";
String APIKEY = "94b17f516833749caad713a4721383c8";        //openweather
String CityID = "317106";                                 //Your City ID
WiFiClient client;
char servername[] = "api.openweathermap.org";
String result;
int  counter = 60;
String weatherDescription = "";
String weatherLocation = "";
String Country;
int Temperature;
float Humidity;
float Pressure;
///////////////
 unsigned long eskiZaman=0;
  unsigned long yeniZaman;
  unsigned long eskiZaman2=0;
  unsigned long yeniZaman2;




void setup() {
  lcd.begin();
  lcd.NoAutoScroll();
  lcd.displayClear();
  lcd.cursorOFF();
  lcd.blinkOFF();
  lcd.displayClear();
  Serial.begin(115200);
  int cursorPosition = 0; 
  lcd.print("   Connecting");
  Serial.println("Connecting");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    lcd.displayClear();
    lcd.setCursor(cursorPosition, 2);
    lcd.print(".");
    cursorPosition++;
  }
  timeClient.begin();
  lcd.displayClear();
  lcd.print("   Connected!");
  Serial.println("Connected");
  delay(1000);
  lcd.displayClear();
  timeClient.begin();
  timeClient.setTimeOffset(10800);

     getWeatherData();
     displayWeather(weatherDescription);
  
     displayConditions(Temperature);
 
}

void loop() {
  yeniZaman=millis();
  if(yeniZaman-eskiZaman >1000)
  {
     tarihSaat();
     eskiZaman = yeniZaman;
  }
  
  

  
 yeniZaman2=millis();
  
  if (yeniZaman2-eskiZaman2 >900000)
  {
    // if (counter == 60) //Get new data every 10 minutes
 // {
  //  counter = 0;
    displayGettingData();
   // delay(1000);
    getWeatherData();
    eskiZaman2 = yeniZaman2;
 }
  else
  {
    //counter++;
    displayWeather(weatherDescription);
   // delay(5000);
    displayConditions(Temperature);
   // delay(5000);
  // eskiZaman2 = yeniZaman2;
  }
  
  }
 
 



void getWeatherData()    //client function to send/receive GET request data.
{
  if (client.connect(servername, 80))
  { //starts client connection, checks for connection
    client.println("GET /data/2.5/weather?id=" + CityID + "&units=metric&APPID=" + APIKEY);
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  }
  else {
    Serial.println("connection failed");        //error message if no client connect
    Serial.println();
  }

  while (client.connected() && !client.available())
    delay(1);                                          //waits for data
  while (client.connected() || client.available())
  { //connected or data available
    char c = client.read();                     //gets byte from ethernet buffer
    result = result + c;
  }

  client.stop();                                      //stop client
  result.replace('[', ' ');
  result.replace(']', ' ');
  Serial.println(result);
  char jsonArray [result.length() + 1];
  result.toCharArray(jsonArray, sizeof(jsonArray));
  jsonArray[result.length() + 1] = '\0';
  StaticJsonBuffer<1024> json_buf;
  JsonObject &root = json_buf.parseObject(jsonArray);

  if (!root.success())
  {
    Serial.println("parseObject() failed");
  }

  String location = root["name"];
  String country = root["sys"]["country"];
  int temperature = root["main"]["temp"];
  float humidity = root["main"]["humidity"];
  String weather = root["weather"]["main"];
  String description = root["weather"]["description"];
  float pressure = root["main"]["pressure"];
  weatherDescription = description;
  weatherLocation = location;
  Country = country;
  Temperature = temperature;
  Humidity = humidity;
  Pressure = pressure;
   Serial.println("hava bilgisi: "+weatherDescription );

}

void displayWeather(String weatherDescription)
{

  if (weatherDescription == "clear sky")
  {
    lcd.setCursor(4, 1);
    lcd.print("Hava Acık");
  }
  else if (weatherDescription == "few clouds")

  {
    lcd.setCursor(4, 1);
    lcd.print("Az bulutlu");
  }
  else if (weatherDescription == "scattered clouds")

  {
    lcd.setCursor(4, 1);
    lcd.print("DağınıkBulut");
  }

  else if (weatherDescription == "broken clouds")

  {
    lcd.setCursor(4, 1);
    lcd.print("ParçalıBulut");
  }
  else if (weatherDescription == "shower rain")

  {
    lcd.setCursor(4, 1);
    lcd.print("Sağanak var");
  }

  else if (weatherDescription == "rain")

  {
    lcd.setCursor(4, 1);
    lcd.print("Yağmur var");
  }

  else if (weatherDescription == "thunderstorm")

  {
    lcd.setCursor(4, 1);
    lcd.print("Şimşek var");
  }

  else if (weatherDescription == "snow")

  {
    lcd.setCursor(4, 1);
    lcd.print("Hava karlı");
  }
  else if (weatherDescription == "mist")

  {
    lcd.setCursor(4, 1);
    lcd.print("Hava sisli");
  }

  else
  {
    lcd.setCursor(4, 1);
    lcd.print("Bilgi yok");
  }

}


void displayConditions(int Temperature)
{
  lcd.setCursor(0, 1);
  lcd.print(Temperature);
  lcd.setCursor(2, 1);
  lcd.print((char)223);
  lcd.setCursor(3, 1);
  lcd.print(" ");
}

void displayGettingData()
{
  lcd.displayClear();
  lcd.print("Getting data");
}

void tarihSaat()
{
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime);
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon + 1;
  int currentYear = ptm->tm_year + 1900;
  String currentDate = String(monthDay) + "." + String(currentMonth) + "." + String(currentYear)  ;

  String formattedTime = timeClient.getFormattedTime();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  String saat = String(currentHour) + ":" + String(currentMinute);
  if (monthDay < 10 && currentMonth < 10)
  {

    lcd.setCursor(0, 0);
    lcd.print("0" + String(monthDay));
    lcd.setCursor(2, 0);
    lcd.print(".");
    lcd.setCursor(3, 0);
    lcd.print("0" + String(currentMonth));
    lcd.setCursor(5, 0);
    lcd.print(".");
    lcd.setCursor(6, 0);
    lcd.print(currentYear);
    lcd.setCursor(10, 0);
    lcd.print(" ");

  }
  else if (monthDay < 10 && currentMonth > 10 )
  {
    lcd.setCursor(0, 0);
    lcd.print("0" + String(monthDay));
    lcd.setCursor(2, 0);
    lcd.print(".");
    lcd.setCursor(3, 0);
    lcd.print(currentMonth);
    lcd.setCursor(5, 0);
    lcd.print(".");
    lcd.setCursor(6, 0);
    lcd.print(currentYear);
    lcd.setCursor(10, 0);
    lcd.print(" ");

  }

  else if (monthDay >= 10 && currentMonth < 10)
  {
    lcd.setCursor(0, 0);
    lcd.print(monthDay);
    lcd.setCursor(2, 0);
    lcd.print(".");
    lcd.setCursor(3, 0);
    lcd.print("0" + String(currentMonth));
    lcd.setCursor(5, 0);
    lcd.print(".");
    lcd.setCursor(6, 0);
    lcd.print(currentYear);
    lcd.setCursor(10, 0);
    lcd.print(" ");
  }

  else
  {
    lcd.setCursor(0, 0);
    lcd.print(currentDate) ;
  }

  lcd.setCursor(11, 0);
  lcd.print(formattedTime);
}
