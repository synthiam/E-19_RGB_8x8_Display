#include <16F1937.h>

#device CONST=ROM

#FUSES MCLR
#FUSES NOWDT                   //No Watch Dog Timer
#FUSES PROTECT                 //Code protected from reads
#FUSES NOBROWNOUT              //No brownout reset
#FUSES NOLVP                   //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES PLL
#FUSES NODEBUG

#use delay(internal=32MHz)

// I2C Slave (0xa2 is 8 bit address, the 7 bit is 0x51)
// The allowable i2c slave base address range is 0x10 through 0xEE.
#use i2c(Slave, force_hw, sda = pin_c4, scl = pin_c3, FAST, address = 0xa2)

#use FAST_IO(ALL)

typedef int1 bool;

#DEFINE ON_DELAY_US 2000

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

const unsigned int8 Trans [] = {pin_d7, pin_d6, pin_d5, pin_d4, pin_d3, pin_d2, pin_d1, pin_d0};

const unsigned int8 gPin [] = {pin_a0, pin_a3, pin_b0, pin_b3, pin_b6, pin_c1, pin_c6, pin_e1};
const unsigned int8 bPin [] = {pin_a1, pin_a4, pin_b1, pin_b4, pin_b7, pin_c2, pin_c7, pin_e2};
const unsigned int8 rPin [] = {pin_a2, pin_a5, pin_b2, pin_b5, pin_c0, pin_c5, pin_e0, pin_a6};


