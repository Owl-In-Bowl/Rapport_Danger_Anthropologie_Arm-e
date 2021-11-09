
#ifndef MY_DEFINE
#define MY_DEFINE
/**/
/*


la définition des lignes et colonne ne limitent pas vraisemblablement le nombre de case
cependant, pour les tableau de pointeur devant tranformer une position cartésienne en un entier
on limite le plateau à un 15 X 15
		==> sinon modifier les type de variable dans le programme (byte -> int)


*/


#define COLONNE 15    // définit le nombre de colonne
#define LIGNE 15     // définit le nombre de ligne

#define FREQ_PLATEAU  40
#define FREQ_LED 22



#define NB_SOLDAT 19 //21
#define NB_PION 9	//9
#define NB_BATIMENT 4//5
#define NB_ARBRE 4//3


#define TEMPS_ALERTE 1500      // en mS 
#define TEMPS_SECURITE 2500   // en mS 
#define TEMPS_DANGER 1800    // en mS 

#define proba_death 30     // % de proba que le pion meurt



/*********** MESURE VESTIAIRE *************/
#define READ_VEST 5    // lit le contact reed sur le vestaire
#define LED_VEST 4    // pilote la led du vestiaire




/*********** MESURE PLATEAU ***************/
#define  SHIFT_L 10  			 // pin 11 (SHIFT)- impression du contenu du registre
#define  DATA_IN_L 11    		// pin 14 (DATA_IN) - byte de lecture
#define  SHIFT_C 9			   // pin 11 (SHIFT)- impression du contenu du registre
#define  DATA_IN_C 8    	  // pin 14 (DATA_IN) - byte de lecture
#define  IMPRESSION 12       // pin 12 (IMPRESSION)- décalage du registre  

#define READ_PIN 6

/************ BLOCAGE DES PION *************/
#define LAST_PION 1
#define LAST_SOLDAT 0
// définis le dernier type de pion à être passé en disparu pour annuler un faux mouvement 
// via bool Last_Disp


/************* PARAMETRAGE INTERNE GENERAL **********/
#define INDIFFERENT_SOLDAT 8 // attention, doit rester le plus grand des état (pour le susperposition de couleur de case)
#define PION 0
#define FIRED 1
#define SECURITE 5
#define DANGER 2
#define ALERTE_RAS 3
#define ALERTE_DANGER 4
#define DISPARU 6    
#define DRAPEAU 7    //ajout FDLS-1.0

// définit le nombre de case autour du soldat in ZDS
#define DIM_ZDS 2 

//défini les fonction des soldats
#define GV 0
#define CDE 1
#define CDG 2
#define VAB 3
#define SOA 4
#define CDS 5


//defini les types de pion 
#define PION_NORM 8   	 //tous les pion normaux   // attention pion norm et pion spe
#define PION_SPE 9    	// les enfant et femmes
#define MUR 10		   // les murs / batiment
#define ARBRE 11	  // les ARBRES

#define Genre_Soldat 0 // pour les formatages de message lcd
#define Genre_Pion 1


#define BALAYAGE_NUL 0
#define BALAYAGE_SOLDAT 1
#define BALAYAGE_PION 2
#define BALAYAGE_SOL_PION 3


/******************** LEDS *************/
#define LED_PIN 7 // pin pour led 


// couleur en GRB
#define BRIGHTNESS 250
#define FADE 220
#define COLOR_INDIF_SOLDAT ( 0, 0,255)
#define COLOR_PION (18,180,150)
#define COLOR_ALERTE (110,255,0)
#define COLOR_DANGER (0,255,0)
#define COLOR_FIRED (255,255,255)
#define COLOR_SECURITE (75,0,0)
#define COLOR_ZDS_SOLDAT (15,15,15) // attention, les numéro rgb doivent etre plus faible que l'un des rgb des état particulier
#define COLOR_SAILLANCE (255,255,0)
#define RST_SAILLANCE (20,20,20)
#define COLOR_DRAPEAU (120,120,255) //ajout FDLS-1.0

/* // battement sinus fired
bool BATT_FIRED = 1;
byte battement = 100;
*/


/*********** POUR LE DEPLACEMENT AUTOMATIQUE DES PIONS **********/
#define PION_DEPLACEMENT 3  // patch FDLS-1.0

/*********** PRISE EN COMPTE DRAPEAU *********/
#define DIST_CAPT_DRAP 2    // patch FDLS-1.0


#endif
