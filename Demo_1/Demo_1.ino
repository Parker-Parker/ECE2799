#include <LiquidCrystal.h>
// **********************************************************************************
#include <RFM69.h>         //get it here: https://github.com/lowpowerlab/rfm69
#include <RFM69_ATC.h>     //get it here: https://github.com/lowpowerlab/RFM69
#include <RFM69_OTA.h>     //get it here: https://github.com/lowpowerlab/RFM69
#include <SPIFlash.h>      //get it here: https://github.com/lowpowerlab/spiflash
#include <SPI.h>           //included with Arduino IDE (www.arduino.cc)

//****************************************************************************************************************
//**** IMPORTANT RADIO SETTINGS - YOU MUST CHANGE/CONFIGURE TO MATCH YOUR HARDWARE TRANSCEIVER CONFIGURATION! ****
//****************************************************************************************************************
#define NODEID         98    //unique for each node on same network
#define NETWORKID     100  //the same on all nodes that talk to each other
#define GATEWAYID     1
//Match frequency to the hardware version of the radio on your Moteino (uncomment one):
//#define FREQUENCY     RF69_433MHZ
#define FREQUENCY     RF69_915MHZ
#define IS_RFM69HW_HCW  //uncomment only for RFM69HW/HCW! Leave out if you have RFM69W/CW!
#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
//*****************************************************************************************************************************
#define ENABLE_ATC      //comment out this line to disable AUTO TRANSMISSION CONTROL
#define ATC_RSSI        -75
//*****************************************************************************************************************************
#define ACK_TIME      30 // max # of ms to wait for an ack

#ifdef __AVR_ATmega1284P__
  #define LED           15 // Moteino MEGAs have LEDs on D15
  #define FLASH_SS      23 // and FLASH SS on D23
#else
  #define LED           9 // Moteinos have LEDs on D9
  #define FLASH_SS      8 // and FLASH SS on D8
#endif

#define SERIAL_BAUD   115200

int TRANSMITPERIOD = 2000; //transmit a packet to gateway so often (in ms)
char buff[50];
byte sendSize=0;

#ifdef ENABLE_ATC
  RFM69_ATC radio;
#else
  RFM69 radio;
#endif






// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 8, en = 7, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int last = 0;
int tempRaw;
int temp;
bool transmit;
bool edge;
String status_msg;
void setup() {
  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  #ifdef IS_RFM69HW_HCW
  radio.setHighPower(); //must include this only for RFM69HW/HCW!
  #endif
  radio.encrypt(ENCRYPTKEY);
  
  #ifdef ENABLE_ATC
  radio.enableAutoPower(ATC_RSSI);
  #endif
  
  sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);

  // set up the LCD's number of columns and rows:
  pinMode(A5, INPUT);
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
}

//long lastPeriod = 0;
//long randNumber1 = 0;
//long randNumber2 = 0;

void loop() {
//    if (radio.receiveDone())
//  {
//    Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("] ");
//    for (byte i = 0; i < radio.DATALEN; i++)
//      Serial.print((char)radio.DATA[i]);
//    Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");
//
//    if (radio.ACKRequested())
//    {
//      radio.sendACK();
//      Serial.print(" - ACK sent");
//    }
//    //Blink(LED,3);
//    Serial.println();
//  }
//  
//  int currPeriod = millis()/TRANSMITPERIOD;
//  if (currPeriod != lastPeriod)
//  {
//    lastPeriod=currPeriod;
//    randNumber1 = random(99);
//    randNumber2 = random(99);
//
//    if (randNumber2 > 70)
//      sprintf(buff, "F:%d.5", randNumber1);
//    else
//      sprintf(buff, "F:-%d.5", randNumber1);
//    sendSize = strlen(buff);
//    
//    if (radio.sendWithRetry(GATEWAYID, buff, sendSize))
//     Serial.print(" ok!");
//    else Serial.print(" nothing...");
//
//    Serial.println();
//   // Blink(LED,3);
//  }
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  tempRaw = analogRead(A5);
  // print the number of seconds since reset:
  if((millis() / 1000)>(last+30)){
    last = (millis() / 1000);
    //transmit
    transmit = true;
  }
  if (transmit) {
    status_msg = "Transmitting...";
    if (((millis() / 1000)-last)>4) {
      transmit = false;
    }
    edge = true;
  }
  else {
    status_msg = "Done.  ";
    edge = false;
  }
  temp = map(tempRaw, 0, 1023, -40, 140);
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print(" Deg C");
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.print((millis() / 1000)-last);
  lcd.print(" ");
  lcd.print(status_msg);
  
  if (edge&&!transmit){
    lcd.clear();
  }  
}
void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
