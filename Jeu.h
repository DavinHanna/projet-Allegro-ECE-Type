//
// Created by Mazouz Ilias on 16/05/2025.
//

#ifndef JEU_H
#define JEU_H


#include <allegro.h>
#include "definition.h"

//Fonctions d'initialisation//
void initialiser_allegro(void);
BITMAP* initialiser_buffer(void);
void initialiser_jeu(Vaisseau *vaisseau, Tir tirs[], Effet effets[], Ennemi ennemis[], Bonus *bonus, Meteor meteors[]);
void charger_ressources_jeu(BITMAP **sprite_vaisseau, BITMAP *sprites_ennemis[], BITMAP **fond);

//Fonctions de gestion du menu//
int gerer_menu(BITMAP *buffer, int *niveau_choisi, char *pseudo);

//Fonctions de gestion du jeu//
void gerer_controles(Vaisseau *vaisseau, Tir tirs[], Effet effets[], int *espace_relache);
void update_jeu(Tir tirs[], Effet effets[], Ennemi ennemis[], Vaisseau *vaisseau, Bonus *bonus, Meteor meteors[], int *score, int *ennemis_tues, int *ennemis_pour_vie);
void gerer_niveau(int *timer_niveau, int *niveau_actuel, int *generer_ennemis, int *niveau_termine,int *boss_genere, Ennemi ennemis[], BITMAP *buffer, int niveau_choisi, int *score,Vaisseau *vaisseau, int *partie_terminee, int *ennemis_actifs);
void gerer_apparition_ennemis(int *timer_apparition_ennemi, Ennemi ennemis[], int niveau_actuel, int generer_ennemis);
int compter_ennemis_actifs(Ennemi ennemis[], int *boss_vivant);

//Fonctions de rendu//
void dessiner_jeu(BITMAP *buffer, BITMAP *fond, int position_fond, Vaisseau *vaisseau,BITMAP *sprite_vaisseau, Tir tirs[], Effet effets[], Ennemi ennemis[],BITMAP *sprites_ennemis[], Bonus *bonus, Meteor meteors[], int score, char *pseudo,int niveau_choisi, int niveau_actuel, int timer_niveau, int generer_ennemis,int ennemis_actifs, int niveau_termine, int boss_vivant, int ennemis_pour_vie);
void afficher_ecran_fin(BITMAP *buffer, int score, char *pseudo);

//Fonction de nettoyage//
void liberer(BITMAP *buffer, BITMAP *sprite_vaisseau, BITMAP *sprites_ennemis[], BITMAP *fond);
void afficher_ecran_fin(BITMAP *buffer, int score, char *pseudo);

//Fonction Leaderboard//
void sauvegarder_score(char *pseudo, int score);
void charger_leaderboard(char pseudo_list[][20], int scores[], int *nb_scores);
void afficher_leaderboard(BITMAP *buffer);

#endif //JEU_H
