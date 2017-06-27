#include <TFT.h>

#include <Wire.h>
#include "Adafruit_TCS34725.h"
//#include <Adafruit_ST7735.h>
//#include <Adafruit_GFX.h>

Adafruit_TCS34725 tcs;

#define buttonpin      2
#define DEBUG          1  
#define NUMAVG         3  
#define DELAYMEASURE   10


// TFT Display Pins
#define cs             8    
#define rst            9 
#define dc             10  

// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF
#define GREY     0xC618



Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);

typedef struct {
  byte r;
  byte g;
  byte b;
} RGB;

RGB resultColor = {0, 0, 0};
RGB oldColor = {0, 0, 0};

int numColors = 6;

String colorMap[]{
  "red",
  "purple",
  "blue",
  "green",
  "yellow",
  "orange"
  
  
};

int colors[][3] = { 
  {255, 0, 0},
  {128, 0, 128},
  {0, 0, 255},
  {0, 255, 0},
  {255, 255, 0},
  {255, 153, 0}
 
};

int sampleData[][3] = {
  {124, 66, 62},
  {103, 70, 79},
  {70, 77, 104},
  {85, 110, 63},
  {100, 99, 40},
  {120, 93, 48}
};

void setup() {
  Serial.begin(9600);

  tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_4X);

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(BLACK);

  if (tcs.begin())
  {
    Serial.println("Found TCS34725 sensor.");
//    pinMode(LED, OUTPUT);
  }
  else
  {
//    digitalWrite(LED, HIGH);
    Serial.println("No TCS34725 sensor found - check your wiring.");
    delay(50);
//    digitalWrite(LED, LOW);
    while (1);
  }

  pinMode(buttonpin, INPUT);
}

//___________________________________Main Loop____________________________________________
//________________________________________________________________________________________
//________________________________________________________________________________________
void loop() {
  int x = digitalRead(buttonpin);
//  Serial.println((3 + 3) % 6);

  if (x == 1) {
    measureColor();
    int index = detectColor(resultColor.r, resultColor.g, resultColor.b, sampleData);
    Serial.print("Color: ");
    Serial.println(colorMap[index]);
    index = (index + 3) % numColors;
    int color = convertRGB24toRGB565(colors[index][0], colors[index][1], colors[index][2]);
    draw(color); 
  } 
  
  delay(300);
  oldColor = resultColor;
}
//________________________________________________________________________________________
//________________________________________________________________________________________
//________________________________________________________________________________________


void measureColor() {
  uint16_t r[NUMAVG], g[NUMAVG], b[NUMAVG];
  unsigned long avgR = 0, avgG = 0, avgB = 0, total = 0;

  for (int i = 0; i < NUMAVG; i++)
  {
    r[i] = tcs.read16(TCS34725_RDATAL);
    g[i] = tcs.read16(TCS34725_GDATAL);
    b[i] = tcs.read16(TCS34725_BDATAL);

    delay(DELAYMEASURE);

    total += r[i] + g[i] + b[i];
    avgR += r[i];
    avgG += g[i];
    avgB += b[i];
  }

  resultColor.r = (avgR * 255.0 / total);
  resultColor.g = (avgG * 255.0 / total);
  resultColor.b = (avgB * 255.0 / total);
  
  int color = convertRGB24toRGB565(resultColor.r, resultColor.g, resultColor.b);


  if (DEBUG)
  {
    Serial.println("Measured color:");
    Serial.print("R: "); Serial.print(resultColor.r); Serial.print(", ");
    Serial.print("G: "); Serial.print(resultColor.g); Serial.print(", ");
    Serial.print("B: "); Serial.print(resultColor.b); Serial.println("");
  }

}

int detectColor(int red, int green, int blue, int sampleData[][3]) {
  float minDist = 1000000;
  int minIndex = 0;
  for (int x = 0; x < numColors; x++){
    int dist = sq(abs(sampleData[x][0] - red));
    dist += sq(abs(sampleData[x][1] - green));
    dist += sq(abs(sampleData[x][2] - blue));
    dist = sqrt(dist);
    if (dist < minDist) {
      minDist = dist;
      minIndex = x;
    }
  }
  return minIndex;
}
 

void draw(int color) {
   
   tft.fillScreen(color);
}

uint16_t convertRGB24toRGB565(uint8_t r, uint8_t g, uint8_t b){
  return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}

int getIndex(String color, String colorMap[]){
  for (int x; x <= numColors; x++) {
    if (colorMap[x].equals(color)) {
      return x;
    }
  }   
}





