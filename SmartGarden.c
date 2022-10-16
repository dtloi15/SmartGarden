#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

char auth[] = "jl8Cdm_7KPFWrEi5FFC-37hK39osP3Dv";

char ssid[] = "wifi";
char pass[] = "matkhau";
//
byte AutoModeFlag;

//button
byte ModeButton,FanButton,LedButton,WaterpumpButton;
byte ModeButtonFlag,FanButtonFlag,LedButtonFlag,WaterpumpButtonFlag;

//INPUT
#define Dht11_Sensor 2          
#define Soilmoisture_sensor 35
#define Light_sensor 34
//OUTPUT
#define Fan_signal 19
#define Waterpump_signal 18
#define Led_signal 5
//TYPE
#define DHTTYPE DHT11
DHT dht(Dht11_Sensor,DHTTYPE);
BlynkTimer timer;
LiquidCrystal_I2C lcd(0x27, 16, 2);
struct Data
{
  float DataReadFromSensor;
  float DataSetFromUser;

}Humidity,Temperature,Soilmoisture,Light;
void ReadDataFromSensorAndUpdateData()
{
  Humidity.DataReadFromSensor = dht.readHumidity();
  Temperature.DataReadFromSensor = dht.readTemperature(); 
  Soilmoisture.DataReadFromSensor = 100 - map(analogRead(Soilmoisture_sensor),0,4095,0,100);
  Light.DataReadFromSensor = 100 - map(analogRead(Light_sensor),0,4095,0,100);
    
  UpdateDataToBlynk();
  
}
void UpdateDataToBlynk()
{
  Blynk.virtualWrite(V5, Humidity.DataReadFromSensor);
  Blynk.virtualWrite(V6, Temperature.DataReadFromSensor);
  Blynk.virtualWrite(V7,Soilmoisture.DataReadFromSensor);
  Blynk.virtualWrite(V8,Light.DataReadFromSensor);
}
void PrintDataSetFromUserToSerial()
{
Serial.println(Humidity.DataSetFromUser);
Serial.println(Temperature.DataSetFromUser);
Serial.println(Soilmoisture.DataSetFromUser);
Serial.println(Light.DataSetFromUser);
}


byte degree [] = 
{
  0B01110,
  0B01010,
  0B01110,
  0B00000,
  0B00000,
  0B00000,
  0B00000,
  0B00000
};

BLYNK_WRITE(V9)// Button choose mode on Blynk
{
  AutoModeFlag = param.asInt();
  Serial.println(AutoModeFlag);
  Blynk.virtualWrite(V10,0);
  Blynk.virtualWrite(V11,0);
  Blynk.virtualWrite(V12,0);
  digitalWrite(Waterpump_signal,0);
  digitalWrite(Led_signal,0);
  digitalWrite(Fan_signal,0);
}
BLYNK_WRITE(V10) //Button On/Off Fan on Blynk
{
  if(AutoModeFlag == 1)
  {
    Blynk.virtualWrite(V10,0);
  }
  else
  {
    byte Fan_status = param.asInt();
    digitalWrite(Fan_signal,Fan_status);
    Serial.println(Fan_status);
  }
}
BLYNK_WRITE(V11) // Button On/Off Waterpump on Blynk
{
  if(AutoModeFlag == 1)
  {
    Blynk.virtualWrite(V11,0);
  }
  else
  {
    byte Waterpump_status = param.asInt();
    digitalWrite(Waterpump_signal,Waterpump_status);
     Serial.println(Waterpump_status);
  }
}
BLYNK_WRITE(V12) // Button On/Off Fan on Blynk
{
  if(AutoModeFlag == 1)
  {
    Blynk.virtualWrite(V12,0);
  }
    else
  
    {
      byte Led_status = param.asInt();
      digitalWrite(Led_signal,Led_status);
       Serial.println(Led_status);
    }
  
}
BLYNK_WRITE(13)
{
  if (AutoModeFlag == 1)
  {
    Soilmoisture.DataSetFromUser = param.asInt();
    Serial.println(Soilmoisture.DataSetFromUser);
  }
}
BLYNK_WRITE(14)
{
  if(AutoModeFlag == 1)
  {
    Light.DataSetFromUser = param.asInt(); 
    Serial.println(Light.DataSetFromUser);
  }
}
BLYNK_WRITE(15)
{
  if(AutoModeFlag == 1)
  {
    Temperature.DataSetFromUser = param.asInt(); 
    Serial.println(Temperature.DataSetFromUser);
  }
}
BLYNK_WRITE(16) // Button Save Data on Blynk
{
  PrintDataSetFromUserToSerial();
  for (int i = 100 ; i<110;i++)
  {
    EEPROM.write(i,0);
    delay(5);
  }
  delay(5);
  Serial.println("Save to EEPROM");
  EEPROM.write(100,Soilmoisture.DataSetFromUser);
  delay(5);
  EEPROM.write(101,Light.DataSetFromUser);
  delay(5);
  EEPROM.write(102,Temperature.DataSetFromUser);
  delay(5);
  EEPROM.write(103,AutoModeFlag);
  delay(5);
  EEPROM.commit();
  
  ESP.restart();
}
void LoadDataFromEEPROM()
{
  AutoModeFlag = EEPROM.read(103);
  Soilmoisture.DataSetFromUser = EEPROM.read(100);
  Light.DataSetFromUser = EEPROM.read(101);
  Temperature.DataSetFromUser = EEPROM.read(102);
  
  Serial.println(Soilmoisture.DataSetFromUser);
  Serial.println(Light.DataSetFromUser);
  Serial.println(Temperature.DataSetFromUser);
  Serial.println(AutoModeFlag);
  
  Blynk.virtualWrite(V13,Soilmoisture.DataSetFromUser);
  Blynk.virtualWrite(V14,Light.DataSetFromUser);
  Blynk.virtualWrite(V15,Temperature.DataSetFromUser);
  Blynk.virtualWrite(V9,AutoModeFlag);
}
void PrintDataReadFromSensorOnLcd()
{
 lcd.setCursor(0,0);
 lcd.print("T:");
 lcd.print(round(Temperature.DataReadFromSensor));
 lcd.write(1);
 lcd.print("C");
 lcd.setCursor(10,0);
 lcd.print("L:");
 lcd.print(Light.DataReadFromSensor);
 lcd.print("%");
 
 lcd.setCursor(0,1);
 lcd.print("H:");
 lcd.print(round(Humidity.DataReadFromSensor));
 lcd.print("%");
 lcd.setCursor(10,1);
 lcd.print("S:");
 lcd.print(Soilmoisture.DataReadFromSensor);
 lcd.print("%");
 delay(100);
 lcd.clear();
}
void PrintAutoModeOnLcd()
{
  
  lcd.setCursor(2,0);
  lcd.print("DANG O CHE DO");
  lcd.setCursor(4,1);
  lcd.print("TU DONG");
  delay(500);
 }

void AutoMode()
{
  if(AutoModeFlag==1)
{
   if (Soilmoisture.DataReadFromSensor<Soilmoisture.DataSetFromUser)
  {
   digitalWrite(Waterpump_signal,1);
   Serial.println("may bom bat");
  }
  if(Soilmoisture.DataReadFromSensor>Soilmoisture.DataSetFromUser)
  {
   digitalWrite(Waterpump_signal,0);
   Serial.println("may bom tat");
  }
  if(Light.DataReadFromSensor<Light.DataSetFromUser)
  {
   digitalWrite(Led_signal,1);
   Serial.println("den bat");
  }
  if(Light.DataReadFromSensor>Light.DataSetFromUser)
  {
   digitalWrite(Led_signal,0);
   Serial.println("den tat");
  }
  if(Temperature.DataReadFromSensor>Temperature.DataSetFromUser)
  {
   digitalWrite(Fan_signal,1);
   Serial.println("quat bat");
  }
  if(Temperature.DataReadFromSensor<Temperature.DataSetFromUser)
  {
   digitalWrite(Fan_signal,0);
   Serial.println("quat tat");
  }
}
}

void setup()
{
  
  Serial.begin(9600);
  EEPROM.begin(512);
  Blynk.begin(auth, ssid, pass);

  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,1);
  lcd.createChar(1,degree);
  pinMode(Fan_signal,OUTPUT);
  pinMode(Waterpump_signal,OUTPUT);
  pinMode(Led_signal,OUTPUT);
  digitalWrite(Fan_signal,LOW);
  digitalWrite(Waterpump_signal,LOW);
  digitalWrite(Led_signal,LOW);
  LoadDataFromEEPROM();
  timer.setInterval(1000L, ReadDataFromSensorAndUpdateData);
   
}

void Button()
{
  ModeButton = touchRead(4);
  if(ModeButton == 1)
  {    
   delay(500);
   if(ModeButton == 1)
   {
     ModeButtonFlag++;
     if(ModeButtonFlag == 1)//
     {
       AutoModeFlag =1;
       PrintAutoModeOnLcd();//
       Blynk.virtualWrite(V9,1);
       Serial.println("AutoMode ON");
     }
     if (ModeButtonFlag == 2)
     {
       AutoModeFlag = 0;
       Serial.println("AutoMode OFF");
       ModeButtonFlag = 0;
       Blynk.virtualWrite(V9,0);
       lcd.setCursor(2,0);
       lcd.print("DANG O CHE DO");
       lcd.setCursor(4,1);
       lcd.print("CHINH TAY");
       delay(500);
       lcd.clear();
      if(Waterpump_signal ==1 || Fan_signal == 1 || Led_signal == 1)
      {
       digitalWrite(Waterpump_signal,0);
       digitalWrite(Fan_signal,0);
       digitalWrite(Led_signal,0);
       Blynk.virtualWrite(V10,0);
       Blynk.virtualWrite(V11,0);
       Blynk.virtualWrite(V12,0);
      }
       delay(500);
      }
    }
  }
  

  if(AutoModeFlag == 1)
    {
      WaterpumpButton = touchRead(27);
      FanButton = touchRead(12);
      LedButton = touchRead(15);
      //khong the thao tac khi AutoMode On
     if(WaterpumpButton==1)
     {
      PrintAutoModeOnLcd();
     }
     if(FanButton==1)
     {
      PrintAutoModeOnLcd();
     }
     if(LedButton==1)
     {
      PrintAutoModeOnLcd();
     }

      PrintDataReadFromSensorOnLcd();
    }
  if (AutoModeFlag == 0)
  {
    ModeButton = 0;
    WaterpumpButton = touchRead(27);
   
    if(WaterpumpButton == 1)
    {
      delay(500);
      if(WaterpumpButton == 1)
      {
        WaterpumpButtonFlag++;
        if(WaterpumpButtonFlag == 1)
        {
         Blynk.virtualWrite(V11,HIGH);
         digitalWrite(Waterpump_signal,HIGH);
         lcd.setCursor(0,0);
         lcd.print("MAY BOM DA DUOC");
         lcd.setCursor(7,1);
         lcd.print("BAT");
         Serial.println("Waterpump ON");
         delay(1000);
         lcd.clear();
        }
        if (WaterpumpButtonFlag==2)
        {
         Blynk.virtualWrite(V11,LOW);
         digitalWrite(Waterpump_signal,LOW);
         lcd.setCursor(0,0);
         lcd.print("MAY BOM DA DUOC");
         lcd.setCursor(7,1);
         lcd.print("TAT");
         Serial.println("Waterpump OFF");
         delay(1000);
         lcd.clear();
         WaterpumpButtonFlag=0;
        }
      }
    }
 
      FanButton = touchRead(12);
     
      if(FanButton == 1)
      {
        delay(500);
        if(FanButton == 1)
        {
          FanButtonFlag++;
          if(FanButtonFlag==1)
          {
           Blynk.virtualWrite(V10,HIGH);
           digitalWrite(Fan_signal, HIGH);
           lcd.setCursor(3,0);
           lcd.print("QUAT DA DUOC");
           lcd.setCursor(7,1);
           lcd.print("BAT");
           delay(1000);
           lcd.clear();
           Serial.println("Fan ON");
          }
          if(FanButtonFlag==2)
          {
           Blynk.virtualWrite(V10,LOW);
           digitalWrite(Fan_signal,LOW);
           lcd.setCursor(3,0);
           lcd.print("QUAT DA DUOC");
           lcd.setCursor(7,1);
           lcd.print("TAT");
           delay(1000);
           lcd.clear();
           FanButtonFlag=0;
           Serial.println("Fan OFF");
          }
        }
      }
  
     LedButton = touchRead(15);
     
     if(LedButton == 1)
     {
     delay(500);
     if(LedButton == 1)
     {
       LedButtonFlag++;
       if(LedButtonFlag == 1)
       {
        Blynk.virtualWrite(V12,HIGH);
        digitalWrite(Led_signal,HIGH);
        lcd.setCursor(3,0);
        lcd.print("DEN DA DUOC");
        lcd.setCursor(7,1);
        lcd.print("BAT");
        delay(1000);
        lcd.clear();
        Serial.println("Led ON");
       }
       if(LedButtonFlag==2)
       {
        Blynk.virtualWrite(V12,LOW);
        digitalWrite(Led_signal,LOW);
        lcd.setCursor(3,0);
        lcd.print("DEN DA DUOC");
        lcd.setCursor(7,1);
        lcd.print("TAT");
        delay(1000);
        lcd.clear();
        LedButtonFlag = 0;
        Serial.println("Led OFF");
       }
     }
   }
  }
}


void loop()
{
  PrintDataReadFromSensorOnLcd();
  Blynk.run();
  timer.run();
  Button();
  AutoMode();
}
