//Alexandre Santana Sousa

#include <msp430.h>
#define TRUE 1
#define FALSE 0
#define open 1
#define closed 0

//monitoramento de s1
int mon_s1(void);
void db(int);

void config_echo();
void config_pinos();
void inicia_sensor();

volatile int val1 = 0, val2 = 0, dif = 0; //valores obtidos na captura e diferença entre os dois
volatile float tempoS = 0; //tempo em segundos
volatile int dist = 0; //distancia

int main(void)
{

	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    config_echo();
    config_pinos();
    __enable_interrupt();

    while(TRUE){
        inicia_sensor();
    }
//Descomentar e comentar linhas 30 - 32 caso queira fazer testes usando a chave

    /*while(TRUE){
        while(mon_s1() == FALSE);
        inicia_sensor();

    }*/

}

void inicia_sensor(){
    P3OUT |= BIT0;  //Libera um pulso
    db(10); //10 microsegundos de atraso
    P3OUT &= ~BIT0; //retorna

     if(dist < 20){
         P1OUT &= ~BIT0;
         P4OUT &= ~BIT7;
     }
     else if(dist > 20 && dist < 40){
         P1OUT &= ~BIT0;
         P4OUT |= BIT7;
     }
     else if(dist > 40 && dist < 60){
         P1OUT |= BIT0;
         P4OUT &= ~BIT7;
     }
     else if(dist > 60){
         P1OUT |= BIT0;
         P4OUT |= BIT7;
     }

     db(10485); //10ms de atraso
}


#pragma vector = 52
__interrupt void interrompe_flancos(){
    TA0IV;
    val2 = val1;
    val1 = TA0CCR1;
    dif = val1 - val2;
    tempoS = dif / 1048576.;
    //divide pela velocidade do som em cm/s e divide por 2, pois a onda vai e volta
    dist = (tempoS * 34300)/2;
}

void config_echo(){
    //modo de captura
    TA0CTL = TASSEL_2 | MC_2; //smclk e modo contínuo
    // captura flanco subida e descida
    TA0CCTL1 = CM_3 | CCIS_0 | SCS | CAP | CCIE;

    TA0CCR0 = 12582; //contagens em 12ms (1048576*0,012) ~ 12582
}


void config_pinos(){
    //chave S1
    P2DIR &= ~BIT1;
    P2REN |= BIT1;
    P2OUT |= BIT1;

    //leds verde e vermelho
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;

    P4DIR |= BIT7;
    P4OUT &= ~BIT7;

    //trigger
    P3DIR |= BIT0;
    P3OUT &= ~BIT0;

    // echo
    P1DIR &= ~BIT2;
    P1SEL |= BIT2;


}

void db(int valor){
    volatile int i;
    for(i = 0; i < valor; i++);
}


volatile int ps1 = open;
int mon_s1(){
   if((P2IN&BIT1) == closed){
       if(ps1 == open){
           db(1000);
           ps1 = closed;
           return TRUE;
       }
       else{
           return FALSE;
       }
   }
   else{
       if(ps1 == open){
           return FALSE;
       }
       else{
           db(1000);
           ps1 = open;
           return FALSE;
       }
   }
}
