#include "Pion_F.h"

int Pion::nbr_pion=0;

Pion::Pion(){
  
  a_ID=nbr_pion;
  a_type=PION_NORM;
  if (nbr_pion == NB_PION-1) a_type=PION_SPE;
  else if (nbr_pion>= NB_PION+NB_BATIMENT) a_type=ARBRE;
  else if (nbr_pion>=NB_PION) a_type=MUR;
  nbr_pion++;
  a_fired=0;
  a_tempo=0;
}

Pion::~Pion(){
	nbr_pion--;
}


bool Pion:: check_fired() { //  il y a un cas ou on renvoi rien 
	if (millis()>=a_tempo+TEMPS_ALERTE) return a_fired;
	else return 0;
}

/*void Pion:: set_time() {   // pas utile
  a_tempo= micros();  
}
*/

void Pion::set_fired(bool dee_fired) {
	a_tempo=millis();
	a_fired=dee_fired;  
}

void Pion::set_position (byte col, byte ligne) {
	a_pos_colonne=col;
	a_pos_ligne=ligne;
}

byte Pion::get_ID() {
	return a_ID;
}


byte Pion::get_type() {
	return a_type;
}

byte Pion::get_col() {
	return a_pos_colonne;
}

byte Pion::get_ligne(){
	return a_pos_ligne;
}
