//
// Created by Mazouz Ilias on 15/05/2025.
//

#ifndef ANIMATION_H
#define ANIMATION_H

#include "definition.h"

// Fonctions de chargement des sprites et animations
BITMAP* lancer_sprite(const char* nom_fichier, int facteur_echelle);
void charger_frames_impact_ennemi(Effet *effets);
void charger_animation_tir_ennemi(Tir *tirs);
void charger_animation_tir(Tir *tirs);
void charger_frames_impact(Effet *effets);

// Fonctions de mise à jour des animations
void update_tirs(Tir tirs[], Effet effets[]);
void update_effets_impact(Effet effets[]);

// Fonction utilitaire pour les sprites
BITMAP* flip(BITMAP* original);

#endif //ANIMATION_H
