#include "define_LEA_Projet_F.h"
#include "Pion_F.h"
#include "shift_register_F.h"
#include "soldat_F.h"

#include <freeRam.h>
#include <FastLED.h>
#include <LiquidCrystal_I2C.h>


#define delai_message_pres 1500 // le delai d'affichage de la présentation des classes officiels d'objets


bool fonc_aff_plat = 1;
float time_perf = 0;
bool debugMove = 1;


// défini l'ordre d'initialisation des fonctions
byte INITIALISATION_FONCTION [] = {GV, GV, GV, GV, CDE, CDE, CDG, VAB,
                                   GV, GV, GV, GV, CDE, CDE, CDG, VAB,
                                   SOA, CDS, VAB
                                  };


// tableau pour affichage de fonction et type de pion via LCD
String NAME_FONCTION[] = {"GV", "CDE", "CDG", "VAB", "SOA", "CDS"};
String NAME_PION[] = {"UN AVION", "UN HOMME", "UN CHIEN", "UNE VOITURE", " UN BALLON" , "UN HOMME", "UN VIRUS" , "UN VEHICULE BLINDE", "UNE FEMME+ENFANT"};

// défini les cases interne et externe de chaque zone de chaque fonction
byte ASSO_Fonction_zone[3][2] = {{1, 2}, {2, 3}, {3, 3}}; //GV  CDE  VAB  //(numero de fonction, interne, externe, balayer pion ou soldat)
//{{1,2},       //GV
//{2,3},       //CDE
//{3,3}};   //VAB

/************ AFFICHAGE LCD ************/
LiquidCrystal_I2C lcd(0x27, 20, 4);


/*------- SOMMAIRE --------

  1- Methode de dev
  a) gestion amélioration
  b) gestion des erreur

  2-declaration var
  a) plateau de jeu
  b) création des instances
  c) variable de run plateau

  3-programme
  a) setup
  b) loop

  4-fonction
  a) fonction de check plateau
    1) gestion de plateau
    2) disparition possible
    3) apparition possible
    4) divers

  b) transformation (mathématique)

  c) allumage des led

*/


/***************** METHODE DE DEV *******************
****************************************************

  ----------- GESTION DES AMELIORATIONS ------------
  // -A- réinitialiser les tampon ailleurs pour faire un test erreur manip joueur
  // -B- Non finis

  // ajouter un systeme de blocage avec une boucle while si leseux tampon de pion sont non nul
  // car cela implique d'avoir enlevé 2 pièces du plateaux et permettre de reposer la dernière pièce


  //Serial.switch  --> toutes les lignes Serial posant problème



  ------------- GESTION DES ERREUR ----------------
  //ERREUR POINTEUR 1 , 2
  //patch 2
  patch2 debugVisuel

  patch3A enlevement de la saillance lorsque qu'un soldat disparait (version new state dans soldat.etat)
  
  patch FDLS-1.0 -> ajouter le drapeau (init drapeau, check drapeau, fin de jeu)
      --> le patch part du principe que la figurine du drapeau possède un aimant 
      --> mais c'est pas nécéssaire et ce serai plus simple qu'il n'y en ai pas
*/


/***************** DECLARATION VAR ******************
****************************************************/

/**---------------   PLATEAU DE JEU ------------------*/
Pion* pPlat_Pion [COLONNE * LIGNE] = {NULL};
Soldat* pPlat_Soldat [COLONNE * LIGNE] = {NULL};


/**--------------- CREATION DES INSTANCES ---------------*/
Pion* pList_Pion [NB_PION + NB_BATIMENT + NB_ARBRE];
Soldat* pList_Soldat[NB_SOLDAT];

/*--------------- TABLE ASSOCIATION DE GROUPE -----------*/
byte table_association[NB_PION + NB_ARBRE + NB_BATIMENT][NB_SOLDAT]; //patch2 passé de NBpion , à la somme de tous les pions

/*--------------- VARIABLE DE RUN PLATEAU ---------------*/
byte read_l = 0;
byte read_c = 0;

Shift_register  shift;

Pion* pPion_Disparu = NULL;
Soldat* pSoldat_Disparu = NULL;

unsigned long tempo_check_plate = 0;
unsigned long DELAI_PLATEAU = 0;

CRGB leds[COLONNE * LIGNE + 1]; // -> inutile : la plus 1 sert de tampon pour comparer les couleurs de chaque case -> éviter d'écraser une case en statut particulier (secu, alerte, danger) par la zds d'un autre soldat
unsigned long tempo_raf_led = 0;
unsigned long DELAI_LED = 0;

/********************* DRAPEAU ********************/  // patch FDLS-1.0
class Drapeau {
  private:
  byte drapl;
  byte drapc;

  public:
  void Setup();
  bool check_drapeau(byte _col, byte _ligne);
  
  byte get_l();
  byte get_c();
};

void Drapeau::Setup(){
  drapc=random(0,COLONNE);
  drapl=random(0,LIGNE);
}

bool Drapeau::check_drapeau(byte _col, byte _ligne){
  if ( _col >= drapc - DIST_CAPT_DRAP  && _col <= drapc+DIST_CAPT_DRAP){
    if (_ligne >= drapl - DIST_CAPT_DRAP && _ligne <= drapl - DIST_CAPT_DRAP) {
      return 1;
    }
  }
  return 0;
}

byte Drapeau::get_l(){  return drapl;}

byte Drapeau::get_c(){  return drapc;}

Drapeau drapeau;

/**************************************************/
/*********************  PROGRAMME *****************/
/**************************************************/



void setup() {
  pinMode(13, OUTPUT);
  digitalWrite (13, HIGH);

  //  Serial.begin (115200);
  Serial.println("Début prog ");

  DELAI_PLATEAU = 1000000 / (FREQ_PLATEAU * COLONNE * LIGNE); // en uS
  DELAI_LED = 1000 / FREQ_LED;                        // en mS

  Serial.print("delai led"); Serial.println(DELAI_LED);
  Serial.print("delai plat"); Serial.println(DELAI_PLATEAU);

  int ARG_LED = COLONNE * LIGNE;
  LEDS.addLeds<WS2812, LED_PIN, RGB>(leds, ARG_LED);
  LEDS.setBrightness(BRIGHTNESS);
  for (byte stamp = 0; stamp != COLONNE * LIGNE + 1; stamp++) {
    leds[stamp] = CRGB (0, 0, 0);
  }
  LEDS.show();

  pinMode(READ_VEST, INPUT);
  pinMode(LED_VEST, OUTPUT);

  shift.init();
  LCD_Bienvenue();
  raf_Led();
  
  lcd.clear();  
  drapeau.Setup(); // patch FDLS-1.0
  raf_Led();
  lpr(0, "veuillez placer");
  lpr(1,"le drapeau sur");
  lpr(2,"la case bleu");

  while(  read_l!=drapeau.get_l()  ||  read_c != drapeau.get_c()  ||  !shift.read()  ){
    decalage_plateau();
  }
    
    


  digitalWrite(13, LOW);
  /*if (1){  // Création des instance Pion & Soldats
    // maintenant soldat est généré au fur et à mesure de l'ajout des pièces
    for (byte i=0; i!=NB_PION+NB_BATIMENT+NB_ARBRE ; i++) {
      pList_Pion [i] = new Pion();
    }

    for (byte x=0; x!=NB_SOLDAT ; x++){
      pList_Soldat [x]= new Soldat(x);
    }


    }
  */
  if (1) {  // Phase d'ajout de pièce sur le plateau

    LCD_Pres_Soldat();
    for (byte soldiers = 0; soldiers != NB_SOLDAT; soldiers++) {
      aff_etat();
      pList_Soldat[soldiers] = new Soldat(soldiers, INITIALISATION_FONCTION[soldiers]);

      LCD_demande_vestiaire(INITIALISATION_FONCTION[soldiers]);
      while (!vestiaire_check()) ;
      LCD_envoi_operation(INITIALISATION_FONCTION[soldiers]);

      while ( (!shift.read()) || (pPlat_Soldat[trans_num(read_c, read_l)] != NULL) ) {  // si soldat connu sur la case, ou case vide
        decalage_plateau();  // si il n'y a pas soldat a cette position, et que la case mesuré renvoi un résulat nul, case suivante
        vestiaire_check();
      }
      pPlat_Soldat[trans_num(read_c, read_l)] = pList_Soldat[soldiers];
      pList_Soldat[soldiers] -> set_position(read_c, read_l);
      raf_Led();
      delay (2);
    }
    vestiaire_check();
    raf_Led();


    LCD_Pres_Artefact();
    for (byte pions = 0; pions != NB_PION; pions++) {
      //Serial.print("artefact n°"); Serial.println(pions);
      //aff_etat();

      raf_Led();
      LCD_Demande_Pion(pions);
      pList_Pion [pions] = new Pion();

      while (  (!shift.read()) || (pPlat_Pion[trans_num(read_c, read_l)] != NULL) || (pPlat_Soldat[trans_num(read_c, read_l)] != NULL) ) {
        decalage_plateau();  // tant qu'on ne capte pas de pièce, ou qu'un soldat est déja présent sur la case, ou qu'un pion est déja présent sur la case
      }
      //while (! ( (shift.read()) && (!pPlat_Pion[trans_num(read_c, read_l)]) &&(!pPlat_Soldat[trans_num(read_c,read_l)]) )) {decalage_plateau(); }
      pPlat_Pion[trans_num(read_c, read_l)] = pList_Pion[pions];
      pList_Pion[pions] -> set_position(read_c, read_l);
      delay(2);
    }
    raf_Led();


    LCD_Pres_Decor();
    for (byte decor = NB_PION; decor != NB_PION + NB_BATIMENT + NB_ARBRE; decor++) {
      //Serial.print("artefact n°"); Serial.println(decor);
      //aff_etat();

      raf_Led();
      pList_Pion [decor] = new Pion();
      LCD_Demande_Pion(decor);

      while (! (( shift.read() ) && (!pPlat_Pion[trans_num(read_c, read_l)]) && (!pPlat_Soldat[trans_num(read_c, read_l)])))  {
        decalage_plateau();
      }
      pPlat_Pion[trans_num(read_c, read_l)] = pList_Pion[decor];
      pList_Pion[decor] -> set_position(read_c, read_l);
      delay(2);
    }
  }
  raf_Led();

  team_management();
  aff_etat();

  lcd.clear();
  lpr(1, "debut du jeu");
  delay (delai_message_pres);

  //initialisation du tableau d'association pion x groupe
  for (byte pio = 0; pio != NB_PION + NB_ARBRE + NB_BATIMENT; pio++) { //patch2 correction du nombre de pion
    for (byte sold = 0; sold != NB_SOLDAT; sold++) {
      table_association[pio][sold] = 0; // les groupe commencent à 1
    }
  }

  tempo_check_plate = micros();
  tempo_raf_led = millis();
}

void loop() {
  if (tempo_check_plate + DELAI_PLATEAU < micros()) { // Phase de mesure plateau
    tempo_check_plate = micros();
    check_plate();
  }

  raf_soldat();

  if (tempo_raf_led + DELAI_LED <= millis()) { // Raffraichissement des led
    raf_Led();
    tempo_raf_led = millis();
  }


}


/********* TEST */
void perf_init() {
  time_perf = micros();
}

void perf_pr(String affichage) {
  float xx_style = micros() - time_perf;
  Serial.print(affichage); Serial.println(xx_style);
}


void aff_etat() {
  if (fonc_aff_plat == 1) {
    Serial.println("**************************");
    Serial.println("SOLDAT :");
    for (byte d = 0; d != LIGNE; d++) {
      for (byte f = 0; f != COLONNE; f++) {
        if (pPlat_Soldat[trans_num(f, d)]) {
          Serial.print (pPlat_Soldat[trans_num(f, d)] -> get_ID()); Serial.print(" ");
        }
        else Serial.print ("x "); //Serial.switch
      }
      Serial.println();
    }

    Serial.println();
    Serial.println("PION :");
    for (byte d = 0; d != LIGNE; d++) {
      for (byte f = 0; f != COLONNE; f++) {
        if (pPlat_Pion[trans_num(f, d)]) {
          Serial.print (pPlat_Pion[trans_num(f, d)] -> get_ID()); Serial.print(" ");
        }
        else Serial.print ("x ");  //Serial.switch
      }
      Serial.println();
    }
    Serial.println();

    /* //Serial.println ("SOLDAT : ");
      for (byte m=0; m != NB_SOLDAT; m++) {
      //Serial.print(m); //Serial.print (" || ETAT : "); //Serial.print(pList_Soldat[m]->get_state());//Serial.print(" || GRPE : "); //Serial.println(pList_Soldat[m]->get_groupe());
      //    //Serial.print (" /  LIGNE : "); //Serial.println(pList_Soldat[m]->get_ligne());
      }
      //Serial.println();
      //Serial.println("PION : ");
      for (byte z=0; z != Pion::nbr_pion; z++) {
       //Serial.print(z); //Serial.print ( " || Col : "); //Serial.print (pList_Pion[z] -> get_col()); //Serial.print(" || Ligne : ");//Serial.print (pList_Pion[z] -> get_ligne()); //Serial.print(" || Type : "); //Serial.println (pList_Pion[z] -> get_type());
      }
    */
  }
}

/***********************************************/
/**************** FONCTION *********************/
/***********************************************/

/*--------------- FONCTION DE CHECK PLATEAU ---------------*/
/*---------------- gestion de plateau--------------------*/
void check_plate() {
  bool etat_mesure = shift.read();

  if ((etat_mesure == 0) ) {      // disparition possible
    Disparition();
  }

  else if (etat_mesure == 1) { // apparition possible
    Apparition();
  }

  decalage_plateau();  // on prépare la prochaine mesure
}

/*------------------------------------------------------------*/
void decalage_plateau() {
  read_c += 1;
  if (read_c >= COLONNE) {
    shift.decalage_colonne(1);
    read_c = 0;
    read_l += 1;
    if (read_l >= LIGNE) {
      shift.decalage_ligne(1);
      read_l = 0;
    }
    else shift.decalage_ligne(0);
  }
  else shift.decalage_colonne(0);
}


/*-----------Disparitio possible ------------------*/
void Disparition() {
  /*      // on check si il y un soldat sur le plateau à la position indiqué
          // si oui alors il a disparu
              // pointeur tampon soldat -> copy pointeur de la case
              // pointeur dans la case devient null
              // le pointeur tampon du pion devient null
          // si non suivant



      // On check si il y un soldat sur le plateau à la position indiqué
          // si oui alors il a disparu
              // pointeur tampon Pion -> copy pointeur de la case
              // pointeur dans la case devient null
              // le pointeur tampon du Soldat devient null
          // si non suivant


      // Pas de pion disparu si Tampon_Soldat=Tampon_Pion=NULL

  */
  if ((pPlat_Soldat[trans_num(read_c, read_l)]) != NULL || (pPlat_Pion[trans_num(read_c, read_l)]) != NULL ) {
    Serial.println();
    Serial.println("Disparition");

    if (pPion_Disparu != NULL || pSoldat_Disparu != NULL) { // erreur de manip
      //erreur de manip
      //afficher l'erreur
      lcd.clear();
      lpr(0, "Erreur de manip :");
      lpr(1, "2 elements retires");
      lpr(2, "replacer la piece :");

      if (pPlat_Soldat[trans_num(read_c, read_l)] != NULL) {
        lpr(3, NAME_FONCTION[pPlat_Soldat[trans_num(read_c, read_l)]->get_fonction()]);
      }

      else if (pPlat_Pion[trans_num(read_c, read_l)] != NULL) {
        if (pPlat_Pion[trans_num(read_c, read_l)]-> get_type() == PION_NORM || pPlat_Pion[trans_num(read_c, read_l)]-> get_type() == PION_SPE) {
          lpr(3, NAME_PION[pPlat_Pion[trans_num(read_c, read_l)]-> get_ID()]);
        }

        else if (pPlat_Pion[trans_num(read_c, read_l)] -> get_type() == MUR) {
          lpr(3, "BATIMENT");
        }

        else if (pPlat_Pion[trans_num(read_c, read_l)] ->get_type() == ARBRE) {
          lpr(3, "ARBRE");
        }
      }

      LEDS.clear();
      leds[trans_num(read_c, read_l)] = CRGB (200, 200, 200);
      FastLED.show();

      while (!shift.read());

      lcd.clear();
      lpr(1, "Reprise du jeu");
      // problème résolu
    }

    else if (pPlat_Soldat[trans_num(read_c, read_l)] != NULL) {
      pSoldat_Disparu = pPlat_Soldat[trans_num(read_c, read_l)];
      pSoldat_Disparu->set_state(DISPARU);//patch3A
      pPlat_Soldat[trans_num(read_c, read_l)] = NULL;
      //pPion_Disparu=NULL;
      Serial.println("==> DISP SOLDAT");
      //              aff_etat(); //TEST
    }

    else if (pPlat_Pion[trans_num(read_c, read_l)] != NULL ) {
      pPion_Disparu = pPlat_Pion[trans_num(read_c, read_l)];
      pPlat_Pion[trans_num(read_c, read_l)] = NULL;
      if (pPion_Disparu->check_fired()) {
        destruction_pion();
        Serial.println("Pion détruit ");
      }
      else {
        //pSoldat_Disparu=NULL;
        Serial.println("==> DISP PION");
        //              aff_etat(); //TEST
      }
    }
    aff_etat();
  }

}

/*---------- Apparition possible ------------------*/
void Apparition () {
  if ((pPion_Disparu ) && (pSoldat_Disparu )) { //ERREUR POINTEUR
    lcd.clear();
    lpr(1, "bug programme :");
    lpr(2, "double apparition");
    lpr(3, "veuillez redemarrer");
  }

  else if ((pPion_Disparu ) && (!pPlat_Pion [trans_num(read_c, read_l)] ) && (!pPlat_Soldat[trans_num(read_c, read_l)]) && (read_c != drapeau.get_c()) && (read_l != drapeau.get_l()) ) {  // PION APPARU  // patch FDLS-1.0
    pPlat_Pion[trans_num(read_c, read_l)] = pPion_Disparu;
    pPion_Disparu->set_position(read_c, read_l);
    apparition_Pion();
    pPion_Disparu = NULL; // -A-
    aff_etat(); //TEST
  }

  else if ((pSoldat_Disparu ) && (!pPlat_Soldat[trans_num(read_c, read_l)] ) && (!pPlat_Pion[trans_num(read_c, read_l)]) && read_c != drapeau.get_c() && read_l != drapeau.get_l()) { // SOLDAT APPARU //patch FDLS-1.0
    pPlat_Soldat[trans_num(read_c, read_l)] = pSoldat_Disparu;
    pSoldat_Disparu->set_position(read_c, read_l);
    apparition_Soldat();
    pSoldat_Disparu = NULL; // -A-
    aff_etat(); // TEST
  }

}

/*------------------------------------------------------------*/
void apparition_Soldat() {
  // on applique les recherches au point stocké en mémoire
  Serial.println(); Serial.println("==> APPA SOLDAT");
  pSoldat_Disparu->set_state(INDIFFERENT_SOLDAT);
  
  for (byte i = 0; i != NB_SOLDAT; i++) { // ON cherche si on entre sur le territoire d'un soldat + check drapeau
    if (drapeau.check_drapeau(pList_Soldat[i]->get_col(), pList_Soldat[i]->get_ligne())){  //patch FDLS-1.0
      fin_de_partie();
    }
    if ((pList_Soldat[i] != pSoldat_Disparu) && (pList_Soldat[i]->ask_presence(pSoldat_Disparu->get_col(), pSoldat_Disparu->get_ligne(), PION_NORM))) {
      pSoldat_Disparu->set_state(SECURITE); //Serial.print("Arrive chez le soldat : ");//Serial.println(i);
      break;
    }
  }

  if (pSoldat_Disparu->get_state() != SECURITE) { // si on est pas entre sur le territoire d'un soldat, alors on test avec les pions
    for (byte y = 0; y != Pion::nbr_pion; y++) {
      if (pSoldat_Disparu->ask_presence(pList_Pion[y]->get_col(), pList_Pion[y]->get_ligne(), PION_NORM)) {
        pSoldat_Disparu->set_state(SECURITE);
        break;
      }
    }
  }

  team_management();
  
  // mettre la team du pion disparu dans son etat
  for (byte df = 0; df != NB_SOLDAT; df++) {
    if (pList_Soldat[df]->get_groupe() == pSoldat_Disparu->get_groupe()) pList_Soldat[df]->set_state(pSoldat_Disparu->get_state());
  }


  // vérifier si les soldat
}

/*------------------------------------------------------------*/
void apparition_Pion() {

  Serial.println(" ==> APPARITION PION");

  // on balaye la ligne du pion enregistré dans association_groupe on vérifie que 1 soldats de chaque groupe est toujours dans les parage
  // sinon on enleve le groupe

  for (byte regy = 0; regy != NB_SOLDAT ; regy ++) { // vérifie si les association pion et groupes sont toujours justifiés
    if (table_association[pPion_Disparu->get_ID()][regy] != 0) {  // si un groupe[regy] est associé au pion
      byte gaunter = 0;
      bool continuation = 1;

      while (continuation) {
        if (pList_Soldat[gaunter]->get_groupe() == table_association [pPion_Disparu->get_ID()][regy]) {
          if (pList_Soldat[gaunter]-> ask_presence(pPion_Disparu-> get_col(), pPion_Disparu->get_ligne(), PION_NORM) ) continuation = 0; // le pion est toujours dans la zone d'au moins un representant du groupe, donc on passe au groupe associé au pion suivant
        }
        
        if ( (continuation == 1) && (gaunter == NB_SOLDAT - 1) ) { // si un représentant du groupe est dans la zone, ou qu'on a balayé tous les oldats
          continuation = 0;  // si on a balaye tous les soldat, et que le pion n'est plus dans la zds d'au moins un membre du groupe checké ==> le l'association du groupe au pion est effacé
          table_association[pPion_Disparu-> get_ID()][regy] = 0;  // cette instruction découlant du test de gaunter est biaisé, mais fonctionnent, il es peu probable vir impossible d'avoir autant de groupe que de soldats (surface du plateau X nombre de soldats)
        }
        gaunter ++;
      }
    }
  }

  /*    // ON lance les dé pour fired or not fired
      // on check si le pion rentre sur la zone d'un soldat
          // on stock le groupe de chaque soldat qui est concerné
      // On change l'état des groupe concerné en alerte, et on stock le temps de changement
      // change le temps du pion*/


  byte tirage = random (0, 100);
  bool dee_fired = 0;
  if (tirage < proba_death) dee_fired = 1;

  Serial.print("fired ? "); Serial.println(dee_fired);
  // on utilise un tampon pour connaitre les groupes des soldat auxquel le pion vient de pénetrer dans la zds
  // le tampon est ensuite utilisé pour mettre tous les groupes concernés dans le même état (alerte ras ou alerte danger, ou sécurité)
  byte tampon_groupe[NB_SOLDAT] = {0};
  for (byte xi = 0; xi != NB_SOLDAT; xi ++) {
    tampon_groupe[xi] = NB_SOLDAT + 1; // plus 1 puisque les groupes commencent à 1
  }

  // on cherche les soldat qui rentre en alerte et on recupère leur groupe
  for (byte z = 0; z != NB_SOLDAT; z++) { // on balaye la liste de soldat
    
    // on recalibre le type du pion à cause des batiments, mur, pion norm, pion spé // on map vers pion_norm ou pion_spe
    byte pionType = pPion_Disparu->get_type();
    if (pionType == MUR || pionType == ARBRE) pionType = PION_NORM;
    
    if (pList_Soldat[z]->ask_presence(pPion_Disparu->get_col() , pPion_Disparu->get_ligne(), pionType)) {//on leur demande si le pion est dans leur zone
      bool continuer = 1;
      byte v = 0;

      // si le pion est déja associé au groupe, alors il ne faut pas effectuer les laus suivant (continuer =0)
      // cela evite de rentrer le pion dans le tampon des groupes à modifier/ interagir

      for (byte sneakers = 0; sneakers != NB_SOLDAT; sneakers++) {
        if (pList_Soldat[z]->get_groupe() == table_association[pPion_Disparu->get_ID()][sneakers]) {
          continuer = 0;
        }
      }

      while (continuer) {
        pPion_Disparu->set_fired(dee_fired); // le pion connait son état si jamais on modifie le tampon groupe (et donc alerte)
        if (tampon_groupe[v] == pList_Soldat[z]->get_groupe()) continuer = 0; // on regarde si le groupe du soldat est déja dans le tampon -> plus besoin de continuer pour ce soldat
        else if (tampon_groupe[v] == NB_SOLDAT + 1) {
          tampon_groupe[v] = pList_Soldat[z]->get_groupe();  // autrement, si la case du tampon est vierge, on rentre le groupe du soldat
          continuer = 0;
        }
        if (v == NB_SOLDAT - 1) continuer = 0;
        v++;
      }
    }
  }
  /*
      //Serial.println( " ESSAI DU TAMPON DE GROUPE");
      for (byte dress=0; dress!=NB_SOLDAT; dress++){
        //Serial.print(tampon_groupe[dress]);//Serial.print (" ");
      }
     //Serial.println( );
  */

  //maintenant qu'on connait les groupe et l'issue de l'alerte, on va stocker cette issue dans les membre des groupe, et
  // et rentrer l'horloge aux soldat pour qu'ils puissent se reset plus tard (fait dans la méthode de classe)
  byte compteur = 0;
  while ( (compteur < NB_SOLDAT) && (tampon_groupe[compteur] != NB_SOLDAT + 1) ) {
    for (byte d = 0; d != NB_SOLDAT; d++) {
      if (pList_Soldat[d]->get_groupe() == tampon_groupe[compteur]) {
        if (dee_fired == 1)pList_Soldat[d]->set_state(ALERTE_DANGER); // si fired
        else pList_Soldat[d]->set_state(ALERTE_RAS);  // si non fired
      }
    }
    compteur++;
  }

  // on actualise le tableau association groupe avec les nouvelles valeurs
  // on regarde pour le pion (id) si les valeurs du tampon groupe sont a ajouter au groupe (sur un emplacement avec groupe nul)
  // pour chaque emplacement dans tampon_groupe, on balaye association_groupe pour le rentrer dans un espace vierge

  for (byte grids = 0; grids != NB_SOLDAT; grids ++) {
    bool stocke = 0;
    if (tampon_groupe[grids] == NB_SOLDAT + 1) break;

    for (byte degs = 0; degs != NB_SOLDAT; degs++) {
      if ((stocke == 0) && (tampon_groupe[grids] != NB_SOLDAT + 1) && (table_association[pPion_Disparu-> get_ID()][degs] == 0)) {
        table_association[pPion_Disparu-> get_ID()][degs] = tampon_groupe[grids]; stocke = 1;
      }
    }
  }

  // debugage de la table asso pion
  Serial.println( "table association du pion apparu -> après actualisation des nouveau groupes");
  for (byte drone = 0; drone != NB_SOLDAT; drone++) {
    Serial.print(table_association [pPion_Disparu->get_ID()][drone]); //Serial.print(" ");
  }
  Serial.println();
  Serial.println();

}

/*-------------------divers-----------------------*/

/*------------------------------------------------------------*/
void team_management() {  // constitue les groupe de soldats
  Serial.println(" Team management ");
  //perf_init();
  byte groupe = 0;
  for (byte tf = 0; tf != NB_SOLDAT; tf++) {
    pList_Soldat[tf]-> set_groupe(0);
    pList_Soldat[tf]-> set_checked(0);
  }

  for (byte s = 0; s != NB_SOLDAT; s++) {         // balaye tous les soldats
    if (!pList_Soldat[s]->get_checked()) {      // si le soldat n'a pas été checké
      groupe++;                 // on incremente le groupe
      pList_Soldat[s]-> set_groupe(groupe);   // on lui associe le groupe actuel
      bool continue_while = 1;
      while (continue_while) {
        byte compteur = 0;
        continue_while = 0;
        while ((compteur < NB_SOLDAT)) { // on cherche un soldat du groupe non checké
          if ((pList_Soldat[compteur]->get_groupe() == groupe ) && (!pList_Soldat[compteur]->get_checked())) {
            continue_while = 1;
            break;
          }
          compteur++;
        }
        if (continue_while == 1) {
          for (byte p = 0;  p != NB_SOLDAT; p++) { // et on vérifie si les autre soldats sont dans sa zone
            if (pList_Soldat[compteur]->ask_presence(pList_Soldat[p]->get_col(), pList_Soldat[p]->get_ligne(), PION_NORM)) { // si oui
              pList_Soldat[p]->set_groupe(groupe);
            }
          }  //alors ce soldat entre dans le groupe
          pList_Soldat[compteur]-> set_checked(1);
        }
      }
    }
  }
  //perf_pr("team mana");
}

/*------------------------------------------------------------*/
void raf_soldat() {
  for (byte dies = 0; dies != NB_SOLDAT; dies++) {
    pList_Soldat[dies]->check_state();
  }
}

/*-------------------------------------------------------------*/
void destruction_pion() {
  //check de liste
  for (byte pero = 0; pero != Pion::nbr_pion; pero++) {
    if (pList_Pion[pero] == pPion_Disparu) {
      for (byte restant = pero; restant != Pion::nbr_pion - 1; restant++) { // -1 parce que le l'instance pas encore détruite ( or elle faisant baisser nbr_pion--)
        pList_Pion[restant] = pList_Pion[restant + 1];
      }
    }
  }
  delete pPion_Disparu;
  pPion_Disparu = NULL;
}

/************ TRANSFORMATION ************/

byte trans_num(byte a_col, byte a_ligne) {  // transforme une position 2D en 1D
  byte num = 0;
  num = (a_ligne) * COLONNE + a_col;
  return num;
}

byte trans_col(byte a_num) { // transforme une position 1D en 2D (colonne)
  byte reste = a_num;
  while (reste >= COLONNE) {
    reste = reste - COLONNE;
  }
  return reste;
}

byte trans_ligne(byte a_num) { // transforme une position 1D en 2D (ligne)
  byte compteur = 0;
  byte reste = a_num;
  while (reste >= COLONNE) {
    reste = reste - COLONNE;
    compteur++;
  }
  return compteur;
}


/******************** SAILLANCE **********************/
void check_saillance(byte ID) {
  byte scol = pList_Soldat[ID] -> get_col();
  byte slin = pList_Soldat[ID] -> get_ligne();

  switch (pList_Soldat[ID]->get_fonction()) {

    case GV:
      //leds[trans_num(scol, slin)] = CRGB (255,255,255);  //patch2 debugVisuel
      if (cherche_environnement(scol, slin, ASSO_Fonction_zone[0][0], ASSO_Fonction_zone[0][1])) allum_sold_saill(ID);
      //delay(300);
      //leds[trans_num(scol, slin)] = CRGB COLOR_INDIF_SOLDAT;  // patch2 debugVisuel
      break;

    case CDE:
      if (cherche_environnement (scol, slin, ASSO_Fonction_zone[1][0], ASSO_Fonction_zone[1][1])) {
        allum_sold_saill(ID);
      }
      if (cherche_soldat(ID)) allum_sold_saill(ID);
      break;

    case VAB:
      if (cherche_environnement (scol, slin, ASSO_Fonction_zone[2][0], ASSO_Fonction_zone[2][1])) {
        allum_sold_saill(ID);
      }
      else allum_sold_saill(ID);
      break;

    case CDG:
      if (cherche_soldat(ID)) allum_sold_saill(ID);
      if (Soldat::nbr_soldat > ID) allum_sold_saill(ID);  // quand le vab (id cdg+1)est posé saillance permanente
      break;


    case SOA:
      //      bool tamps=0; patch2
      //      tamps =
      cherche_soldat(ID);
      allum_sold_saill(ID);
      break;

    /*case Radio:   // retiré de la nouvelle version
      allum_sold_saill(ID);
      // afficher chef de section
      break;
    */

    case CDS:
      allum_sold_saill(ID);
      break;

      /*case CDU:   // retire de la nouvelle version
        allum_sold_saill(ID);
        break;
      */
  }
}

void allum_sold_saill(byte ID) {
  leds[trans_num(pList_Soldat[ID]->get_col(), pList_Soldat[ID]->get_ligne())] = CRGB COLOR_SAILLANCE;
}

bool cherche_environnement (byte col, byte li, byte interne, byte externe) {
  bool is_saill = 0;

  signed int coloc = col - externe;
  if (coloc < 0) coloc = 0;
  byte fin_coloc = col + externe;
  if (fin_coloc >= COLONNE) fin_coloc = COLONNE - 1;

  while (coloc != fin_coloc + 1) {
    signed int liness = li - externe;
    if (liness < 0) liness = 0;
    byte fin_liness = li + externe;
    if (fin_liness >= LIGNE) fin_liness = LIGNE - 1;

    while (liness != fin_liness + 1) {
      if (  !((coloc > col - interne && coloc < col + interne) && (liness > li - interne && liness < li + interne)) ) {
        if (pPlat_Pion[trans_num(coloc, liness)] != NULL && pPlat_Pion[trans_num(coloc, liness)]->get_ID() >= NB_PION && pPlat_Pion[trans_num(coloc, liness)]->check_fired() != 1) {
          leds[trans_num(coloc, liness)] = CRGB COLOR_SAILLANCE;
          is_saill = 1;
        }
      }
      liness++;
    }
    coloc++;
  }
  return is_saill;
}

bool cherche_soldat(byte ID) {
  bool back = 0;

  switch (pList_Soldat[ID]->get_fonction()) {
    case SOA:
      //balayer les babos du coin
      for (byte human = 0; human < NB_SOLDAT; human++) {
        if ( pList_Soldat[human]->ask_presence(pList_Soldat[ID]->get_col(), pList_Soldat[ID]->get_ligne(), PION_NORM) && human != ID) {
          if (pList_Soldat[human]->get_state() != DISPARU) {  //patch3A
            // on allume le soldat
            leds[trans_num(pList_Soldat[human]->get_col(), pList_Soldat[human]->get_ligne())] = CRGB COLOR_SAILLANCE;

            // on retourne le resulat
            back = 1;
          }
        }
      }

      break;

    case CDE:
      for (byte steack = 0; steack < NB_SOLDAT; steack++) {
        if ( pList_Soldat[steack]->get_fonction() == GV ) { // si le soldat balayé est un GV
          if ( pList_Soldat[steack]->ask_presence(pList_Soldat[ID]->get_col(), pList_Soldat[ID]->get_ligne(), PION_NORM) && pList_Soldat[steack]->get_state() != DISPARU) { // si le GV est présent  patch3A
            //allum soldat
            leds[trans_num(pList_Soldat[steack]->get_col() , pList_Soldat[steack]->get_ligne())] = CRGB COLOR_SAILLANCE;

            //returne 1 pour allumer le CDE
            back = 1;
          }
        }
      }
      break;

    case CDG:
      for (byte stack = 0; stack < NB_SOLDAT; stack++) {
        if ( pList_Soldat[stack]->get_fonction() == CDE && pList_Soldat[stack]->ask_presence(pList_Soldat[ID]->get_col(), pList_Soldat[ID]->get_ligne(), PION_NORM) && pList_Soldat[stack]->get_state() != DISPARU) { // est GV et preseny  patch3A
          //allum soldat
          leds[trans_num(pList_Soldat[stack]->get_col() , pList_Soldat[stack]->get_ligne())] = CRGB COLOR_SAILLANCE;
          //returne 1
          back = 1;
        }
      }
      break;

  }
  return back;
}


bool vestiaire_check() {
  bool state = digitalRead(READ_VEST);
  digitalWrite(LED_VEST, state);
  return state;
}

/***************  ALLUMAGE DES LED *******************/
void raf_Led () {
  fadeall();

  // On colorise les PIONS
  for (byte n = 0; n != LIGNE * COLONNE; n++) {
    if (pPlat_Pion[n] != NULL) set_color(pPlat_Pion[n]->get_col(), pPlat_Pion[n]->get_ligne(), pPlat_Pion[n]->check_fired());
  }


  // on colorise les   #SOLDATS INDIFFERENT#   d'abord
  for (byte smite = 0; smite != LIGNE * COLONNE; smite++) {
    if (pPlat_Soldat[smite] != NULL) {
      if (pPlat_Soldat[smite]->get_state() == INDIFFERENT_SOLDAT) set_color(pPlat_Soldat[smite]->get_col(), pPlat_Soldat[smite]->get_ligne(), pPlat_Soldat[smite]->get_state());
    }
  }


  // on rafraichi les   #SAILLANCE#   maintenant pour s'affranchir de l'ordre de lecture du plateau (écrase une saillance par un COLOR_INDIFF) - patch2
  for (byte smi = 0; smi != NB_SOLDAT; smi++) {
    if (pList_Soldat[smi] != NULL) {
      if (pList_Soldat[smi]->get_state() == INDIFFERENT_SOLDAT) check_saillance (smi);
    }
  }


  // cela permet d'écraser la couleur des zones affichant de l'indiffernt par des zones afficgant
  // un etat #SOLDAT SPECIALE#
  for (byte l = 0; l != LIGNE * COLONNE; l++) {
    if (pPlat_Soldat[l] != NULL) {
      if (pPlat_Soldat[l]->get_state() != INDIFFERENT_SOLDAT) set_color(pPlat_Soldat[l]->get_col(), pPlat_Soldat[l]->get_ligne(), pPlat_Soldat[l]->get_state());
    }
  }

  //leds[trans_num(read_c, read_l)]=CRGB (200,200,200);
  
  set_color(drapeau.get_c(), drapeau.get_l(), DRAPEAU);   //patch FDLS-1.0
  
  FastLED.show();
}

void set_color(byte x_col, byte x_ligne, byte x_state) {
  if (x_state == ALERTE_RAS) x_state = ALERTE_DANGER;

  switch (x_state) {
    case INDIFFERENT_SOLDAT:
      led_zds (x_col , x_ligne, INDIFFERENT_SOLDAT);
      leds[trans_num(x_col, x_ligne)] = CRGB COLOR_INDIF_SOLDAT;
      break;

    case FIRED:
      leds[trans_num(x_col, x_ligne)] = CRGB COLOR_FIRED;
      break;

    case PION:
      leds[trans_num(x_col, x_ligne)] = CRGB COLOR_PION;
      break;

    case SECURITE:
      led_zds(x_col, x_ligne, SECURITE);
      break;

    case DANGER:
      led_zds(x_col, x_ligne, DANGER);
      break;

    case ALERTE_DANGER:    // patch FDLS-1.0
      led_zds(x_col, x_ligne, ALERTE_DANGER);
      break;

    case DRAPEAU:
      leds[trans_num(x_col, x_ligne)] = CRGB COLOR_DRAPEAU;
      break;
  }
}

void led_zds(byte c, byte l, byte etat) {
  /*for (int z=c-DIM_ZDS; z<= c+DIM_ZDS; z++) {
    byte cols = 0;
    if (z>=COLONNE) {cols=COLONNE-1;break;}
    else if (z<0) cols=0;

    for (int w=l-DIM_ZDS; w<= l+DIM_ZDS; w++){
      byte lines=0;
      if (w>= LIGNE) {lines=LIGNE-1; break;}
      else if (w<0) lines=0;*/
  signed int cols = c - DIM_ZDS;
  if (cols < 0) cols = 0;

  byte fin_cols = c + DIM_ZDS;
  if (fin_cols >= COLONNE) fin_cols = COLONNE - 1;



  while (cols != fin_cols + 1) {
    signed int lines = l - DIM_ZDS;
    if (lines < 0) lines = 0;

    byte fin_lines = l + DIM_ZDS;
    if (fin_lines >= LIGNE) fin_lines = LIGNE - 1;


    while (lines != fin_lines + 1) {
      switch (etat) {
        case INDIFFERENT_SOLDAT:
          //          perf_init();
          if ((pPlat_Pion[trans_num(cols, lines)] == NULL) && (pPlat_Soldat[trans_num(cols, lines)] == NULL) ) { // si il n'y a pas de pion ou de soldat sur la case
            //leds[COLONNE*LIGNE] = CRGB COLOR_ZDS_SOLDAT;
            leds[trans_num(cols, lines)] = CRGB COLOR_ZDS_SOLDAT;
          }
          // on vérifie si on est dans la zds de chacun des soldat
          // si on est dans la zds d'aucun soldat, ou que tous les soldat sont en indifferent, alors on ecrit la couleur zds sur cette case
          /*bool roll = 1; // permettre d'arreter la boucle
            byte counter = 0;
            while (roll){
            if (pList_Soldat[counter] -> ask_presence(cols, lines, PION_NORM)) {
              if (pList_Soldat[counter] -> get_state() < INDIFFERENT_SOLDAT) { // si un seul soldat à un état particulier, alors on arrete
               roll = 0;
              }
            }
            if ((counter == NB_SOLDAT-1) && (roll == 1)) { // si apres avoir vérifier tous les soldat, il n'y a pas eu de contre indication, on arrete la boucle et on écrit color_zds
              roll = 0;

            }
            counter++;
            }
            }*/

          //          perf_pr("detail");
          break;

        case ALERTE_DANGER:
          leds[trans_num(cols, lines)] = CRGB COLOR_ALERTE;
          break;

        case DANGER:
          leds[trans_num(cols, lines)] = CRGB COLOR_DANGER;
          break;

        case SECURITE:
          //          if (pPlat_Pion[trans_num(cols,lines)] == NULL)
          leds[trans_num(cols, lines)] = CRGB COLOR_SECURITE;
          break;
      }
      lines++;
    }
    cols++;
  }
}

void fadeall() {
  for (int i = 0; i < LIGNE * COLONNE; i++) {
    leds[i].nscale8(FADE);
  }
}

/************ LCD *****************************************/
/***************************************************************/
void LCD_Bienvenue() {
  LCD_Init();
  lpr(0, "BIENVENUE !");
  delay(delai_message_pres);

  lpr(1, "Enlever les pions");
  lpr(2, "Pour commencer");
  delay(2 * delai_message_pres);

  lcd.clear();
}
void LCD_Init() {
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  lcd.clear();
}

void LCD_Pres_Soldat() {
  lcd.clear();
  lpr(1, "SOLDAT :");
  delay(delai_message_pres);
}
void LCD_demande_vestiaire(byte number) {
  lcd.clear();
  lpr(0, "Un "); lcd.print(NAME_FONCTION[number]);
  if (number == VAB) lpr(1, "va au garage");
  else lpr(1, "va au vestiaire");
}
void LCD_envoi_operation(byte number) {
  lcd.clear();
  lpr(0, "Le "); lcd.print(NAME_FONCTION[number]);
  lpr(1, "part sur le terrain");
  lpr(2, "Veuillez le placer");
  lpr(3, "sur le plateau");
}

void LCD_Pres_Artefact() {
  lcd.clear();
  lpr(1, "ARTEFACT :");
  delay(delai_message_pres);
}
void LCD_Demande_Pion(byte numero) {
  lcd.clear();
  lpr(0, "Veuillez placer");

  if (numero > NB_PION + NB_BATIMENT + NB_ARBRE - 1) lpr(1, "ERREUR ID PION");
  else if (numero < NB_PION) { //cas artefact
    lpr(1, NAME_PION[numero]);
  }
  else if (numero >= NB_PION + NB_BATIMENT) { // cas arbre
    lpr(1, "UN ARBRE");
  }
  else if (numero >= NB_PION) {
    lpr(1, "UN BATIMENT");
  }

  lpr(2, "sur le plateau");
  //"posez le pion x sur le plateau"
}

void LCD_Pres_Decor() {
  lcd.clear();
  lpr(1, "ENVIRONEMENT :");
  delay (delai_message_pres);
}

/*
  void Pion_Detruit(byte ID){   //patch3A
  lcd.clear();
  lpr(1, "l'artefact");
  lpr(2,    );
  lpr(3, "a ete detruit");
  }
*/


// Raccourci pour print un string "String str" , à la ligne "byte ligne" sur le LCD
void lpr(byte ligne, String str) {
  lcd.setCursor(0, ligne);
  lcd.print(str);
}

/*************** FIN DE PARTIE ****************/
void fin_de_partie(){   // patch FDLS-1.0
  // print lcd fin de game
  // éteindre lights  || jeu de lumière
  // exctinction
}
