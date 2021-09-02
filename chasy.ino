#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <DS1307RTC.h>
#include "IRremote.h"

#define pinCS 10 // Подключаем CS к 10-му пину, DIN к MOSI и CLK к SCK 
#define buttonPin 2  
#define numberOfHorizontalDisplays 1 // Количество модулей по горизонтали
#define numberOfVerticalDisplays 4 // Количество модулей по вертикали

IRrecv irrecv(3); // указываем вывод, к которому подключен приемник
decode_results results;

int prirost,prirost2,stsec;
int pr1,pr2,pr3,pr4,pr5;

int buttonState = 0;         // переменная для хранения состояния кнопки
int Rezhim =0;

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

unsigned long timing;
tmElements_t tm;


#include "my_cifry.h"

void setup() {

  // инициализируем пин, подключенный к кнопке, как вход
  pinMode(buttonPin, INPUT);   

  Serial.begin(9600);
  irrecv.enableIRIn(); // запускаем прием
  
  matrix.setIntensity(3); // Яркость от 0 до 15
  matrix.setRotation(matrix.getRotation()+1); //1 - 90  2 - 180   3 - 270
  prirost=0;
}

void loop() {

if ( irrecv.decode( &results )) { // если данные пришли
    switch ( results.value ) {
    case 0xFF6897:// 1
        Rezhim=0;
        break;
    case 0xFF9867:
        Rezhim=1;
        break;
    case 0xFFB04F:
        Rezhim=2;
        break;

        
    }    
    irrecv.resume(); // принимаем следующую команду
  }
  
  // считываем значения с входа кнопки
  buttonState = digitalRead(buttonPin);
 
  // проверяем нажата ли кнопка
  if (buttonState == HIGH) {   
    Rezhim++; 
    delay(300);
  }

if( Rezhim>2)  Rezhim=0;
if( Rezhim==0) RezhimBig();
if( Rezhim==1) RezhimSchet();
if( Rezhim==2) RezhimData();

if (millis() - timing > 60)
{ 
  timing = millis(); 
   RTC.read(tm);
   if (tm.Second != stsec) 
   {
     stsec = tm.Second;
     prirost =0;  
   } else prirost ++;
   if ( prirost >9) prirost=9;
     prirost2 = tm.Second/5; 
 }
}

void PutCifra(int i,int p, int n)
{
   p=p*9+8;
    for ( int y = 0; y < 8; y++ ) {
       if ((y+n)>p) n=-y;
        for ( int x = 0; x < 8; x++ ) {
            // зажигаем x-й пиксель в y-й строке
            matrix.drawPixel(x+i, y, data[y+n] & (1<<(7-x)));
           
        }
    }
}

void RezhimSchet()
{
  
 pr1=0;
 pr2=0;
 pr3=0;
 pr4=0;
 pr5=0;

int k = 9;
if ( (tm.Second % 10)==9) 
{
  pr5 = prirost ;
  if ( tm.Second ==59)   
  {   
    pr4 = prirost ;
    if ( (tm.Minute % 10)==9)
    {
      pr3 = prirost ;
      if ( tm.Minute ==59)
      {
        pr2 = prirost ;
        if ( (tm.Hour % 10)==9)   pr1 = prirost ;
        if ( tm.Hour == 23) {  pr1 = prirost ; k = 3; }
        
      }
    }
  }
}
    matrix.fillScreen(LOW); // очистка матрицы


PutCifra(0,  2,(tm.Hour   / 10)*9 + pr1);
PutCifra(5,  k,(tm.Hour   % 10)*9 + pr2);
PutCifra(10, 100, 10*8+ 90);
PutCifra(12, 5,(tm.Minute / 10)*9 + pr3);
PutCifra(17, 9,(tm.Minute % 10)*9 + pr4);
PutCifra(23, 5,(tm.Second / 10)*9 + pr5);
PutCifra(28, 9,(tm.Second % 10)*9 + prirost );

 matrix.write(); // Вывод сообщения на экран

}

void RezhimBig()
{


tm.Hour =21;
tm.Minute =9;
  
matrix.fillScreen(LOW); // очистка матрицы
PutCifra(1,    100, (tm.Hour   / 10)*8+ BigCifr);
PutCifra(8,    100, (tm.Hour   % 10)*8+ BigCifr);

if (prirost2 == 0) PutCifra(15,   100, mySym + 13); else 
  if (prirost2 == 6) PutCifra(15,   100, mySym + 8); else
    if (prirost2 < 6) PutCifra(16,   100, mySym + 26 - prirost2 ); else 
      PutCifra(15,   100, mySym + 14 + prirost2 );
PutCifra(18,   100, (tm.Minute / 10)*8+ BigCifr);
PutCifra(25,   100, (tm.Minute % 10)*8+ BigCifr);
matrix.write(); // Вывод сообщения на экран
}


void RezhimData()
{

    int a = (14 - tm.Month) / 12;
    int y = tmYearToCalendar(tm.Year) - a;
    int m = tm.Month + 12 * a - 2;
    int d_n = (tm.Day + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12) % 7;

Serial.println(d_n);
matrix.fillScreen(LOW); // очистка матрицы
PutCifra(1,  100,(tm.Month   / 10)*9 );
PutCifra(6,  100,(tm.Month   % 10)*9 );
PutCifra(11, 100, 7*8 + SymDN);
PutCifra(13, 100,(tm.Day / 10)*9 );
PutCifra(18, 100,(tm.Day % 10)*9 );
PutCifra(24, 100,d_n*8 + SymDN);
matrix.write(); // Вывод сообщения на экран 

  
  }
