#ifndef MY_DEFINE
#define MY_DEFINE
/**/
/*


la définition des lignes et colonne ne limitent pas vraisemblablement le nombre de case
cependant, pour les tableau de pointeur devant tranformer une position cartésienne en un entier
on limite le plateau à un 14 X 14
		==> sinon modifier les type de varable dans le programme


*/


#define COLONNE 9 // définit le nombre de colonne
#define LIGNE 9 // définit le nombre de ligne

#define FREQ_PLATEAU  25
#define FREQ_LED 40



#define NB_SOLDAT 8
#define NB_PION 9

#define TEMPS_ALERTE 1500    // en mS (2.5s)
#define TEMPS_SECURITE 2500  // en mS (3s)
#define TEMPS_DANGER 1800    // en mS (3s)

#define proba_death 30  // % de proba que le pion meurt



/*********** MESURE VESTIAIRE *************/
#define READ_VEST 5    // lit le contact reed sur le vestaire
#define LED_VEST 4     // pilote la led du vestiaire



/*********** MESURE PLATEAU ***************/
#define  SHIFT_L 10  			// pin 11 (SHIFT)- impression du contenu du registre
#define  DATA_IN_L 11    		// pin 14 (DATA_IN) - byte de lecture
#define  SHIFT_C 9			// pin 11 (SHIFT)- impression du contenu du registre
#define  DATA_IN_C 8    		// pin 14 (DATA_IN) - byte de lecture
#define  IMPRESSION 12      		// pin 12 (IMPRESSION)- décalage du registre  

#define READ_PIN 6

/************ BLOCAGE DES PION *************/
#define LAST_PION 1
#define LAST_SOLDAT 0
// définis le dernier type de pion à être passé en disparu pour annuler un faux mouvement 
// via bool Last_Disp


/************* PARAMETRAGE INTERNE GENERAL **********/
#define INDIFFERENT_SOLDAT 6 // attention, doit rester le plus grand des état (pour le susperposition de couleur de case)
#define PION 0
#define FIRED 1
#define SECURITE 5
#define DANGER 2
#define ALERTE_RAS 3
#define ALERTE_DANGER 4

// définit le nombre de case autour du soldat in ZDS
#define DIM_ZDS 2 

//defini les types de pion 
#define PION_NORM 0    //tous les pion normaux
#define PION_SPE 1     // les enfant et femmes


/******************** LEDS *************/
#define LED_PIN 7 // pin pour led 


// couleur en GRB
#define BRIGHTNESS 250
#define FADE 240
#define COLOR_INDIF_SOLDAT ( 0, 0,255)
#define COLOR_PION (15,180,150)
#define COLOR_ALERTE (110,255,0)
#define COLOR_DANGER (0,255,0)
#define COLOR_FIRED (255,255,255)
#define COLOR_SECURITE (75,0,0)
#define COLOR_ZDS_SOLDAT (15,15,15) // attention, les numéro rgb doivent etre plus faible que l'un des rgb des état particulier


/* // battement sinus fired
bool BATT_FIRED = 1;
byte battement = 100;
*/




#endif
