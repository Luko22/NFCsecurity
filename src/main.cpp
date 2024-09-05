#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
// #include <Keypad.h>

#include <SPI.h>
#include <MFRC522.h>

////////////////////////MFRC522 /////////////////////////////////
#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
byte accessUID[4]= {0xE3, 0x59, 0xFD, 0x0B};

/////////////////SSD1306////////////////////////
// Screen dimensions and I2C address
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C  // Change if needed

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* secretName = "Laudo";
const char* secretAge = "23";
const char* secretNationality = "Angola";
const char* secretThoughts = "code works";
const char* secretGit = "Luko22";

const char* secretLang[] = {"Pt", "De", "Sp", "En"};

// const byte ROWS = 4; //four rows
// const byte COLS = 4; //four columns
// //define the cymbols on the buttons of the keypads
// char hexaKeys[ROWS][COLS] = {
//   {'1','2','3','A'},
//   {'4','5','6','B'},
//   {'7','8','9','C'},
//   {'*','0','#','D'}
// };
// byte rowPins[ROWS] = {0, 8, 7, 6}; //connect to the row pinouts of the keypad
// byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

// //initialize an instance of class NewKeypad
// Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup() {

  Serial.begin(115200);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Initialize the display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  SPI.begin();			// Init SPI bus
  mfrc522.PCD_Init();		// Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
  pinMode(8, OUTPUT);

  delay(2000);
}

void loop() {

  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 30);
    display.print("SCAN YOUR CARD OR TAG");
    display.display();
    delay(10);
    return;
  }

  int numLangs = sizeof(secretLang) / sizeof(secretLang[0]);
  int totalLength = 0;

  for (int i = 0; i < numLangs; i++) {
    totalLength += strlen(secretLang[i]);
  }
  totalLength += numLangs - 1; // Add space for the "/" separators

  char concatenated[totalLength + 1]; // +1 for the null terminator
  concatenated[0] = '\0';

  for (int i = 0; i < numLangs; i++) {
    strcat(concatenated, secretLang[i]);
    if (i < numLangs - 1) {
      strcat(concatenated, "/");
    }
  }

  // Check if the UID matches
  if(mfrc522.uid.uidByte[0] == accessUID[0] && 
     mfrc522.uid.uidByte[1] == accessUID[1] && 
     mfrc522.uid.uidByte[2] == accessUID[2] && 
     mfrc522.uid.uidByte[3] == accessUID[3]){

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(30, 30);
    display.print("ACCESS GRANTED");
    display.display(); 
    delay(1000);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.print("Name: ");
    display.println(secretName);

    display.setCursor(0, 10);
    display.print("Age: ");
    display.println(secretAge);

    display.setCursor(0, 20);
    display.print("I'm from: ");
    display.print(secretNationality);

    display.setCursor(0, 30);
    display.print("I speak: ");
    display.print(concatenated);

    display.setCursor(0, 40);
    display.print("Thoughts: ");
    display.print(secretThoughts);

    display.setCursor(0, 50);
    display.print("GitHub: ");
    display.print(secretGit);

    display.display(); 
    delay(5000); 
  } 
  else {
    display.clearDisplay();
    display.setTextSize(5);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(55, 20);
    display.print("X");
    display.display(); 
    delay(1000); 

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 30);
    display.print("YOU DON'T HAVE ACCESS");
    display.display(); 
    delay(3000);
  }

  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}