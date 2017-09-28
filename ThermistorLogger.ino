/* Arduino thermistor logger
 * Creation date 28-Sep-2017 by Antony Burness
 *   Logs data from a two NTC self heated flow meter and inputs to Excel through PLX-DAQ
 * 
 * Update: None
 * Date:
 * 
 */

// Define the constants for the temperature and current calculations
/* circuit diagram
 *                     V_cur
 *                   *       * 
 *              Vcur_out     Vcur_in(PinRcur)
 *              ¦            ¦        ==========    Iset
 *       Vntc---O----[Rcur]--O-------|| LM334Z ||-----------O V+  (5V)
 *   (PinNTC)   ¦            ¦        ==========
 *              ¦            ¦               ¦
 *           [Rntc]          O-------[Rread]-O                          
 *              ¦                          
 *            GND                                                                          
 *                                     
 */ 
 
 /*Suggested values
  *  NTC 1k ohm 1%
  *  RREAD 136.7 ohm
  */ 
  
 //Constant values (make sure you leave the decimal notation to ensure the number is stored as a float type)
const float RCUR = 510.0;     //ohm
const float VOLTSCALE = 5.0/1023.0 ; // V/bit

//Pin outs for the data collection
#define PINRNTC1 0  //A0
#define PINRCUR1 1  //A1
#define PINRNTC2 2  //A2
#define PINRCUR2 3  //A3
//#define   //A4
//#define   //A5

//a counter for the timer interrupt
int timer1_counter;

float flVNTC1 = 0.0;
float flVNTC2 = 0.0;
float flVcuri1 = 0.0;
float flVcuri2 = 0.0;
float flVcur1 = 0.0;
float flVcur2 = 0.0;
float flIcur1 = 0.0;
float flIcur2 = 0.0;

void setup() {
  //Set the built-in led to LED mode
  pinMode(LED_BUILTIN,OUTPUT);

  //Initialise the interupt timer
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  // Set the timer to 2 Hz (i.e. run every 0.5 seconds)
  timer1_counter = 34286;  //65536 - 16MHz/256/2Hz
  TCNT1 = timer1_counter;
  TCCR1B |= (1 << CS12); // use the 256 prescalar
  TIMSK1 |= (1 << TOIE1); // enable the timer overflow interrupt
  interrupts();
  
// Link to PLX-DAQ (download from parallax PLX-DAQ)
  Serial.begin(9600); // the bigger number the better
  Serial.println("CLEARDATA"); //clears up any data left from previous projects
  Serial.println("LABEL,TIME[sec],ICUR1[mA],VNTC1[V],ICUR2[mA],VNTC2[V]"); //always write LABEL, so excel knows the next things will be the names of the columns (instead of Acolumn you could write Time for instance)
  Serial.println("RESETTIMER"); //resets timer to 0
}

void loop() {
  //any non-critical timing stuff here
}

//Timer interrupt program to collect and log data then output to Excel
ISR(TIMER1_OVF_vect)
{
  //Collect data
  digitalWrite(LED_BUILTIN,HIGH);
  flVNTC1 = float(analogRead(PINRNTC1))*VOLTSCALE;
  flVNTC2 = float(analogRead(PINRNTC2))*VOLTSCALE;
  flVcuri1 = float(analogRead(PINRCUR1))*VOLTSCALE;
  flVcuri2 = float(analogRead(PINRCUR2))*VOLTSCALE;
  digitalWrite(LED_BUILTIN,LOW);
  
//Process data
  digitalWrite(LED_BUILTIN,HIGH);
  // Calc the current resistor voltage drop
  flVcur1 = flVcuri1 - flVNTC1;
  flVcur2 = flVcuri2 - flVNTC2;
  // Convert current resistor voltage to current
  flIcur1 = flVcur1/(RCUR/1000.0);
  flIcur2 = flVcur2/(RCUR/1000.0);
  digitalWrite(LED_BUILTIN,LOW);
  
//Send data to Excel
  Serial.print("DATA,TIME,"); //writes the time in the first column A and the time since the measurements started in column B
  Serial.print(flIcur1);
  Serial.print(flIcur2);
  Serial.print(flVNTC1);
  Serial.print(flVNTC2);
  Serial.println(); //be sure to add println to the last command so it knows to go into the next row on the second run

  //flash led to signify complete
  digitalWrite(LED_BUILTIN,HIGH);
  delay(5);
  digitalWrite(LED_BUILTIN,LOW);
  delay(5);
  digitalWrite(LED_BUILTIN,HIGH);
  delay(5);
  digitalWrite(LED_BUILTIN,LOW);
    
//Blank data
  flVNTC1 = 0.0;
  flVNTC2 = 0.0;
  flVcuri1 = 0.0;
  flVcuri2 = 0.0;
  flVcur1 = 0.0;
  flVcur2 = 0.0;
  flIcur1 = 0.0;
  flIcur2 = 0.0;
}
