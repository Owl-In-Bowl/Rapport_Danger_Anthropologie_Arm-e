#include "define_LEA_Projet.h"
#include <Arduino.h>


#include "shift_register.h"
Shift_register   shift;


#include "soldat.h"  // chercher "ENTER INSTANCE CHECK"
Soldat S1 (1,1,1);




byte pos_read_ligne=0;
byte pos_read_colonne=0;


bool presence [COLONNNE][LIGNE]={0};





unsigned long tempo=0;   //PAS DANS FINAL
const unsigned long ACTUALISATION=1000; //PAS DANS FINAL

void setup() {
  cli();
  // insérer le timer 2
  sei();
  
  shift.init();
  
  tempo=millis();
}



void loop() {
 balayage(); 
}




void balayage (){
  if (shift.read()!=presence[pos_read_colonne][pos_read_ligne]) {
    // check qui a apparut ou disparu - ENTER INSTANCE CHECK
    if (S1.check_disparu(x,i,presence[x][i])) presence [pos_read_colonne][pos_read_ligne]=shift.read();
    // else if etc...
    else Serial.println ("ERREUR : la recherche du changement statut plateau  a echoué");
  }

  if (millis()>tempo+ACTUALISATION) {
    tempo=millis();
    pos_read_colonne+=1;
    if (pos_read_colonne>=COLONNE) {
        shift.decalage_colonne(1);
        pos_read_colonne=0;      
        pos_read_ligne+=1;
        if (pos_read_ligne>=LIGNE) {
          shift.decalage_ligne(1);
          pos_read_ligne=0;
        }
        else shift.decalage_ligne(0);
        }
    else shift.decalage_colonne(0);
  }
}
