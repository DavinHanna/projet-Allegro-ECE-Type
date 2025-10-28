//
// Created by Mazouz Ilias on 15/05/2025.
//

#ifndef DEFINITION_H
#define DEFINITION_H

//Dimension de la fenetre//
#define LARGEUR 1000
#define HAUTEUR 800


#define MAX_TIRS 30 //Nombre max de tirs simultanés//
#define MAX_ENNEMIS 8 //Nombre max d'ennemis simultanés//
#define VAISSEAU_X 50 //Position initial fixe x du joueur//
#define VITESSE_TIR 10 //Vitesse des tirs du joueur//
#define VITESSE_TIR_ENNEMI 10 // Vitesse des tirs des ennemis//
#define TAILLE_VAISSEAU 30 // Taille du joueur//
#define TAILLE_ENNEMI 25 // Taille des ennemis//
#define VIES 3 //Nombres de vies initiales du joueur//


#define NIVEAU_1 60   //1 seconde à 60 FPS : Seuil pour le premier niveau de charge du tir//
#define NIVEAU_2 120  //2 seconde à 60 FPS : Seuil pour le deuxième niveau de charge du tir//

#define DELAI_TIR_ENNEMI 240 //Délai entre les tirs ennemis//
#define ENNEMIS_POUR_BONUS 5 //Nombre d'ennemis à tuer pour obtenir un bonus//
#define ENNEMIS_POUR_VIE 15 // Nombre d'ennemis à tuer pour obtenir une vie supplémentaire//
#define DUREE_DOUBLE_TIR 600 //10 secondes//
#define DUREE_TRIPLE_TIR 300 //5 secondes//
#define DUREE_NIVEAU 1800 //30 secondes//

#define NIVEAU_MAX 3   //Nombre maximum de niveaux//
#define VIE_BOSS 35      //Points de vie du boss//
#define TAILLE_BOSS 60        //Taille du boss//
#define DELAI_TIR_BOSS 60  //Délai entre les tirs du boss//
#define DELAI_APPARITION_BOSS 300
#define COULEUR_TRANS makecol(255, 0, 255)
#define NB_IMAGES_TIR 5      //5 images pour l'animation du tir//
#define NB_IMAGES_IMPACT 5       //5 images pour l'animation d'explosion//
#define DERNIERE_IMAGE_TIR 4      //Dernière image du tir//
#define DELAI_IMAGE 3          //Vitesse d'animation//

//Dimension Hitbox//
#define LARGEUR_HITBOX 20
#define HAUTEUR_HITBOX 10

//Nombre maximum de météores à l'écran//
#define MAX_METEORS 3


// Tableau pour stocker les sprites des 3 types d'ennemis//
BITMAP *sprites_ennemis[3];



typedef struct {
    float x, y; //Position du tir//
    float dx, dy; //Vecteur déplacement du tir//
    int actif; //Indique si le tir est actif//
    int tir_ennemi; // Indique si le tir est un tir ennemi//
    int puissance; // Puissance du tir => en fonction du niveau//
    int image; //Image actuelle de l'animation//
    int compteur_image; //Compteur pour la vitesse d'animation//
    BITMAP *image_tir[NB_IMAGES_TIR]; // Tableau des images d'animation pour les tirs du joueur//
    BITMAP *image_tir_ennemi[NB_IMAGES_TIR];  // Tableau pour les images des tirs ennemis//
} Tir;

typedef struct {
    float x, y; //Position de l'effet//
    int actif; //Indique si l'effet est actif//'
    int image_courante; //Image de l'animation//
    int compteur_image; //Compteur pour la vitesse d'animation//
    int impact_ennemi;  // Pour distinguer entre explosion de tir joueur/ennemi//
    BITMAP *image_impact[NB_IMAGES_IMPACT]; //Tableau des images d'animation pour les explosions de tirs//
    BITMAP *image_impact_ennemi[8];  //8 images pour les explosions des tirs ennemis//
} Effet;

typedef struct {
    float x, y;              //Position de l'ennemi//
    float dx, dy;            //Vecteur de déplacement de l'ennemi//
    int actif;               //Indique si l'ennemi est actif//
    int type_mouvement;      //Type de mouvement de l'ennemi//
    float angle;             //Angle pour les mouvements circulaires//
    float rayon;             //Rayon pour les mouvements circulaires//
    float centre_x, centre_y; //Centre de rotation pour les mouvements circulaires//
    int compteur_tir;        //Compteur pour gérer la cadence de tir//
    int vie;                 //Points de vie de l'ennemi//
    int niveau;              //Niveau de l'ennemi//
    int boss;                //Indique si c'est un boss ou un ennemi//
    int compteur_spawn;      //Pour le boss=> délai entre les apparitions d'ennemis//
} Ennemi;

typedef struct {
    float x, y;              //Position du bonus//
    int actif;               //Indique si le bonus est actif//
    float vitesse;           //Vitesse de déplacement du bonus//
    int type;                //Type de bonus: 0=double tir, 1=triple tir//
} Bonus;

typedef struct {
    float x, y;              //Position du vaisseau//
    int vies;                //Nombre de vies restantes//
    int compteur_charge;     //Compteur pour la charge du tir spécial//
    int charge;              //Indique si le joueur est en train de charger un tir//
    int double_tir;          //Indique si le bonus de double tir est actif//
    int compteur_double_tir; //Durée restante du bonus de double tir//
    int triple_tir;          //Indique si le bonus de triple tir est actif//
    int compteur_triple_tir; //Durée restante du bonus de triple tir//
    int bouclier_actif;      // Indique si le bouclier est actif//
    int energie_bouclier;    //Énergie actuelle du bouclier//
    int energie_max_bouclier; //Énergie maximale du bouclier//
    int compteur_immunite;   //Compteur d'immunité après collision//
} Vaisseau;

typedef struct {
    char pseudo[20];
    int score;
}Leaderboard;


typedef struct {
    float x, y;          //Position du météore//
    float vitesse_x;     //Vitesse horizontale du météore//
    float vitesse_y;     //Vitesse verticale du météore//
    int actif;           //Indique si le météore est actif//
    int taille;          //Taille du météore//
    float rotation;      //Angle de rotation du météore//
    float vitesse_rotation; //Vitesse de rotation du météore//
    BITMAP *sprite;      //Sprite du météore//
}Meteor;




#endif //DEFINITION_H
