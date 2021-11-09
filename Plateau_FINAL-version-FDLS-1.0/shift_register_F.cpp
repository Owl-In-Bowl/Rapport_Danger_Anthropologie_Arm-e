#include "shift_register_F.h"




Shift_register::Shift_register(){}; // crée une instance

void Shift_register::init(){// initialise les lignes et colonne de manière à faire un C : 1-0-0-0 || L : 0-1-1-1
  pinMode(SHIFT_L, OUTPUT);pinMode(DATA_IN_L, OUTPUT);
  pinMode(SHIFT_C, OUTPUT);pinMode(DATA_IN_C, OUTPUT);
  pinMode(IMPRESSION, OUTPUT);
  pinMode(READ_PIN, INPUT);
  
  for (byte x=0; x<COLONNE; x++) {
    store_c(0);
  }
  store_c(1);
  
  for (byte y=0; y<LIGNE; y++) {
    store_l(1);
  }
  store_l(0);

  imprime();
}

void Shift_register::decalage_ligne(bool depassement_l){  // décale la ligne de 1 ==> Lanté: 0-1-1-1 -> L: 1-0-1-1
  if (depassement_l)store_l(0);
  else {store_l(1);}
  imprime();
}

void Shift_register::decalage_colonne(bool depassement_c){ // décale la colonne de 1 ==> Canté: 1-0-0-0 ->C: 0-1-0-0
  if (depassement_c) store_c(1);
else {store_c(0); imprime();} 
}


void Shift_register::store_l(bool etat){
  digitalWrite(SHIFT_L,0);
  digitalWrite(DATA_IN_L,etat);
  delayMicroseconds(1);
  digitalWrite(SHIFT_L,1);
}
void Shift_register::store_c(bool etat){
  digitalWrite(SHIFT_C,0);
  digitalWrite(DATA_IN_C,etat);
  delayMicroseconds(1);
  digitalWrite(SHIFT_C,1);      
}
void Shift_register::imprime() {
  digitalWrite(IMPRESSION,0);
  delayMicroseconds(1);
  digitalWrite(IMPRESSION,1);
}

bool Shift_register::read() {
	return digitalRead(READ_PIN);  
}