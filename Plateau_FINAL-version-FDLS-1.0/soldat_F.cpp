#include "soldat_F.h"

byte Soldat::nbr_soldat=0;


Soldat::Soldat(byte ID,byte in_fonction) {
  a_ID=ID;
  nbr_soldat+=1;
  a_pos_colonne=0;
  a_pos_ligne=0;
  a_etat=INDIFFERENT_SOLDAT;
  a_fonction = in_fonction;
}

/*Soldat::Soldat(byte ID, byte col, byte ligne) {
  a_ID=ID;
  a_pos_colonne=col;
  a_pos_ligne=ligne;
}*/

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

byte Soldat::get_fonction(){
	return a_fonction;
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
	




bool Soldat::ask_presence(byte colonne, byte ligne, byte type ) { // area c'est une zone ou un point
	// on doit regarder si la position entrée est dans la zone du soldat, avec une variante pour les femme enfant
	bool retour=0;
	if ((colonne<=a_pos_colonne+DIM_ZDS) && (colonne >=a_pos_colonne-DIM_ZDS) && (ligne<=a_pos_ligne+DIM_ZDS) && (ligne>=a_pos_ligne-DIM_ZDS)){
		if (type==PION_NORM && !(colonne == a_pos_colonne && ligne == a_pos_ligne)) retour=1;
		else if (type ==PION_SPE && ((colonne<=a_pos_colonne+1) && (colonne >=a_pos_colonne-1) && (ligne<=a_pos_ligne+1) && (ligne>=a_pos_ligne-1))) retour=1;
	}
	return retour;	// ATTENTION MODIF depuis else
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
