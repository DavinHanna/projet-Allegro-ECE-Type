//
// Created by Mazouz Ilias on 15/05/2025.
//

#ifndef DEFINITION_H
#define DEFINITION_H


#define LARGEUR 1000
#define HAUTEUR 800
#define MAX_TIRS 30
#define MAX_ENNEMIS 8
#define VAISSEAU_X 50
#define VITESSE_TIR 7
#define VITESSE_TIR_ENNEMI 5
#define TAILLE_VAISSEAU 30
#define TAILLE_ENNEMI 25
#define VIES 3


#define NIVEAU_1 60   // 1 seconde à 60 FPS
#define NIVEAU_2 120  // 2 secondes à 60 FPS

#define DELAI_TIR_ENNEMI 240
#define ENNEMIS_POUR_BONUS 5 // Nombre d'ennemis à tuer pour obtenir un bonus
#define ENNEMIS_POUR_VIE 15 // Nombre d'ennemis à tuer pour obtenir une vie supplémentaire
#define DUREE_DOUBLE_TIR 600 // 10 secondes à 60 FPS
#define DUREE_TRIPLE_TIR 300 // 5 secondes à 60 FPS
#define DUREE_NIVEAU 1800 // 30 secondes à 60 FPS

#define NIVEAU_MAX 3   // Nombre maximum de niveaux
#define VIE_BOSS 60      // Points de vie du boss
#define TAILLE_BOSS 60        // Taille du boss
#define DELAI_TIR_BOSS 60  // Délai entre les tirs du boss (plus rapide)
#define DELAI_APPARITION_BOSS 300
#define COULEUR_TRANS makecol(255, 0, 255)
#define NB_IMAGES_TIR 5      // 5 images pour l'animation du tir
#define NB_IMAGES_IMPACT 5       // 5 images pour l'animation d'impact
#define DERNIERE_IMAGE_TIR 4      // Dernière frame du tir (0-based)
#define DELAI_IMAGE 3          // Vitesse d'animation
#define LARGEUR_HITBOX 20
#define HAUTEUR_HITBOX 10


BITMAP *sprites_ennemis[3];



typedef struct {
    float x, y;
    float dx, dy;
    int actif;
    int tir_ennemi;
    int puissance;
    int image;
    int compteur_image;
    BITMAP *image_tir[NB_IMAGES_TIR];
    BITMAP *image_tir_ennemi[NB_IMAGES_TIR];  // Tableau pour les images des tirs ennemis
} Tir;

typedef struct {
    float x, y;
    int actif;
    int image_courante;
    int compteur_image;
    int impact_ennemi;  // Pour distinguer entre explosion de tir joueur et ennemi
    BITMAP *image_impact[NB_IMAGES_IMPACT];
    BITMAP *image_impact_ennemi[8];  // 8 images pour les explosions des tirs ennemis
} Effet;

typedef struct {
    float x, y;
    float dx, dy;
    int actif;
    int type_mouvement;
    float angle;
    float rayon;
    float centre_x, centre_y;
    int compteur_tir;
    int vie;         // Points de vie de l'ennemi
    int niveau;          // Niveau de l'ennemi (pour différencier les comportements)
    int boss;        // Indique si c'est un boss
    int compteur_spawn;    // Pour le boss: délai entre les apparitions d'ennemis
} Ennemi;

typedef struct {
    float x, y;
    int actif;
    float vitesse;
    int type; // 0=double tir, 1=triple tir
} Bonus;

typedef struct {
    float x, y;
    int vies;
    int compteur_charge;
    int charge;
    int double_tir; // 0=normal, 1=double tir
    int compteur_double_tir; // Durée du double tir
    int triple_tir; // 0=normal, 1=triple tir
    int compteur_triple_tir; // Durée du triple tir
    int bouclier_actif;
    int energie_bouclier;
    int energie_max_bouclier;
    int compteur_immunite; // Compteur d'immunité après collision
} Vaisseau;

#endif //DEFINITION_H
