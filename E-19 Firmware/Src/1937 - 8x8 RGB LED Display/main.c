#include "main.h"

volatile unsigned bool redA[8][8];
volatile unsigned bool greenA[8][8];
volatile unsigned bool blueA[8][8];

unsigned int8 _address;
volatile char          _digits[3] = {0, 0, 0};

volatile unsigned int16 _cnt = 0;

#inline
void clearDisplay() {

  for (int x = 0; x < 8; x++)
    for (int y = 0; y < 8; y++) {
    
      redA[x][y] = false;
      greenA[x][y] = false;
      blueA[x][y] = false;
    }
}

#inline
void clearDisplayColor(bool r, bool g, bool b) {

  for (int x = 0; x < 8; x++)
    for (int y = 0; y < 8; y++) {
    
      redA[x][y] = r;
      greenA[x][y] = g;
      blueA[x][y] = b;
    }
}

void SetLEDPosition(unsigned int8 x, unsigned int8 y, bool red, bool green, bool blue) {
 
  redA[x][7 - y] = red;
  greenA[x][7 - y] = green;
  blueA[x][7 - y] = blue;
}

void loop1() {

  int x = 0;
  
  while (true) {
  
    output_low(Trans[x]);
  
    if (greenA[x][0])
      output_low(gPin[0]);
  
    if (greenA[x][1])
      output_low(gPin[1]);
  
    if (greenA[x][2])
      output_low(gPin[2]);
  
    if (greenA[x][3])
      output_low(gPin[3]);
  
    if (greenA[x][4])
      output_low(gPin[4]);
  
    if (greenA[x][5])
      output_low(gPin[5]);
  
    if (greenA[x][6])
      output_low(gPin[6]);
  
    if (greenA[x][7])
      output_low(gPin[7]);
  
    // blue
    if (blueA[x][0])
      output_low(bPin[0]);
  
    if (blueA[x][1])
      output_low(bPin[1]);
  
    if (blueA[x][2])
      output_low(bPin[2]);
  
    if (blueA[x][3])
      output_low(bPin[3]);
  
    if (blueA[x][4])
      output_low(bPin[4]);
  
    if (blueA[x][5])
      output_low(bPin[5]);
  
    if (blueA[x][6])
      output_low(bPin[6]);
  
    if (blueA[x][7])
      output_low(bPin[7]);
  
    // red
    if (redA[x][0])
      output_low(rPin[0]);
  
    if (redA[x][1])
      output_low(rPin[1]);
  
    if (redA[x][2])
      output_low(rPin[2]);
  
    if (redA[x][3])
      output_low(rPin[3]);
  
    if (redA[x][4])
      output_low(rPin[4]);
  
    if (redA[x][5])
      output_low(rPin[5]);
  
    if (redA[x][6])
      output_low(rPin[6]);
  
    if (redA[x][7])
      output_low(rPin[7]);
  
    delay_us(ON_DELAY_US);
    
    output_high(rPin[0]);        
    output_high(rPin[1]);        
    output_high(rPin[2]);        
    output_high(rPin[3]);        
    output_high(rPin[4]);        
    output_high(rPin[5]);        
    output_high(rPin[6]);        
    output_high(rPin[7]);        

    output_high(bPin[0]);        
    output_high(bPin[1]);        
    output_high(bPin[2]);        
    output_high(bPin[3]);        
    output_high(bPin[4]);        
    output_high(bPin[5]);        
    output_high(bPin[6]);        
    output_high(bPin[7]);        

    output_high(gPin[0]);        
    output_high(gPin[1]);        
    output_high(gPin[2]);        
    output_high(gPin[3]);        
    output_high(gPin[4]);        
    output_high(gPin[5]);        
    output_high(gPin[6]);        
    output_high(gPin[7]);        

    output_high(Trans[x]);      
    
    x++;
    
    if (x == 8)
      x = 0;
  }
}

volatile unsigned int8 _mode = 0; // 1 = change led by address, 2 = change i2c address, 3 = clear display
volatile unsigned int8 _changeAddress;
volatile unsigned int8 _tmp;
volatile unsigned int8 _state;

#INT_SSP
void ssp_interupt() {

  _state = i2c_isr_state();
    
  if (_state == 0) {      
  
    // this device address matched for the master to write to us
    // we read but it's our own address anyway
    _tmp = i2c_read();    
    _mode = 0;
    
  } else if (_state >= 1 && _state <= 127) {

    // now we receive all the data that the master is writing to us
    // this is the "memory" that the client is writing. All sequential data after this byte is "data"
    _tmp = i2c_read();

    if (_state == 1) {
      
      // let's get the address they are writing to
      
      if (_tmp == 200) {

        // the next byte of data is going to be the new i2c address to store in eeprom
        _mode = 2;
      } else if (_tmp >= 240 && _tmp <= 247) {
        
        _mode = 3;
        
        clearDisplayColor(CHECK_BIT(_tmp, 2), CHECK_BIT(_tmp, 1), CHECK_BIT(_tmp, 0));
      } else {

        // all next bytes of data are led colors
        _mode = 1;

        // get the address that we will begin writing to for all consecutive byte
        _changeAddress = _tmp;
      }
    } else {
    
      if (_mode == 1) {
  
        // now that we know the starting memory address from the first byte send, we will write the data from this transmission to that address and then increment
  
        unsigned int8 y = _changeAddress / 8;
        unsigned int8 x = _changeAddress % 8;
  
        SetLEDPosition(x, y, CHECK_BIT(_tmp, 2), CHECK_BIT(_tmp, 1), CHECK_BIT(_tmp, 0));
        
        // increment the address (which is actually the next led) incase there is an additional transmission
        _changeAddress++;
      } else if (_mode == 2) {
  
        // if the tmp data is within an acceptable i2c address range, write it to eeprom and we will use it for next power-up
        if (_tmp >= 130 && _tmp <= 180) {
        
          write_eeprom(0, _tmp);

          clearDisplay();
                    
          // O
          SetLEDPosition(1, 2, false, false, true);
          SetLEDPosition(2, 2, false, false, true);
          SetLEDPosition(0, 3, false, false, true);
          SetLEDPosition(3, 3, false, false, true);
          SetLEDPosition(0, 4, false, false, true);
          SetLEDPosition(3, 4, false, false, true);
          SetLEDPosition(1, 5, false, false, true);
          SetLEDPosition(2, 5, false, false, true);

          // K
          SetLEDPosition(5, 2, false, false, true);
          SetLEDPosition(5, 3, false, false, true);
          SetLEDPosition(5, 4, false, false, true);
          SetLEDPosition(5, 5, false, false, true);
          SetLEDPosition(6, 3, false, false, true);
          SetLEDPosition(6, 4, false, false, true);
          SetLEDPosition(7, 2, false, false, true);
          SetLEDPosition(7, 5, false, false, true);          
        }
      }
    }
  }
}

void displayDigit(unsigned int digit) {
  
  clearDisplay();
  
  switch (digit) {
    case 0:
      SetLEDPosition(2, 1, false, false, true);
      SetLEDPosition(3, 1, false, false, true);
      SetLEDPosition(4, 1, false, false, true);
      SetLEDPosition(5, 1, false, false, true);      
      
      SetLEDPosition(2, 6, false, false, true);
      SetLEDPosition(3, 6, false, false, true);
      SetLEDPosition(4, 6, false, false, true);
      SetLEDPosition(5, 6, false, false, true);

      SetLEDPosition(1, 2, false, false, true);
      SetLEDPosition(1, 3, false, false, true);
      SetLEDPosition(1, 4, false, false, true);
      SetLEDPosition(1, 5, false, false, true);

      SetLEDPosition(6, 2, false, false, true);
      SetLEDPosition(6, 3, false, false, true);
      SetLEDPosition(6, 4, false, false, true);
      SetLEDPosition(6, 5, false, false, true);
      break;   
    case 1:
      SetLEDPosition(3, 1, false, false, true);
      SetLEDPosition(3, 2, false, false, true);
      SetLEDPosition(3, 3, false, false, true);
      SetLEDPosition(3, 4, false, false, true);
      SetLEDPosition(3, 5, false, false, true);
      SetLEDPosition(3, 6, false, false, true);
    
      SetLEDPosition(2, 2, false, false, true);    

      SetLEDPosition(2, 6, false, false, true);    
      SetLEDPosition(4, 6, false, false, true);    
      break;
    case 2:
      SetLEDPosition(2, 1, false, false, true);
      SetLEDPosition(3, 1, false, false, true);
      SetLEDPosition(4, 1, false, false, true);
      SetLEDPosition(5, 1, false, false, true);
      

      SetLEDPosition(1, 2, false, false, true);
      SetLEDPosition(6, 2, false, false, true);

      SetLEDPosition(6, 3, false, false, true);

      SetLEDPosition(2, 4, false, false, true);
      SetLEDPosition(3, 4, false, false, true);
      SetLEDPosition(4, 4, false, false, true);
      SetLEDPosition(5, 4, false, false, true);
    
      SetLEDPosition(1, 5, false, false, true);
    
      SetLEDPosition(1, 6, false, false, true);
      SetLEDPosition(2, 6, false, false, true);
      SetLEDPosition(3, 6, false, false, true);
      SetLEDPosition(4, 6, false, false, true);
      SetLEDPosition(5, 6, false, false, true);
      SetLEDPosition(6, 6, false, false, true);
      break;
    case 3:
      SetLEDPosition(2, 1, false, false, true);
      SetLEDPosition(3, 1, false, false, true);
      SetLEDPosition(4, 1, false, false, true);
      SetLEDPosition(5, 1, false, false, true);

      SetLEDPosition(6, 2, false, false, true);
      
      SetLEDPosition(5, 3, false, false, true);
      
      SetLEDPosition(4, 4, false, false, true);
      SetLEDPosition(5, 4, false, false, true);

      SetLEDPosition(6, 5, false, false, true);

      SetLEDPosition(2, 6, false, false, true);
      SetLEDPosition(3, 6, false, false, true);
      SetLEDPosition(4, 6, false, false, true);
      SetLEDPosition(5, 6, false, false, true);
      break;      
    case 4:
      SetLEDPosition(2, 1, false, false, true);
      SetLEDPosition(2, 2, false, false, true);
      SetLEDPosition(2, 3, false, false, true);
      SetLEDPosition(2, 4, false, false, true);

      SetLEDPosition(5, 1, false, false, true);
      SetLEDPosition(5, 2, false, false, true);
      SetLEDPosition(5, 3, false, false, true);
      SetLEDPosition(5, 4, false, false, true);
      SetLEDPosition(5, 5, false, false, true);
      SetLEDPosition(5, 6, false, false, true);
    
      SetLEDPosition(3, 4, false, false, true);
      SetLEDPosition(4, 4, false, false, true);
      SetLEDPosition(6, 4, false, false, true);
    
      SetLEDPosition(5, 5, false, false, true);
      SetLEDPosition(5, 6, false, false, true);
      break;
    case 5:
      SetLEDPosition(1, 1, false, false, true);
      SetLEDPosition(2, 1, false, false, true);
      SetLEDPosition(3, 1, false, false, true);
      SetLEDPosition(4, 1, false, false, true);
      SetLEDPosition(5, 1, false, false, true);
     
      SetLEDPosition(1, 2, false, false, true);
     
      SetLEDPosition(1, 3, false, false, true);
      SetLEDPosition(2, 3, false, false, true);
      SetLEDPosition(3, 3, false, false, true);
      SetLEDPosition(4, 3, false, false, true);

      SetLEDPosition(5, 4, false, false, true);
      
      SetLEDPosition(5, 5, false, false, true);   
      SetLEDPosition(1, 5, false, false, true);

      SetLEDPosition(2, 6, false, false, true);   
      SetLEDPosition(3, 6, false, false, true);   
      SetLEDPosition(4, 6, false, false, true);      
      break;
    case 6:
      SetLEDPosition(2, 1, false, false, true);
      SetLEDPosition(3, 1, false, false, true);
      SetLEDPosition(4, 1, false, false, true);
      
      SetLEDPosition(1, 2, false, false, true);
      
      SetLEDPosition(1, 3, false, false, true);

      SetLEDPosition(1, 4, false, false, true);
      SetLEDPosition(2, 4, false, false, true);
      SetLEDPosition(3, 4, false, false, true);
      SetLEDPosition(4, 4, false, false, true);
      SetLEDPosition(5, 4, false, false, true);
     
      SetLEDPosition(1, 5, false, false, true);
      SetLEDPosition(5, 5, false, false, true);
     
      SetLEDPosition(2, 6, false, false, true);
      SetLEDPosition(3, 6, false, false, true);
      SetLEDPosition(4, 6, false, false, true);
      break;
    case 7:
      SetLEDPosition(1, 1, false, false, true);
      SetLEDPosition(2, 1, false, false, true);
      SetLEDPosition(3, 1, false, false, true);
      SetLEDPosition(4, 1, false, false, true);
      SetLEDPosition(5, 1, false, false, true);

      SetLEDPosition(5, 2, false, false, true);
    
      SetLEDPosition(4, 3, false, false, true);
    
      SetLEDPosition(3, 4, false, false, true);
      SetLEDPosition(3, 5, false, false, true);
      SetLEDPosition(3, 6, false, false, true);
      break;
    case 8:
      SetLEDPosition(2, 1, false, false, true);
      SetLEDPosition(3, 1, false, false, true);
      SetLEDPosition(4, 1, false, false, true);
      
      SetLEDPosition(1, 2, false, false, true);
      SetLEDPosition(5, 2, false, false, true);
    
      SetLEDPosition(2, 3, false, false, true);
      SetLEDPosition(3, 3, false, false, true);
      SetLEDPosition(4, 3, false, false, true);
    
      SetLEDPosition(1, 4, false, false, true);
      SetLEDPosition(5, 4, false, false, true);
    
      SetLEDPosition(1, 5, false, false, true);
      SetLEDPosition(5, 5, false, false, true);

      SetLEDPosition(2, 6, false, false, true);
      SetLEDPosition(3, 6, false, false, true);
      SetLEDPosition(4, 6, false, false, true);
      break;
    case 9:
      SetLEDPosition(2, 1, false, false, true);
      SetLEDPosition(3, 1, false, false, true);
      SetLEDPosition(4, 1, false, false, true);
      
      SetLEDPosition(1, 2, false, false, true);
      SetLEDPosition(5, 2, false, false, true);

      SetLEDPosition(1, 3, false, false, true);
      SetLEDPosition(5, 3, false, false, true);
     
      SetLEDPosition(2, 4, false, false, true);
      SetLEDPosition(3, 4, false, false, true);
      SetLEDPosition(4, 4, false, false, true);
      SetLEDPosition(5, 4, false, false, true);
     
      SetLEDPosition(5, 5, false, false, true);
      SetLEDPosition(5, 6, false, false, true);     
      break;
    }
}

#INT_TIMER1
void isr_tmr1() {

  if (_cnt == 0) 
    displayDigit(_digits[2]);
  else if (_cnt == 100)
    clearDisplay();
  else if (_cnt == 150)
    displayDigit(_digits[1]);
  else if (_cnt == 250)
    clearDisplay();
  else if (_cnt == 300) 
    displayDigit(_digits[0]);   
  else if (_cnt == 400) {

    clearDisplay();
  
    disable_interrupts(INT_TIMER1);
    
    setup_timer_1(T1_DISABLED);
  }
  
  _cnt++;
}

void main() {

  clearDisplay();

  // ----------------------------------------------------------------------------------------------
  
  _address = read_eeprom(0);

  if (_address >= 130 && _address <= 180)
    i2c_slaveaddr(_address);
  else
    _address = 0xa2;
    
  int digitsSize = 0;
  
  do {                 

    _digits[digitsSize++] = _address % 10;   /* get next digit */
  } while ((_address /= 10) > 0);   /* delete it */

  // ----------------------------------------------------------------------------------------------

  for (int t = 0; t < 8; t++) {
  
    output_drive(Trans[t]);
    output_high(Trans[t]);

    output_drive(rPin[t]);
    output_high(rPin[t]);

    output_drive(gPin[t]);
    output_high(gPin[t]);

    output_drive(bPin[t]);
    output_high(bPin[t]);
  }

  // ----------------------------------------------------------------------------------------------

  delay_ms(1000);

  // ----------------------------------------------------------------------------------------------

  setup_adc_ports(NO_ANALOGS);
  setup_vref(false);
  setup_comparator(NC_NC_NC_NC);

  setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);

  enable_interrupts(INT_SSP);
  enable_interrupts(INT_TIMER1);
  enable_interrupts(GLOBAL);

  // ----------------------------------------------------------------------------------------------
         
  loop1();
}

