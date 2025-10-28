//
// Created by Davin on 10/05/2025.
//

#ifndef GRAPHIQUE_H
#define GRAPHIQUE_H


#include <allegro.h>
#include "definition.h"

//Fonctions de dessin des éléments du jeu//
void dessiner_vaisseau(Vaisseau *vaisseau, BITMAP *buffer, BITMAP *sprite_vaisseau);
void dessiner_ennemis(Ennemi ennemis[], BITMAP *buffer, BITMAP *sprites_ennemi[]);
void dessiner_tirs(Tir tirs[], BITMAP *buffer);
void dessiner_effets_impact(Effet effets[], BITMAP *buffer);
void dessiner_bonus(Bonus *bonus, BITMAP *buffer);
void dessiner_vies(Vaisseau *vaisseau, BITMAP *buffer);

//Fonctions de dessin des interfaces//
void dessiner_barre_charge(Vaisseau *vaisseau, BITMAP *buffer);
void dessiner_barre_bouclier(Vaisseau *vaisseau, BITMAP *buffer);
void dessiner_fin(int score, BITMAP *buffer);
void dessiner_niveau_termine(int niveau, int score, BITMAP *buffer);

//Fonctions pour le menu//
void dessiner_menu(BITMAP *buffer, int selection, char *pseudo, int *niveau_choisi);
void saisir_pseudo(BITMAP *buffer, char *pseudo);

//Fonctions pour le fond//
void update_fond(int *position_fond);
void dessiner_fond(BITMAP *buffer, BITMAP *fond, int position_fond);


#endif //GRAPHIQUE_H


