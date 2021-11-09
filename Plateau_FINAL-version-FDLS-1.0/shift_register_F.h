
#ifndef MY_SHIFT
#define MY_SHIFT

#include "define_LEA_Projet_F.h"
#include <Arduino.h>


// on peut améliorer un intégrant le reboot ligne/colonne dans le décalage en entrant la spec ligneXcolonne
// on peut entrer une variable type "reboot_ligne" dans le programme qui engendre une impression de la singularité

class Shift_register {
  
  public:
     Shift_register(); // crée une instance

     void init();  // initialise les lignes et colonne de manière à faire un C : 1-0-0-0 || L : 0-1-1-1

     void decalage_ligne(bool depassement_l);  // décale la ligne de 1 ==> Lanté: 0-1-1-1 -> L: 1-0-1-1

     void decalage_colonne(bool depassement_c); // décale la colonne de 1 ==> Canté: 1-0-0-0 ->C: 0-1-0-0
   
     void store_l(bool etat);
     
     void store_c(bool etat);

     void imprime();
	 
	 bool read();

  
};



#endif
