/* ================================================================================================================================================================

  @autor  Samuel Simão
  @data   08/2021

  == --- Controle de Motor de Passo Unipolar através do bluetooth --- ==
  
    * Ângulo percorrido
    * Sentido de giro
    * Velocidade

/* == --- PROTOCOLO DE COMUNICAÇÃO --- ==
 * 
 * -- Disparar motor
 *    -> C1*
 * -- Interromper motor
 *    -> C0*
 * -- Ângulo de giro
 *    -> AA*
 *    -> xxx*
 * -- Velocidade de giro
 *    -> VV*
 *    -> x*     //2 a 9
 * -- Sentido de giro 
 *    -> SH*     //horário
 *    -> SA*    //anti-horário
 * -- Reposicionar Motor
 *    -> RM*
 */

 /* == --- Valores de velocidade --- ==
 *  t = 1 --> ------ ---
 *  t = 2 --> 14,184 rpm
 *  t = 3 --> 09,600 rpm
 *  t = 4 --> 07,481 rpm
 *  t = 5 --> 05,964 rpm
 *
============================================================================================================================================================= */

// ==============================================================================================================================
// --- Bibliotecas auxiliares ---
#include <SoftwareSerial.h>


// ==============================================================================================================================
// --- Mapeamento de Hardware ---

// pinos do driver ULN2003
#define   in1   8     //entrada 1 do ULN2003
#define   in2   9     //entrada 2 do ULN2003
#define   in3  10     //entrada 3 do ULN2003
#define   in4  11     //entrada 4 do ULN2003

// Chaves fim de curso
#define   cfcDireito   5    //entrada 1 do ULN2003
#define   cfcEsquerdo   6    //entrada 1 do ULN2003



// ==============================================================================================================================
// --- Variaveis ---
int graus = 180;    // quantos graus o motor vai percorrer;
int t     = 4;      // velocidade. 2 - rápido -- 10 - lento

bool   sentidoGiro;       // escolher sentido de giro (0 - anti-horário --- 1 - horário)
bool   disparoGiro;       // disparar o giro do motor

bool   ReposicionarMotor = false;

bool   cfcDireitoAntes;
bool   cfcDireitoAtual;
bool   cfcEsquerdoAntes;
bool   cfcEsquerdoAtual;

String palavra;      // palavra recebida na serial

// ==============================================================================================================================
// --- Protótipo de funções ---
void horario(int grau);
void A_horario(int grau);


// =============================================================================================================================================================
// --- Configurações Iniciais ---
void setup() 
{
    // iniciar a serial
    Serial.begin(9600);  
  
    // pinos do driver
    pinMode(in1, OUTPUT); 
    pinMode(in2, OUTPUT); 
    pinMode(in3, OUTPUT); 
    pinMode(in4, OUTPUT); 
  
    // Entradas das chaves fim de curso
    pinMode(cfcDireito, INPUT);
    pinMode(cfcEsquerdo, INPUT);

  

} //end setup

// =============================================================================================================================================================
// --- Loop Infinito ---

void loop() 
{
    // ==============================================================================================================================
    // --- Comandos Seriais ---
    
    // Entra no if quando recebe algo pela serial (pino RX)
    if(Serial.available() > 0){
  
        // Lê a serial
        palavra = Serial.readStringUntil('*');       // guarda leitura
    
        if(palavra == "C1"){
            disparoGiro = true;
        }
    
        if(palavra == "SH"){
            Serial.write("fsh");    // horário
            sentidoGiro = true;
          }
        else if (palavra == "SA"){
            Serial.write("fsa");    // anti-horário
            sentidoGiro = false;
        }
    
        if(palavra == "AA"){
          while(Serial.available() == 0){} // espera receber os números
          graus = Serial.parseInt();
            Serial.write("faa");    // graus setados
            Serial.print(graus);
        }
    
        if(palavra == "VV"){
            while(Serial.available() == 0){} // espera receber os números
            t = Serial.parseInt();
            Serial.write("fvv");
            Serial.print(t);
        }


        if(palavra == "RM"){
            ReposicionarMotor = true;
        }

        // reseta o buffer da serial
        palavra = "";
        
    }// end if serial available

    // ==============================================================================================================================
    // --- Execuções do motor ---

    
    // manda o motor girar
    if(disparoGiro){
  
        Serial.write("fd");   // Disparar disparado!
    
        // reseta a variável
        disparoGiro = false;
        
        // define o sentido de giro
        if(sentidoGiro){
            if(not digitalRead(cfcDireito))
                horario(graus);     // Gira horário
        }
        else{
            if(not digitalRead(cfcEsquerdo))
                A_horario(graus);   // Gira anti-horário
        }
    }

    //Rotina para reposicionar o motor
    if(ReposicionarMotor == true){
        ReposicionarMotor = false;
        if(not digitalRead(cfcDireito))
          horario(200);     // Gira horário até bater no cfcDireita    
    }

} //end loop



// =============================================================================================================================================================
// --- Funções ---

// Sentido de giro horário
void horario(int grau){
  grau = map(grau, 0, 360, 0, 512);
  
  for(int i=0; i<grau; i++){
    //8 e 9 ligados
    PORTB = B00001001;
    delay(t);

    //9 e 10 ligados
    PORTB = B00001100;
    delay(t);

    //10 e 11 ligados
    PORTB = B00000110;
    delay(t);

    //11 e 8 ligados
    PORTB = B00000011;
    delay(t);

    // condição de retorno
    if(Serial.available() > 0) {
      palavra = Serial.readStringUntil('*');       // guarda leitura
  
      if(palavra == "C0"){
        disparoGiro = false;
        Serial.write("fs");
        return;
      }
    }


    cfcDireitoAntes = cfcDireitoAtual;
    cfcDireitoAtual = digitalRead(cfcDireito);

    //fim de curso direito
    if(cfcDireitoAtual==true and cfcDireitoAntes==false){
        disparoGiro = false;
        cfcDireitoAntes = true;
        Serial.write("fcd");
        return;
    }
    
  }

  Serial.write("fmc");   // Motor chegou
  
}//end horario()
 

// Sentido de giro anti-horário
void A_horario(int grau){
  grau = map(grau, 0, 360, 0, 512);
  
  for(int i=0; i<grau; i++){
    //8 e 9 ligados
    PORTB = B00000011;
    delay(t);

    //9 e 10 ligados
    PORTB = B00000110;
    delay(t);

    //10 e 11 ligados
    PORTB = B00001100;
    delay(t);

    //11 e 8 ligados
    PORTB = B00001001;
    delay(t);

    // condição de retorno
    if(Serial.available() > 0) {
      palavra = Serial.readStringUntil('*');       // guarda leitura
  
      if(palavra == "C0"){
        disparoGiro = false;
        Serial.write("fs");
        return;
      }
    }



    cfcEsquerdoAntes = cfcEsquerdoAtual;
    cfcEsquerdoAtual = digitalRead(cfcEsquerdo);

    //fim de curso direito
    if(cfcEsquerdoAtual==true and cfcEsquerdoAntes==false){
        disparoGiro = false;
        cfcEsquerdoAntes = true;
        Serial.write("fce");
        return;
    }

  }

  Serial.write("fmc");   // Motor chegou
}//end A_horario()
