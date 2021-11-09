# Rapport_Danger_Anthropologie_Arm-e

Ce dépot comporte les codes sources permettant de faire fonctionner les plateau de jeux électronique inclus dans la méthodologie de présentation des résultats de recherche de Léa RUELLE.
Il y 3 dossiers concernant : 
1) les code pour le plateau de relation au DANGER entre les militiares et l'nevironnement extérieurs.
2) Les code pour le plateau de relation au DANGER et à la SAILLANCE entre militaire et entre militaire et environnement
3) Les codes de test et de détournement du plateau (utiliser le plateau comme une matrice de LED pour faire des jeu de lumière basé sur des formules mathématiques)

Les différnets codes sont conçu pour fonctionner sur une carte Arduino nano Every. Le choix de cette à été conditionné à la fois par sa fréquence d'horloge plus élévée qu'un Arduino classique ( 60 MHz par rapport aux 20 MHz d'une carte UNO),
ainsi que la quantité plus importante de mémoire flash.

Les plateaux sont basées sur un assemblage d'une matrice de capteur magnétique (à base de relai REED), de LED, et d'un microcontroleur ARM Arduino.
