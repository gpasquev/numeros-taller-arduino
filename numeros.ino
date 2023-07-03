/* Codigo para manejar un tetclado de 12 teclas y un display TM1637 (display de 
4 dífgitos compuestos por 7 LEDs cada uno)

En el marco del Taller, Electricida, Electrónica y Programación dicatado en el
colegio Roberto Themis Speroni durante el mayo-junio de 2023. 

El teclado tiene 7 pines que se conectan al arduino del pin
Teclado 1 --> Pin 8
Teclado 2 --> Pin 2
Teclado 3 --> Pin 3
Teclado 4 --> Pin 4
Teclado 5 --> Pin 5
Teclado 6 --> Pin 6
Teclado 7 --> Pin 7
Dependiendo del teclado puede que sea necesario modificar la asiganción de pines
ya sea en el físico, o en el código. 

El display se conceta:
GND --> GND
Vin --> 3.3V
CLK --> pin12
DIO --> pin13



*/



#include <stdlib.h>
#include <Keypad.h>
#include <TM1637Display.h>

//char customKey;
char cadena[5]; // cuatro char para entrada VARIABLE GLOBAL
char numero[5]; // número que hay que adivinar
bool gano; // variable booleana que indica si ganó


// ===================================================================================
// CONFIGURACION DEL TECLADO =========================================================
const byte ROWS = 3;
const byte COLS = 4;

/* Después de mucho esfuerzo encontré que debo darle la matriz de la siguiente manera */
char keys[ROWS][COLS] = {
  {'3','6','9','#'},
  {'2','5','8','0'},
  {'1','4','7','*'},
 
};

/* la salida 1 del cable hasta la salida 7, se conectaron así:
 *  1 -> 8, 2-> 2, 3-> 3 ,4 -> 4, 5-> 5 , 6-> 6, 7-> 7
 */
byte rowPins[ROWS] = {8,2,3};    //connect to the row pinouts of the keypad
byte colPins[COLS] = {7,6,5,4};  //connect to the column pinouts of the keypad

Keypad customKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); 



// ====================================================================================
// CONFIGURACION DEL VISOR ============================================================

// Define the connections pins:
#define CLK 12
#define DIO 13

// Create display object of type TM1637Display:
TM1637Display display = TM1637Display(CLK, DIO);

// Create array that turns all segments on:
const uint8_t ochos[] = {0xff, 0xff, 0xff, 0xff};

// Create Push
const uint8_t push[] = {
  SEG_A | SEG_B | SEG_G | SEG_F | SEG_E,           // P
  SEG_C | SEG_D | SEG_E,                           // u
  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,           // s
  SEG_C | SEG_G | SEG_E | SEG_F                    // h
};

// Create Guiones 
const uint8_t guiones[] = {
  SEG_G,           // -
  SEG_G,           // -
  SEG_G,           // -
  SEG_G            // -
};

// Create error 
const uint8_t error[] = {
  SEG_A | SEG_F | SEG_G | SEG_D | SEG_E,    // E
  SEG_E | SEG_G ,                           // r
  SEG_E | SEG_G ,                           // r
  SEG_E | SEG_G |SEG_C|SEG_D                // o

};


// Create seg_gano 
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
}


// =====================================================================================
// Función loop ========================================================================

void loop() {

   inicio();

   // Queda en el siguiente loop hasta que "gano" es true (verdadero).
   while( gano== false){
     getnumber();
     verifica();
   }

   // Hace un gfestejo y finalmente vuelve a colocar "gano" = "false"
   festejo();
  
}


void inicio(){
   // inicia el juego y genera el número aletorio, se guardará en cadena (arreglo de cuatro caracteres) 
   // como cuatro caracteres que se corresponden con los digitos de izquierda a derecha.
  
   unsigned long start_time, end_time;
   unsigned long duration;
  
   // Crear un arreglo de caracteres para los dígitos del 0 al 9
   char digitos[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  
   // Obtener el tiempo de inicio antes de presionar el botón
   start_time = millis();

   // Cartel de inicio
   display.setSegments(push);

   while(customKeypad.getKey() == 0){}
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



// funcion para leer numero desde el teclado y mostrarlo en el display
void getnumber(){
  // Espera que se entré un número desde el teclado. A medida que se incorporan los números son mostrados
  // en el display y alamacenados en el arreglo global cadena. Al llegar al cuarto numero finaliza la función. 
  
  // borra el display
  display.clear();

  int i =0;
  char c;
  
  cadena[0] = 0;
  cadena[1] = 0;
  cadena[2] = 0;
  cadena[3] = 0;
  display.clear();
  while(i<4){
    c = customKeypad.getKey();
    if(c != 0){
      cadena[i] = c; 
      Serial.println(c);      
      display.showNumberDec(cadena[i]-'0',false,1,i);
      i++;
      delay(500);
    }
 
  }
  
}

void verifica(){
   // Verifica que el número guardado en cadena sea un número valido, en caso afirmativo obtiene el número de cifras
   // correctas (bien) y el numero de cifras desordenadas (regular), y si el número de correctas es 4 cambia la variable
   // global "gano" a "true". 

  
   // verifica que no hay cifras repetidas en el número introducido. Si el número tiene cifras repetidas la variable
   // numvalido pasa de true a false
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
   if(!numvalido){
        display.clear();
        display.setSegments(error);
   }
   else{
     // cuenta el número de cifras en la posición correcta
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
   cadena[0] = ' ';
   cadena[1] = '0'+bien;
   cadena[2] = ' ';
   cadena[3] = '0'+regular;

   // muestra resultado en el display
   display.clear();
   display.showNumberDec(bien,false,1,1);
   display.showNumberDec(regular,false,1,3);    
   }
  if (bien == 4){
    gano = true;
  }


  //display.showNumberDec(atoi(cadena));
  while(customKeypad.getKey() == 0 && bien != 4){}
  display.setSegments(guiones);

  delay(500);
    
}

void festejo(){
  while(customKeypad.getKey()!='*'){
   display.clear();
   delay(500);
   display.setSegments(seg_gano);
   delay(500);
  }

}


void displaycadena(){
   for( int i=0;i<4;i++){
   display.showNumberDec(atoi(cadena[i]),false,1,i+1);

   }
}
