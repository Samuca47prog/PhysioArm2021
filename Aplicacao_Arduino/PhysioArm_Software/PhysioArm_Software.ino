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

// Chaves fim de curso
#define   cfcM1ODir   2    //entrada 1 do ULN2003
#define   cfcM1OEsq   3    //entrada 1 do ULN2003

#define   cfcM2CDir   12    //entrada 1 do ULN2003
#define   cfcM2CEsq  13    //entrada 1 do ULN2003

// ================================================================================================================================
// --- Variaveis ---

// -------------------------------------------------------------------- variáveis de controle do motor
bool    interromperGiro;      // interromper o giro do motor
bool    disparoGiroM1O;          // disparar o giro do motor 1
bool    disparoGiroM2C;        // disparar o giro do motor 2
bool    ReposM1Od;    // posicionar o motor 1 em 0º
bool    ReposM2Cd;  // posicionar o motor 2 em 0º
bool    ReposM1Oe;    // posicionar o motor 1 em 0º
bool    ReposM2Ce;  // posicionar o motor 2 em 0º
int     graus = 180;          // quantos graus o motor vai percorrer;
int     t     = 3;            // velocidade. 2 - rápido -- 10 - lento

// -------------------------------------------------------------------- configurações dos exercícios
bool   enRepeticoes;          // habilita o exercicio por repetições
bool   enTempo;               // habilita o exercicio por tempo
int    numRepeticoes;         // guarda o número de repetições setado
int    numTempo;              // guarda a quantidade de tempo selecionada

// -------------------------------------------------------------------- retornos das chaves fim de curso
bool   cfcM1ODirAnt;       // chave fim de curso Direita antes
bool   cfcM1OEsqAnt;      // chave fim de curso Esquerda antes
bool   cfcM1ODirAtu;       // chave fim de curso Direita No momento
bool   cfcM1OEsqAtu;      // chave fim de curso Esquerda No momento
bool   cfcM2CDirAnt;          // chave fim de curso M2C Direita antes
bool   cfcM2CEsqAnt;          // chave fim de curso M2C Esquerda antes
bool   cfcM2CDirAtu;          // chave fim de curso M2C Direita No momento
bool   cfcM2CEsqAtu;          // chave fim de curso M2C Esquerda No momento

// -------------------------------------------------------------------- Serial
String palavra;               // palavra recebida na serial

// ================================================================================================================================
// --- Protótipo de funções ---

// ---------------------------------------------- Exercícios selecionáveis
void ex_porRepeticoes(int numRep, int espera, int angulo);
void ex_porTempo(int numRep, int espera, int angulo);
bool pedidoInterromperGiro();

// ---------------------------------------------- Executar giros
void horario(int grau);
void A_horario(int grau);

// ================================================================================================================================================================
// --- Configurações Iniciais ---
void setup()
{
  // iniciar a serial
  Serial.begin(9600);

  // pinos do M1O
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  // pinos do M2C
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);

  // Entradas das chaves fim de curso
  pinMode(cfcM1ODir, INPUT);
  pinMode(cfcM1OEsq, INPUT);
  pinMode(cfcM2CDir, INPUT);
  pinMode(cfcM2CEsq, INPUT);

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
            disparoGiroM1O = true;
        }
        
        // ------------------------------------------ comando para disparar o motor 2
        if (palavra == "DM2C") {
            disparoGiroM2C = true;
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
        if(palavra == "RM1Od"){
            ReposM1Od = true;
            Serial.println("M1O na direita");
        }

        // ------------------------------------------ comando para reposicionar o motor
        if(palavra == "RM2Cd"){
            ReposM2Cd = true;
            Serial.println("M2C na direita");
        }

        // ------------------------------------------ comando para reposicionar o motor
        if(palavra == "RM1Oe"){
            ReposM1Oe = true;
            Serial.println("M1O na esquerda");
        }

        // ------------------------------------------ comando para reposicionar o motor
        if(palavra == "RM2Ce"){
            ReposM2Ce = true;
            Serial.println("M2C na esquerda");
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
    if(ReposM1Od == true){
        ReposM1Od = false;
        if(not digitalRead(cfcM1ODir))
          disparoGiroM1O = true;
          horario(200);     // Gira horário até bater no cfcDireita  
          disparoGiroM1O = false;  
    }


    //Rotina para reposicionar o motor
    if(ReposM2Cd == true){
        ReposM2Cd = false;
        if(not digitalRead(cfcM2CDir))
          disparoGiroM2C = true;
          horario(200);     // Gira horário até bater no cfcDireita    
          disparoGiroM2C = false;
    }

    
    //Rotina para reposicionar o motor
    if(ReposM1Oe == true){
        ReposM1Oe = false;
        if(not digitalRead(cfcM1OEsq))
          disparoGiroM1O = true;
          A_horario(200);     // Gira horário até bater no cfcDireita  
          disparoGiroM1O = false;  
    }


    //Rotina para reposicionar o motor
    if(ReposM2Ce == true){
        ReposM2Ce = false;
        if(not digitalRead(cfcM2CEsq))
          disparoGiroM2C = true;
          A_horario(200);     // Gira horário até bater no cfcDireita    
          disparoGiroM2C = false;
    }

    
    // manda o motor girar
    if (disparoGiroM1O==true or disparoGiroM2C==true) {
        
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

        // reseta a variável
        disparoGiroM1O = false;

        // reseta a variável
        disparoGiroM2C = false;
  
    } //end if(disparoGiroM1O)
    
} //end loop



// ================================================================================================================================================================
// --- Exercícios ---

// ---------------------------------------------------------------------------------- exercício por repetições
void ex_porRepeticoes(int numRep, int espera, int angulo){
  
  for (int j = 0; j < numRep; j++) {
      // -------------------------------------------------- executa o giro no sentido anti-horário
      A_horario(angulo); 
      
      if (interromperGiro){             // quebra o laço caso tenha sido pedido
          //interromperGiro = false;
          break;
      }
      
      delay(espera);

      // -------------------------------------------------- executa o giro no sentido horário
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
        A_horario(angulo); 
        
        if (interromperGiro){             // quebra o laço caso tenha sido pedido
            //interromperGiro = false;
            break;
        }
        
        delay(espera);
    
        // -------------------------------------------------- executa o giro no sentido horário
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


  if (disparoGiroM1O == true )
  {
       for (int i = 0; i < grau; i++) {

          // ----------------------------------------------------------------- lógica das chaves fim de curso
          cfcM1ODirAtu = digitalRead(cfcM1ODir);
      
          //fim de curso direito
          if(cfcM1ODirAtu==true){
              Serial.write("fcd*");
              return;
          } 
          else {
          // ----------------------------------------------------------------- sinais para rotacionar o motor
              //8 e 9 ligados
              PORTB = B00001001;    delay(t);
          
              //9 e 10 ligados
              PORTB = B00001100;    delay(t);
          
              //10 e 11 ligados
              PORTB = B00000110;    delay(t);
          
              //11 e 8 ligados
              PORTB = B00000011;    delay(t);
              }
  
          // ----------------------------------------------------------------- condição de retorno pela serial
          if(pedidoInterromperGiro())
              return;
      
      }

      Serial.write("mch*");           // responde que chegou no destino
  }


  if (disparoGiroM2C == true )
  {
       for (int i = 0; i < grau; i++) {

          // ----------------------------------------------------------------- lógica das chaves fim de curso
          cfcM2CDirAtu = digitalRead(cfcM2CDir);
      
          //fim de curso direito
          if(cfcM2CDirAtu==true){
              Serial.write("fcd*");
              return;
          } 
          else {
          // ----------------------------------------------------------------- sinais para rotacionar o motor
              //7 e 4 ligados
              PORTD = B10010000;    delay(t);
            
              //6 e 7 ligados
              PORTD = B11000000;    delay(t);
              
              //5 e 6 ligados
              PORTD = B01100000;    delay(t);
              
              //4 e 5 ligados
              PORTD = B00110000;    delay(t);
          }
  
          // ----------------------------------------------------------------- condição de retorno pela serial
          if(pedidoInterromperGiro())
              return;
    
      }

      Serial.write("mch*");           // responde que chegou no destino
  }
}//end horario()

// --------------------------------------------------------------------------------------------------- Sentido de giro anti-horário
void A_horario(int grau) {

  //converte o valor em graus para o valor relativo aos passos do motor
  grau = map(grau, 0, 360, 0, 512);


  if (disparoGiroM1O == true )
  {
       for (int i = 0; i < grau; i++) {
          // ----------------------------------------------------------------- lógica das chaves fim de curso
          cfcM1OEsqAtu = digitalRead(cfcM1OEsq);
      
          //fim de curso direito
          if(cfcM1OEsqAtu==true){                                  
              Serial.write("fcd*");
              return;
          }  
          else {
          // ----------------------------------------------------------------- sinais para rotacionar o motor
              //8 e 9 ligados
              PORTB = B00000011;    delay(t);
              
              //9 e 10 ligados
              PORTB = B00000110;    delay(t);
              
              //10 e 11 ligados
              PORTB = B00001100;    delay(t);
              
              //11 e 8 ligados
              PORTB = B00001001;    delay(t);
          }
          // ----------------------------------------------------------------- condição de retorno pela serial
          if(pedidoInterromperGiro())
              return;
              
      }//end for graus
  }//end if motor M1O


  if (disparoGiroM2C == true )
  {
       for (int i = 0; i < grau; i++) {

          // ----------------------------------------------------------------- lógica das chaves fim de curso
          cfcM2CEsqAtu = digitalRead(cfcM2CEsq);
      
          //fim de curso direito
          if(cfcM2CEsqAtu==true){
              Serial.write("fcd*");
              return;
          } 
          else {
          // ----------------------------------------------------------------- sinais para rotacionar o motor
              //4 e 5 ligados
              PORTD = B00110000;    delay(t);
            
              //5 e 6 ligados
              PORTD = B01100000;    delay(t);
            
              //6 e 7 ligados
              PORTD = B11000000;    delay(t);
            
              //7 e 4 ligados
              PORTD = B10010000;    delay(t);
          }
  
          // ----------------------------------------------------------------- condição de retorno pela serial
          if(pedidoInterromperGiro())
              return;
              
      }//end for graus
  }//end if motor M2C
  
  Serial.write("mch*");   // responde que chegou no destino
  
}//end A_horario()

// --------------------------------------------------------------------------------------------------- Confere se foi pedido para parar o motor
bool pedidoInterromperGiro() {
  if (Serial.available() > 0) {
    palavra = Serial.readStringUntil('*');
    //Se foi recebido IM, retorna a função principal
    if (palavra == "IM") {
      interromperGiro = true;
      Serial.write("min*");
      return true;
    }
  }
}//end pedidoInterrompido()
