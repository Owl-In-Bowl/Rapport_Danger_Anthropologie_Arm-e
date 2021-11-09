#ifndef MY_SOLDAT_H
#define MY_SOLDAT_H

#include <Arduino.h>
#include <define_LEA_Projet_D.h>

class Soldat {

  private:  
    byte a_ID;
    byte a_pos_colonne;
    byte a_pos_ligne;
    byte a_etat;  
	byte a_groupe;
	unsigned long a_tempo;
	bool a_checked;

  public:
    Soldat(byte identifiant);
    Soldat(byte identifiant,byte col, byte ligne);
    	
	bool ask_presence(byte colonne, byte ligne,  bool type =PION_NORM); 
	void check_state();
	
    
    void set_position(byte col, byte ligne);
	void set_state(byte new_state);  // quand on déclare l'état, suivat létat indiqué, on entre l'horloge aussi
	void set_groupe(byte in_groupe);
	//void set_tempo(unsigned long in_tempo);   // on peut insérer le in tempo quand on set_state
	void set_checked(bool checked);
	
	
	byte get_col();
    byte get_ligne();
	byte get_state();
	byte get_groupe();
	bool get_checked();
	byte get_ID();

    //bool get_disparu();
   //bool check_disparu(byte colonne, byte ligne, bool check_type);  // chack type identifie une disp (1) ou apparation de pièce (0)

//    get_state();
//    set_state(byte etat);  
    
  
};

#endif
