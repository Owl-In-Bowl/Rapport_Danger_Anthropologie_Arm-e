#define led_du_soldat trans_num(pList_Soldat[ID]->get_col(), pList_Soldat[ID]->get_ligne())
void check_saillance(ID)

	switch (pList_Soldat[ID]->get_fonction){
		
		case GV:
			if (cherche_environnement(Col, Li, ASSO_Fonction_zone[0][0], ASSO_Fonction_zone[0][1])){
				allum_sold_saill(ID);
			}
			break;		

		case CDE:
			if (cherche_environnement (Col, Li, ASSO_Fonction_zone[1][0], ASSO_Fonction_zone[1][1])) {
				allum_sold_saill(ID);
			}
			cherche_soldat(ID);
			break;
			
		case VAB:
			if (cherche_environnement (Col, Li, ASSO_Fonction_zone[2][0], ASSO_Fonction_zone[2][1])){
				allum_sold_saill(ID);
			}
			else allum_sold_saill(ID);
			break;
			
		case CDG:
			cherche_soldat(ID);
			break;		

			
		case SOA:
			bool tamps=0;
			tamps = cherche_soldat(ID);
			allum_sold_saill(ID);
			break;

		case Radio:
			allum_sold_saill(ID);
			break;

		case CDS:
			allum_sold_saill(ID);
			break;
		
		case CDU:
			allum_sold_saill(ID);
			break;
	}


void allum_sold_saill(byte ID){
	leds[led_du_soldat]= CRGB COLOR_SAILLANCE;
}


#define bumcol pList_Pion[pips]->get_col() 
#define bumli pList_Pion[pips]->get_ligne()

bool cherche_environnement (byte col, byte li, byte interne, byte externe) { 
  bool is_saill =0;
  for (byte pips =NB_PION; pips< NB_PION+NB_ARBRE+NB_BATIMENT; pips++){
    // si le pion est dans la zone de test
    if (((bumcol >= col-externe && bumcol <=col -interne) || (bumcol >= col+interne && bumcol <= col+externe))||((bumli >= li-externe && bumli <= li-interne) || (bumli<=li+externe && bumli>=li+interne))) {
      leds[trans_num(bumcol, bumli)] = CRGB COLOR_SAILLANCE; 
      is_saill=1;
    }
  }
  return is_saill;
}
 
 
bool cherche_soldat(ID){
	bool back=0;

	switch (pList_Soldat[ID]->get_fonction()){
		case SOA:
			//balayer les babos du coin
			for (byte human=0; human < NB_SOLDAT; human++){
				if (pList_Soldat[human]->ask_presence(pList_Soldat[ID]->get_col() pList_Soldat[ID]->get_ligne(), PION_NORM) && human !=ID){
					// on allume le soldat
					leds[trans_num(pList_Soldat[human]->get_col(), pList_Soldat[human]->get_ligne())]= CRGB COLOR_SAILLANCE;
					// on retourne le resulat
					back=1;
				}
			}
			break;
			
		case CDE:
			for (byte steack=0; steack <NB_SOLDAT; steack++){
				if ( pList_Soldat[steack]->get_fonction() == GV && pList_Soldat[steack]->ask_presence(pList_Soldat[ID]->get_col(),pList_Soldat[ID]->get_ligne(),PION_NORM)){// est GV et preseny
					//allum soldat
					leds[trans_num(pList_Soldat[steack]->get_col,pList_Soldat[steack]->get_ligne())]=CRGB COLOR_SAILLANCE;
					//returne 1
					back=1;
				}
			}
			break;
		
		case CDG:
			for (byte stack=0; stack <NB_SOLDAT; stack++){
				if ( pList_Soldat[stack]->get_fonction() == CDE && pList_Soldat[stack]->ask_presence(pList_Soldat[ID]->get_col(),pList_Soldat[ID]->get_ligne(),PION_NORM)){// est GV et preseny
					//allum soldat
					leds[trans_num(pList_Soldat[stack]->get_col,pList_Soldat[stack]->get_ligne())]=CRGB COLOR_SAILLANCE;
					//returne 1
					back=1;
				}
			}
			break;
		
	}
	return back;
}


/*************************************/

void led_zds(byte c, byte l, byte etat) {
	for (int z=c-DIM_ZDS; z<= c+DIM_ZDS; z++) {
		byte cols = 0;
		if (z>=COLONNE) {cols=COLONNE-1;break;}
		else if (z<0) cols=0;
		
		for (int w=l-DIM_ZDS; w<= l+DIM_ZDS; w++){
		  byte lines=0;
		  if (w>= LIGNE) {lines=LIGNE-1; break;}
		  else if (w<0) lines=0;
	}}
}

void led_zds(byte c, byte l, byte etat){
	signed int cols=c-DIM_ZDS;
	byte fin_cols=c+DIM_ZDS;
	if (fin_cols >= COLONNE) fin_cols=COLONNE;	
	if (cols<0) cols=0;
	
	
	while (cols !=fin_cols){
		signed int lines=c-DIM_ZDS;
		byte fin_lines=c+DIM_ZDS;
		if (fin_lines>= LIGNE) fin_lines=LIGNE;
		if (lines<0) lines=0;
		while (lines != fin_lines){
			
			
			lines+=1;
		}
		
		
		cols+=1;
	}
			
			
			
			
			
		

	
	// si on est dans la bande externe
	// 	si on est c-DIM_ZDS<0

	
	if (c-DIM_ZDS == -2) {cols=0; cols_fin=2;}
	else if (c-DIM_ZDS) == -1){cols=0;sols_fin=3;}
	else {cols=c-DIM_ZDS; sols
	
	switch (c-DIM_ZDS):
		case -2:
			cols=0;
			cols_fin=2;
			break;
		case -1:
			cols=0;
			fin_cols=3;
			break;
			
			
			
	
test de la ligne de départ
defnition de la ligne limite
iteration
 test de la colonne de depart
 test de la colonne de find
 iteration

	}
	
/************************************/

test de colonne exterieur	
	test de ligne exterieur
		test interne colonne ou interne ligne

bool cherche_environnement (byte col, byte li, byte interne, byte externe) { 
  bool is_saill =0;
  for (byte pips =NB_PION; pips< NB_PION+NB_ARBRE+NB_BATIMENT; pips++){
    // si le pion est dans la zone de test
	if (bumcol>=col-externe && bumcol<=col+externe){
		if (bumli>=li-externe && bumli<=li+externe){
			// on est maintenant dans le carré externe
			// on regarde si on est pas à l'intérieur du carrée interne
			if (! ( (bumcol >= col-interne && bumcol <= col+interne) && (bumli>= li-interne && bumli <=li+interne) ) ){
				leds[trans_num(bumcol, bumli)] = CRGB COLOR_SAILLANCE; 
				is_saill=1;
			}
  }
  return is_saill;
}