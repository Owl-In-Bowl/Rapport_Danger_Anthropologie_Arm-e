# Rapport_Danger_Anthropologie_Arm-e

Ce dépot comporte les codes sources permettant de faire fonctionner les plateau de jeux électronique inclus dans la méthodologie de présentation des résultats de recherche de Léa RUELLE.
Il y 3 dossiers concernant : 
1) les code pour le plateau de relation au DANGER entre les militiares et l'nevironnement extérieurs.
2) Les code pour le plateau de relation au DANGER et à la SAILLANCE entre militaire et entre militaire et environnement
3) Les codes de test et de détournement du plateau (utiliser le plateau comme une matrice de LED pour faire des jeu de lumière basé sur des formules mathématiques)

Les différnets codes sont conçu pour fonctionner sur une carte Arduino nano Every. Ce choix est justifié sa fréquence d'horloge plus élévée qu'un Arduino classique ( 60 MHz par rapport aux 20 MHz d'une carte UNO) garantissant une plus grande réactivité,
ainsi que la quantité plus importante de mémoire flash.

Les plateaux sont basées sur un assemblage d'une matrice de capteur magnétique (à base de relai REED), de LED, et d'un microcontroleur ARM Arduino.


Il aurait pu être choisi un esp 32 ayant une vitesse de calcul beaucoup plus élévé (horloge à 240 MHz, ?et double coeur? ), et permattant une liaison wifi pour couplae avec un interface télévisuel (Réalité augmenté, jumeau numérique avec mise en mouvement des pièces pour faciliter la transmission de connaissance)


# suite 
Le code est basé sur les variables globales suivantes:

- deux représentations matriciel du plateau ==> les militaires & les éléments
extérieurs / sous la forme de deux matrices de pointeurs

- deux listes des pièces ==> Les militaires & les éléments extérieurs / sous la
forme de deux listes de pointeurs

Il y a effectivement une redondance, mais celle ci permet de faciliter les
interaction contextuelle entre les pièces ( par exemple : 1) formation des
équipe de militaires par balayage des effectifs (Liste militaire) / 2) Recherche
de saillance par balayage des militaires (Liste militaire)  3) recherche de
pièces militaire ou extérieurs dans un périmètre suite à un déplacement de pièce
( représentation du plateau coté militaire, ou coté éléments extérieurs) )

Le balayage du plateau est automatisé dans le code principale (avec une tempo
pour fixer le délai entre deux décalage de la case scrutée), et a pour but de
vérifier si les capteurs magnétiques renvoient une valeur conforme à ce qui est
en mémoire dans l’Arduino ( présence d'une instance ou non à la position
scrutée) (la référence étant dans ce cas la représentation plateau militaire, ou
pion). Si une différence est constatée :

1) Soit enlèvement d'une pièce ==> conservation du pointeur de l'instance dans
un tampon "disparition" (il y en a un pour les soldats, et un pour les éléments
extérieurs, la dure loi des pointeurs et de leur création... )

2) Soit Apparition d'une pièce ==> s'en suit toute les vérifications
contextuelles : est-ce un pion ou un militaire, est-ce que ce pion ou militaire
s'approche d'un pion ou d'un militaire, reconstitution des équipes de militaire.

D'un point de vu Hardware, entre le temps de montée des registres à décalage,
des portes AND, des pin Arduino, il me parait risqué de changer de case de
scrutation moins que toutes les 500 nS. C'est pour cette raison que la routine
de mesure du plateau commence par 1)une mesure, 2)amorce le décalage de case
scrutée. De cette façon, le temps entre deux décalage de case est respecté grâce
aux opérations à effectuer par le microcontrôleur.

D'autre part, de manière régulière, les états de toutes les pièces sont scrutées
pour afficher les couleurs correspondant à leurs états (états qui sont un
attribut de chaque instance). Le plateau prenant en compte la saillance réalise
les interactions de saillances à ce moment (grosso modo on balaye les soldats
(la liste de pointeurs) et cherche en fonction de leur responsabilités si des
éléments sont à porté de saillance).

Lorsqu’un éléments extérieur est abattu, son instance est détruite (pas
nécessaire, puisqu'on ne risque pas de fuite de mémoire, mais c'est plus propre
de faire comme ça, alors pourquoi s'en priver).

Un système de sécurité bloquant le plateau à été mis en place pour parer aux
mauvaises manipulations (2 pièces retirés en même temps par exemple). Le plateau
s'éteint et affiche la dernière pièce retiré (en blanc) afin que l'utilisateur
la repose dessus, et que le jeu reprenne avec une seule pièce en état de
disparition.
