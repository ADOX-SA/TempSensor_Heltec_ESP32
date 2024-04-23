#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_SDA 17
#define OLED_SCL 18
#define OLED_RST 21       // 4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

void oled_esp32_begin(){
    pinMode(OLED_RST, OUTPUT); // reseteamos la pantalla OLED para comenzar
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  Wire.begin(OLED_SDA, OLED_SCL); // inicia OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
/*
  display.clearDisplay();
  display.display();

  display.setCursor(0, 55); // Start at top-left corner

  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.println(firmVer);

  display.setCursor(15, 0); // Start at top-left corner

  display.setTextSize(4); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.println(F("ADOX"));

  display.setCursor(0, 35); // Start at top-left corner

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.println(oled_modo);

  display.display();
  */
}

void oled_receptor(String data_received){

  display.clearDisplay();
  display.display();

  display.setCursor(0, 0); // Start at top-left corner
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.println("Receptor");

  display.setCursor(0, 25); // Start at top-left corner
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.println("Data:");
  
  display.setCursor(0, 35); // Start at top-left corner
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.println(data_received);


  display.display();
}

void oled_transmisor(String data_sent){

  display.clearDisplay();
  display.display();

  display.setCursor(0, 0); // Start at top-left corner
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.println("Transmisor");

  display.setCursor(0, 25); // Start at top-left corner
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.println("Data:");
  
  display.setCursor(0, 35); // Start at top-left corner
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.println(data_sent);


  display.display();
}


void oled_init()
{

  display.clearDisplay();
  display.display();
  display.setTextColor(SSD1306_WHITE);

  int time1 = 200;

  String aux = "ADOX";
  int tam = aux.length();
  display.setCursor(25, 0); // Start at top-left corner
  display.setTextSize(3);   // Draw 2X-scale text
  for (int i = 0; i < tam; i++)
  {
    display.print(aux[i]);
    display.display();
    delay(time1);
  }

  delay(400);
  time1 = 75;
  aux = "Termometro";
  tam = aux.length();
  display.setCursor(0, 26); // Start at top-left corner
  display.setTextSize(2);
  for (int i = 0; i < tam; i++)
  {
    if (i == 4)
    {
      display.cp437(true);
      display.write(162);
    }
    else
    {
      display.print(aux[i]);
    }
    display.display();
    delay(time1);
  }

  aux = "Infrarrojo";
  tam = aux.length();
  display.setCursor(0, 44); // Start at top-left corner
  display.setTextSize(2);

  for (int i = 0; i < tam; i++)
  {

    display.print(aux[i]);

    display.display();
    delay(time1);
  }

  aux = firmVer;
  tam = aux.length();
  display.setCursor(105, 13); // Start at top-left corner
  display.setTextSize(1);
  for (int i = 0; i < tam; i++)
  {
    display.print(aux[i]);
    display.display();
    delay(time1);
  }
}

