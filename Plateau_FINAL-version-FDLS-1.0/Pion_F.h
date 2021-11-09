
#ifndef Pion_H
#define Pion_H

#include <Arduino.h>
#include "define_LEA_Projet_F.h"



class Pion {

  private:  
    byte a_ID;
    byte a_pos_colonne;
    byte a_pos_ligne;
    byte a_type;
    bool a_fired;
    unsigned long a_tempo;
	

  public:    
    Pion (); 
    ~Pion (); 
	bool check_fired();
    //void set_time();  // INUTILE 
    void set_fired(bool dee_fired);  // enregistre l'horloge du changement en fired ou non
	void set_position(byte col, byte ligne);
	
	byte get_type();
	unsigned long get_time();	
	byte get_col();
	byte get_ligne();
	byte get_ID();
	
	static int nbr_pion;
    
};




#endif
