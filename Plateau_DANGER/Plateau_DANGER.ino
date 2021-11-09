#include "define_LEA_Projet_D.h"
#include "Soldat_D.h"
#include "Pion_D.h"
#include "shift_register_D.h"
#include "freeRam.h"
#include "FastLED.h"

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

*----------- GESTION DES AMELIORATIONS ------------ *
// -A- réinitialiser les tampon ailleurs pour faire un test erreur manip joueur
// -B- Non finis

// ajouter un systeme de blocage avec une boucle while si leseux tampon de pion sont non nul
// car cela implique d'avoir enlevé 2 pièces du plateaux et permettre de reposer la dernière pièce




* ------------- GESTION DES ERREUR ---------------- *
//ERREUR POINTEUR 1 , 2

*/


/***************** DECLARATION VAR ******************
****************************************************/

/**---------------   PLATEAU DE JEU ------------------*/
Pion* pPlat_Pion [COLONNE*LIGNE]={NULL};
Soldat* pPlat_Soldat [COLONNE*LIGNE]={NULL};


/**--------------- CREATION DES INSTANCES ---------------*/
Pion* pList_Pion [NB_PION];
Soldat* pList_Soldat[NB_SOLDAT];

/*--------------- TABLE ASSOCIATION DE GROUPE -----------*/
byte table_association[NB_PION][NB_SOLDAT]={0};

/*--------------- VARIABLE DE RUN PLATEAU ---------------*/
byte read_l = 0;
byte read_c = 0;

Shift_register  shift;

Pion* pPion_Disparu=NULL;
Soldat* pSoldat_Disparu=NULL;

unsigned long tempo_check_plate=0;
unsigned long DELAI_PLATEAU=0;

CRGB leds[COLONNE*LIGNE + 1]; // la plus 1 sert de tampon pour comparer les couleurs de chaque case -> éviter d'écraser une case en statut particulier (secu, alerte, danger) par la zds d'un autre soldat
unsigned long tempo_raf_led=0;
unsigned long DELAI_LED=0;


/**************************************************/
/*********************  PROGRAMME *****************/
/**************************************************/

void setup() {
  //Serial.begin (115200);

  DELAI_PLATEAU = 1000000/(FREQ_PLATEAU*COLONNE*LIGNE); // en uS
  DELAI_LED = 1000/FREQ_LED;                            // en mS
  
  int ARG_LED=COLONNE*LIGNE;
  LEDS.addLeds<WS2812,LED_PIN,RGB>(leds,ARG_LED);
  LEDS.setBrightness(BRIGHTNESS);
  Serial.println("Début prog ");
  shift.init();

  if (1){  // Création des instance Pion & Soldats
    for (byte i=0; i!=NB_PION ; i++) {
      pList_Pion [i] = new Pion();
    }
  
    for (byte x=0; x!=NB_SOLDAT ; x++){
      pList_Soldat [x]= new Soldat(x);
    }
  }
  
  
  if (1){   // Phase d'ajout de pièce sur le plateau
    for (byte soldiers = 0; soldiers != NB_SOLDAT; soldiers++) {
        while (! ( (shift.read()) && (!pPlat_Soldat[trans_num(read_c, read_l)]) )) {decalage_plateau();raf_Led();}
        pPlat_Soldat[trans_num(read_c,read_l)] = pList_Soldat[soldiers];
        pList_Soldat[soldiers] -> set_position(read_c, read_l);
        delay (2);
    }
    for (byte pions = 0; pions != NB_PION; pions++) {
        while (! ( (shift.read()) && (!pPlat_Pion[trans_num(read_c, read_l)]) &&(!pPlat_Soldat[trans_num(read_c,read_l)]) )) {decalage_plateau(); raf_Led();}
        pPlat_Pion[trans_num(read_c,read_l)] = pList_Pion[pions];
        pList_Pion[pions] -> set_position(read_c, read_l);
        delay(2);  
    }
  }
  
  team_management();
  aff_etat();

  //initailisation du tableau d'association pion x groupe
  for (byte pio=0; pio != NB_PION; pio++){
    for(byte sold=0; sold != NB_SOLDAT; sold++) {
       table_association[pio][sold]=0; // les groupe commencent à 1
    }
  }


  tempo_check_plate=micros();
  tempo_raf_led=millis();
}

void loop() {
  if (tempo_check_plate + DELAI_PLATEAU<micros()){  // Phase de mesure plateau
    tempo_check_plate = micros();
    check_plate();
  }

  raf_soldat();
  
  if (tempo_raf_led+DELAI_LED <= millis()) {   // Raffraichissement des led
    tempo_raf_led=millis();
    raf_Led();
//    Serial.print("espace RAM restant : ");Serial.println(freeRam());
  }

}


/********* TEST */

void aff_etat(){
  Serial.println("SOLDAT :");
  for (byte d=0; d!= LIGNE; d++){
    for (byte f=0; f !=COLONNE; f++) {
      if (pPlat_Soldat[trans_num(f,d)]) {Serial.print (pPlat_Soldat[trans_num(f,d)] -> get_ID());Serial.print(" ");}
      else Serial.print ("x ");
    }
    Serial.println();
  }
    Serial.println();
    Serial.println("PION :");
    for (byte d=0; d!= LIGNE; d++){
      for (byte f=0; f !=COLONNE; f++) {
        if (pPlat_Pion[trans_num(f,d)]) {Serial.print (pPlat_Pion[trans_num(f,d)] -> get_ID());Serial.print(" ");}
        else Serial.print ("x ");
      }
      Serial.println();
    }
  Serial.println();
  
 /* Serial.println ("SOLDAT : ");
  for (byte m=0; m != NB_SOLDAT; m++) {
   Serial.print(m); Serial.print (" || ETAT : "); Serial.print(pList_Soldat[m]->get_state());Serial.print(" || GRPE : "); Serial.println(pList_Soldat[m]->get_groupe());
//    Serial.print (" /  LIGNE : "); Serial.println(pList_Soldat[m]->get_ligne());
  }
  Serial.println();
  Serial.println("PION : ");
  for (byte z=0; z != Pion::nbr_pion; z++) {
    Serial.print(z); Serial.print ( " || Col : "); Serial.print (pList_Pion[z] -> get_col()); Serial.print(" || Ligne : ");Serial.print (pList_Pion[z] -> get_ligne()); Serial.print(" || Type : "); Serial.println (pList_Pion[z] -> get_type());
  }
  */
}



/***********************************************/
/**************** FONCTION *********************/
/***********************************************/

/*--------------- FONCTION DE CHECK PLATEAU ---------------*/
/*---------------- gestion de plateau--------------------*/
void check_plate() {
  bool etat_mesure=shift.read();

  if ((etat_mesure==0) ) {        // disparition possible
    Disparition();
  }

  else if (etat_mesure==1) {   // apparition possible
    Apparition();
  }

  decalage_plateau();  // on prépare la prochaine mesure
}

/*------------------------------------------------------------*/
void decalage_plateau() {
  read_c+=1;
  if (read_c>=COLONNE) {
      shift.decalage_colonne(1);
      read_c=0;      
      read_l+=1;
      if (read_l>=LIGNE) {
          shift.decalage_ligne(1);
          read_l=0;
      }
      else shift.decalage_ligne(0);
      }
  else shift.decalage_colonne(0);
}


/*-----------Disparitio possible ------------------*/
void Disparition(){
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
    if ((pPlat_Soldat[trans_num(read_c, read_l)]) || (pPlat_Pion[trans_num(read_c, read_l)])){  
        Serial.println();
        Serial.println("   CHECK DU PLATEAU   ");
        //pPion_Disparu=NULL;      // -A- réinitialiser les tampon ailleurs pour faire un test erreur manip joueur
        //pSoldat_Disparu=NULL;   // -A- réinitialiser les tampon ailleurs pour faire un test erreur manip joueur
         
        
        if (pPion_Disparu != NULL || pSoldat_Disparu != NULL) {
          LEDS.clear();
          leds[trans_num(read_c,read_l)]= CRGB (200, 200, 200);
          FastLED.show();
          
          while (!shift.read());
        }
        
        //else if ((pPlat_Soldat[trans_num(read_c,read_l)]) && (pPlat_Pion[trans_num(read_c,read_l)])){
         // Serial.println (" ERREUR POINTEUR 1 : 2 Pièces sur la même position");}
        
        else if (pPlat_Soldat[trans_num(read_c,read_l)]) {
            pSoldat_Disparu=pPlat_Soldat[trans_num(read_c,read_l)];
            pPlat_Soldat[trans_num(read_c,read_l)]=NULL;
            //pPion_Disparu=NULL;
            Serial.println(" ==> DISPARITION SOLDAT ");
//              aff_etat(); //TEST
            }
        
        else if (pPlat_Pion[trans_num(read_c,read_l)]) {
          pPion_Disparu=pPlat_Pion[trans_num(read_c,read_l)];
          pPlat_Pion[trans_num(read_c,read_l)]=NULL;          
          if (pPion_Disparu->check_fired()) {
            destruction_pion();
            Serial.println("Pion détruit ");}
          else {            
            //pSoldat_Disparu=NULL;
            Serial.println(" ==> DISPARITION PION  ");
//              aff_etat(); //TEST
          }
      }          
    }
}

/*---------- Apparition possible ------------------*/
void Apparition () {
  /*
// if (tampon_soldat!=null && tampon_pion != null) ==> problème de pointeur 1
          // ecrire en serial un probleme de pointeur n°1

      // else if (tampon soldat!=null) et ( pas de soldat au poin de mesure)
          // action apparition soldat
          
      //esle if (tampon pion != null) et (pas de pion au point de mesure)
         // action apparition pion
          
      // Si (Tampon_Soldat=Tampon_Pion=NULL) ==> pas de pion disparu donc pas de pb


// FAUX

verifier si il n'y rien sur le plateau pion et que tampon pion non nul
  alors action pion
verifier si il n'y a rien sur le plateau soldat et tampon soldat non nul
  alors action soldat



 */
    
      if ((pPion_Disparu ) && (pSoldat_Disparu )){  //ERREUR POINTEUR
          Serial.println( "ERREUR POINTEUR 2 : les deux pointeurs tampon sont non null");
      }

      else if ((pPion_Disparu ) && (!pPlat_Pion [trans_num(read_c,read_l)] ) && (!pPlat_Soldat[trans_num(read_c,read_l)]) ){     // PION APPARU
         pPlat_Pion[trans_num(read_c,read_l)]=pPion_Disparu;
         pPion_Disparu->set_position(read_c, read_l);
         apparition_Pion();
         pPion_Disparu=NULL; // -A-
         aff_etat(); //TEST
      }

      else if ((pSoldat_Disparu ) && (!pPlat_Soldat[trans_num(read_c,read_l)] ) && (!pPlat_Pion[trans_num(read_c,read_l)]) ){   // SOLDAT APPARU
         pPlat_Soldat[trans_num(read_c,read_l)]=pSoldat_Disparu; 
         pSoldat_Disparu->set_position(read_c, read_l);
         apparition_Soldat();
         pSoldat_Disparu=NULL; // -A-
         aff_etat(); // TEST
      }
}

/*------------------------------------------------------------*/
void apparition_Soldat(){
// on applique les recherches au point stocké en mémoire
  Serial.println();  Serial.println(" ==> APPARITION SOLDAT");
  pSoldat_Disparu->set_state(INDIFFERENT_SOLDAT);
  for (byte i=0; i!= NB_SOLDAT; i++) { // ON cherche si on entre sur le territoire d'un soldat
    if ((pList_Soldat[i]!=pSoldat_Disparu) && (pList_Soldat[i]->ask_presence(pSoldat_Disparu->get_col(), pSoldat_Disparu->get_ligne()))){
      pSoldat_Disparu->set_state(SECURITE); Serial.print("Arrive chez le soldat : ");Serial.println(i);
      break;
    }
  }

  if (pSoldat_Disparu->get_state()!=SECURITE) { // si on est pas entre sur le territoire d'un soldat, alors on test avec les pions
    for (byte y =0; y!= Pion::nbr_pion; y++) {
      if (pSoldat_Disparu->ask_presence(pList_Pion[y]->get_col(), pList_Pion[y]->get_ligne())) {
        pSoldat_Disparu->set_state(SECURITE);
        break;
      }
    }    
  }
  
  team_management();
  // mettre la team du pion disparu dans son etat
  for (byte df=0; df!=NB_SOLDAT; df++) {
    if (pList_Soldat[df]->get_groupe() == pSoldat_Disparu->get_groupe()) pList_Soldat[df]->set_state(pSoldat_Disparu->get_state());
  }
  
//  unsigned long heure_soldat=millis();
//    for (byte q=0; q!=NB_SOLDAT; q++) {
//      pList_Soldat[q]-> set
//    }
  
/*  
  // on passe son etat en indifférent  
  // on check si il arrive sur la zone d'un soldat
      //si oui securité
  // si securité != 1 on check si il arrive sur la zone d'un pion
      // ==> en fait tout les pion lui demande si ils sont dans sa zone
      // si oui securité

  // si toujours indifférent alors ==> FIN
  // si sécurité, on reforme les groupes de soldat
  // les autres soldat du groupe du soldat prennent l'état du soldat déplacé
  // on rentre l'horloge pour tout le groupe de soldat
  // POINTEUR TAMPON NULL -A- 
*/
}

/*------------------------------------------------------------*/
void apparition_Pion(){
  
    Serial.println(" ==> APPARITION PION");

// il faut rajouter un bloc pour vérifier si le pion ne vient pas de sortir d'un groupe 
// on balaye la ligne du pion enregistré dans association_groupe on vérifie que 1 soldats de chaque groupe est toujours dans les parage
// sinon on enleve le groupe
// 
Serial.println( "table association du pion apparu -> début");
for (byte drone=0; drone!= NB_SOLDAT; drone++) {
  Serial.print(table_association [pPion_Disparu->get_ID()][drone]); Serial.print(" ");
}
Serial.println();

for (byte regy=0; regy != NB_SOLDAT ; regy ++) {
  if (table_association[pPion_Disparu ->get_ID()][regy] != 0) {
    byte gaunter=0;
    bool continuation = 1;
    while (continuation) {
      if (pList_Soldat[gaunter]->get_groupe()==table_association [pPion_Disparu->get_ID()][regy]){
        if (pList_Soldat[gaunter]-> ask_presence(pPion_Disparu-> get_col(), pPion_Disparu->get_ligne()) ) continuation = 0; // le pion est toujours dans la zone d'au moin un represnetant du groupe, donc on passe au groupe associé au pion suivant 
      }
      if ( (continuation == 1) && (gaunter == NB_SOLDAT-1) ) {continuation = 0; table_association[pPion_Disparu-> get_ID()][regy]=0;} // si on a balaye tous les soldat, et que le pion n'est plus dans la zds d'au moins un membre du groupe checké ==> le l'association du groupe au pion est effacé 
      gaunter ++;
    }
  }
}

Serial.println( "table association du pion apparu -> après vérif sorti");
for (byte drone=0; drone!= NB_SOLDAT; drone++) {
  Serial.print(table_association [pPion_Disparu->get_ID()][drone]); Serial.print(" ");
}
Serial.println();
/*    // ON lance les dé pour fired or not fired
    // on check si le pion rentre sur la zone d'un soldat    
        // on stock le groupe de chaque soldat qui est concerné
    // On change l'état des groupe concerné en alerte, et on stock le temps de changement
    // change le temps du pion*/
    

    byte tirage=random (0,100);
    bool dee_fired=0;
    if (tirage < proba_death) dee_fired = 1;  
      
    Serial.print("id fired ? "); Serial.println(dee_fired);
    // on utilise un tampon pour connaitre les groupes des soldat auxquel le pion vient de pénetrer dans la zds
    // le tampon est ensuite utiliser pour mettre tous les groupe concerné dans le même état (alerte ras ou alerte danger, ou sécurité)
    byte tampon_groupe[NB_SOLDAT]={0}; 
    for (byte xi=0; xi != NB_SOLDAT; xi ++) {
      tampon_groupe[xi]=NB_SOLDAT+1;   // plus 1 puisque les groupes commencent à 1
    }
    
    // on cherche les soldat qui rentre en alerte et on recupère leur groupe
    for (byte z=0; z!=NB_SOLDAT; z++) {  // on balaye la liste de soldat
      if (pList_Soldat[z]->ask_presence(pPion_Disparu->get_col() , pPion_Disparu->get_ligne(), pPion_Disparu->get_type())) {//on leur demande si le pion est dans leur zone
        bool continuer = 1;
        byte v = 0;


        // si le pion est déja associé au groupe, alors il ne faut pas effectuer les laus suivant (continuer =0)
        // cela evite de rentrer le pion dans le tampon des groupes à modifier/ interagir

        for (byte sneakers = 0; sneakers != NB_SOLDAT; sneakers++) {
          if (pList_Soldat[z]->get_groupe() == table_association[pPion_Disparu->get_ID()][sneakers]){
            continuer = 0;
          }
        }
        
        while (continuer) {
          pPion_Disparu->set_fired(dee_fired); // le pion connait son état si jamais on modifie le tampon groupe (et donc alerte)
          if (tampon_groupe[v] == pList_Soldat[z]->get_groupe()) continuer = 0; // on regarde si le groupe du soldat est déja dans le tampon -> plus besoin de continuer pour ce soldat
          else if (tampon_groupe[v] == NB_SOLDAT+1) {tampon_groupe[v] = pList_Soldat[z]->get_groupe(); continuer = 0;} // autrement, si la case du tampon est vierge, on rentre le groupe du soldat
          if (v==NB_SOLDAT-1) continuer =0;
          v++;
        }
      }
    }

    Serial.println( " ESSAI DU TAMPON DE GROUPE");
    for (byte dress=0; dress!=NB_SOLDAT; dress++){
      Serial.print(tampon_groupe[dress]);Serial.print (" ");
    }
   Serial.println( );
   
    //maintenant qu'on connait les groupe et l'issue de l'alerte, on va stocker cette issue dans les membre des groupe, et 
    // et rentrer l'horloge aux soldat pour qu'ils puissent se reset plus tard (fait dans la méthode de classe)
    byte compteur=0;
    while ( (compteur <NB_SOLDAT) && (tampon_groupe[compteur]!=NB_SOLDAT+1) ){
      for (byte d=0; d != NB_SOLDAT; d++) {
        if (pList_Soldat[d]->get_groupe() == tampon_groupe[compteur]){
          if (dee_fired==1)pList_Soldat[d]->set_state(ALERTE_DANGER); // si fired 
          else pList_Soldat[d]->set_state(ALERTE_RAS);  // si non fired
        }
      }
      compteur++;
    }

// on actualise le tableau association groupe avec les nouvelles valeurs
// on regarde pour le pion (id) si les valeurs du tampon groupe sont a ajouter au groupe (sur un emplacement avec groupe nul)
// pour chaque emplacement dans tampon_groupe, on balaye association_groupe pour le rentrer dans un espace vierge
 
for (byte grids=0; grids != NB_SOLDAT; grids ++) {
  bool stocke =0;
  if (tampon_groupe[grids] == NB_SOLDAT+1) break;
  
  for (byte degs = 0; degs != NB_SOLDAT; degs++) {
    if ((stocke == 0) && (tampon_groupe[grids]!=NB_SOLDAT+1) && (table_association[pPion_Disparu-> get_ID()][degs]==0)){
      table_association[pPion_Disparu-> get_ID()][degs]= tampon_groupe[grids]; stocke = 1;
      }
  }  
}

Serial.println( "table association du pion apparu -> après actualisation des nouveau groupes");
for (byte drone=0; drone!= NB_SOLDAT; drone++) {
  Serial.print(table_association [pPion_Disparu->get_ID()][drone]); Serial.print(" ");
}
Serial.println();
Serial.println();
}

/*-------------------divers-----------------------*/

/*------------------------------------------------------------*/
void team_management() {
  Serial.println(" Team management ");
  byte groupe=0;
  for (byte tf=0; tf!=NB_SOLDAT; tf++) {
    pList_Soldat[tf]-> set_groupe(0);
    pList_Soldat[tf]-> set_checked(0);
  }
  
  for (byte s=0; s!= NB_SOLDAT; s++) {            // balaye tous les soldats
    if (!pList_Soldat[s]->get_checked()){       // si le soldat n'a pas été checké
      groupe++;                 // on incremente le groupe
      pList_Soldat[s]-> set_groupe(groupe);   // on lui associe le groupe actuel
      bool continue_while=1;
      while (continue_while) {
        byte compteur=0;
        continue_while=0;
        while ((compteur<NB_SOLDAT)) {  // on cherche un soldat du groupe non checké
          if ((pList_Soldat[compteur]->get_groupe() == groupe ) && (!pList_Soldat[compteur]->get_checked())) {
            continue_while=1; 
            break;
          }
          compteur++;
          }
        if (continue_while==1){
          for (byte p=0;  p!=NB_SOLDAT; p++) {    // et on vérifie si les autre soldats sont dans sa zone
              if (pList_Soldat[compteur]->ask_presence(pList_Soldat[p]->get_col(), pList_Soldat[p]->get_ligne())) {  // si oui 
              pList_Soldat[p]->set_groupe(groupe);}}  //alors ce soldat entre dans le groupe
          pList_Soldat[compteur]-> set_checked(1);      
        }
      }
    }  
  }
}

/*------------------------------------------------------------*/
void raf_soldat() {
  for (byte dies=0; dies!=NB_SOLDAT; dies++){
    pList_Soldat[dies]->check_state();
  }
}

/*-------------------------------------------------------------*/
void destruction_pion(){
  //check de liste
  for (byte pero=0; pero!=Pion::nbr_pion; pero++) {
    if (pList_Pion[pero]==pPion_Disparu){
      for (byte restant=pero; restant!=Pion::nbr_pion-1; restant++){ // -1 parce que le l'instance pas encore détruite ( or elle faisant baisser nbr_pion--)
        pList_Pion[restant]=pList_Pion[restant+1];
      }
    }
  }
  delete pPion_Disparu; 
  pPion_Disparu=NULL; 
}

/************ TRANSFORMATION ************/

byte trans_num(byte a_col, byte a_ligne){
  byte num=0;
  num=(a_ligne)*COLONNE+a_col;
  return num;
}

byte trans_col(byte a_num) {
  byte reste=a_num;
  while (reste>=COLONNE){
    reste=reste-COLONNE;
  }
  return reste;
}

byte trans_ligne(byte a_num){
  byte compteur=0;
  byte reste=a_num;
  while (reste >= COLONNE){
    reste=reste-COLONNE;
    compteur++;
  }
  return compteur;
}



/***************  ALLUMAGE DES LED *******************/

void raf_Led (){
  /* rabaisser toutes les led de X
   * lire l'état des pion
   *    choper la position via get_col, get_ligne
   *    appel à la led en (trans_num, check_fired)
   *      ==> écrit la couleur correspondante à la bonne led
   *    
   *    adresser la couleur à la led via trans_num()
   * lire l'état des soldat 
   *    adresser la couleur
   */

  fadeall();
  
  for (byte n=0; n!= LIGNE*COLONNE; n++){
    if (pPlat_Pion[n]!=NULL) set_color(pPlat_Pion[n]->get_col(), pPlat_Pion[n]->get_ligne(), pPlat_Pion[n]->check_fired());
  }
  
  for (byte l=0; l!= LIGNE*COLONNE; l++) {
    if (pPlat_Soldat[l]!=NULL && pPlat_Soldat[l]->get_state() == INDIFFERENT_SOLDAT)  set_color(pPlat_Soldat[l]->get_col(), pPlat_Soldat[l]->get_ligne(), pPlat_Soldat[l]->get_state());
  }

  for (byte lex=0; lex != LIGNE*COLONNE; lex++) {
    if (pPlat_Soldat[lex] != NULL && pPlat_Soldat[lex]->get_state() != INDIFFERENT_SOLDAT) set_color(pPlat_Soldat[lex]->get_col(), pPlat_Soldat[lex]->get_ligne(), pPlat_Soldat[lex]->get_state());
  }
  

  
  FastLED.show();

}

void set_color(byte x_col, byte x_ligne, byte x_state) {
  if (x_state == ALERTE_RAS) x_state=ALERTE_DANGER;
  switch (x_state) {
    
    case INDIFFERENT_SOLDAT:
      led_zds (x_col , x_ligne, INDIFFERENT_SOLDAT);
      leds[trans_num(x_col,x_ligne)] = CRGB COLOR_INDIF_SOLDAT;
      // si il y a un soldat à cette position, pas de changement de couleur
      break;

    case FIRED:
      leds[trans_num(x_col,x_ligne)] = CRGB COLOR_FIRED;
/*      if (BATT_FIRED) {
//          byte Brightness = (BRIGHTNESS / 2)* (1 + sin(millis()/battement)) +1 ;
//          leds[trans_num(x_col,x_ligne)].nscale8_video( Brightness );
//          } 
*/         
      break;
      
    case PION:
      leds[trans_num(x_col,x_ligne)] = CRGB COLOR_PION;
      break;
      
    case SECURITE:
      led_zds(x_col, x_ligne, SECURITE);
      break;

    case DANGER:
      led_zds(x_col, x_ligne, DANGER);
      break;
    
    case ALERTE_DANGER:
      led_zds(x_col, x_ligne, ALERTE_DANGER);
      break;
  }
}

void led_zds(byte c, byte l, byte etat) {
  for (int z=c-DIM_ZDS; z<= c+DIM_ZDS; z++) {
    byte cols = z;
    if (z>=COLONNE) cols=COLONNE-1;
    else if (z<0) cols=0;
    
    for (int w=l-DIM_ZDS; w<= l+DIM_ZDS; w++){
      byte lines=w;
      if (w>= LIGNE) lines=LIGNE-1;
      else if (w<0) lines=0;
      
      switch (etat) {
        
        case INDIFFERENT_SOLDAT:
          if ((pPlat_Pion[trans_num(cols, lines)] == NULL)&&(pPlat_Soldat[trans_num(cols, lines)] == NULL)){ // si il n'y a pas de pion ou de soldat sur la case
            leds[trans_num(cols, lines)] = CRGB COLOR_ZDS_SOLDAT;
          }/*
            
 // on vérifie s on est dans la zds de tous les soldat
 // si on est dans la zds d'aucun soldat, ou que tous les soldat sont en indifferent, alors on ecrit la couleur zds
            bool roll = 1; // permettre d'arreter la boucle
            byte counter = 0;
            while (roll){               
              if (pList_Soldat[counter] -> ask_presence(cols, lines)) {
                if (pList_Soldat[counter] -> get_state() < INDIFFERENT_SOLDAT) { // si un seul soldat à un état particulier, alors on arrete
                 roll = 0; 
                }
              }
              if ((counter == NB_SOLDAT-1) && (roll == 1)) { // si apres avoir vérifier tous les soldat, il n'y a pas eu de contre indication, on arrete la boucle et on écrit color_zds
                roll = 0;
                leds[trans_num(cols,lines)] = CRGB COLOR_ZDS_SOLDAT;
              }
              counter++;
            }
          }
          */
          break;
          
        case ALERTE_DANGER:
          leds[trans_num(cols,lines)] = CRGB COLOR_ALERTE;
          break;

        case DANGER:
          leds[trans_num(cols,lines)] = CRGB COLOR_DANGER;
          break;

        case SECURITE:
          if (pPlat_Pion[trans_num(cols,lines)] == NULL) leds[trans_num(cols,lines)] = CRGB COLOR_SECURITE;
          break;
      }           
    }
  }  
}

void fadeall() { for(int i = 0; i < LIGNE*COLONNE; i++) { leds[i].nscale8(FADE);}}
