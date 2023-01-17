#include <Adafruit_NeoPixel.h>

#define BOARD_NUM 1
#define PIN_NEOPIXEL 2
#define CDSVALUE_1 305.00    // CDS 센서의 값이 CDSVALUE_1보다 크면 (밝은거니까) 그 칸에 말이 없는 것
#define LED_NUMBERS 128
#define BRIGHTNESS 100
int ending =1;
int cdsState[8][8];

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_NUMBERS, PIN_NEOPIXEL);

uint32_t white = strip.Color(200, 200, 200);
uint32_t red = strip.Color(200, 0, 0);
uint32_t green = strip.Color(0, 200, 0);
uint32_t blue = strip.Color(0, 0, 200);
uint32_t blank = strip.Color(0, 0, 0);
  
void setup() {
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();
  
  Serial.begin(9600);
  
  for(int i=2*BOARD_NUM-2;i<2*BOARD_NUM;i++){
    for(int j=0;j<8;j++){
      if (BOARD_NUM == 4 || BOARD_NUM == 1){
        cdsState[i][j]=1;
      }
      else{
        cdsState[i][j]=0;
      }
    }
  }
}


void loop() {
  
  if (Serial.available()>0){
    String S = Serial.readString();
    char color = S[0];
    
    if ( color == 'E'){
      ending = 1;
    }
    
    else if ( color == 'w'){
      for(int n=0;n<5;n++){
        if(BOARD_NUM == 1 or BOARD_NUM == 2){   //White
          for(int i=1;i>=0;i--){
            for(int j=0;j<8;j++){
              neo(i,j,'W');
            }
            delay(500);
          }
          for(int i=1;i>=0;i--){
            for(int j=0;j<8;j++){
              neo(i,j,'b');
            }
            delay(500);
          }
        }
        else{                                   //Black
          for(int i=0;i<2;i++){
            for(int j=0;j<8;j++){
              neo(i,j,'W');
            }
            delay(500);
          }
          for(int i=0;i<2;i++){
            for(int j=0;j<8;j++){
              neo(i,j,'b');
            }
            delay(500);
          }
        }
      }
      for(int n=1;n<5;n++){
        for(int i=0;i<2;i++){
           for(int j=0;j<8;j++){
             neo(i,j,'W');
           }
        }
        delay(500);
        for(int i=0;i<2;i++){
           for(int j=0;j<8;j++){
             neo(i,j,'b');
           }
        }
        delay(500);
      }
    }
    
    else{
      int n = S.length()-1;
      n = n/2;
      for(int i=0;i<n;i++){
        int S1 = int(S[2*i+1]);
        int S2 = int(S[2*i+2]);
        int x = S1-48;
        int y = S2-48;
        neo(x,y,color);
       }
     }
  }
  
  if (ending==1){
    allblank();
    ///////////////////// 시작하기 ///////////////////////
    int toStart = 0;
    while ( toStart == 0 ){
      if (Serial.available()>0){
        char waitingToStart = Serial.read();
        if ( waitingToStart == 'S'){
          toStart = 1;
        }
      }
      delay(50);
    }

  //시작하기 전에 말들이 제자리에 있는지 확인
  for(int i=2*BOARD_NUM-2;i<2*BOARD_NUM;i++){
    for(int j=0;j<8;j++){
      if (BOARD_NUM == 4 || BOARD_NUM == 1){
        if (nowcds(i,j) > CDSVALUE_1){      //없으면
          while(nowcds(i,j) > CDSVALUE_1){
            neo(i,j,'R');
            delay(700);
            neo(i,j,'b');
            delay(700);
          }
        }
      }
      else{
        if (nowcds(i,j) < CDSVALUE_1){      //있으면
          while(nowcds(i,j) < CDSVALUE_1){
            neo(i,j,'R');
            delay(700);
            neo(i,j,'b');
            delay(700);
          }
        }
      }
    }
  }

  if (BOARD_NUM == 1){
    Serial.println("ardu1Done");
  }
  else if (BOARD_NUM == 2){
    Serial.println("ardu2Done");
  }
  else if (BOARD_NUM == 3){
    Serial.println("ardu3Done");
  }
  else{
    Serial.println("ardu4Done");
  }
  
  char mayIgo = 0;
  while (mayIgo == 0){
    if(Serial.available()>0){
      char perhaps = Serial.read();
      if (perhaps == 'G'){
        mayIgo = 1;
      }
    }
  }
  
  // Starting EFFECT!!
  rainbowCycle(3);
  ending = 0;
  }
  
  cdsChecking();
  delay(10);
}

//NeoPixel에 달린 LED를 각각 다른색으로 시작하여 다양한색으로 5번 반복한다
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { 
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void cdsChecking(){
  for(int i=2*BOARD_NUM-2;i<2*BOARD_NUM;i++){
    for(int j=0;j<8;j++){
        if (cdsState[i][j] != stateDecide(i,j)){
          String ii = String(i);
          String jj = String(j);
          if (cdsState[i][j]==0){         //이제 생긴거면
            Serial.println("1"+ii+jj);
            cdsState[i][j]=1;
          }
          else if (cdsState[i][j]==1){    //이제 사라진거면
            Serial.println("0"+ii+jj);
            cdsState[i][j]=0;
          }
        }
    }    
  }
}

int stateDecide(int i,int j){
  if (nowcds(i,j) > CDSVALUE_1){
    return 0;
  }
  else{
    return 1;
  }
}

double nowcds(int i,int j){
  i=i-2*(BOARD_NUM-1);
  return analogRead(8*i+j);
}

void allblank(){
  for (int i=0;i<LED_NUMBERS;i++){
    strip.setPixelColor(i,blank);
  }
  strip.show();
}

//(i,j)칸의 neopixel 제어
void neo(int i, int j, char color){
  i=i-2*(BOARD_NUM-1);
  j=7-j;
  if(BOARD_NUM == 1 || BOARD_NUM == 4){
    if (color == 'W'){
      for(int k=0;k<4;k++){
        int m = 64*i+4*j+k;
        if (m==64){
          strip.setPixelColor(m+32,white);
          continue;
        }
        else if (m>64){
          m=m-1;
        }
        strip.setPixelColor(m,white);
        strip.setPixelColor(m+32,white);
      }
    }
    else if (color == 'R'){
      for(int k=0;k<4;k++){
        int m = 64*i+4*j+k;
        if (m==64){
          strip.setPixelColor(m+32,red);
          continue;
        }
        else if (m>64){
          m=m-1;
        }
        strip.setPixelColor(m,red);
        strip.setPixelColor(m+32,red);
      }
    }
    else if (color == 'G'){
      for(int k=0;k<4;k++){
        int m = 64*i+4*j+k;
        if (m==64){
          strip.setPixelColor(m+32,green);
          continue;
        }
        else if (m>64){
          m=m-1;
        }
        strip.setPixelColor(m,green);
        strip.setPixelColor(m+32,green);
      }
    }
    else if (color == 'B'){
      for(int k=0;k<4;k++){
        int m = 64*i+4*j+k;
        if (m==64){
          strip.setPixelColor(m+32,blue);
          continue;
        }
        else if (m>64){
          m=m-1;
        }
        strip.setPixelColor(m,blue);
        strip.setPixelColor(m+32,blue);
      } 
    }
    else if (color == 'b'){
      for(int k=0;k<4;k++){
        int m = 64*i+4*j+k;
        if (m==64){
          strip.setPixelColor(m+32,blank);
          continue;
        }
        else if (m>64){
          m=m-1;
        }
        strip.setPixelColor(m,blank);
        strip.setPixelColor(m+32,blank);
      }
    }
  }
  else{
    if (color == 'W'){
      for(int k=0;k<4;k++){
        strip.setPixelColor(64*i+4*j+k,white);
        strip.setPixelColor(64*i+4*j+k+32,white);
      }
    }
    else if (color == 'R'){
      for(int k=0;k<4;k++){
        strip.setPixelColor(64*i+4*j+k,red);
        strip.setPixelColor(64*i+4*j+k+32,red);
      }
    }
    else if (color == 'G'){
      for(int k=0;k<4;k++){
        strip.setPixelColor(64*i+4*j+k,green);
        strip.setPixelColor(64*i+4*j+k+32,green);
      }
    }
    else if (color == 'B'){
      for(int k=0;k<4;k++){
        strip.setPixelColor(64*i+4*j+k,blue);
        strip.setPixelColor(64*i+4*j+k+32,blue);
      }
    }
    else if (color == 'b'){
      for(int k=0;k<4;k++){
        strip.setPixelColor(64*i+4*j+k,blank);
        strip.setPixelColor(64*i+4*j+k+32,blank);
      }
    }
  }
  strip.show();
}
