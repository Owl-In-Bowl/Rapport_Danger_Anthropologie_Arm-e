#include "soldat_D.h"

Soldat::Soldat(byte ID) {
  a_ID=ID;
  a_pos_colonne=0;
  a_pos_ligne=0;
  a_etat=INDIFFERENT_SOLDAT;
}

Soldat::Soldat(byte ID, byte col, byte ligne) {
  a_ID=ID;
  a_pos_colonne=col;
  a_pos_ligne=ligne;
}




byte Soldat::get_col(){
  return a_pos_colonne;
}

byte Soldat::get_ligne(){
  return a_pos_ligne;
}

byte Soldat::get_state(){
	return a_etat;
}

byte Soldat::get_groupe(){
	return a_groupe;
}

bool Soldat::get_checked(){
	return a_checked;
}

byte Soldat::get_ID(){
	return a_ID;
}


void Soldat::set_position(byte col, byte ligne){
  a_pos_colonne=col;
  a_pos_ligne=ligne;
}

void Soldat::set_state(byte in_state) {
	a_etat=in_state;
	a_tempo=millis();
}

void Soldat::set_groupe(byte in_groupe){
a_groupe=in_groupe;
}

/*void Soldat::set_tempo(unsigned long in_tempo){
	a_tempo=in_tempo;
}
*/

void Soldat::set_checked(bool checked) {
	a_checked=checked;
}
	




bool Soldat::ask_presence(byte colonne, byte ligne, bool type ) { // area c'est une zone ou un point
	// on doit regarder si la position entrée est dans la zone du soldat, avec une variante pour les femme enfant
	if ((colonne<=a_pos_colonne+DIM_ZDS) && (colonne >=a_pos_colonne-DIM_ZDS) && (ligne<=a_pos_ligne+DIM_ZDS) && (ligne>=a_pos_ligne-DIM_ZDS)){
		if (type==PION_NORM) return 1;
		else if ((colonne<=a_pos_colonne+1) && (colonne >=a_pos_colonne-1) && (ligne<=a_pos_ligne+1) && (ligne>=a_pos_ligne-1)) return 1;
	}
	return 0;	// ATTENTION MODIF depuis else
}

void Soldat::check_state(){
	switch (a_etat) {
		case ALERTE_RAS:
			if (millis()>= a_tempo + TEMPS_ALERTE){
				this-> set_state(SECURITE);
			}
			break;
		
		case ALERTE_DANGER:
			if (millis()>= a_tempo + TEMPS_ALERTE) {
				this-> set_state(DANGER);
			}
			break;
		
		case DANGER:
			if (millis()>=a_tempo + TEMPS_DANGER){
				this-> set_state(SECURITE);
			}
			break;
		
		case SECURITE:
			if (millis()>= a_tempo + TEMPS_SECURITE) {
				this-> set_state(INDIFFERENT_SOLDAT);
			}
			break;
	}
}

//get_state();
//set_state(byte etat);  
