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
void vibor(uint8_t var);
void vivod_ascii();

void vivod_error();

void ping_diag();

void volt_diag();
void TW_diag();
void TA_diag();
void O2_diag();
void TPS_diag();
void MAF_diag();

void taho_diag();
void speed_diag();

void vivod_DAC();

void prog_pin_2();
void crash_check();

void error_diag();
void LOAD_diag();
void ANGLE_diag();
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

void clear_buffer()
{
  SerialPort.flush();
  while (SerialPort.available() > 0)
  {

    SerialPort.read();
  }
}
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

    if (XZ == 1)
    {
      XZ = 0;
      SerialBT.print("1");
      SerialBT.println();
    }
    Serial.print("GO_1");
    Serial.println();
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
  ecu_ressive();
  if (tipe == 0xFB)

    vivod_DAC();
  if (tipe == 0xFE)
    vivod_ROM();

  if (tipe == 0xFC)
    vivod_error();
  digitalWrite(2, HIGH);
  ecu_transive();

  if (SerialBT.available() > 0)
  {
    vibor(SerialBT.read());
  }

  if (Serial.available() > 0)
  {
    vibor(Serial.read());
  }
}
void vivod_ROM()
{
  prog_pin_2();
  time_2 = millis();
  if ((diag[3] == 0x01) && (diag[5] == 0x16))
  {
    float speed_vivod = dme[3] * 1.224358969230769;
    if (XZ == 1)
    {
      XZ = 0;
      SerialBT.print("Speed ");
      SerialBT.println(speed_vivod, 0);
    }
    Serial.print("Speed ");
    Serial.println(speed_vivod, 0);
  }
  if ((diag[3] == 0x01) && (diag[5] == 0xCF))
  {
    float taho_vivod = dme[3];
    taho_vivod = taho_vivod * 40;
    if (XZ == 1)
    {
      XZ = 0;
      SerialBT.print("Taho ");
      SerialBT.println(taho_vivod, 0);
    }
    Serial.print("Taho ");
    Serial.println(taho_vivod, 0);
  }
  if ((diag[3] == 0x01) && (diag[5] == 0xD5))
  {
    float load_vivod = dme[3];
    if (XZ == 1)
    {
      XZ = 0;
      SerialBT.print("Load ");
      SerialBT.println(load_vivod);
    }
    Serial.print("Load ");
    Serial.println(load_vivod);
  }
    if ((diag[3] == 0x01) && (diag[5] == 0xB0))
  {
    float load_vivod = dme[3];
    if (XZ == 1)
    {
      XZ = 0;
      SerialBT.print("Angle ");
      SerialBT.println(load_vivod);
    }
    Serial.print("Angle ");
    Serial.println(load_vivod);
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

void vibor(uint8_t var1)
{
  while (SerialBT.available() > 0)
  {
    SerialBT.read();
  }

  Serial.println();

   Serial.println(var1);
  switch (var1)
  {
  case 49:
    esp_restart();
    break;

  case 50:
    ping_diag();
    break;

  case 51:
    volt_diag();
    break;

  case 52:
    TW_diag();
    break;

  case 53:
    TA_diag();
    break;

  case 54:
    speed_diag();
    break;

  case 55:
    O2_diag();
    break;

  case 56:
    vivod_ascii();
    break;

  case 57:
    taho_diag();
    break;

  case 97:
    // error_diag();
    break;

  case 98:
    TPS_diag();
    break;

  case 99:
    MAF_diag();
    break;

  case 100:
    LOAD_diag();
    break;
  case 101:
    ANGLE_diag();
    break;
    // default:
    //  выполняется, если не выбрана ни одна альтернатива
    //  default необязателен
  }
}

void vivod_ascii()
{
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
  // Serial.println(SerialBT.availableForWrite());
  prog_pin_2();
  ping_diag();
}
void ping_diag()
{
  diag[0] = 0x03;
  diag[1] = 0x00;
  diag[2] = 0x09;
  diag[3] = 0x03;
}

void volt_diag()
{
  diag[0] = 0x04;
  diag[1] = 0x00;
  diag[2] = 0x08;
  diag[3] = 0x03;
  diag[4] = 0x03;
}

void TW_diag()
{
  diag[0] = 0x04;
  diag[1] = 0x00;
  diag[2] = 0x08;
  diag[3] = 0x07;
  diag[4] = 0x03;
}

void TA_diag()
{
  diag[0] = 0x04;
  diag[1] = 0x00;
  diag[2] = 0x08;
  diag[3] = 0x01;
  diag[4] = 0x03;
}

void O2_diag()
{
  diag[0] = 0x04;
  diag[1] = 0x00;
  diag[2] = 0x08;
  diag[3] = 0x06;
  diag[4] = 0x03;
}

void speed_diag()
{
  diag[0] = 0x06;
  diag[1] = 0x00;
  diag[2] = 0x01;
  diag[3] = 0x01;
  diag[4] = 0x09;
  diag[5] = 0x16;
  diag[6] = 0x03;
}
void taho_diag()
{
  diag[0] = 0x06;
  diag[1] = 0x00;
  diag[2] = 0x01;
  diag[3] = 0x01;
  diag[4] = 0x00;
  diag[5] = 0xCF;
  diag[6] = 0x03;
}
// Чтение TC 07 (неисправности) 03 NN 07 03
//                  05 TC 03 Сброс NN 05 03
void error_diag()
{
  diag[0] = 0x03;
  diag[1] = 0x00;
  diag[2] = 0x07;
  diag[3] = 0x03;
}
void TPS_diag()
{
  diag[0] = 0x04;
  diag[1] = 0x00;
  diag[2] = 0x08;
  diag[3] = 0x02;
  diag[4] = 0x03;
}
void MAF_diag()
{
  diag[0] = 0x04;
  diag[1] = 0x00;
  diag[2] = 0x08;
  diag[3] = 0x00;
  diag[4] = 0x03;
}
void LOAD_diag()
{
  diag[0] = 0x06;
  diag[1] = 0x00;
  diag[2] = 0x01;
  diag[3] = 0x01;
  diag[4] = 0x00;
  diag[5] = 0xD5;
  diag[6] = 0x03;
}
void ANGLE_diag()
{
  diag[0] = 0x06;
  diag[1] = 0x00;
  diag[2] = 0x01;
  diag[3] = 0x01;
  diag[4] = 0x00;
  diag[5] = 0xB0;
  diag[6] = 0x03;
}
void vivod_DAC()
{
  prog_pin_2();

  // Serial.println("vivod_DAC");
  int i = diag[3];
  int j = diag[0];
  if ((i == 0x03) && (j >= 0x04))
  {
    volt1 = ((((dme[3]) * 255) + (dme[4]))) * 0.01757 * 0.9806666666666667;
    if (XZ == 1)
    {
      XZ = 0;
      SerialBT.print("Voltage ");
      SerialBT.println(volt1, 2);
    }
    Serial.print("Voltage ");
    Serial.println(volt1, 2);
  }
  if (i == 0x07)
  {
    if (XZ == 1)
    {
      XZ = 0;
      SerialBT.print("Temp_water ");
      SerialBT.println((-32.5 + (1024 - (((dme[3]) * 255) + (dme[4]))) / 6.171945701357466) * 0.9, 1);
    }
    Serial.print("Temp_water ");
    Serial.println((-32.5 + (1024 - (((dme[3]) * 255) + (dme[4]))) / 6.171945701357466) * 0.9, 1);
  }
  if (i == 0x01)
  {
    if (XZ == 1)
    {
      XZ = 0;
      SerialBT.print("Temp_air ");
      SerialBT.println((-32.5 + (1024 - (((dme[3]) * 255) + (dme[4]))) / 6.171945701357466) * 0.9, 1);
    }
    Serial.print("Temp_air ");
    Serial.println((-32.5 + (1024 - (((dme[3]) * 255) + (dme[4]))) / 6.171945701357466) * 0.9, 1);
  }
  if (i == 0x06)
  {
    float O2_vivod = dme[4] * 0.004;
    if (XZ == 1)
    {
      XZ = 0;
      SerialBT.print("O_2 ");
      SerialBT.println(O2_vivod, 3);
    }
    Serial.print("O_2 ");
    Serial.println(O2_vivod, 3);
  }
  if (i == 0x02)
  {
    float tps_vivod = ((dme[3] * 255) + dme[4]) / 204.3;
    float tps_procent_vivod = ((dme[3] * 255) + dme[4]) / 10.23;
    if (XZ == 1)
    {
      XZ = 0;
      SerialBT.print("TPS_V ");
      SerialBT.print(tps_vivod);
      SerialBT.println();
    }
    Serial.print("TPS_V ");
    Serial.print(tps_vivod);
    Serial.println();
  }
  if (i == 0x00)
  {
    float maf_vivod = ((dme[3] * 255) + dme[4]);
    if (XZ == 1)
    {
      XZ = 0;
      SerialBT.print("MAF ");
      SerialBT.print(maf_vivod, 0);
      SerialBT.println();
    }
    Serial.print("MAF ");
    Serial.print(maf_vivod, 0);
    Serial.println();
  }
  prog_pin_2();
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