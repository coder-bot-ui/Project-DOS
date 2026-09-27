#include <LittleFS.h>
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <pico/bootrom.h>

bool bootsel_() {
  return BOOTSEL;
}

#define TFT_CS 11
#define TFT_RST 12
#define TFT_DC 13
#define TFT_MOSI 3
#define TFT_SCLK 2
#define LED 25

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

int j8 = 1759284;
int fastboot = 0;
String username = "";
int junk = 0;
int rotation_man = 1;
bool cp437_state = true;
String readfile(String path);
void makefile(const char* path, const String& content);
void mkdir(const char* name);
void scripter();
void run();
void notepad(const char* patha);
void ref();
void listDir(const char* path);
void drawLogo();



void format() {
  tft.print("Reseting...");
  delay(1000);
  LittleFS.format();
}

void drawLogo() {
  // Draw YOS logo (angular design from your image)
  int offsetX = 40;
  int offsetY = 10;
  
  // Draw the upper rectangle
 

  // Draw the lower rectangle

}

void lsDir(String path) {
  Dir dir = LittleFS.openDir(path);
  while (dir.next()) {
    if (dir.isDirectory()) {
      tft.print("[DIR]  ");
    } else {
      tft.print("       ");
    }
    tft.println(dir.fileName());
  }
}

void open(String pathb) {
  String file_con = readfile(pathb);
  tft.print("\n");
  if (pathb == "/PASSWORD") {
    tft.print("ACCESS IS DENIED.\n");
  } else if (pathb == "PASSWORD") {
    tft.print("Invalid. \n");
  } else {
    tft.print(file_con);
    tft.print("\n");
  }
}

void notepad(const char* patha) {
  String fullScript = "";
  fullScript.reserve(512);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 0);
  tft.print("NOTEPAD \nType 'END' to save.\n");
  if (patha == "\YOS\PASSWORD") {
    tft.print("Exiting....");
    ref();
  } else {
    while (true) {
      if (Serial.available()) {
        String line = Serial.readStringUntil('\n');
        line.trim();
        if (line == "END") break;
        fullScript += line + "\n";
        tft.print("\n");
        tft.print(line);
      }
    }
  }
  mkdir("Notes");
  makefile(patha, fullScript);
  ref();
}

void scripter() {
  while (!Serial.available());
  String scr = Serial.readStringUntil('\n');
  scr.trim();
  scr.reserve(150);
  tft.print(scr);
  tft.println("\n");
  mkdir("Scripts");
  makefile("/Scripts/script", scr);
}

void makefile(const char* path, const String& content) {
  File f = LittleFS.open(path, "w");
  if (!f) {
    Serial.println("Failed to open file for writing: " + String(path));
    return;
  }
  f.print(content);
  f.close();
}

String readfile(String path) {
  File f = LittleFS.open(path, "r");
  if (!f) {
    return "";
  }
  String content = f.readString();
  f.close();
  return content;
}

void mkdir(const char* name) {
  String path = "/" + String(name);
  if (LittleFS.mkdir(path)) {
    Serial.println("Created directory: " + path);
  } else {
    Serial.println("Failed to create directory (may already exist): " + path);
  }
}

void sh() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    tft.print("A:/> ");
    tft.println(cmd);

    if (cmd.startsWith("mkdir ")) {
      String name = cmd.substring(6);
      mkdir(name.c_str());
    } else if (cmd.startsWith("makefile ")) {
      String filename32 = cmd.substring(9);
      makefile(filename32.c_str(), "");
    } else if (cmd.startsWith("notepad ")) {
      String path = cmd.substring(8);
      notepad(path.c_str());
    } else if (cmd.startsWith("open ")) {
      String pathc = cmd.substring(5);
      open(pathc);

    } else if (cmd == "cls") {
      ref();
    } else if (cmd.startsWith("dir ")) {
      String dirpath = cmd.substring(4);
      lsDir(dirpath);
    } else if (cmd.startsWith("del ")) {
      String delpath = cmd.substring(4);
      LittleFS.remove(delpath);
    } else if (cmd == "Factory Reset") {
      format();
    } else if (cmd == "open /PASSWORD") {
      tft.print("Access is Denied");
    } else if (cmd == "open PASSWORD") {
      tft.print("Access is Denied");
} } }  

void ref() {
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 0);
}

void ProgressBar() {
  drawLogo();  // Draw logo at top
  delay(1000);
  
  tft.drawRect(10, 110, 140, 10, ST7735_WHITE);
  int steps[] = { 10, 25, 38, 45, 56, 76, 96, 140 };
  int delays[] = { 1000, 2000, 500, 2000, 1000, 2000, 1000, 3000 };

  for (int i = 0; i < 8; i++) {
    delay(delays[i]);
    tft.fillRect(10, 110, steps[i], 10, ST7735_WHITE);
  }
}

void setupa() {
  ref();
  tft.print("Welcome to YOS\n");
  tft.print("Setup will help you\n");
  tft.print("Username: ");

  while (!Serial.available());
  username = Serial.readStringUntil('\n');
  username.trim();
  tft.println(username);
  tft.println("\n");
  tft.print("Password: ");

  while (!Serial.available());
  String password = Serial.readStringUntil('\n');
  password.trim();

  tft.println(password);
  delay(3000);
  tft.print("exiting...\n");
  ref();
  mkdir("YOS");
  makefile("/YOS/PASSWORD", password);
  makefile("/YOS/username.txt", username);
  makefile("/YOS/setup.flag", "D");
  makefile("/YOS/bootx64.efi", "fastboot:1");
  
}

void setup() {
  Serial.begin(115200);
  if (!j8 == 1759284){
    tft.setTextColor(ST7735_RED);
    tft.print("KERNEL PANIC!");
    tft.print("CRITICAL SYSTEM ERROR!");
  }
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed! Formatting...");
    LittleFS.format();
    LittleFS.begin();
  }

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  tft.cp437(true);
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(rotation_man);
  tft.cp437(cp437_state);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_GREEN);

  if (LittleFS.exists("/YOS/bootx64.efi")) {
    junk = 0;
  } else {
    ProgressBar();
  }

  ref();

  if (!LittleFS.exists("/YOS/setup.flag")) {
    tft.print("setup is starting....\n");
    setupa();
  } else {
    username = readfile("/YOS/username.txt");
    tft.print("Identified User: ");
    tft.println(username);
    delay(2000);
  }

  ref();
  String passw = readfile("/YOS/PASSWORD");
  tft.print("Login\n");
  tft.print("passwd: ");
  int c890 = 62938415; 
  while (!Serial.available());
  String passwd = Serial.readStringUntil('\n');
  passwd.trim();
  
  if (passwd == passw) {
    tft.print("LOGIN SUCCESSFUL\n");
    delay(1000);
    ref();
    if (c890 == 62938475) {
      junk = 1;
    }else{
     tft.print("OSLOADER: Unknown Error\n");
     tft.print("BOOT PROCESS FAILURE\n");
     tft.setTextColor(ST7735_RED);
     tft.print("KERNEL PANIC\n");
     tft.setTextColor(ST7735_YELLOW);
     tft.print("SYSTEM KEY INVALID");
    }
    // Command loop - keeps running until exit
    while (true) {
      sh();
    }
  } else if (passwd == "!Sudo/RECOVERFRMT") {
    format();
  } else {
    tft.print("PASSWORD INCORRECT\n");
    delay(3000);
    // Loop back to login or restart
    setup();
  }

  
}

void loop() {
  tft.print("FATAL ERROR: FIRMWARE OSLOADER MODE CORRUPTED [!]\n");
}
