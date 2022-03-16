/* ================================================================================================================================================================

  @autor  Samuel Simão
  @data   08/2021

  == --- Controle de Motor de Passo Unipolar através do bluetooth --- ==

      Ângulo percorrido
      Sentido de giro
      Velocidade

   == --- VOID LOOP --- ==

   ===== Comunicação =====
   -- Disparar motor
      -> DM*
   -- Número de repetições
      -> RR*
   -- Tempo de duração
      -> TT*
   -- Setar velocidade
      -> SV*
   -- Setar angulo
      -> SA*
   -- Reposicionar motor
      -> RM**

   ===== Controles do motor =====
   -- IF Reposicionar motor
      
   -- IF Disparar motor

   == --- Exercícios --- ==
   -- exercicio por repetição
      
   -- exercicio por tempo

================================================================================================================================================================ */

// ================================================================================================================================
// --- Bibliotecas auxiliares ---
#include <SoftwareSerial.h>

// ================================================================================================================================
// --- Mapeamento de Hardware ---

// pinos do driver ULN2003
#define   in1   8     //entrada 1 do ULN2003
#define   in2   9     //entrada 2 do ULN2003
#define   in3  10     //entrada 3 do ULN2003
#define   in4  11     //entrada 4 do ULN2003

#define   in5  12     //pino de teste

// Chaves fim de curso
#define   cfcDireito   5    //entrada 1 do ULN2003
#define   cfcEsquerdo   6    //entrada 1 do ULN2003


// ================================================================================================================================
// --- Variaveis ---

// -------------------------------------------------------------------- variáveis de controle do motor
bool    interromperGiro;      // interromper o giro do motor
bool    disparoGiro;          // disparar o giro do motor 1
bool    disparoGiroM2;        // disparar o giro do motor 2
bool    ReposicionarMotor;    // posicionar o motor 1 em 0º
bool    ReposicionarMotorM2;  // posicionar o motor 2 em 0º
int     graus = 180;          // quantos graus o motor vai percorrer;
int     t     = 3;            // velocidade. 2 - rápido -- 10 - lento

// -------------------------------------------------------------------- configurações dos exercícios
bool   enRepeticoes;          // habilita o exercicio por repetições
bool   enTempo;               // habilita o exercicio por tempo
int    numRepeticoes;         // guarda o número de repetições setado
int    numTempo;              // guarda a quantidade de tempo selecionada

// -------------------------------------------------------------------- retornos das chaves fim de curso
bool   cfcDireitoAntes;       // chave fim de curso Direita antes
bool   cfcEsquerdoAntes;      // chave fim de curso Esquerda antes
bool   cfcDireitoAtual;       // chave fim de curso Direita No momento
bool   cfcEsquerdoAtual;      // chave fim de curso Esquerda No momento

// -------------------------------------------------------------------- Serial
String palavra;               // palavra recebida na serial

// ================================================================================================================================
// --- Protótipo de funções ---

// ---------------------------------------------- Exercícios selecionáveis
void ex_porRepeticoes(int numRep, int espera, int angulo);
void ex_porTempo(int numRep, int espera, int angulo);

// ---------------------------------------------- Executar giros
void horario(int grau);
void A_horario(int grau);

// ================================================================================================================================================================
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

  // pino de teste
  pinMode(in5, OUTPUT);

  // Entradas das chaves fim de curso
  pinMode(cfcDireito, INPUT);
  pinMode(cfcEsquerdo, INPUT);

} //end setup

// ================================================================================================================================================================
// --- Loop Infinito ---
void loop(){
    
    // ================================================================================================================================
    // -- Comandos via Serial --


    // Entra no if quando recebe algo pela serial (pino RX)
    if (Serial.available() > 0) {
    
        // Lê a serial
        palavra = Serial.readStringUntil('*');

        // ------------------------------------------ comando para disparar o motor 1
        if (palavra == "DM") {
            disparoGiro = true;
        }
        
        // ------------------------------------------ comando para disparar o motor 2
        if (palavra == "DM2") {
            disparoGiroM2 = true;
        }

        // ------------------------------------------ comando para executar por tempo de duração
        if (palavra == "TT") {
            enTempo = true;
        }
    
        // ------------------------------------------ comando para receber o número de repetições
        if (palavra == "RR") {
            // espera receber os números
            while (Serial.available() == 0) {} 
      
            // Lê a serial
            palavra = Serial.readStringUntil('*');
    
            numRepeticoes = palavra.toInt();
            
            enRepeticoes = true;
        }

        // ------------------------------------------ comando para receber a velocidade setada
        if (palavra == "SV") {
            // espera receber os números
            while (Serial.available() == 0) {} 
      
            // Lê a serial
            palavra = Serial.readStringUntil('*');
    
            t = palavra.toInt();
        }

        // ------------------------------------------ comando para receber o ângulo setado
        if (palavra == "SA") {
            // espera receber os números
            while (Serial.available() == 0) {} 
      
            // Lê a serial
            palavra = Serial.readStringUntil('*');
    
            graus = palavra.toInt();
        }

        // ------------------------------------------ comando para reposicionar o motor
        if(palavra == "RM"){
            ReposicionarMotor = true;
        }

        // ------------------------------------------ comando para reposicionar o motor
        if(palavra == "RM2"){
            ReposicionarMotor = true;
        }
        
        // ------------------------------------------ Reseta o buffer da serial
        palavra = "";
        
    }// end if Serial.available
    
    // ================================================================================================================================
    // -- Controles do motor --

    /* Disparou o motor!
     * - Qual exercício foi selecionado?
     * - Qual quantidade foi selecionada?
     */

    //Rotina para reposicionar o motor
    if(ReposicionarMotor == true){
        ReposicionarMotor = false;
        if(not digitalRead(cfcDireito))
          horario(200);     // Gira horário até bater no cfcDireita    
    }


    //Rotina para reposicionar o motor
    if(ReposicionarMotorM2 == true){
        ReposicionarMotorM2 = false;
        if(not digitalRead(cfcDireito))
          horario(200);     // Gira horário até bater no cfcDireita    
    }

    
    // manda o motor girar
    if (disparoGiro==true) {
        // reseta a variável
        disparoGiro = false;
        
        // Responde que o motor foi disparado
        Serial.write("mdi");   

            // ---------------------------------------- Setado por número de repetições
            if (enRepeticoes == true) {
                //reseta a variável
                enRepeticoes = false;
                
                //manda executar o exercício 01
                ex_porRepeticoes(numRepeticoes, 1000, graus);

                if(interromperGiro == true){
                    Serial.write("exc*");     // responde que o exercício foi concluido
                }
                
                interromperGiro = false;
            }

            // ---------------------------------------- Setado por tempo de duração
            if (enTempo == true) {
                //reseta a variável
                enTempo = false;
                
                //manda executar o exercício 01
                ex_porTempo(numTempo, 1000, graus);

                if(interromperGiro == true){
                    Serial.write("exc*");     // responde que o exercício foi concluido
                }
                
                interromperGiro = false;
            }
  
    } //end if(disparoGiro)
    
} //end loop



// ================================================================================================================================================================
// --- Exercícios ---

// ---------------------------------------------------------------------------------- exercício por repetições
void ex_porRepeticoes(int numRep, int espera, int angulo){
  
  for (int j = 0; j < numRep; j++) {
      // -------------------------------------------------- executa o giro no sentido anti-horário
      if(not digitalRead(cfcEsquerdo))
          A_horario(angulo); 
      
      if (interromperGiro){             // quebra o laço caso tenha sido pedido
          //interromperGiro = false;
          break;
      }
      
      delay(espera);

      // -------------------------------------------------- executa o giro no sentido horário
      if(not digitalRead(cfcDireito))
          horario(angulo); 
          
      if (interromperGiro){             // quebra o laço caso tenha sido pedido durante a função de giro
          //interromperGiro = false;
          break;
      }
      
      delay(espera);
    }
}


// ---------------------------------------------------------------------------------- exercício por tempo
void ex_porTempo(int numRep, int espera, int angulo){
    
    //enquanto não receber o caractere de fim...
    while(1) {
        // -------------------------------------------------- executa o giro no sentido anti-horário
        if(not digitalRead(cfcEsquerdo))
            A_horario(angulo); 
        
        if (interromperGiro){             // quebra o laço caso tenha sido pedido
            //interromperGiro = false;
            break;
        }
        
        delay(espera);
    
        // -------------------------------------------------- executa o giro no sentido horário
        if(not digitalRead(cfcDireito))
            horario(angulo); 
            
        if (interromperGiro){             // quebra o laço caso tenha sido pedido durante a função de giro
            //interromperGiro = false;
            break;
        }
        
        delay(espera);
    }      
}


// ================================================================================================================================================================
// --- Funções de giro ---

// --------------------------------------------------------------------------------------------------- Sentido de giro horário
void horario(int grau) {

  //converte o valor em graus para o valor relativo aos passos do motor
  grau = map(grau, 0, 360, 0, 512);

  //loop para percorrer o ângulo
  for (int i = 0; i < grau; i++) {

    // ----------------------------------------------------------------- sinais para rotacionar o motor
    //8 e 9 ligados
    PORTB = B00001001;    delay(t);

    //9 e 10 ligados
    PORTB = B00001100;    delay(t);

    //10 e 11 ligados
    PORTB = B00000110;    delay(t);

    //11 e 8 ligados
    PORTB = B00000011;    delay(t);

    // ----------------------------------------------------------------- condição de retorno pela serial
    if (Serial.available() > 0) {
        palavra = Serial.readStringUntil('*');
        //Se foi recebido IM, retorna a função principal
        if (palavra == "IM") {
          disparoGiro = false;
          interromperGiro = true;
          Serial.write("min*");
          return;
        }
    }

    // ----------------------------------------------------------------- lógica das chaves fim de curso
    cfcDireitoAntes = cfcDireitoAtual;
    cfcDireitoAtual = digitalRead(cfcDireito);

    //fim de curso direito
    if(cfcDireitoAtual==true and cfcDireitoAntes==false){
        disparoGiro = false;                                                  // ---------+++++++++----------- precisa disso?
        cfcDireitoAntes = true;
        Serial.write("fcd*");
        return;
    }
    
  }// end for percorrer ângulo 

  Serial.write("mch*");           // responde que chegou no destino

}//end horario()

// --------------------------------------------------------------------------------------------------- Sentido de giro anti-horário
void A_horario(int grau) {

  //converte o valor em graus para o valor relativo aos passos do motor
  grau = map(grau, 0, 360, 0, 512);

  //loop para percorrer o ângulo
  for (int i = 0; i < grau; i++) {

    // ----------------------------------------------------------------- sinais para rotacionar o motor
    //8 e 9 ligados
    PORTB = B00000011;    delay(t);

    //9 e 10 ligados
    PORTB = B00000110;    delay(t);

    //10 e 11 ligados
    PORTB = B00001100;    delay(t);

    //11 e 8 ligados
    PORTB = B00001001;    delay(t);

        // ----------------------------------------------------------------- condição de retorno pela serial
    // condição de retorno
    if (Serial.available() > 0) {
      palavra = Serial.readStringUntil('*');       // guarda leitura
      //Se foi recebido IM, retorna a função principal
      if (palavra == "IM") {
        disparoGiro = false;
        interromperGiro = true;
        Serial.write("min*");
        return;
      }
    }

    // ----------------------------------------------------------------- lógica das chaves fim de curso
    cfcEsquerdoAntes = cfcEsquerdoAtual;
    cfcEsquerdoAtual = digitalRead(cfcEsquerdo);

    //fim de curso direito
    if(cfcEsquerdoAtual==true and cfcEsquerdoAntes==false){
        disparoGiro = false;
        cfcEsquerdoAntes = true;
        Serial.write("fce*");
        return;
    }
    
  }// end for percorrer ângulo

  Serial.write("mch*");           // responde que chegou no destino

}//end A_horario()
