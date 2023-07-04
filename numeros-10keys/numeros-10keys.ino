/* 
Juego de adivinar el número de 4 cifras.

Código para manejar:
-- un teclado de 10 teclas. Cada tecla a un puerto INPUT_PULLUP diferente.
-  un display TM1637 (display de 4 dígitos compuestos por 7 LEDs cada uno)

El display se conecta:
GND --> GND
Vin --> 3.3V
CLK --> pin12
DIO --> pin13

*/


#include <stdlib.h>
#include <TM1637Display.h>

// Variales globales 
char cadena[4]; // En "cadena" guarda los cuatro digitos entrados por teclado.
char numero[4]; // En "numero" esta guardado el númro secreto.
bool gano;      // La variable "gano" es booleana y define si se ganó o aún no se ganó. 

// ===================================================================================
// CONFIGURACION DEL TECLADO =========================================================
// Puertos donde se conectará cada tecla.
# define K0PIN 11
# define K1PIN 10
# define K2PIN 9
# define K3PIN 8
# define K4PIN 7
# define K5PIN 6
# define K6PIN 5
# define K7PIN 4
# define K8PIN 3
# define K9PIN 2


// ====================================================================================
// CONFIGURACION DEL DISPLAY ==========================================================

// Define the connections pins:
#define CLK 12
#define DIO 13

// Create display object of type TM1637Display:
TM1637Display display = TM1637Display(CLK, DIO);

// Create array that turns all segments on:
const uint8_t ochos[] = {0xff, 0xff, 0xff, 0xff};

// push: arreglo de bytes para escribir "Push" usando display.setSegments()
const uint8_t push[] = {
  SEG_A | SEG_B | SEG_G | SEG_F | SEG_E,           // P
  SEG_C | SEG_D | SEG_E,                           // u
  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,           // s
  SEG_C | SEG_G | SEG_E | SEG_F                    // h
};

// guiones: arreglo de bytes para escribir "----" usando display.setSegments()
const uint8_t guiones[] = {
  SEG_G,           // -
  SEG_G,           // -
  SEG_G,           // -
  SEG_G            // -
};

// error: arreglo de bytes para escribir "Erro" usando display.setSegments()
const uint8_t error[] = {
  SEG_A | SEG_F | SEG_G | SEG_D | SEG_E,    // E
  SEG_E | SEG_G ,                           // r
  SEG_E | SEG_G ,                           // r
  SEG_E | SEG_G |SEG_C|SEG_D                // o

};


// seg_gano: arreglo de bytes para escribir "Gano" usando display.setSegments()
const uint8_t seg_gano[] = {
  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D | SEG_E,    // G
  SEG_A | SEG_B | SEG_C | SEG_G | SEG_E | SEG_F,    // a
  SEG_G | SEG_E | SEG_C ,                           // n
  SEG_G | SEG_E | SEG_C | SEG_D                     // o
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


// =====================================================================================
// Función setup =======================================================================

void setup() {
 Serial.begin(9600);  
 display.setBrightness(7);
 pinMode(K0PIN,INPUT_PULLUP);
 pinMode(K1PIN,INPUT_PULLUP);
 pinMode(K2PIN,INPUT_PULLUP);
 pinMode(K3PIN,INPUT_PULLUP);
 pinMode(K4PIN,INPUT_PULLUP);
 pinMode(K5PIN,INPUT_PULLUP);
 pinMode(K6PIN,INPUT_PULLUP);
 pinMode(K7PIN,INPUT_PULLUP);
 pinMode(K8PIN,INPUT_PULLUP);
 pinMode(K9PIN,INPUT_PULLUP);
 }


// =====================================================================================
// Función loop ========================================================================

void loop() {

   // Genera el número secreto y asigna "false" a "gano".
   inicio();

   // Queda en el siguiente loop hasta que "gano" es "true" (verdadero).
   while( gano== false){
     getnumber();
     verifica();
   }

   // Hace un festejo
   festejo();
  
}


void inicio(){
   // Inicia el juego y genera el número aletorio, se guardará en "cadena" (arreglo de cuatro caracteres). 
   // El primer caracter de cadena es la cifra más significativa y el cuarto la cifra de las unidades.
  
   unsigned long start_time, end_time;
   unsigned long duration;
  
   // Crear un arreglo de caracteres para los dígitos del 0 al 9
   char digitos[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  
   // Obtener el tiempo de inicio antes de presionar el botón
   start_time = millis();

   // Cartel de inicio
   display.setSegments(push);

   while(leetecla() == 'X'){}
   // Obtener el tiempo de finalización después de presionar el botón
   end_time = millis();

   // Calcular la duración en segundos
   duration = end_time - start_time;

   // Modificar la semilla de la función rand() en función de la duración
   randomSeed(duration);
   int ind = 0;

   for(int ind = 0; ind<4;ind++){
     int i = random(10-ind);
     numero[ind] = digitos[i];
     // elimina número de la lista digitos. 
     for(int j = 0;j<9-ind;j++){
       if(j < i){
         digitos[j] = digitos[j];
       }
       else{
         digitos[j] = digitos[j+1];
       }
     }
     
   }
   Serial.print("numero secreto: ");        
   Serial.println(numero);
   gano = false; 
}



// Función para leer número desde el teclado y mostrarlo en el display
void getnumber(){
  // Espera que se entré un número desde el teclado. A medida que se incorporan los números son mostrados
  // en el display y alamacenados en el arreglo global "cadena". Al llegar al cuarto número finaliza la función. 
  
  // borra el display
  display.clear();

  int i =0;
  char c;
  
  while(i<4){
    c = leetecla();
    if(c != 'X'){
      cadena[i] = c; 
      Serial.println(c);      
      display.showNumberDec(cadena[i]-'0',false,1,i);
      i++;
      delay(500);
    }
 
  }
  
}

void verifica(){
   // Verifica que el número guardado en "cadena" sea un número válido, en caso afirmativo obtiene el número de cifras
   // correctas ("bien") y el número de cifras desordenadas ("regular"). Si el número de cifras correctas es 4 cambia la variable
   // global "gano" de "false" a "true". 

   // Primer verificación. Verifica que no hay cifras repetidas en el número introducido. Si el número 
   // tiene cifras repetidas la variable "numvalido" pasa de "true" a "false".
   bool numvalido = true;
   int bien = 0;
   int regular = 0;
   for( int i=0;i<4;i++){
      for(int j=i+1;j<4;j++){
          if (numvalido && cadena[i] == cadena[j]){
              numvalido = false;
              Serial.println("ERROR");
          }
      }
   }
   
   // Si hubo cifras repetidas manda el cartel "error" al display.
   if(!numvalido){
        display.clear();
        display.setSegments(error);
   }
  // Si no hubo cifras repetida analiza el númro de cifras correctas ("bien") y desordenadas ("regular")
   else{
     for(int i = 0; i<4; i++){
       for(int j = 0; j<4; j++){
         if(cadena[i] ==numero[j]){
           if (i == j){++bien;}
           else {++regular;}
         }
       }
     }
   Serial.print("bien ");        
   Serial.println(bien);
   Serial.print("regular ");        
   Serial.println(regular);  

   // Muestra el resultado en el display.
   display.clear();
   display.showNumberDec(bien,false,1,1);
   display.showNumberDec(regular,false,1,3);    
   }
  // Si "bien" es 4 modfifica el valor de la variable "gano": "gano" -> true 
  if (bien == 4){
    gano = true;
    delay(1000);
  }


  // Esperamos a que se apriete alguna tecla para continuar.
  while(leetecla() == 'X' and !gano){}
  display.setSegments(guiones);

  delay(500);
    
}

void festejo(){
  while(leetecla()!='0'){
   display.clear();
   delay(500);
   display.setSegments(seg_gano);
   delay(500);
  }

}

// Función leeteclado devuelve la tecla que está oprimida cada vez que se la llama. 
// Devuelve 0 (Null) si no hay tecla oprimida. 
char leetecla(){
  char c = 'X';
    if(digitalRead(13)==LOW){
      c = '0';
    }
    if(digitalRead(12)==LOW){
      c = '1';
    }
    if(digitalRead(11)==LOW){
      c = '2';
    }
    if(digitalRead(10)==LOW){
      c = '3';
    }
    if(digitalRead(9)==LOW){
      c = '4';
    }
    if(digitalRead(8)==LOW){
      c = '5';
    }
    if(digitalRead(7)==LOW){
      c = '6';
    }
    if(digitalRead(6)==LOW){
      c = '7';
    }
    if(digitalRead(5)==LOW){
      c = '8';
    }
    if(digitalRead(4)==LOW){
      c = '9';
    }
  return c;
  
}
