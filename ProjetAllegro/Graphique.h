//
// Created by Mazouz Ilias on 15/05/2025.
//

#ifndef GRAPHIQUE_H
#define GRAPHIQUE_H


#include <allegro.h>
#include "definition.h"

// Fonctions de dessin des éléments du jeu
void dessiner_vaisseau(Vaisseau *vaisseau, BITMAP *tampon, BITMAP *sprite_vaisseau);
void dessiner_ennemis(Ennemi ennemis[], BITMAP *tampon, BITMAP *sprites_ennemi[]);
void dessiner_tirs(Tir tirs[], BITMAP *tampon);
void dessiner_effets_impact(Effet effets[], BITMAP *tampon);
void dessiner_bonus(Bonus *bonus, BITMAP *tampon);
void dessiner_vies(Vaisseau *vaisseau, BITMAP *tampon);

// Fonctions de dessin des interfaces
void dessiner_barre_charge(Vaisseau *vaisseau, BITMAP *tampon);
void dessiner_barre_bouclier(Vaisseau *vaisseau, BITMAP *tampon);
void dessiner_fin(int score, BITMAP *tampon);
void dessiner_niveau_termine(int niveau, int score, BITMAP *tampon);

// Fonctions pour le menu
void dessiner_menu(BITMAP *tampon, int selection, char *pseudo, int *niveau_choisi);
void saisir_pseudo(BITMAP *tampon, char *pseudo);

// Fonctions pour le
void update_fond(int *position_fond);
void dessiner_fond(BITMAP *tampon, BITMAP *fond, int position_fond);


#endif //GRAPHIQUE_H

