#include "C:\Users\Laboratorio\Desktop\Douglas Autoban\Projetos\Horimetro\Firmware\FW\TesteDS1307.h"
#include <SSD1306OLED.c>

//#include <18F4550.h>

#define SH1106_128_64

#define LED_ON                   0
#define LED_OFF                  1
#define HR_START                 0x50
#define TP_START                 0x56

#define LED     PIN_C0   // OUT = 0
#define bot2    PIN_D1   //OUT
#define bot1    PIN_D3   //OUT    
#define bot3    PIN_D2   //OUT
#define bot4    PIN_D0   //OUT

/*------------VARIAVEIS----------*/

// Variables declaration

/*---STRINGS---*/
   char horas[]   = "  :  ";
   char data[]    = "  /  /20  ";
   char hori[]    = "H:00000:00 ";
   char tmr[]     = "T:00000:00 ";
   char txt1[]    = "Relogio";
   char txt2[]    = "Data";
   char txt3[]    = "Timer";
   char txt4[]    = "Sobre";
   char txt5[]    = "Horas";
   char txt6[]    = "Minutos";
   char txt7[]    = "Mes";
   char txt8[]    = "Dia";
   char txt9[]    = "Ano";
   char txt10[]   = "Init  temp";
   char txt11[]   = "Stop  temp";
   char txt12[]   = "Reset temp";
   char txt13[]   = "Dia sem";
   char seg1[] = "SEGUNDA", ter[] = "TERCA", qua[] = "QUARTA", qui[] = "QUINTA", sex[] = "SEXTA", sab[] = "SABADO", dom[] ="DOMINGO";
   
   
int1 init = 0, readm = 0, flag1, flag2, flag3, flag4, flag28, flag30, flagi=0, flags=0, flagr=0;
int8 i, position_count, seg, min, hr, s_dia, dia, mes, ano;
int16 debouncing = 0, register_cont = 0, register_cont1 = 0, aux = 0, aux1;


struct flag{
    int8 flagaux;
    int8 flagtrava;
    int8 flagmenu;
};
struct flag menu1, menu2, menu3, menu4, menu5, sub2, sub3, sub4;

struct aux{
   int8 hor;
   int16 hmin;
   int32 hhr;
   int16 tmin;
   int16 thr;
};
struct aux horimetro;

   /*DEFININDO CLOCK*/
   
void inicializar_flags(void){
   menu1.flagmenu =  0x00;
   menu1.flagtrava = 0x00;
   menu2.flagmenu =  0x00;
   menu2.flagtrava = 0x00;
   menu3.flagmenu =  0x00;
   menu3.flagtrava = 0x00;
   menu4.flagmenu =  0x00;
   menu4.flagtrava = 0x00;
   menu4.flagaux =   0x00;
   //horimetro.hmin =  0x00;
   //horimetro.hhr  =  0x00;
   horimetro.tmin =  0x00;
   horimetro.thr  =  0x00;
   init = 0x00;
   flag1 = 0x00;
   flag2 = 0x00;
   flag3 = 0x00;
   flag4 = 0x00;
   flag28 = 0x00;
   flag30 = 0x00;
}

#use delay(clock = 20000000)//20MHz

//#use delay(clock = 8000000)//8MHz

/*-----PROTOTIPOS DE FUNC-----*/
void display_dia();
void DS1307_display();
void escreve_ds(int8 hr, int8 min, int8 s_dia, int8 dia, int8 mes, int8 ano);
void le_DS1307(void);
void trata_teclas(void);
void atualiza_menu(void);
void conv_valor(char buff[],int16 aux);
void atualiza_disp(void);
         /*VOID TRATA INTERRUPÇÃO TIMER0*/
#INT_TIMER0
void timer_0(void){
   register_cont++;
   if(init == 1)aux++;
   if(aux == 1500){
      output_bit(LED, LED_ON);
   }
   if(aux == 3500){
      output_bit(LED, LED_OFF);
      aux = 0;
      aux1++;
   }
   if(register_cont == 30){
      //output_bit(LED, LED_ON);
      trata_teclas();
      //atualiza_menu();
      register_cont = 0;
   }
   if(aux1==10 && readm ==1){
      
      write_eeprom(HR_START + 0, (horimetro.hhr >> 16) & 0xFF);//(horimetro.hhr >> 16) --> Quebrando a variavel em partes de 8 bits pra salvar na eeprom
                                                               //descola a variavel de 32 bits(usamos apenas 24) 16 vezes, alocando o primeiro octeto da var na posição(0x50) da memoria (8bits)
      write_eeprom(HR_START + 1, (horimetro.hhr >>  8) & 0xFF);//MASCARA -> & 0XFF SOMANDO ESSE VALOR A VARIAVEL GARANTIMOS APENAS OS 8 BITS E DESCARTAMOS O RESTANTE
                                                               //desloca a veriavel de 32 bits(usamos apenas 24) 8 vezes, alocando o segundo octeto da var na posição(0x51) de memoria (8bits)
      write_eeprom(HR_START + 2, (horimetro.hhr) & 0xFF);      //desloca a veriavel de 32 bits(usamos apenas 24) 8 vezes, alocando o terceiro octeto da var na posição(0x52) de memoria 
   
      write_eeprom(TP_START + 0, (horimetro.hmin >> 8) & 0xFF);//(horimetro.hhr >> 16) --> Quebrando a variavel em partes de 8 bits pra salvar na eeprom
   
      write_eeprom(TP_START + 1, (horimetro.hmin) & 0xFF);
   }
}

void main()
{

   setup_adc_ports(NO_ANALOGS|VSS_VDD);
   setup_adc(ADC_CLOCK_DIV_2);
   setup_psp(PSP_DISABLED);
   setup_wdt(WDT_OFF);
   //Para 20MHZ
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_2|RTCC_8_bit);
   setup_timer_1(T1_DISABLED);
   setup_timer_2(T2_DISABLED,0,1);
   //Para 8MHZ
   /*setup_timer_0(RTCC_INTERNAL|RTCC_DIV_8|RTCC_8_bit);
   setup_timer_1(T1_DISABLED);
   setup_timer_2(T2_DISABLED,0,1);*/
   setup_comparator(NC_NC_NC_NC);
   setup_vref(FALSE);
   clear_interrupt(INT_TIMER0);     //LIMPA AS INTERRUPÇOES
   clear_interrupt(INT_TIMER1);
   enable_interrupts(INT_TIMER0);   //HABILITA A INTERRUPÇÃO DO TIMER0
   enable_interrupts(INT_TIMER1);   //HABILITA A INTERRUPÇÃO DO TIMER1
   enable_interrupts(GLOBAL);       //HABILITA AS INTERRUPÇOES GLOBAIS
   setup_oscillator(OSC_16MHZ);                // Set internal oscillator to 8MHz
   port_b_pullups(TRUE);                      // Enable PORTB internal weak pull-ups
   /*---INICIALIZA FUNÇOES---*/
   SSD1306_Begin();
   SSD1306_Display(); 
   SSD1306_ClearDisplay();    // Clear the buffer.
   inicializar_flags();
   delay_ms(50);
   
   for(position_count = 0; position_count < 3; position_count++){
      horimetro.hhr = horimetro.hhr << 8;   
      horimetro.hhr += read_eeprom(HR_START + position_count); 
   }
   if(horimetro.hhr == 0xFFFFFFFF)horimetro.hhr=0x00000000;
   position_count = 0;
   for(position_count = 0; position_count < 2; position_count++){
      horimetro.hmin = horimetro.hmin << 8;   
      horimetro.hmin += read_eeprom(TP_START + position_count);
   }
   if(horimetro.hmin == 0xFFFF)horimetro.hmin=0x0000;
   hori[9] = horimetro.hmin % 10 + 0x30;
   hori[8] = horimetro.hmin / 10 + 0x30;
   readm = 1;
   conv_valor(hori, horimetro.hhr);  
   
   while(True){
   

      atualiza_menu();
      
      if(horimetro.hor != min &&  menu2.flagmenu!=2){
         horimetro.hor = min;
         horimetro.hmin++;
         if(flagi == 1 && flags ==0 && flagr ==0){
            horimetro.tmin++;   
         }
         if(horimetro.hmin>59){
            horimetro.hhr++;
            horimetro.hmin = 0;
         }
         if(horimetro.tmin>59){
            horimetro.thr++;
            horimetro.tmin = 0;
         }
         hori[9] = horimetro.hmin % 10 + 0x30;
         hori[8] = horimetro.hmin / 10 + 0x30;
         conv_valor(hori, horimetro.hhr);
         tmr[9] = horimetro.tmin % 10 + 0x30;
         tmr[8] = horimetro.tmin / 10 + 0x30;  
         conv_valor(tmr, horimetro.thr);
      }

      if(flagr == 1 && flagi == 0 && flags ==0){
         horimetro.tmin == 0;
         horimetro.thr == 0;
         tmr[9] = horimetro.tmin % 10 + 0x30;
         tmr[8] = horimetro.tmin / 10 + 0x30;
         conv_valor(tmr, horimetro.thr);
      }

   }
   
}

void trata_teclas(void){
  /*---TECLA CONF---*/
  if(!input(bot1)){
      debouncing = 10;                   //BOTAO PRESSIONADO: FLAG DE DEBOUCING CARREGADA COM O VALOR 25
      while(debouncing > 0)debouncing--; //ENQUANTO CONTADOR NAO É ZERADO FICA PRESO NO LAÇO DECREMENTANDO(TEMPO DE ESTABILIZAR BOTÃO)
      while(!input(bot1))flag1 = 0;      //SE O BOTAO CONTINUAR PRESSIONADO FICA PRESO NO LOOP SEM FAZER NADA ATÉ QUE SEJA SOLTO O BOTÃO
      flag1 = 1;                         //FLAG QUE SINALIZA QUE UM BITÃO FOI PRESSIONADO
  }
  /*---SAI DA TELA DE DESCANSO PRO MENU 1---*/
  if(flag1 && menu1.flagmenu == 0){    //SE A FLAG DE SINALIZAÇÃO ESTIVER SETADA E O MENU1 ESTIVER NA OPOSIÇÃO 0(POSIÇÃO DE DESCANSO)
      menu1.flagmenu = 1;              //O MENU 1 IRA PARA PRIMERA POSIÇÃO ONDE MOSTRARA A LISTA DE OPÇOES
      menu1.flagtrava =1;              //SETA A FLAG DE INTERTRAVAMENTO DO MENU1
      flag1 = 0;                       //RESETA A FLAG DE SINALIZAÇÃO DE PRESSIONAMENTO DO BOTÃO
  }
  /*---ENTRA NA PRIMERA OPÇÃO DO MENU 1---*/
  if (flag1 == 1 && menu1.flagmenu==1 && menu2.flagmenu ==0 ){ //CONDIÇOES DE INTERTRAVAMENTO PARA ENTRAR NA PRIMERA OPÇÃO DO MENU1
      menu2.flagmenu = 1;
      menu2.flagtrava =1;
      flag1 = 0;
  }
  /*---ENTRA NA SEGUNDA OPÇÃO DO MENU 1---*/
  if (flag1 == 1 && menu1.flagmenu==2 && menu3.flagmenu ==0){  //CONDIÇOES DE INTERTRAVAMENTO PARA ENTRAR NA SEGUNDA OPÇÃO DO MENU1
      menu3.flagmenu = 1;
      menu3.flagtrava = 1;
      flag1 = 0;
  }
  /*---ENTRA NA TERCEIRA OPÇÃO DO MENU 1---*/   //CONDIÇOES DE INTERTRAVAMENTO PARA ENTRAR NA TERCEIRA OPÇÃO DO MENU1
  if (flag1 == 1 && menu1.flagmenu==3 && menu4.flagmenu ==0){
      menu4.flagmenu = 1;
      menu4.flagtrava = 1;
      flag1 = 0;
  }
  /*---ENTRA NA QUARTA OPÇÃO DO MENU 1---*/   //CONDIÇOES DE INTERTRAVAMENTO PARA ENTRAR NA TERCEIRA OPÇÃO DO MENU1
  if (flag1 == 1 && menu1.flagmenu==4){
      menu5.flagmenu = 1;
      menu5.flagtrava =1;
      flag1 = 0;
  }
  /*---CONF MENU2---*/
  if (flag1 == 1 && menu2.flagtrava ==1 ){ //CONDIÇOES PARA TECLA CONFIRMA DENTRO DO SUBMENU2
      menu2.flagmenu++;
      if(menu2.flagmenu >2){              //O MENU IRA INCREMENTAR ATÉ 3 AO PRESSIONAR A TECLA CONFIRMA  
         menu2.flagtrava =0;              //APÓS IRA ZERAR AS FLAGS DE INTERTRAVAMENTO E VOLTAR AO MENU ANTERIOR  
         menu2.flagmenu = 0; 
         escreve_ds(hr, min, s_dia, dia, mes, ano);
      }
      flag1 = 0;
  }
  /*---CONF MENU3---*/
  if(flag1 ==1 && menu3.flagtrava ==1){   //CONDIÇOES PARA TECLA CONFIRMA DENTRO DO SUBMENU2
      menu3.flagmenu++;                     
      if(menu3.flagmenu>4){               //O MENU IRA INCREMENTAR ATÉ 3 AO PRESSIONAR A TECLA CONFIRMA
         menu3.flagtrava = 0;             //APÓS IRA ZERAR AS FLAGS DE INTERTRAVAMENTO E VOLTAR AO MENU ANTERIOR
         menu3.flagmenu = 0;
         escreve_ds(hr, min, s_dia, dia, mes, ano);
      }
      flag1 = 0;
  }
  /*---CONF MENU4---*/
  if(flag1 ==1 && menu4.flagtrava ==1 && menu4.flagmenu > 0){   //CONDIÇOES PARA TECLA CONFIRMA DENTRO DO SUBMENU2
      if(menu4.flagmenu == 1){
         flagi = 1;
         flags = 0;
         flagr = 0;
      }
      if(menu4.flagmenu == 2){
         flagi = 0;
         flags = 1;
         flagr = 0;
      }
      if(menu4.flagmenu == 3){
         flagi = 0;
         flags = 0;
         flagr = 1;
      }
      if(menu4.flagmenu>0){               //O MENU IRA INCREMENTAR ATÉ 3 AO PRESSIONAR A TECLA CONFIRMA
         menu4.flagtrava = 0;             //APÓS IRA ZERAR AS FLAGS DE INTERTRAVAMENTO E VOLTAR AO MENU ANTERIOR
         menu4.flagmenu = 0;
      }
      flag1 = 0;
  }
  /*---TECLA VOLTA---*/
  if(!input(bot4)){                       //BOTAO PRESSIONADO: FLAG DE DEBOUCING CARREGADA COM O VALOR 25
      debouncing = 10;                    //ENQUANTO CONTADOR NAO É ZERADO FICA PRESO NO LAÇO DECREMENTANDO(TEMPO DE ESTABILIZAR BOTÃO)
      while(debouncing > 0)debouncing--;  //SE O BOTAO CONTINUAR PRESSIONADO FICA PRESO NO LOOP SEM FAZER NADA ATÉ QUE SEJA SOLTO O BOTÃO
      while(!input(bot4))                 //FLAG QUE SINALIZA QUE UM BITÃO FOI PRESSIONADO
      flag2 = 1;
  }
  /*---VOLTA MENU 1---*/
  if(flag2==1 && menu1.flagmenu>0 && menu2.flagmenu==0 && menu3.flagmenu==0 && menu4.flagmenu==0){ //CONDIÇOES PARA TECLA VOLTA PARA IR PARA TELA DE DESCANSO
      menu1.flagmenu=0;
      menu1.flagtrava=0;
      flag2=0;
  }
  /*---VOLTA MENU 2---*/
  if(flag2==1 && menu1.flagmenu>0 && menu2.flagmenu>0){  //CONDIÇOES PARA TECLA VOLTA PARA IR PARA INCREMENTAR A FLAG ATÉ VOLTAR AO MENU1
      menu2.flagmenu--;
      if(menu2.flagmenu == 0)menu2.flagtrava = 0;
      flag2 = 0;
  }
  /*---VOLTA MENU 3---*/
  if(flag2==1 && menu1.flagmenu>0 && menu3.flagmenu>0){  //CONDIÇOES PARA TECLA VOLTA PARA IR PARA INCREMENTAR A FLAG ATÉ VOLTAR AO MENU1
      menu3.flagmenu--;
      if(menu3.flagmenu==0)menu3.flagtrava = 0;
      flag2 = 0;
  }
  /*---VOLTA MENU 4---*/
  if(flag2==1 && menu4.flagmenu>0 && menu4.flagtrava == 1){  //CONDIÇOES PARA TECLA VOLTA PARA IR PARA INCREMENTAR A FLAG ATÉ VOLTAR AO MENU1
      menu4.flagtrava--;
      if(menu4.flagtrava==0)menu4.flagmenu = 0;
      flag2 = 0;
  }
  /*---TECLA CIMA--*/
  if(!input(bot2)){
      debouncing = 10;                    //BOTAO PRESSIONADO: FLAG DE DEBOUCING CARREGADA COM O VALOR 25
      while(debouncing > 0)debouncing--;  //ENQUANTO CONTADOR NAO É ZERADO FICA PRESO NO LAÇO DECREMENTANDO(TEMPO DE ESTABILIZAR BOTÃO)
      while(!input(bot2))                 //SE O BOTAO CONTINUAR PRESSIONADO FICA PRESO NO LOOP SEM FAZER NADA ATÉ QUE SEJA SOLTO O BOTÃO
      flag4 = 1;                          //FLAG QUE SINALIZA QUE UM BITÃO FOI PRESSIONADO
  }
  /*---NAVEGAÇÃO NO MENU 1---*/
  if(flag4== 1 && menu1.flagmenu > 0 && menu1.flagtrava ==1 && menu2.flagtrava ==0 && menu3.flagtrava ==0 && menu4.flagtrava ==0){
      if(menu1.flagmenu == 1)menu1.flagmenu = 5;         //PARA NAVEGAR NO MENU(P/CIMA P/BAIXO)GARANTIR QUE NAO TENHA NENHUMA FLAG DE SUBMENU SETADA
      menu1.flagmenu--;
      flag4 = 0;
  }
  /*---INCREMENTO DA VARIAVEL MES DO MENU DATA---*/
  if(flag4==1 && menu3.flagmenu == 1){                   //BLOCO COM A LOGICA DOS MESES COM MAIS E MENOS DIAS
      mes++;                                             //DECREMENTA DIRETAMENTE A VARIAVEL
      if(mes == 4 || mes == 6 || mes == 9 || mes == 12){ 
         flag30 = 1;
         flag28 = 0;
      }
      else if(mes == 2){
         flag28=1;
         flag30=0;
      }
      else{
         flag28=0;
         flag30=0;
      }
      if(mes>12)mes=1;
      flag4=0;
  }
  /*---INCREMENTO DA VARIAVEL DIA DO MENU DATA---*/
  if(flag4==1 && menu3.flagmenu == 2){                   //BLOCO COM A LOGICA DAS RESTRIÇOES DE DIAS PARA MESES COM 30, 31 E 28 DIAS
      dia++;                                             //DECREMENTA DIRETAMENTE A VARIAVEL
      if(flag30==1){
         if(dia>30)dia=1;
      }
      else if(flag28==1){
         if(dia>28)dia=1;
      }
      else if(dia>31)dia=1;
      flag4=0;
  }
  /*---INCREMENTO DA VARIAVEL ANO DO MENU DATA---*/
  if(flag4==1 && menu3.flagmenu==3){                     //BLOCO COM A LOGICA DO INCREMENTO DO ANO
      ano++;                                             //DECREMENTA DIRETAMENTE A VARIAVEL
      if(ano>60)ano = 10;
      flag4=0;
  }
  /*---INCREMENTO DIA DA SEMANA---*/
  if(flag4==1 && menu3.flagmenu==4){
      s_dia++;
      if(s_dia>7)s_dia = 1;
      flag4 = 0;
  }
  /*---INCREMENTO DA VARIAVEL HORA DO MENU HORA---*/
  if(flag4==1 && menu2.flagmenu==1){                     //BLOCO COM A LOGICA DO INCREMENTO DAS HORAS 00 ATÉ 23
      hr++;                                            //DECREMENTA DIRETAMENTE A VARIAVEL
      if(hr>23)hr=0;
      flag4=0;
  }
  /*---INCREMENTO DA VARIAVEL MINUTOS DO MENU HORA---*/ 
  if(flag4==1 && menu2.flagmenu==2){
     min++;
     if(min>59)min=0;
                 //DECREMENTA DIRETAMENTE A VARIAVEL
     flag4=0;
   } 
  /*---INCREMENTO DO SUBMENU3 TIMER ---*/
  if(flag4== 1 && menu4.flagtrava ==1 && menu4.flagmenu >= 0){
     menu4.flagmenu--;
     if(menu4.flagmenu == 0)menu4.flagmenu = 3;         //PARA NAVEGAR NO MENU(P/CIMA P/BAIXO)GARANTIR QUE NAO TENHA NENHUMA FLAG DE SUBMENU SETADA
     flag4 = 0;
  }
  /*---TECLA BAIXO---*/
  if(!input(bot3)){ 
      debouncing = 10;                    //BOTAO PRESSIONADO: FLAG DE DEBOUCING CARREGADA COM O VALOR 25
      while(debouncing > 0)debouncing--;  //ENQUANTO CONTADOR NAO É ZERADO FICA PRESO NO LAÇO DECREMENTANDO(TEMPO DE ESTABILIZAR BOTÃO)
      while(!input(bot3))                 //SE O BOTAO CONTINUAR PRESSIONADO FICA PRESO NO LOOP SEM FAZER NADA ATÉ QUE SEJA SOLTO O BOTÃO
      flag3 = 1;                          //FLAG QUE SINALIZA QUE UM BITÃO FOI PRESSIONADO
  }
  /*---NAVEGAÇÃO MENU 1---*/
  if(flag3 == 1 && menu1.flagmenu > 0 && menu1.flagtrava ==1 && menu2.flagtrava == 0 && menu3.flagtrava ==0 && menu4.flagtrava ==0){ 
      menu1.flagmenu++;                   //PARA NAVEGAR NO MENU(P/CIMA P/BAIXO)GARANTIR QUE NAO TENHA NENHUMA FLAG DE SUBMENU SETADA
      if(menu1.flagmenu > 4)menu1.flagmenu = 1;
      flag3 = 0;
  }
  /*DECREMENTA A VARIAVEL MES DO MENU DATA*/
  if(flag3==1 && menu3.flagmenu == 1){                   //BLOCO COM A LOGICA DOS MESES COM MAIS E MENOS DIAS
    mes--;                                               //DECREMENTA DIRETAMENTE A VARIAVEL      
      if(mes == 4 || mes == 6 || mes == 9 || mes == 12){
         flag30=1;
         flag28=0;
      }
      else if(mes == 2){
         flag28=1;
         flag30=0;
      }
      else{
         flag28=0;
         flag30=0;
      }
      if(mes < 1)mes=12;
      flag3 = 0;
      escreve_ds(hr, min, s_dia, dia, mes, ano);
  }
  /*DECREMENTA A VARIAVEL DIA DO MENU DATA*/
  if(flag3==1 && menu3.flagmenu == 2){                   //BLOCO COM A LOGICA DAS RESTRIÇOES DE DIAS PARA MESES COM 30, 31 E 28 DIAS
      dia--;                                             //DECREMENTA DIRETAMENTE A VARIAVEL 
      if(flag30 == 1){
         if(dia==0)dia=30;
      }
      else if(flag28 == 1){
         if(dia==0)dia=28;
      }
      else if(dia==0)dia=31;
      //if(mes<1)mes=12;
      flag3 = 0;
      escreve_ds(hr, min, s_dia, dia, mes, ano);
  }
  /*---DECREMENTO DA VARIAVEL ANO DO MENU DATA---*/      
  if(flag3==1 && menu3.flagmenu==3){                     //BLOCO COM A LOGICA DO DECREMENTO DO ANO
      ano--;                                             //DECREMENTA DIRETAMENTE A VARIAVEL
      if(ano==10)ano = 60;
      flag3 = 0;
  }
  if(flag3==1 && menu3.flagmenu==4){
      s_dia--;
      if(s_dia==0)s_dia = 7;
      flag3 = 0;
      escreve_ds(hr, min, s_dia, dia, mes, ano);
  }
  /*---DECREMENTO DA VARIAVEL HORA DO MENU HORA---*/
  if(flag3==1 && menu2.flagmenu ==1){                    //BLOCO COM A LOGICA DO DECREMENTO DAS HORAS 00 ATÉ 23
      hr--;                                            //DECREMENTA DIRETAMENTE A VARIAVEL 
      if(hr<=0)hr=23;
      flag3=0;
  }
  /*---DECREMENTO DA VARIAVEL MINUTOS DO MENU HORA---*/
  if(flag3==1 && menu2.flagmenu ==2){                  //BLOCO COM A LOGICA DO DECREMENTO DOS MINUTOS 00 ATÉ 59 
      if(min==0)min=59;
      min--;                                             //DECREMENTA DIRETAMENTE A VARIAVEL
      flag3=0;
  }
  /*---DECREMENTO DO SUBMENU3 TIMER ---*/
  if(flag3== 1 && menu4.flagtrava ==1 && menu4.flagmenu >= 0){
      menu4.flagmenu++;
      if(menu4.flagmenu > 3)menu4.flagmenu = 1;         //PARA NAVEGAR NO MENU(P/CIMA P/BAIXO)GARANTIR QUE NAO TENHA NENHUMA FLAG DE SUBMENU SETADA
      flag3 = 0;
  }
}

void atualiza_menu(void){
   switch(menu1.flagmenu){
      case 0 :
         init = 1;
         if(aux == 50){
            le_DS1307();
         }
      break;
      case 1 :
         if(menu2.flagtrava == 0){
         SSD1306_ClearDisplay();    // Clear the buffer.
         SSD1306_DrawText(2, 4, txt1, 2);
         SSD1306_DrawRect(0,2,128,18);
         SSD1306_DrawText(2, 25, txt2, 2);
         //SSD1306_DrawRect(0,23,128,18);
         SSD1306_DrawText(2, 46, txt3, 2);
         //SSD1306_DrawRect(0,44,128,18);
         SSD1306_Display();
         delay_ms(2); 
         }
         switch(menu2.flagmenu){   
            case 1:
               atualiza_disp();
               SSD1306_ClearDisplay();    // Clear the buffer.
               SSD1306_DrawText(32, 4, txt5, 2);
               SSD1306_DrawText(32, 25, horas, 2);
               SSD1306_Display();
               delay_ms(2);
            break;
            case 2:
               atualiza_disp();
               SSD1306_ClearDisplay();    // Clear the buffer.
               SSD1306_DrawText(20, 4, txt6, 2);
               SSD1306_DrawText(32, 25, horas, 2);
               SSD1306_Display();
               delay_ms(2);
            break;
          }
       break;
       case 2 :
         if(menu3.flagtrava == 0){
         SSD1306_ClearDisplay();    // Clear the buffer.
         SSD1306_DrawText(2, 4, txt1, 2);
         //SSD1306_DrawRect(0,2,128,18);
         SSD1306_DrawText(2, 25, txt2, 2);
         SSD1306_DrawRect(0,23,128,18);
         SSD1306_DrawText(2, 46, txt3, 2);
         //SSD1306_DrawRect(0,44,128,18);
         SSD1306_Display();
         delay_ms(2);
         }
         switch(menu3.flagmenu){   
            case 1:
               atualiza_disp();
               SSD1306_ClearDisplay();    // Clear the buffer.
               SSD1306_DrawText(45, 4, txt7, 2);
               SSD1306_DrawText(5, 25, data, 2);
               SSD1306_Display();
               delay_ms(2);
            break;
            case 2:
               atualiza_disp();
               SSD1306_ClearDisplay();    // Clear the buffer.
               SSD1306_DrawText(45, 4, txt8, 2);
               SSD1306_DrawText(5, 25, data, 2);
               SSD1306_Display();
               delay_ms(2);
            break;
            case 3:
               atualiza_disp();
               SSD1306_ClearDisplay();    // Clear the buffer.
               SSD1306_DrawText(45, 4, txt9, 2);
               SSD1306_DrawText(5, 25, data, 2);
               SSD1306_Display();
               delay_ms(2);
            break;
            case 4:
               switch(s_dia){
                  case 1:
                     SSD1306_ClearDisplay();    // Clear the buffer.
                     SSD1306_DrawText(25, 4, txt13, 2);
                     SSD1306_DrawText(25, 25, dom, 2);
                     SSD1306_Display();
                     delay_ms(2);
                  break;
                  case 2:
                     SSD1306_ClearDisplay();    // Clear the buffer.
                     SSD1306_DrawText(25, 4, txt13, 2);
                     SSD1306_DrawText(25, 25, seg1, 2);
                     SSD1306_Display();
                     delay_ms(2);
                  break; 
                  case 3:
                     SSD1306_ClearDisplay();    // Clear the buffer.
                     SSD1306_DrawText(25, 4, txt13, 2);
                     SSD1306_DrawText(35, 25, ter, 2);
                     SSD1306_Display();
                     delay_ms(2);
                  break; 
                  case 4:
                     SSD1306_ClearDisplay();    // Clear the buffer.
                     SSD1306_DrawText(25, 4, txt13, 2);
                     SSD1306_DrawText(30, 25, qua, 2);
                     SSD1306_Display();
                     delay_ms(2);
                  break; 
                  case 5:
                     SSD1306_ClearDisplay();    // Clear the buffer.
                     SSD1306_DrawText(25, 4, txt13, 2);
                     SSD1306_DrawText(30, 25, qui, 2);
                     SSD1306_Display();
                     delay_ms(2);
                  break; 
                  case 6:
                     SSD1306_ClearDisplay();    // Clear the buffer.
                     SSD1306_DrawText(25, 4, txt13, 2);
                     SSD1306_DrawText(35, 25, sex, 2);
                     SSD1306_Display();
                     delay_ms(2);
                  break; 
                  case 7:
                     SSD1306_ClearDisplay();    // Clear the buffer.
                     SSD1306_DrawText(25, 4, txt13, 2);
                     SSD1306_DrawText(30, 25, sab, 2);
                     SSD1306_Display();
                     delay_ms(2);
                  break; 
               }
          }
        break;
        case 3 :
         if(menu4.flagtrava == 0){
         SSD1306_ClearDisplay();    // Clear the buffer.
         SSD1306_DrawText(2, 4, txt1, 2);
         //SSD1306_DrawRect(0,2,128,18);
         SSD1306_DrawText(2, 25, txt2, 2);
         //SSD1306_DrawRect(0,23,128,18);
         SSD1306_DrawText(2, 46, txt3, 2);
         SSD1306_DrawRect(0,44,128,18);
         SSD1306_Display();
         delay_ms(2);
         }
         switch(menu4.flagmenu){
            case 1:
                  SSD1306_ClearDisplay();    // Clear the buffer.
                  SSD1306_DrawText(2, 4, txt10, 2);
                  SSD1306_DrawRect(0,2,128,18);
                  SSD1306_DrawText(2, 25, txt11, 2);
                  //SSD1306_DrawRect(0,23,128,18);
                  SSD1306_DrawText(2, 46, txt12, 2);
                  //SSD1306_DrawRect(0,44,128,18);
                  SSD1306_Display();
                  delay_ms(2);
            break;
            case 2:
                  SSD1306_ClearDisplay();    // Clear the buffer.
                  SSD1306_DrawText(2, 4, txt10, 2);
                  //SSD1306_DrawRect(0,2,128,18);
                  SSD1306_DrawText(2, 25, txt11, 2);
                  SSD1306_DrawRect(0,23,128,18);
                  SSD1306_DrawText(2, 46, txt12, 2);
                  //SSD1306_DrawRect(0,44,128,18);
                  SSD1306_Display();
                  delay_ms(2);
            break;
            case 3:
                  SSD1306_ClearDisplay();    // Clear the buffer.
                  SSD1306_DrawText(2, 4, txt10, 2);
                  //SSD1306_DrawRect(0,2,128,18);
                  SSD1306_DrawText(2, 25, txt11, 2);
                  //SSD1306_DrawRect(0,23,128,18);
                  SSD1306_DrawText(2, 46, txt12, 2);
                  SSD1306_DrawRect(0,44,128,18);
                  SSD1306_Display();
                  delay_ms(2);
            break;
         }            
        break;
        case 4 :
         SSD1306_ClearDisplay();    // Clear the buffer.
         SSD1306_DrawText(2, 4, txt2, 2);
         //SSD1306_DrawRect(0,2,128,18);
         SSD1306_DrawText(2, 25, txt3, 2);
         //SSD1306_DrawRect(0,23,128,18);
         SSD1306_DrawText(2, 46, txt4, 2);
         SSD1306_DrawRect(0,44,128,18);
         SSD1306_Display();
         delay_ms(2);
        break;
        
   }

}

void display_dia() { 
   //BUSCA O DIA DA SEMANA DA VARIAVEL W_DAY E ESCREVE O NOME CORRESPONDENTE NO DISPLAY
  switch(s_dia){
    case 1:  

      SSD1306_Drawtext(49, 0, dom, 1); break;
    case 2:  
      SSD1306_Drawtext(49, 0, seg1, 1); break;
    case 3:
      SSD1306_Drawtext(49, 0, ter, 1); break;
    case 4:
      SSD1306_Drawtext(49, 0, qua, 1); break;
    case 5:  
      SSD1306_Drawtext(49, 0, qui, 1); break;
    case 6:
      SSD1306_Drawtext(49, 0, sex, 1); break;
    default: 
      SSD1306_Drawtext(49, 0, sab, 1); break;
  }
 
}

void DS1307_display() {
      // FAZ A CONVERSÃO DOS DADOS RECEBIDOS PELA I2C DO DS1307 DE BINARIO PARA DECIMAL
  seg = (seg >> 4) * 10 + (seg & 0x0F);
  min = (min >> 4) * 10 + (min & 0x0F);
  hr  = (hr  >> 4) * 10 + (hr  & 0x0F);
  dia = (dia >> 4) * 10 + (dia & 0x0F);
  mes = (mes >> 4) * 10 + (mes & 0x0F);
  ano = (ano >> 4) * 10 + (ano & 0x0F);
  
      // End conversion
      //CONVERTE O VALOR DECIMAL PARA ASCII PARA SER MOSTRADO NO DISPLAY
  horas[7] = seg % 10 + 0x30; //RESTO DA DIVISÃO POR 10 DE SEG RETORNA APENAS A UNIDADE DO VALOR (EX: 15 % 10 = 5) E ADC +48(0x30) DO ASCII
  horas[6] = seg / 10 + 0x30; //DIVIDE UM INTEIRO POR 10, CASO HOUVER RESTO O MESMO É DESCARTADO, RESTANDO APENAS O VALOR DA DEZENA, SOMA +48 ASCII
  horas[4] = min % 10 + 0x30;
  horas[3] = min / 10 + 0x30;
  horas[1] = hr  % 10 + 0x30;
  horas[0] = hr  / 10 + 0x30;
  data[9] = ano % 10 + 0x30;
  data[8] = ano / 10 + 0x30;
  data[4] = mes % 10 + 0x30;
  data[3] = mes / 10 + 0x30;
  data[1] = dia % 10 + 0x30;
  data[0] = dia / 10 + 0x30;
  SSD1306_ClearDisplay();    // Clear the buffer. 
  SSD1306_DrawText(15,  12, data, 1);        // IMPRIME A STRING DE DATA NO DISPLAY
  SSD1306_DrawText(84, 12, horas, 1);        // IMPRIME A STRING DE TEMPO NO DISPLAY
 
}

void escreve_ds(int8 hr, int8 min, int8 s_dia, int8 dia, int8 mes, int8 ano){
    // Convert decimal to BCD
         // Convert decimal to BCD
   min = ((min  / 10) << 4) + (min  % 10);
   hr  = ((hr   / 10) << 4) + (hr   % 10);
   dia = ((dia  / 10) << 4) + (dia  % 10);
   mes = ((mes  / 10) << 4) + (mes  % 10);
   ano = ((ano  / 10) << 4) + (ano  % 10);
    // End conversion

   // ESCREVE DADOS DA HORA NO DS1307 RTC
    i2c_start(SSD1306_STREAM);             // INICIA I2C
    i2c_write(SSD1306_STREAM, 0xD0);       // ENDEREÇO DS1307
    i2c_write(SSD1306_STREAM, 0);          // ENVIA PARA O ENDEREÇO DO REGISTRADOR
    i2c_write(SSD1306_STREAM, 0);          // RESETA OS SEGUNDOS E INICIA O OSCILADOR
    i2c_write(SSD1306_STREAM, min);        // ESCREVE O VALOR DOS MINS NO DS1307
    i2c_write(SSD1306_STREAM, hr);          // ESCREVE O VALOR DAS HRS NO DS1307
    i2c_write(SSD1306_STREAM, s_dia);      // ESCREVE O VALOR DO DIA DA SEMANA NO DS1307
    i2c_write(SSD1306_STREAM, dia);        // ESCREVE O VALOR DO DIA NO DS1307
    i2c_write(SSD1306_STREAM, mes);         // ESCREVE O VALOR DO MES DS1307
    i2c_write(SSD1306_STREAM, ano);          // ESCREVE O VALOR DO ANO NO DS1307
    i2c_stop(SSD1306_STREAM);              // PARA A COMUNICAÇÃO I2C
 
    delay_ms(200);                     // AGUARDA 200MS   
}

void le_DS1307(void){
    // Read current time and date
    i2c_start(SSD1306_STREAM);              // Start I2C
    i2c_write(SSD1306_STREAM, 0xD0);        // DS1307 address
    i2c_write(SSD1306_STREAM, 0);           // Send register address
    i2c_start(SSD1306_STREAM);              // Restart I2C
    i2c_write(SSD1306_STREAM, 0xD1);        // Initialize data read
    seg =   i2c_read(SSD1306_STREAM, 1);      // Read seconds from register 0
    min =   i2c_read(SSD1306_STREAM, 2);      // Read minuts from register 1
    hr  =   i2c_read(SSD1306_STREAM, 3);      // Read hour from register 2
    s_dia = i2c_read(SSD1306_STREAM, 4);      // Read day of week from register 3
    dia =   i2c_read(SSD1306_STREAM, 5);      // Read day from register 4
    mes =   i2c_read(SSD1306_STREAM, 6);      // Read month from register 5
    ano =   i2c_read(SSD1306_STREAM, 7);      // Read year from register 6
    i2c_stop(SSD1306_STREAM);               // Stop I2C

  SSD1306_ClearDisplay();    // Clear the buffer.
  DS1307_display();   // Diaplay time & Date
  display_dia();      // Display day of the week
  SSD1306_DrawText(2, 25, hori, 2);
  SSD1306_DrawText(2, 45, tmr, 2);
  SSD1306_Display();
  delay_ms(2);
}

void conv_valor(char buff[],unsigned int16 aux){
  unsigned char dezena,unidade,centena, milhar, dmilhar;
  unsigned int16 aux1;
  dmilhar = 0;
  milhar = 0;
  centena = 0;
  dezena  = 0;
  unidade = 0;
  aux1 = aux; 
  for(;aux>=10000;aux -=10000){dmilhar++;}
  for(;aux>=1000;aux -=1000){milhar++;}
  for(;aux>=100;aux -=100){centena++;}
  for(;aux>=10;aux -=10){dezena++;}
  for(;aux>=1;aux -=1){unidade++;}
  dmilhar = (dmilhar + 48);
  milhar  = (milhar  + 48);  
  centena = (centena + 48);
  dezena  = (dezena  + 48);
  unidade = (unidade + 48);
  
  
  
  
  
  buff[6] = unidade;
  buff[5] = dezena;
  buff[4] = centena;
  buff[3] = milhar;
  buff[2] = dmilhar;
}

void atualiza_disp(void){
  horas[7] = seg % 10 + 0x30; //RESTO DA DIVISÃO POR 10 DE SEG RETORNA APENAS A UNIDADE DO VALOR (EX: 15 % 10 = 5) E ADC +48(0x30) DO ASCII
  horas[6] = seg / 10 + 0x30; //DIVIDE UM INTEIRO POR 10, CASO HOUVER RESTO O MESMO É DESCARTADO, RESTANDO APENAS O VALOR DA DEZENA, SOMA +48 ASCII
  horas[4] = min % 10 + 0x30;
  horas[3] = min / 10 + 0x30;
  horas[1] = hr  % 10 + 0x30;
  horas[0] = hr  / 10 + 0x30;
  data[9] = ano % 10 + 0x30;
  data[8] = ano / 10 + 0x30;
  data[4] = mes % 10 + 0x30;
  data[3] = mes / 10 + 0x30;
  data[1] = dia % 10 + 0x30;
  data[0] = dia / 10 + 0x30;
}
         
