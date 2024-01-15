/*#include <18F4550.h> //8MHZ

#device adc=8

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES WDT128                   //Watch Dog Timer uses 1:128 Postscale
#FUSES INTRC_IO                 //Internal RC Osc, no CLKOUT
#FUSES NOPROTECT                //Code not protected from reading
#FUSES NOBROWNOUT               //No brownout reset
#FUSES BORV20                   //Brownout reset at 2.0V
#FUSES NOPUT                    //No Power Up Timer
#FUSES NOCPD                    //No EE protection
#FUSES STVREN                   //Stack full/underflow will cause reset
#FUSES NODEBUG                  //No Debug mode for ICD
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES NOWRT                    //Program memory not write protected
#FUSES NOWRTD                   //Data EEPROM not write protected
#FUSES IESO                     //Internal External Switch Over mode enabled
#FUSES FCMEN                    //Fail-safe clock monitor enabled
#FUSES NOPBADEN                 //PORTB pins are configured as digital I/O on RESET
#FUSES NOWRTC                   //configuration not registers write protected
#FUSES NOWRTB                   //Boot block not write protected
#FUSES NOEBTR                   //Memory not protected from table reads
#FUSES NOEBTRB                  //Boot block not protected from table reads
#FUSES NOCPB                    //No Boot Block code protection
#FUSES MCLR                     //Master Clear pin enabled
#FUSES LPT1OSC                  //Timer1 configured for low-power operation
#FUSES NOXINST                  //Extended set extension and Indexed Addressing mode disabled (Legacy mode)
#FUSES PLL1                     //Divide By 2(8MHz oscillator input)
#FUSES CPUDIV1                  //System Clock by 4
#FUSES USBDIV                   //USB clock source comes from PLL divide by 2
#FUSES VREGEN                   //USB voltage regulator enabled
#FUSES ICPRT                    //ICPRT enabled


#use delay(clock=8000000)
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)
#use i2c(Master,Fast, stream = SSD1306_STREAM, sda=PIN_B0,scl=PIN_B1)*/

#include <18F4550.h>    //20MHZ
#device adc=8

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES WDT128                   //Watch Dog Timer uses 1:128 Postscale
#FUSES HSPLL                    //High Speed Crystal/Resonator with PLL enabled
#FUSES NOPROTECT                //Code not protected from reading
#FUSES NOBROWNOUT               //No brownout reset
#FUSES BORV20                   //Brownout reset at 2.0V
#FUSES NOPUT                    //No Power Up Timer
#FUSES NOCPD                    //No EE protection
#FUSES STVREN                   //Stack full/underflow will cause reset
#FUSES NODEBUG                  //No Debug mode for ICD
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES NOWRT                    //Program memory not write protected
#FUSES NOWRTD                   //Data EEPROM not write protected
#FUSES IESO                     //Internal External Switch Over mode enabled
#FUSES FCMEN                    //Fail-safe clock monitor enabled
#FUSES NOPBADEN                   //PORTB pins are configured as analog input channels on RESET
#FUSES NOWRTC                   //configuration not registers write protected
#FUSES NOWRTB                   //Boot block not write protected
#FUSES NOEBTR                   //Memory not protected from table reads
#FUSES NOEBTRB                  //Boot block not protected from table reads
#FUSES NOCPB                    //No Boot Block code protection
#FUSES MCLR                     //Master Clear pin enabled
#FUSES LPT1OSC                  //Timer1 configured for low-power operation
#FUSES NOXINST                  //Extended set extension and Indexed Addressing mode disabled (Legacy mode)
#FUSES PLL5                     //PARA CRISTA EXTERNO É NECESSARIO PLL(5 P/ 20MHZ)
#FUSES CPUDIV4                  //No System Clock Postscaler
#FUSES USBDIV                   //USB clock source comes from PLL divide by 2
#FUSES VREGEN                   //USB voltage regulator enabled
#FUSES ICPRT                    //ICPRT enabled

#use delay(clock=20000000)
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)
#use i2c(Master,Fast, stream = SSD1306_STREAM, sda=PIN_B0,scl=PIN_B1)
