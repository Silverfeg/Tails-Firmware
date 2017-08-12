/*
 C�digo de controle do rob� de sum� Tails de 500g da equipe FEG-Rob�tica
 Data de in�cio do c�digo: 10/05/2015
 Data da �ltima atualiza��o: 01/06/2015
 Desenvolvedor:         Silas label_address ()()
 */

#include <18F2431.h> //Informa qual o PIC utilizado

#device adc=10  //Informa qual o range do conversor anal�gico-digital atrav�s do n�mero de bits no chip conversor anal�gico-digital

#fuses HS, NOWDT, NOPROTECT, NOBROWNOUT, PUT, NOLVP //Fus�veis para configura��o de uso do PIC

#define POWER_PWM_PERIOD 2000

#use delay(clock=20000000) //Estabelece que o cristal � de 20MHz para calibragem do clock


/*
//HS -> Cristal de alta velocidade
//NOWDT -> Watch Dog desligado (protege contra travamento como em la�os infinitos, por exemplo)
//NOPROTECT -> C�digo n�o protegido contra c�pias
//NOBROWNOUT -> Sem reset por Brownout em caso de baixa tens�o (inferior � 4V)
//PUT -> Com power-up (retardo de 72ms ap�s o Reset para evitar ru�dos na alimenta��o quando o PIC for ligado)
//NOLVP -> Desabilita programa��o em baixa tens�o no pino PGM, garantindo-o como entrada e sa�da convencional
*/

int16 leitura; //Vari�vel gen�rica que armazena a leitura de sensores infravermelho ou de linha
int8 linhaBits = 0; //Inicialmente carrega o valor 0x00 que equivale a n�o ter acionado nenhum sensor de linha
int8 infraBits = 0; //Inicialmente carrega o valor 0x00 que equivale a n�o ter encontrado nenhum advers�rio
char lado; //Vari�vel que define de qual lado est� o nosso oponente

//In�cio das declara��es de fun��es que ser�o usadas na main() do programa

void mover_motor1(int8 velocidade, char sentido) // Fun��o que move os dois motores em um sentido arbitr�rio
{

switch(sentido)
{
case 'f': output_high(PIN_C5); output_low(PIN_C4); break;
case 'a': output_low(PIN_C5); output_high(PIN_C4); break;
 //Define juntamente com o pino C5 o sentido que o motor1 ir� girar, neste caso ele envia n�vel l�gico alto para a sa�da do pino C4
 //Define juntamente com o pino C4 o sentido que o motor1 ir� girar, neste caso ele envia n�vel l�gico baixo para a sa�da do pino C5
}
set_power_pwm0_duty((int16)((POWER_PWM_PERIOD *4) * (1 -(velocidade*0.01)))); // Define o ciclo de trabalho da onda PWM, onde a vari�vel velocidade indica a porcentagem da pot�ncia m�xima que ir� para o motor

}

void mover_motor2(int8 velocidade, char sentido) // Fun��o que move os dois motores em um sentido arbitr�rio
{
switch(sentido)
{
case 'a': output_high(PIN_C6); output_low(PIN_C7); break;
case 'f': output_low(PIN_C6); output_high(PIN_C7); break;
}
 //Define juntamente com o pino C7 o sentido que o motor2 ir� girar, neste caso ele envia n�vel l�gico baixo para a sa�da do pino C6
//Define juntamente com o pino C6 o sentido que o motor2 ir� girar, neste caso ele envia n�vel l�gico alto para a sa�da do pino C7
set_power_pwm2_duty((int16)((POWER_PWM_PERIOD *4) * (1 - (velocidade*0.01)))); // Define o ciclo de trabalho da onda PWM, onde a vari�vel velocidade indica a porcentagem da pot�ncia m�xima que ir� para o motor
}

void parar_motor1()
{
   output_low(PIN_C5); 
   output_low(PIN_C4);
   set_power_pwm0_duty((int16)((POWER_PWM_PERIOD *4) * (0)));
   //Manda n�vel l�gico baixo para as duas portas que definem o sentido do motor, fazendo o mesmo parar
}

void parar_motor2()
{
   output_low(PIN_C6); 
   output_low(PIN_C7);
   set_power_pwm2_duty((int16)((POWER_PWM_PERIOD *4) * (0)));
   //Manda nivel l�gico baixo para as duas portas que definem o sentido do motor, fazendo o mesmo parar
}


void  leituraSensorLinha(int8 canal, int8 bit)  //Fun��o que faz a leitura de um determinado sensor de linha, dependendo dos par�metros da mesma.Nem sempre o canal vai ser o mesmo valor do bit da palavra, d�i a escolha de usar sempre 2 par�metros
{

switch(canal) //Verifica qual o canal selecionado para ativ�-lo.
{
     case 0: set_adc_channel(PIN_A0); break; //Prepara o pino A0 para fazer uma convers�o A/D
     case 1: set_adc_channel(PIN_A1); break; //Prepara o puno A1 para fazer uma convers�o A/D
     case 2: set_adc_channel(PIN_A2); break; //Prepara o pino A2 para fazer uma convers�o A/D
     case 3: set_adc_channel(PIN_A3); break; //Prepara o pino A3 para fazer uma convers�o A/D
     case 4: set_adc_channel(PIN_A4); break; //Prepara o pino A4 para fazer uma convers�o A/D
}
   leitura = read_adc();           //realiza a leitura e convers�o anal�gica-digital
   if (leitura < 400)               //******TESTAR ESTE VALOR*****
      bit_set(linhaBits,bit);      //Estabelece valor 1 para o bit "bit" da palavra de 8 bits linhaBits
   else
      bit_clear(linhaBits,bit); //Estabelece valor 0 para o bit "bit" da palavra de 8 bits linhaBits
}

void sensoresLinha() //Fun��o de leitura dos sensores de linha, � pr�tico ter uma fun��o como essa para agrupar todas as leituras de uma s� vez. 
{
   leituraSensorLinha(0,0);//Envio de par�metros 8 referente ao AN8 do pino B2, e par�metro 0 para escrita no bit   0 da palavra linhaBits
  // leituraSensorLinha(1,1);//Envio de par�metros 9 referente ao AN9 do pino B3, e par�metro 1 para escrita no bit   1 da palavra linhaBits
}

void trataLinha() //Fun��o para o tratamento dos valores dos sensores de linha
{
   switch (linhaBits)
    {                                                                                                  //                                   A0 A1 
      case 0:  parar_motor1(); break;                                                                                               // Nenhum sensor acionado            0  0                                                                                         
      case 1:  mover_motor1(60,'f');/* mover_motor2(60,'a');*/  break;                                 //Sensor Direita acionado            0  1  
      case 2:  mover_motor1(60,'a'); mover_motor2(100,'a');  break;                                 //Sensor Esquerda acionado           1  0  
      case 3:  mover_motor1(100,'a'); mover_motor2(100,'a');  break;                                //Ambos Sensores acionados           1  1  
   default: break;
   }
linhaBits = 0; //Reinicializa a  vari�vel linhaBits
delay_us(10); //Delay �nfimo similar ao do trataInfra
}

void leituraSensorInfra(int8 canal, int8 bit) //Par�metro com 8 bits (menor compat�vel)
{
   switch(canal)
   {
     case 0: set_adc_channel(PIN_A0); break; //Prepara o pino A0 para fazer uma convers�o A/D
     case 1: set_adc_channel(PIN_A1); break; //Prepara o puno A1 para fazer uma convers�o A/D
     case 2: set_adc_channel(PIN_A2); break; //Prepara o pino A2 para fazer uma convers�o A/D
     case 3: set_adc_channel(PIN_A3); break; //Prepara o pino A3 para fazer uma convers�o A/D
     case 4: set_adc_channel(PIN_A4); break; //Prepara o pino A4 para fazer uma convers�o A/D
   }
   delay_us(10);                    //delay de 10 us (valor suscet�vel a erro devido ao uso da interrup��o do Timer1)
                                   //Importante apenas ser um valor �nfimo
   leitura = read_adc();          //Realiza a leitura do canal anal�gico-digital
   if (leitura < 65)     {       //De acordo com c�lculos baseados no datasheet do sensor, 65 seria o valor que representa o limite de algo ser enxergado pelo mesmo
      bit_clear(infraBits,bit);  //Estabelece valor 0 para o bit "canal" da palavra de 8 bits infraBits
   }
   else
      bit_set(infraBits,bit);   //Estabelece valor 1 para o bit "canal" da palavra de 8 bits infraBits
}

void sensoresInfra() //Fun��o de leitura dos sensores de dist�ncia
{
   leituraSensorInfra(2,0); //Sensor da Esquerda
   leituraSensorInfra(3,1); //Sensor da Direita
   leituraSensorInfra(4,2); //Sensor Central com tempo de resposta melhor e alcance menor(para compensar a limita��o para dist�ncias curtas dos outros sensores)
}

void trataInfra() //Fun��o para o tratamento dos valores dos sensores infravermelho
{
   switch (infraBits)
   {                                                                                             //                                 A0 A1 A2 
      case 0: 
      if(lado == 'e'){mover_motor1(50,'a');  mover_motor2(50,'f');}
      else mover_motor1(50,'f');  mover_motor2(50,'a');  
      break;                                                                                // Nenhum sensor acionado          0  0  0 rob� continua em sua busca
      case 1:  mover_motor1(90, 'f'); mover_motor2(90, 'f'); break;                         //Sensor Direita acionado            0  0  1  rob� vai para frente atacando o advers�rio
      case 2:  mover_motor1(90, 'f'); mover_motor2(90, 'f'); break;                         //Sensor Esquerda acionado           0  1  0  rob� vai para frente atacando o advers�rio
      case 3:  mover_motor1(90, 'f'); mover_motor2(90, 'f'); break;                         //Ambos Sensores acionados           0  1  1  rob� vai para frente atacando o advers�rio
      case 4:  mover_motor1(90, 'f'); mover_motor2(90, 'f'); break;                          //                                  1  0  0
      case 5:  mover_motor1(90, 'f'); mover_motor2(90, 'f'); break;                         //                                   1  0  1    
      case 6:  mover_motor1(90, 'f'); mover_motor2(90, 'f'); break;                         //                                   1  1  0
      case 7:  mover_motor1(90, 'f'); mover_motor2(90, 'f'); break;                          //                                  1  1  1  
  
   default: break; //Caso ele n�o encontre nenhuma das possibiliades listadas acima ele faz nada.
   }
   
infraBits = 0x00; //Reinicializa a  vari�vel linhaBits
delay_us(10); //Delay �nfimo similar ao do trataInfra
}
int8 leituraEstrategia() //Fun��o que l� os pinos necess�rio para definir qual estrat�gia ser� utiliada na partida
{
   int8 estrategia;
   
   if(input(PIN_C0)) bit_set(estrategia,0);
   else bit_clear(estrategia,0);
   
   if(input(PIN_C3)) bit_set(estrategia,1);
   else bit_clear(estrategia,1);
   
   return estrategia;
}


void arco(char direcao) //fun��o que executa estrat�gia de arco
{
if (direcao == 'd'){ mover_motor1(100, 'f'); mover_motor2(60, 'f'); delay_ms(3000);}
else mover_motor1(60,'f'); mover_motor2(100, 'f'); delay_ms(3000);
//define um motor mais r�pido que o outro para fazer um arco em torno do oponente 
}

char escolhaLado()
{
  set_adc_channel(PIN_A2);
  delay_us(10);
  leitura = read_adc();          //Realiza a leitura do canal anal�gico-digital
   if (leitura < 65)     {       //De acordo com c�lculos baseados no datasheet do sensor, 65 seria o valor que representa o limite de algo ser enxergado pelo mesmo
      output_high(PIN_C1);       //Acende o led que indica que o lado direito foi selecionado
      return 'd';               //Estabelece valor 0 para o bit "canal" da palavra de 8 bits infraBits
      break;
   }
   else
      output_high(PIN_C2);
      return 'e';   //Estabelece valor 1 para o bit "canal" da palavra de 8 bits infraBits
}

void costas()
{
   mover_motor1(100,'f');
   delay_ms(1000);
   //vai pra frente com toda a velocidade
   
}

// Fim das declara��es das fun��es que ser�o chamadas na main() do programa

void main() //Fun��o principal do programa que efetivamente � executada quando o rob� � ligado
{
/*A fun��o main � dividida em tr�s partes: 
Configura��es, onde os pinos s�o definidos como entrada/sa�da e os pinos PWM s�o configurados
Estrat�gia, � a rotina respons�vel por ser executada primeiro que � a estrat�gia inicial do rob�
Loop, que � a parte respons�vel por dar continuidade ao combate lidando com os sensores dispon�veis at� que o rob� advers�rio seja empurrado para fora
*/
//delay_ms(5000);

//////////CONFIGURA��ES//////////////////////////////////////////////
   //Defini��o das portas como entrada ou sa�da 
   set_tris_a(0b11111111);       //Port A I/O Configura��o das portas como entrada ou sa�da, isso pode ser chamado novamente no programa caso necess�rio  
   set_tris_b(0b00000011);       //Port B I/O Configura��o das portas como entrada ou sa�da, isso pode ser chamado novamente no programa caso necess�rio  
   set_tris_c(0b10010000);       //Port C I/O Configura��o das portas como entrada ou sa�da, isso pode ser chamado novamente no programa caso necess�rio  
   
   //Configura��o dos m�dulos PWM do PIC18F2431
   setup_power_pwm_pins(PWM_COMPLEMENTARY,PWM_COMPLEMENTARY,PWM_COMPLEMENTARY,PWM_COMPLEMENTARY); // Configura os 3 m�dulos PWM como n�o complementares.
   setup_power_pwm(PWM_FREE_RUN, 1, 0, POWER_PWM_PERIOD, 0, 1,30);
   
   //Configura��o dos canais ADC
   //setup_adc(ADC_CLOCK_INTERNAL); //Utiliza o mesmo clock do PIC para o conversor A/D
  // setup_adc_ports(NO_ANALOGS); //Configura todas as portas A/D como anal�gicas, logo o sinal recebido ser� convertido em um valor anal�gico de 0 a 1023
    
 //  lado = escolhaLado(); //
   
   //delay_ms(1000); // Espera do rob� antes do in�cio da partida (Regra)
   
/////////FIM CONFIGURA��ES//////////////////////////////////////////////
   
////////INICIO ESTRATEGIAS////////////////////////////////////////////

/*switch(leituraEstrategia) //switch que escolhe a estrat�gia para in�cio do round 
{
   case 1: arco(lado); break;
   default: break;
}
*/
////////FIM DAS ESTRAT�GIAS///////////////////////////////////////////
  
//////////LOOP/////////////////////////////////////////
   while(1)
   {
   if(input(pin_A0) == 0){
   mover_motor1(60, 'f');
   mover_motor2(60, 'f');  
   }
   else parar_motor1(); parar_motor2();
   }
  //sensoresLinha();
  //trataLinha();
  // sensoresLinha();
  // trataLinha();
   
   
///////////FIM DO LOOP////////////////////////////////
}


