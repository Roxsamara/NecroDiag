#include "Arduino.h"
TaskHandle_t Task1;
TaskHandle_t Task2;
uint32_t timer_start_1 = 0;
uint32_t time_stress1 = 0;
uint32_t time_stress2 = 0;
HardwareSerial SerialPort(2); // use UART2

int incomingByte = 0; // для хранения полученного байта
uint8_t nomer = 0;
uint8_t byte_in = 0x09;
uint8_t dlina = 0;
uint8_t byte_2 = 0;
uint8_t aski = 0;
uint8_t tipe = 0;
uint32_t time_1 = 0;
uint32_t time_2 = 0;
uint8_t dme[14] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t diag[14] = {0x03, 0x00, 0x09, 0x03, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t ASCII_1[14];
uint8_t ASCII_2[14];
uint8_t ASCII_3[14];
uint8_t ASCII_4[14];
uint8_t ASCII_5[14];
bool WUP = 0;
bool BT_par_ok = 0;
char target[] = "reset";
char target_07[] = "go";
bool serv_bit = 0;
float volt1 = 0;

void init_dme();
void clear_buffer();
void ecu_ressive();
void ecu_transive();
void Task1code(void *pvParameters);
void task1_setup();
void task1_mirgalka();
void Task2code(void *pvParameters);
void vivod_ROM();
void vibor();
void viborBT();
void vivod_ascii();

void vivod_error();
void ping_diag();
void vivod_DAC();
void prog_pin_2();
void crash_check();


#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;

bool XZ = 0;
void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
  if (event != 31)
  { // ждем событие 31
    XZ = 1;
  }
}
// здесь производится вейкап
void init_dme()
{
  if (XZ == 1)
  {
    XZ = 0;
    SerialBT.print("GO_2");
    SerialBT.println();
  }
  Serial.print("GO_2");
  Serial.println();

  delay(2000);
  digitalWrite(22, HIGH);
  digitalWrite(2, HIGH);
  if (XZ == 1)
  {
    XZ = 0;
    SerialBT.print("GO_3");
    SerialBT.println();
  }
  Serial.print("GO_3");
  Serial.println();

  delay(2000);
  digitalWrite(22, LOW);
  digitalWrite(2, LOW);
  if (XZ == 1)
  {
    XZ = 0;
    SerialBT.print("GO_4");
    SerialBT.println();
  }
  Serial.print("GO_4");
  Serial.println();

  delay(1000);
  digitalWrite(22, HIGH);
  digitalWrite(2, HIGH);
  if (XZ == 1)
  {
    XZ = 0;
    SerialBT.print("GO_5");
    SerialBT.println();
  }
  Serial.print("GO_5");
  Serial.println();

  delay(200);
  digitalWrite(22, LOW);
  digitalWrite(2, LOW);
  if (XZ == 1)
  {
    XZ = 0;
    SerialBT.print("GO_6");
    SerialBT.println();
  }
  Serial.print("GO_6");
  Serial.println();

  delay(600);

  digitalWrite(22, HIGH);
  digitalWrite(2, HIGH);
  digitalWrite(13, HIGH);

  while (1)
  {
    if (SerialPort.available() > 0)
    {
      int inByte = SerialPort.read();
      if (inByte == 0x81)
      {
        delay(28);
        SerialPort.write(0xFF - inByte); // 0x7E
        clear_buffer();
      }
      if (inByte == 0x0D)
      {
        timer_start_1 = millis();
        //  SerialBT.println("эбу успешно пнулся!");
        WUP = 1;
        return;
      }
    }
  }
}
// функция очистки буфера
void clear_buffer()
{
  SerialPort.flush();
  while (SerialPort.available() > 0)
  {

    SerialPort.read();
  }
}
// прием
void ecu_ressive() ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  digitalWrite(13, HIGH);
  byte_2 = 0;

  while (true)
  {
    crash_check();
    delay(1);
    if (SerialPort.available() > 0)
    {
      uint8_t inByte = SerialPort.read();
      clear_buffer();
      uint8_t out_byte = 0xFF - inByte;

      if (byte_2 == 0)
        dlina = inByte;
      while (byte_2 > dlina)
      {
        crash_check();
        Serial.println("error dlinna ");
        Serial.println(dlina);
        delay(1000);
      }

      if (byte_2 == 1)
      {
        nomer = inByte;
      }
      if (byte_2 == 2)
      {
        tipe = inByte;
      }

      if (((inByte == 0x03) && (byte_2 == dlina)))
      {
        dme[byte_2] = inByte;
        byte_2++;
        clear_buffer();
        // digitalWrite(13, LOW);

        if (nomer == 1)
        {
          if (((tipe == 0xF6) && (true)))
          {
            for (int i = 0; i < 14; i++)
            {
              ASCII_1[i] = dme[i];
            }
          }
        }
        if (nomer == 3)
        {
          if (((tipe == 0xF6) && (true)))
          {
            for (int i = 0; i < 14; i++)
            {
              ASCII_2[i] = dme[i];
            }
          }
        }
        //  Serial.println("ressive ok");
        digitalWrite(13, LOW);
        return;
      }
      else
      {
        delay(4);
        SerialPort.write(out_byte);
        SerialPort.flush();
        clear_buffer();
        delay(2);
      }
      dme[byte_2] = inByte;
      byte_2++;
    }
  }
}
// функция отправки
void ecu_transive() ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{

  digitalWrite(13, HIGH);
  if (nomer == 255)
    nomer = 0;
  else
    nomer++;
  diag[1] = nomer;
  delay(4);

  for (uint8_t i = 0; i < (diag[0]); i++)
  {
    delay(3);
    SerialPort.write(diag[i]);
    while (SerialPort.read() != (0xFF - diag[i]))
    {
      crash_check();
    }
  }
  delay(3);
  SerialPort.write(diag[diag[0]]); // отправляем 0х03 из массива, по его адресу.
  SerialPort.flush();

  while (SerialPort.read() != diag[diag[0]])
  {
    crash_check();
  }

  digitalWrite(13, LOW);
}
// определяем обрыв ссвязи
void crash_check()
{
  if ((millis() - time_stress1 > 2000) && (WUP == 1) && (millis() - timer_start_1 > 2000))
    esp_restart();
}

void setup()
{
  Serial.begin(115200);
  pinMode(22, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(2, OUTPUT);
  SerialBT.register_callback(callback);
  SerialPort.begin(9600, SERIAL_8N1, 16, 17);
  time_2 = millis();
  SerialBT.begin("ESP32_DIAG"); // Bluetooth device name
  
  while ((SerialBT.available() == 0) && (Serial.available() == 0))
  {
    digitalWrite(2, (!digitalRead(2)));

    delay(100);
/*
    if (XZ == 1)
    {
      XZ = 0;
      SerialBT.print("1");
      SerialBT.println();
    }*/
    Serial.print("GO_1");
    Serial.println();
    SerialBT.print("GO_1");
    SerialBT.println();
    
  }

}

void loop()

{


  time_stress2 = millis() - time_stress1;
  time_stress1 = millis();
  // Serial.print("loop_time ");
  // if (time_stress2 > 250)
  // Serial.print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  // Serial.println(time_stress2);

  if (WUP == 0)
  {
    init_dme();
  }

  digitalWrite(2, LOW);
  // сначала делаем прием далее определяем тип ответа, рам, ром, ацп
  ecu_ressive();
  if (tipe == 0xFB)
  { 
    vivod_DAC();}
  if (tipe == 0xFE)
  {
    vivod_ROM();}
  if (tipe == 0xF6)
  {
    vivod_ascii();}
  if (tipe == 0xFE)
    vivod_error();
  digitalWrite(2, HIGH);
  ecu_transive();

  if (SerialBT.available() > 0)
  {
    viborBT();
  }

  if (Serial.available() > 0)
  {
    vibor();
  }
}



void vivod_ascii(){
  prog_pin_2();

  Serial.print("DME ");
  for (uint8_t i = 0; i < (ASCII_1[0] + 1); i++)
  {
    if ((1 <= i) && (10 >= i))
    {
      Serial.write(ASCII_1[ASCII_1[0] - i]);
    }
  }

  Serial.println();

  Serial.print("SOFT ");
  for (uint8_t i = 0; i < (ASCII_2[0] + 1); i++)
  {
    if ((1 <= i) && (10 >= i))
    {

      Serial.write(ASCII_2[ASCII_2[0] - i]);
    }
  }
  Serial.println();

  if (XZ == 1)
  {
    XZ = 0;
    SerialBT.print("DME ");
    for (uint8_t i = 0; i < (ASCII_1[0] + 1); i++)
    {
      if ((1 <= i) && (10 >= i))
      {
        SerialBT.write(ASCII_1[ASCII_1[0] - i]);
      }
    }
    SerialBT.println();
    SerialBT.print("SOFT ");
    for (uint8_t i = 0; i < (ASCII_2[0] + 1); i++)
    {
      if ((1 <= i) && (10 >= i))
      {
        SerialBT.write(ASCII_2[ASCII_2[0] - i]);
      }
    }
    SerialBT.println();
  }
  prog_pin_2();
  ping_diag();
}

void vivod_DAC()
{
  prog_pin_2();

  int i = diag[2];
  int j = diag[0];

  {
float dme3 = dme[3];
float dme4 = dme[4];

    if (XZ == 1)
    {
      XZ = 0;
      SerialBT.print("A ");
      SerialBT.print((dme3),0);
      SerialBT.print(" ");
      SerialBT.println((dme4),0);
    }
    Serial.print("A ");
    Serial.print((dme3),0);
    Serial.print(" ");
    Serial.println((dme4),0);
  }


  prog_pin_2();
}

void vivod_ROM()
{
  prog_pin_2();
  time_2 = millis();
  {
float dme3 = dme[3];
float dme4 = dme[4];

    if (XZ == 1)
    {
      XZ = 0;
      SerialBT.print("R ");
      SerialBT.print((dme3),0);
      SerialBT.print(" ");
      SerialBT.println((dme4),0);
    }
    Serial.print("R ");
    Serial.print((dme3),0);
    Serial.print(" ");
    Serial.println((dme4),0);
  }

  prog_pin_2();
}

void vivod_error()
{
  prog_pin_2();
  time_2 = millis();
  if ((dme[0] == 0x04))
  {
    if (XZ == 1)
    {
      XZ = 0;
      SerialBT.print("NO_ERROR ");
      SerialBT.print(0);
      SerialBT.println();
    }
  }
  if ((dme[0] > 0x04))
  {
    if (XZ == 1)
    {
      XZ = 0;
      SerialBT.print("ERROR  ");
      SerialBT.print(dme[3], HEX);
      SerialBT.print("_");
      SerialBT.print(dme[4], HEX);
      SerialBT.print("_");
      SerialBT.print(dme[5], HEX);
      SerialBT.print("_");
      SerialBT.print(dme[6], HEX);
      SerialBT.print("_");
      SerialBT.print(dme[7], HEX);
      SerialBT.println();
    }
  }
  prog_pin_2();
}
//принимает в DEC переводит в массив
void vibor()
{

  while (Serial.available() > 0)
  {
  //  char diag2[14];
  char str1[99];
   int amount =  Serial.readBytesUntil (';',str1,99);
   str1[amount]=0;
    int data2[10];
    int count = 0;
    char* offset = str1;
    while (true)
    {
      diag[count++] = atoi(offset),HEX;
      offset = strchr(offset,',');
      if (offset) offset++;
      else break;
    }
      }
}
void viborBT()
{
  while (SerialBT.available() > 0)
  {
  //  char diag2[14];
  char str1[99];
   int amount =  SerialBT.readBytesUntil (';',str1,99);
   str1[amount]=0;
    int data2[10];
    int count = 0;
    char* offset = str1;
    while (true)
    {
      diag[count++] = atoi(offset),HEX;
      offset = strchr(offset,',');
      if (offset) offset++;
      else break;
    }


  }
}

void prog_pin_2()
{
  digitalWrite(13, (HIGH));
  digitalWrite(13, (LOW));
  digitalWrite(13, (HIGH));
  digitalWrite(13, (LOW));
  digitalWrite(13, (HIGH));
  digitalWrite(13, (LOW));
  digitalWrite(13, (HIGH));
  digitalWrite(13, (LOW));
  digitalWrite(13, (HIGH));
  digitalWrite(13, (LOW));
  digitalWrite(13, (HIGH));
  digitalWrite(13, (LOW));
  digitalWrite(13, (HIGH));
  digitalWrite(13, (LOW));
  digitalWrite(13, (HIGH));
  digitalWrite(13, (LOW));
  digitalWrite(13, (HIGH));
  digitalWrite(13, (LOW));
  digitalWrite(13, (HIGH));
  digitalWrite(13, (LOW));
  digitalWrite(13, (HIGH));
}
void ping_diag()
{
  diag[0] = 0x03;
  diag[1] = 0x00;
  diag[2] = 0x09;
  diag[3] = 0x03;
}