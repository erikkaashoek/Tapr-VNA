// 
// UVnArduino
//

#include <Wire.h> 
#define SI_OVERCLOCK 910000000      // 1.0 GHz in Hz
// #include "si5351mcu.h"
//Si5351mcu si5351;
#include "si5351_nano.h"

//#include <LiquidCrystal_I2C.h>
// LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#define MAINVCO  2
#define LOVCO  0

int correction = 23260; // use the Si5351 correction sketch to find the frequency correction factor


#define freqMin  100000
#define freqMax 180000000

#define IFFreq 5000
#define StartFreq 30000000
#define MAXFREQ 250000000UL

unsigned char Mode;       //mode powerdown ou normal
unsigned long  StartF;    //DS_FTW fréquence de départ
unsigned int NumberF;     //nombre d'échantillons
unsigned long  StepF;     //Fréquence d'incrémentation en FTW (non en HZ)
int StepTime;

//char Temp;
unsigned int intTemp;    //variable de boucle pour le balayage
unsigned int adcmag;     //variables des 2 ADC mesures
unsigned int adcphs;
boolean check=0;



#define led  LED_BUILTIN        //Affectation des broches
#define Rele  5       //relais refexion, transmission 
#define ADC0  A0       //entrée mag
#define ADC1  A1      //entrée phs

//#define  dpInEncoderA  2
//#define  dpInEncoderB  3
//#define  dpInEncoderPress 4


#define adc2Db 60/1024  // pente pleine echelle Db / resolution ADC = 0.0586
#define offsetDb -30  // décallage de -30db, pour ADC=512 -> 0db
#define Adc2Angle 180/1024  // pente pleine echelle Angle / resolution ADC = 0.175
#define D2R 3.14159/180    //degrés to radians


//#define calMag 0.703125      //before calibrate function
//#define calPhs 1.58203125

float calMag;
float calPhs;

struct vector_reflection{
  double Freq;
  float RL;
  float Phi;
  float Rho;
  float Rs;
  float Xs;
  float Swr;
  float Z;
};
vector_reflection Point;

struct vector_transmission{
  double Freq;
  float TL;
  float TP;
};

volatile long freq = 5000000;
byte vnaMode=0;
volatile byte menuSwapp=0;
byte menuChoose=0;
byte bandChoose;
volatile byte bandSwapp=0;
volatile byte bandSwappPrec=11;
double dds_reg;
int last_led = false;
long last_freq = 0;
long freq_prec = 0;               
long freqStep = 0;

#define PLLB_FREQ    87600000000ULL

void(* resetFunc) (void) = 0; //declare reset function @ address 0 

char testKey()
{
  if(Serial.available() > 0)   // see if incoming serial data:
  {
    return(Serial.read());  // read oldest byte in serial buffer:
  }
  return(0);
}

void setFreq(unsigned long freq, int vfo, int strength)
{
//    si5351.setFreq(vfo, freq);
    si5351_set_frequency(vfo, freq, strength);
}


void dds_setup()
{
#if 0
  si5351.init(25000000);
  si5351.setPower(0, SIOUT_8mA);
  si5351.setPower(2, SIOUT_8mA);
  // pre-load some sweet spot freqs
  si5351.setFreq(0, StartFreq+IFFreq);
  si5351.enable(0);
  si5351.setFreq(2, StartFreq);
  si5351.enable(2);
  //si5351.reset();
#else
  si5351_init();
#endif  


}

void setup(){
#if 0
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Started!");
#endif
  Serial.begin(115200);
  Serial.setTimeout(10);
  Serial.println("TAPR VNA v4");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(Rele, OUTPUT);
  analogReference(EXTERNAL);  //tension de référence extérieure 1.8V de l'ad8302
  dds_setup();
  Serial.println("Init done");
  si5351_set_frequency_with_offset(30000000UL, IFFreq, SI5351_CLK_DRIVE_STRENGTH_2MA);
  //setFreq((long)130000000,MAINVCO,SI5351_CLK_DRIVE_STRENGTH_2MA);
  //setFreq((long)130000000+IFFreq,LOVCO,SI5351_CLK_DRIVE_STRENGTH_2MA);
  Serial.println("Setfreq done");
  delay(1);
#if 0
  pinMode(dpInEncoderA, INPUT);
  digitalWrite(dpInEncoderA, HIGH);
  pinMode(dpInEncoderB, INPUT);
  digitalWrite(dpInEncoderB, HIGH);
  pinMode(dpInEncoderPress, INPUT);
  digitalWrite(dpInEncoderPress, HIGH);
#endif




  adcmag=1; // EEPROM.read(2)*256+EEPROM.read(1);
  adcphs=1; // EEPROM.read(4)*256+EEPROM.read(3);
  calMag=((float)adcmag*adc2Db)+offsetDb;
  calPhs=((float)adcphs*Adc2Angle);
  //Serial.println(calMag);              //verif calibration
  //Serial.println(calPhs);




  //lcd.clear();
  //lcd.print(F("     VNA v1.0"));    //intro
  //lcd.setCursor(0, 1);
  //lcd.print(F("    F4GOH 2015"));
  //delay(4000);
  //lcd.clear();
  Serial.flush();

//  attachInterrupt(0, doEncoder, CHANGE);  // encoder pin on interrupt 0 - pin 2

}


void Debug(int c, int l, long d)
{
#if 1
  Serial.print(c);
  Serial.print(" ");

  Serial.println(d);
  // lcd.clear();
//  lcd.setCursor(c, l);
//  lcd.print(d);
#endif
}


void DebugText( char * t)
{
  Serial.println(t);
}

//main loop

void loop(){
  if ((millis() & (1024 - 1) )== 0) {
    last_led = ! last_led;
    digitalWrite(LED_BUILTIN, last_led);// waiting
    Serial.println("TAPR VNA v4");
    delay(2);
  }

  Jnva();
}


/********************************************************
* Jvna
********************************************************/

void Jnva()
{
//  Serial.flush();
//  lcd.setCursor(0,0);
//  lcd.print("Waiting!");
  long old_milis;
  unsigned long old_micros;
  unsigned long new_micros;
  if ((millis() & (1024 - 1) )== 0) {
    last_led = ! last_led;
    digitalWrite(LED_BUILTIN, last_led);// waiting
  }


  if (Serial.available()) {
    DebugText("Start decoding input");
    Mode = Serial.parseInt();
 //   lcd.clear();
    Debug(0,0,Mode);
    StartF  = Serial.parseInt();
    Debug(3,0,StartF);
    if (Serial.available())
      NumberF = Serial.parseInt();
    else
      NumberF = 1;
    Debug(0,1,NumberF);
    if (Serial.available())
      StepF = Serial.parseInt();
    else
      StepF = 0;
    Debug(6,1,StepF);
    if (Serial.available())
      StepTime = Serial.parseInt();
    else 
      StepTime = 5;
    Debug(14,1,StepTime);
    while (Serial.available())
      Serial.read();
    DebugText("Input available");
  }

  //  lcd.clear();
  //lcd.print(F("Waiting"));    //intro

 //Mode = 2;
  if ((NumberF!=0)) // si le décodage est bon et que le nombre d'échantillons n'est pas nul alors mesure
  {
    DebugText("Scanning started");

    digitalWrite(LED_BUILTIN, HIGH);      //vérification visuelle de la mesure
    if (Mode == 1) digitalWrite(Rele, HIGH);   // commande relais tansmission 0 ou refection 1
    else digitalWrite(Rele, LOW);
    old_micros = micros();
    for (intTemp=0; intTemp < NumberF; intTemp++)    //boucle des mesures
    {
//      DebugText("Step");
//      Debug(0,0,StartF);

      if (Mode == 1) digitalWrite(Rele, LOW);   // commande relais tansmission 0 ou refection 1
      else if (Mode == 0) digitalWrite(Rele, HIGH);
      else digitalWrite(Rele, LOW);

//      affiche_freqs();    //pour debug
      if (StartF > freqMin)
      {
        if (Mode < 3 || intTemp == 0) {
//          si5351.disable(0);
            si5351_disable_output();

            if (StartF > (MAXFREQ + IFFreq) * 3) {
//              si5351_set_frequency_with_offset(StartF, IFFreq, SI5351_CLK_DRIVE_STRENGTH_2MA);
              setFreq((long)((StartF)/5L ),MAINVCO,SI5351_CLK_DRIVE_STRENGTH_2MA);
              setFreq((long)((StartF + IFFreq)/7L ),LOVCO,SI5351_CLK_DRIVE_STRENGTH_2MA);
            } else if (StartF > (MAXFREQ + IFFreq) ) {
//                si5351_set_frequency_with_offset(StartF, IFFreq, SI5351_CLK_DRIVE_STRENGTH_2MA);
              setFreq((long)((StartF)/3L ),MAINVCO,SI5351_CLK_DRIVE_STRENGTH_2MA);
              setFreq((long)((StartF + IFFreq)/5L ),LOVCO,SI5351_CLK_DRIVE_STRENGTH_2MA);
            } else 
            {
//                si5351_set_frequency_with_offset(StartF, IFFreq, SI5351_CLK_DRIVE_STRENGTH_2MA);
              setFreq((long) StartF ,MAINVCO,SI5351_CLK_DRIVE_STRENGTH_2MA);
              setFreq((long)(StartF + IFFreq ),LOVCO, SI5351_CLK_DRIVE_STRENGTH_2MA);
          }

          last_freq = StartF;
          if ((millis() & (127) )== 0) {
            last_led = ! last_led;
            digitalWrite(LED_BUILTIN, last_led);// waiting
          }
#define SWITCH_DELAY  3000
          while (micros() - old_micros < SWITCH_DELAY) {
            delayMicroseconds(100);
          }
            si5351_enable_output();
//          si5351.enable(0);
        } else if (intTemp == 1) {
            si5351_disable_output();
//          si5351.disable(0);
          setFreq((long)(StartF + IFFreq ),LOVCO, SI5351_CLK_DRIVE_STRENGTH_2MA);
        } else        
          setFreq((long)(StartF + IFFreq ),LOVCO, SI5351_CLK_DRIVE_STRENGTH_2MA);
      }
      new_micros = micros();
      while (new_micros - old_micros < SWITCH_DELAY + StepTime * 1000L) {
          delayMicroseconds(100);
          new_micros = micros();
      }
      if (Mode == 0 || Mode == 1) {
        if (Mode == 1)  { digitalWrite(Rele, HIGH);  } // commande relais tansmission 0 ou refection 1
        else { digitalWrite(Rele, LOW); }
        new_micros = micros();
        while (new_micros - old_micros < SWITCH_DELAY + 2*StepTime * 1000L) {
            delayMicroseconds(100);
            new_micros = micros();
          }
      }
      old_micros = new_micros;
      StartF = StartF+StepF;
    }
    si5351_enable_output();
//    si5351.enable(0);
    NumberF = 0;
    DebugText("Scanning done");
    digitalWrite(LED_BUILTIN, LOW);        //fin de mesure
    Serial.flush();

  }
  //
}
#if 0

void magPhsADC()  //average 20 ech
{
  unsigned int amp;
  unsigned int phs;  
  amp=0;
  phs=0;
  for (int n=0;n<20;n++){
    amp = amp +  analogRead(ADC0);   //mesure 10 bits
    phs = phs + analogRead(ADC1);
  }
  adcmag=amp/20;
  adcphs=phs/20;
}

// traitement chaine série

char DecodeCom (void)
{
  char data, i, err=0,  Param[11];

  data = getRX();                           // récupère un caractère réflextion , transmission
  switch (data)
  {
  case '0': Mode = 0; break;
  case '1': Mode = 1; break;
  default: err = 1;
  }
  data = getRX();                           // ok c'est un retour chariot 
  if  (data !=0x0D && data != 0x0A  && data != 0x20 ) err = 2;
  Debug(err,0,data);
  if  (err != 0)  return (err);               // ou erreur

  for (i = 0; i <= 10; i++)                  // lire l'info sur le mot DS_FTW de start
  {
    data = getRX();
    if (data == 0x0D  || data == 0x0A  || data == 0x20) break;                  // sort de la boucle si retour chariot 
    if (isdigit(data) ==1) Param[i] = data;    // vérification si c'est un un chiffre
    else  err = 3;                         // sinon erreur
  }

  if  ((i==0) | (i>10))  err = 6;              // erreur si DS_FTW start nul ou trop long
  Param[i] = 0;                                // char nul avant conversion
  if (err == 0) StartF = atol (Param);                       // conversion en unsigned long
  Debug(err,0,StartF);
  if  (err != 0)  return (err);                //  return si erreur

  for (i = 0; i <= 5; i++)                   // meme principe avec le  nombre d'échantillons
  {
    data = getRX();
    if (data == 0x0D  || data == 0x0A  || data == 0x20 ) break;                  
    if (isdigit(data) ==1) Param[i] = data;   
    else  err = 4;                         
  }
  if  ((i==0) | (i>5))  err = 6;              
  Param[i] = 0;                               
  if (err == 0) NumberF = atoi (Param);                     
  Debug(err,0,NumberF);
  if  (err != 0)  return (err);               


  for (i = 0; i <= 10; i++)                  // meme principe avec le  DS_FTW  step
  {
    data = getRX();
    if (data == 0x0D  || data == 0x0A  || data == 0x20) break;                  
    if (isdigit(data) ==1) Param[i] = data;   
    else  err = 5;                         
  }
  if  ((i==0) || (i>10))  err = 6;            
  Param[i] = 0;                               
  if  (err == 0)  StepF = atol (Param);                       
  Debug(err,0,StepF);
  if  (err != 0)  return (err);               

  for (i = 0; i <= 10; i++)                  // meme principe avec le  DS_FTW  step
  {
    data = getRX();
    if (data == 0x0D  || data == 0x0A  || data == 0x20) break;                  
    if (isdigit(data) ==1) Param[i] = data;   
    else  err = 5;                         
  }
  StepTime = 5;
  if  ((i>10))  err = 6;            
  Param[i] = 0;                               
  if  (err == 0 && i>0)  StepTime = atol (Param);                       
  Debug(err,0,StepF);
  if  (err != 0)  return (err);               



  return (0);
}

/*
mode bluetooth 115200 bauds

<\r><\n>CONNECT,BCCFCC36B9BC<\r> <\n>0<\r>0<\r>1<\r>0<\r>

0<\r>1073804<\r>1000<\r>1931773<\r>

<\r><\n>DISCONNECT<\r><\n>
*/

char getRX(void)
{
  char data=0;
  do
  {
    if (Serial.available()) {
      data=Serial.read();
      if(check==0) {
        if (data==0x0D || data==0x0A) {
          data=0;
          while (data!=0x0d && data!=0x0A )                                
          {
            if (Serial.available()) data=Serial.read();  
          }
          while (data!='0')
          {
            if (Serial.available()) data=Serial.read();  
          }
        }
      }
      check=1;
    }
  }
  while((data != 0x0A)&&(data != 0x0D)&&(data != 0x20)&&(isdigit(data) !=1));
  return data;
}



void affiche_freqs(void)
{
    //     delete_char(1,8,19); 
    //lcd.setCursor(0,0);
    //lcd.print((unsigned long)ticksToFreq(StartF));
    //lcd.print(" Hz");
    //     delete_char(2,8,19); 
    //lcd.setCursor(0,1);
    //lcd.print((unsigned long)ticksToFreq(StepF));
    //lcd.print(" Hz");
    //     delete_char(3,8,19); 
    //lcd.setCursor(12,1);
    //lcd.print(NumberF);
}

double ticksToFreq(long f)
{
  return ((double)f) * 180000000.0 / 4294967296.0;  
}


/********************************************************
* Bluetooth
********************************************************/

// géré dans le JvnaPC


void BCD (unsigned long b, char* o)
{
  for (int i=10; i; --i)
  {
    *o = (b % 10)+48;
    b /= 10;
    o++;
  }
}




void delete_char(byte line, byte start, byte end)
{
  while(start<=end) {
    //lcd.setCursor(start, line);
    //lcd.write(32);
    start++;
  }
}




/********************************************************
* Compute Vna datas
********************************************************/

/*
//formulas
RL=-20log(Rho)
Rho=10^(Rl/-20)
Z=(ZL-ZO)/(ZL+Z0) avec Z0=50ohms
Z=a+jb avec 
a=Rho*cos(phi)
b=Rho*sin(phi)
ZL=(1+Z)/(1-Z)*Z0
ZL=RS+jXS avec
RS=abs(1-a²-b²)/((1-a)²+b²)
XS=abs(2b/((1-a)²-b²))
|Z|=sqrt(RS²+XS²)
SWR=(1+Rho)/(1-Rho)
*/
#if 0
void calculDut(int adcMag, int adcPhs)
{
  Point.Freq=freq;  
  Point.RL=((float)adcMag*adc2Db)+offsetDb-calMag;
  Point.Phi=((float)adcPhs*Adc2Angle)-calPhs;
  Point.Rho=pow(10.0,Point.RL/-20.0);
  float re=Point.Rho*cos(Point.Phi * D2R);
  float im=Point.Rho*sin(Point.Phi * D2R);
  float denominator=((1-re)*(1-re)+(im*im));
  Point.Rs=fabs((1-(re*re)-(im*im))/denominator)*50.0;
  Point.Xs=fabs(2.0*im)/denominator*50.0;
  Point.Z=sqrt(Point.Rs*Point.Rs+Point.Xs*Point.Xs);
  Point.Swr=fabs(1.0+Point.Rho)/(1.001-Point.Rho);
  Point.RL*=-1;
}

void vna_print()
{
  Serial.print(freq);
  Serial.write(9);
  Serial.print(adcmag);
  Serial.write(9);
  Serial.print(adcphs);
  Serial.write(9);
  Serial.print(Point.RL);
  Serial.write(9);
  Serial.print(Point.Phi);
  Serial.write(9);
  Serial.print(Point.Rho);
  Serial.write(9);
  Serial.print(Point.Rs);
  Serial.write(9);
  Serial.print(Point.Xs);
  Serial.write(9);
  Serial.print(Point.Z);
  Serial.write(9);
  Serial.println(Point.Swr);
}


/********************************************************
* Encoder 
********************************************************/


void doEncoder() {
  if (digitalRead(dpInEncoderA) == digitalRead(dpInEncoderB)) {
    menuSwapp=(menuSwapp+1)%4;
    bandSwappPrec=bandSwapp;
    bandSwapp=(bandSwapp+1)%12;
    freq+=freqStep;
    if (freq>freqMax) freq=freqMax; 
  } else {
    freq-=freqStep;
    if (freq<freqMin) freq=freqMin; 
  }
}

#endif
#endif
