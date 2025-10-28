/**************************************************************************
JEU D'ARCADE NIVEAU 1 :
            Un programme avec un vaisseau contrôlé par le joueur
            et des vaisseaux ennemis qui apparaissent et tirent
**************************************************************************/

#include <allegro.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROJECTILES_JOUEUR 20  // Nombre maximum de projectiles du joueur
#define MAX_ENNEMIS 5              // Nombre maximum d'ennemis à l'écran
#define MAX_PROJECTILES_ENNEMIS 20 // Nombre maximum de projectiles ennemis
#define ENNEMIS_POUR_BONUS 5       // Nombre d'ennemis à détruire pour faire apparaître un bonus

/****************************/
/*     STRUCTURES           */
/****************************/

// données du vaisseau du joueur
typedef struct vaisseau
{
    int largeur, hauteur;  // dimensions du vaisseau
    int x, y;              // coordonnées (du coin sup. gauche)
    int vitesse;           // vitesse de déplacement vertical
    BITMAP *image;         // image du vaisseau
    int double_tir;        // 1 si le joueur peut tirer deux projectiles, 0 sinon
} t_vaisseau;



// données d'un projectile
typedef struct projectile
{
    int x, y;              // position du projectile
    int vitesse;           // vitesse horizontale
    int largeur, hauteur;  // dimensions du projectile
    int actif;             // 1 si le projectile est actif, 0 sinon
} t_projectile;

// données d'un ennemi
typedef struct ennemi
{
    int largeur, hauteur;  // dimensions du vaisseau ennemi
    int x, y;              // coordonnées (du coin sup. gauche)
    int vitesse;           // vitesse de déplacement
    int actif;             // 1 si l'ennemi est actif, 0 sinon
    int delai_tir;         // délai entre les tirs
    int compteur_tir;      // compteur pour le prochain tir
    BITMAP *image;         // image du vaisseau ennemi
} t_ennemi;

// données du bonus
typedef struct bonus
{
    int x, y;              // position du bonus
    int largeur, hauteur;  // dimensions du bonus
    int actif;             // 1 si le bonus est actif, 0 sinon
    int vitesse;           // vitesse de déplacement
    BITMAP *image;         // image du bonus
} t_bonus;

/****************************/
/*     SOUS-PROGRAMMES      */
/****************************/

// Création et initialisation du vaisseau du joueur
t_vaisseau *creer_vaisseau()
{
    t_vaisseau *vaisseau;

    vaisseau = (t_vaisseau *)malloc(sizeof(t_vaisseau));
    if (!vaisseau) {
        allegro_message("Erreur d'allocation mémoire pour le vaisseau");
        exit(EXIT_FAILURE);
    }

    vaisseau->largeur = 50;
    vaisseau->hauteur = 30;
    vaisseau->x = 100;  // Position fixe sur l'axe horizontal (côté gauche)
    vaisseau->y = SCREEN_H/2 - vaisseau->hauteur/2;  // Centré verticalement
    vaisseau->vitesse = 5;  // Vitesse de déplacement
    vaisseau->double_tir = 0;  // Pas de double tir au début

    // Création d'une image simple pour le vaisseau
    vaisseau->image = create_bitmap(vaisseau->largeur, vaisseau->hauteur);
    if (!vaisseau->image) {
        allegro_message("Erreur de création de l'image du vaisseau");
        free(vaisseau);
        exit(EXIT_FAILURE);
    }

    // Dessiner un vaisseau triangulaire simple
    clear_to_color(vaisseau->image, makecol(255, 0, 255));  // Fond transparent (magenta)

    // Corps du vaisseau (triangle)
    int points[6] = {
        0, vaisseau->hauteur/2,                    // Pointe avant
        vaisseau->largeur, 0,                      // Coin supérieur droit
        vaisseau->largeur, vaisseau->hauteur       // Coin inférieur droit
    };
    polygon(vaisseau->image, 3, points, makecol(200, 200, 200));  // Contour

    // Remplir le vaisseau
    floodfill(vaisseau->image, vaisseau->largeur/2, vaisseau->hauteur/2, makecol(0, 150, 255));

    // Ajouter un cockpit
    circlefill(vaisseau->image, vaisseau->largeur - 15, vaisseau->hauteur/2, 5, makecol(255, 0, 0));

    return vaisseau;
}

// Initialisation du bonus
void initialiser_bonus(t_bonus *bonus)
{
    bonus->largeur = 20;
    bonus->hauteur = 20;
    bonus->actif = 0;
    bonus->vitesse = 3;

    // Création d'une image pour le bonus
    bonus->image = create_bitmap(bonus->largeur, bonus->hauteur);
    if (!bonus->image) {
        allegro_message("Erreur de création de l'image du bonus");
        exit(EXIT_FAILURE);
    }

    // Dessiner un cercle pour le bonus
    clear_to_color(bonus->image, makecol(255, 0, 255));  // Fond transparent
    circlefill(bonus->image, bonus->largeur/2, bonus->hauteur/2, bonus->largeur/2, makecol(255, 255, 0));  // Cercle jaune
    circle(bonus->image, bonus->largeur/2, bonus->hauteur/2, bonus->largeur/2, makecol(255, 128, 0));  // Contour orange
}

// Création d'un bonus
void creer_bonus(t_bonus *bonus)
{
    if (!bonus->actif) {
        bonus->x = SCREEN_W - bonus->largeur;
        bonus->y = rand() % (SCREEN_H - bonus->hauteur);
        bonus->actif = 1;
    }
}

// Mise à jour du bonus
void mettre_a_jour_bonus(t_bonus *bonus)
{
    if (bonus->actif) {
        bonus->x -= bonus->vitesse;

        // Désactiver le bonus s'il sort de l'écran
        if (bonus->x + bonus->largeur < 0) {
            bonus->actif = 0;
        }
    }
}

// Vérification des collisions entre le vaisseau et le bonus
int verifier_collision_vaisseau_bonus(t_vaisseau *vaisseau, t_bonus *bonus)
{
    if (bonus->actif) {
        if (vaisseau->x + vaisseau->largeur >= bonus->x &&
            vaisseau->x <= bonus->x + bonus->largeur &&
            vaisseau->y + vaisseau->hauteur >= bonus->y &&
            vaisseau->y <= bonus->y + bonus->hauteur) {

            // Collision détectée
            bonus->actif = 0;
            return 1;
        }
    }
    return 0;
}

// Affichage du bonus
void afficher_bonus(BITMAP *buffer, t_bonus *bonus)
{
    if (bonus->actif) {
        set_trans_blender(255, 0, 255, 128);
        draw_trans_sprite(buffer, bonus->image, bonus->x, bonus->y);
    }
}

// Libération des ressources du bonus
void detruire_bonus(t_bonus *bonus)
{
    if (bonus->image) {
        destroy_bitmap(bonus->image);
    }
}

// Initialisation des ennemis
void initialiser_ennemis(t_ennemi ennemis[], int nb_ennemis)
{
    int i;
    for (i = 0; i < nb_ennemis; i++) {
        ennemis[i].actif = 0;
        ennemis[i].largeur = 40;
        ennemis[i].hauteur = 25;
        ennemis[i].vitesse = 2 + rand() % 3;  // Vitesse entre 2 et 4
        ennemis[i].delai_tir = 50 + rand() % 100;  // Délai entre 50 et 149 frames
        ennemis[i].compteur_tir = ennemis[i].delai_tir;

        // Création d'une image simple pour l'ennemi
        ennemis[i].image = create_bitmap(ennemis[i].largeur, ennemis[i].hauteur);
        if (!ennemis[i].image) {
            allegro_message("Erreur de création de l'image de l'ennemi");
            exit(EXIT_FAILURE);
        }

        // Dessiner un vaisseau ennemi (triangle inversé)
        clear_to_color(ennemis[i].image, makecol(255, 0, 255));  // Fond transparent

        // Corps du vaisseau ennemi (triangle inversé)
        int points[6] = {
            ennemis[i].largeur, ennemis[i].hauteur/2,  // Pointe avant (droite)
            0, 0,                                      // Coin supérieur gauche
            0, ennemis[i].hauteur                      // Coin inférieur gauche
        };
        polygon(ennemis[i].image, 3, points, makecol(200, 200, 200));  // Contour

        // Remplir le vaisseau ennemi
        floodfill(ennemis[i].image, ennemis[i].largeur/2, ennemis[i].hauteur/2, makecol(255, 100, 100));

        // Ajouter un cockpit
        circlefill(ennemis[i].image, 15, ennemis[i].hauteur/2, 5, makecol(255, 0, 0));
    }
}

// Création d'un nouvel ennemi
void creer_nouvel_ennemi(t_ennemi ennemis[], int nb_ennemis)
{
    int i;

    // Chercher un emplacement d'ennemi inactif
    for (i = 0; i < nb_ennemis; i++) {
        if (!ennemis[i].actif) {
            // Positionner l'ennemi à droite de l'écran à une hauteur aléatoire
            ennemis[i].x = SCREEN_W - ennemis[i].largeur;
            ennemis[i].y = rand() % (SCREEN_H - ennemis[i].hauteur);
            ennemis[i].actif = 1;
            ennemis[i].compteur_tir = ennemis[i].delai_tir;
            break;
        }
    }
}

// Initialisation des projectiles du joueur
void initialiser_projectiles_joueur(t_projectile projectiles[], int nb_projectiles)
{
    int i;
    for (i = 0; i < nb_projectiles; i++) {
        projectiles[i].actif = 0;
        projectiles[i].largeur = 10;
        projectiles[i].hauteur = 4;
        projectiles[i].vitesse = 8;
    }
}

// Initialisation des projectiles ennemis
// Initialisation des projectiles ennemis
void initialiser_projectiles_ennemis(t_projectile projectiles[], int nb_projectiles)
{
    int i;
    for (i = 0; i < nb_projectiles; i++) {
        projectiles[i].actif = 0;
        projectiles[i].largeur = 10;
        projectiles[i].hauteur = 4;
        projectiles[i].vitesse = -6;  // Vitesse négative (vers la gauche)
    }
}

// Création d'un nouveau projectile du joueur
void tirer_projectile_joueur(t_projectile projectiles[], int nb_projectiles, t_vaisseau *vaisseau)
{
    int i;
    int projectiles_tires = 0;

    // Chercher un ou deux projectiles inactifs selon le mode de tir
    for (i = 0; i < nb_projectiles && projectiles_tires < (vaisseau->double_tir ? 2 : 1); i++) {
        if (!projectiles[i].actif) {
            // Positionner le projectile à la pointe du vaisseau
            projectiles[i].x = vaisseau->x + vaisseau->largeur;

            if (vaisseau->double_tir && projectiles_tires == 0) {
                // Premier projectile (en haut)
                projectiles[i].y = vaisseau->y + vaisseau->hauteur/4 - projectiles[i].hauteur/2;
            } else if (vaisseau->double_tir && projectiles_tires == 1) {
                // Deuxième projectile (en bas)
                projectiles[i].y = vaisseau->y + 3*vaisseau->hauteur/4 - projectiles[i].hauteur/2;
            } else {
                // Tir normal (au milieu)
                projectiles[i].y = vaisseau->y + vaisseau->hauteur/2 - projectiles[i].hauteur/2;
            }

            projectiles[i].actif = 1;
            projectiles_tires++;
        }
    }
}

// Création d'un nouveau projectile ennemi
void tirer_projectile_ennemi(t_projectile projectiles[], int nb_projectiles, t_ennemi *ennemi)
{
    int i;

    // Chercher un projectile inactif
    for (i = 0; i < nb_projectiles; i++) {
        if (!projectiles[i].actif) {
            // Positionner le projectile à la pointe du vaisseau ennemi
            projectiles[i].x = ennemi->x;
            projectiles[i].y = ennemi->y + ennemi->hauteur/2 - projectiles[i].hauteur/2;
            projectiles[i].actif = 1;
            break;  // Un seul projectile à la fois
        }
    }
}

// Mise à jour des projectiles du joueur
void mettre_a_jour_projectiles_joueur(t_projectile projectiles[], int nb_projectiles)
{
    int i;

    for (i = 0; i < nb_projectiles; i++) {
        if (projectiles[i].actif) {
            // Déplacer le projectile
            projectiles[i].x += projectiles[i].vitesse;

            // Désactiver le projectile s'il sort de l'écran
            if (projectiles[i].x > SCREEN_W) {
                projectiles[i].actif = 0;
            }
        }
    }
}

// Mise à jour des projectiles ennemis
void mettre_a_jour_projectiles_ennemis(t_projectile projectiles[], int nb_projectiles)
{
    int i;

    for (i = 0; i < nb_projectiles; i++) {
        if (projectiles[i].actif) {
            // Déplacer le projectile
            projectiles[i].x += projectiles[i].vitesse;

            // Désactiver le projectile s'il sort de l'écran
            if (projectiles[i].x < 0) {
                projectiles[i].actif = 0;
            }
        }
    }
}

// Mise à jour des ennemis
void mettre_a_jour_ennemis(t_ennemi ennemis[], int nb_ennemis, t_projectile projectiles_ennemis[], int nb_projectiles_ennemis, t_vaisseau *vaisseau)
{
    int i;

    for (i = 0; i < nb_ennemis; i++) {
        if (ennemis[i].actif) {
            // Déplacer l'ennemi vers le joueur sur l'axe vertical
            if (ennemis[i].y + ennemis[i].hauteur/2 < vaisseau->y + vaisseau->hauteur/2) {
                // Le joueur est en dessous de l'ennemi
                ennemis[i].y += ennemis[i].vitesse;
            } else if (ennemis[i].y + ennemis[i].hauteur/2 > vaisseau->y + vaisseau->hauteur/2) {
                // Le joueur est au-dessus de l'ennemi
                ennemis[i].y -= ennemis[i].vitesse;
            }

            // Limites de l'écran
            if (ennemis[i].y < 0) {
                ennemis[i].y = 0;
            }
            if (ennemis[i].y + ennemis[i].hauteur > SCREEN_H) {
                ennemis[i].y = SCREEN_H - ennemis[i].hauteur;
            }

            // Gestion du tir
            ennemis[i].compteur_tir--;
            if (ennemis[i].compteur_tir <= 0) {
                tirer_projectile_ennemi(projectiles_ennemis, nb_projectiles_ennemis, &ennemis[i]);
                ennemis[i].compteur_tir = ennemis[i].delai_tir;
            }
        }
    }
}

// Vérification des collisions entre projectiles du joueur et ennemis
int verifier_collisions_projectiles_joueur_ennemis(t_projectile projectiles[], int nb_projectiles, t_ennemi ennemis[], int nb_ennemis)
{
    int i, j;
    int ennemis_detruits = 0;

    for (i = 0; i < nb_projectiles; i++) {
        if (projectiles[i].actif) {
            for (j = 0; j < nb_ennemis; j++) {
                if (ennemis[j].actif) {
                    // Vérifier si le projectile touche l'ennemi
                    if (projectiles[i].x + projectiles[i].largeur >= ennemis[j].x &&
                        projectiles[i].x <= ennemis[j].x + ennemis[j].largeur &&
                        projectiles[i].y + projectiles[i].hauteur >= ennemis[j].y &&
                        projectiles[i].y <= ennemis[j].y + ennemis[j].hauteur) {

                        // Collision détectée
                        projectiles[i].actif = 0;
                        ennemis[j].actif = 0;
                        ennemis_detruits++;
                        break;
                    }
                }
            }
        }
    }

    return ennemis_detruits;
}

// Vérification des collisions entre projectiles ennemis et joueur
int verifier_collisions_projectiles_ennemis_joueur(t_projectile projectiles[], int nb_projectiles, t_vaisseau *vaisseau)
{
    int i;

    for (i = 0; i < nb_projectiles; i++) {
        if (projectiles[i].actif) {
            // Vérifier si le projectile touche le joueur
            if (projectiles[i].x + projectiles[i].largeur >= vaisseau->x &&
                projectiles[i].x <= vaisseau->x + vaisseau->largeur &&
                projectiles[i].y + projectiles[i].hauteur >= vaisseau->y &&
                projectiles[i].y <= vaisseau->y + vaisseau->hauteur) {

                // Collision détectée
                return 1;
            }
        }
    }

    return 0;
}

// Affichage des projectiles
void afficher_projectiles(BITMAP *buffer, t_projectile projectiles[], int nb_projectiles, int couleur)
{
    int i;

    for (i = 0; i < nb_projectiles; i++) {
        if (projectiles[i].actif) {
            // Dessiner le projectile (un simple rectangle)
            rectfill(buffer,
                    projectiles[i].x,
                    projectiles[i].y,
                    projectiles[i].x + projectiles[i].largeur,
                    projectiles[i].y + projectiles[i].hauteur,
                    couleur);
        }
    }
}

// Affichage des ennemis
void afficher_ennemis(BITMAP *buffer, t_ennemi ennemis[], int nb_ennemis)
{
    int i;

    for (i = 0; i < nb_ennemis; i++) {
        if (ennemis[i].actif) {
            // Définir la couleur magenta comme transparente
            set_trans_blender(255, 0, 255, 128);
            draw_trans_sprite(buffer, ennemis[i].image, ennemis[i].x, ennemis[i].y);
        }
    }
}

// Déplacement du vaisseau avec les flèches (uniquement vertical)
void deplacer_vaisseau(t_vaisseau *vaisseau)
{
    // Déplacement vers le haut
    if (key[KEY_UP]) {
        vaisseau->y -= vaisseau->vitesse;
    }

    // Déplacement vers le bas
    if (key[KEY_DOWN]) {
        vaisseau->y += vaisseau->vitesse;
    }

    // Limites de l'écran (empêcher le vaisseau de sortir)
    if (vaisseau->y < 0) {
        vaisseau->y = 0;
    }
    if (vaisseau->y + vaisseau->hauteur > SCREEN_H) {
        vaisseau->y = SCREEN_H - vaisseau->hauteur;
    }
}

// Affichage du vaisseau
void afficher_vaisseau(BITMAP *buffer, t_vaisseau *vaisseau)
{
    // Définir la couleur magenta comme transparente
    set_trans_blender(255, 0, 255, 128);
    draw_trans_sprite(buffer, vaisseau->image, vaisseau->x, vaisseau->y);

    // Afficher un indicateur de double tir si actif
    if (vaisseau->double_tir) {
        circlefill(buffer, vaisseau->x + 10, vaisseau->y - 10, 5, makecol(255, 255, 0));
    }
}

// Libération des ressources du vaisseau
void detruire_vaisseau(t_vaisseau *vaisseau)
{
    if (vaisseau) {
        if (vaisseau->image) {
            destroy_bitmap(vaisseau->image);
        }
        free(vaisseau);
    }
}

// Libération des ressources des ennemis
void detruire_ennemis(t_ennemi ennemis[], int nb_ennemis)
{
    int i;

    for (i = 0; i < nb_ennemis; i++) {
        if (ennemis[i].image) {
            destroy_bitmap(ennemis[i].image);
        }
    }
}

/******************************************/
/* PROGRAMME PRINCIPAL                    */
/******************************************/

int main()
{
    t_vaisseau *vaisseau;                                  // Le vaisseau du joueur
    t_ennemi ennemis[MAX_ENNEMIS];                         // Tableau d'ennemis
    t_projectile projectiles_joueur[MAX_PROJECTILES_JOUEUR]; // Projectiles du joueur
    t_projectile projectiles_ennemis[MAX_PROJECTILES_ENNEMIS]; // Projectiles des ennemis
    t_bonus bonus;                                         // Bonus de double tir
    BITMAP *buffer;                                        // Buffer d'affichage
    int quitter = 0;                                       // Flag pour quitter le jeu
    int touche_espace_pressee = 0;                         // Pour éviter les tirs multiples
    int compteur_apparition_ennemis = 0;                   // Compteur pour l'apparition des ennemis
    int delai_apparition_ennemis = 100;                    // Délai entre l'apparition des ennemis
    int game_over = 0;                                     // Flag de fin de jeu
    int ennemis_detruits = 0;                              // Compteur d'ennemis détruits
    int score = 0;                                         // Score du joueur

    // Initialisation d'Allegro
    allegro_init();
    install_keyboard();
    install_timer();

    // Initialisation du générateur aléatoire
    srand(time(NULL));

    // Configuration du mode graphique
    set_color_depth(desktop_color_depth());
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0) != 0) {
        allegro_message("Erreur lors de l'initialisation du mode graphique");
        return 1;
    }

    // Création du buffer d'affichage
    buffer = create_bitmap(SCREEN_W, SCREEN_H);
    if (!buffer) {
        allegro_message("Erreur lors de la création du buffer");
        return 1;
    }

    // Création du vaisseau
    vaisseau = creer_vaisseau();

    // Initialisation des ennemis
    initialiser_ennemis(ennemis, MAX_ENNEMIS);

    // Initialisation des projectiles
    initialiser_projectiles_joueur(projectiles_joueur, MAX_PROJECTILES_JOUEUR);
    initialiser_projectiles_ennemis(projectiles_ennemis, MAX_PROJECTILES_ENNEMIS);

    // Initialisation du bonus
    initialiser_bonus(&bonus);

    // Boucle principale du jeu
    while (!quitter && !game_over) {
        // Gestion des entrées
        if (key[KEY_ESC]) {
            quitter = 1;
        }

        // Gestion du tir (espace)
        if (key[KEY_SPACE] && !touche_espace_pressee) {
            tirer_projectile_joueur(projectiles_joueur, MAX_PROJECTILES_JOUEUR, vaisseau);
            touche_espace_pressee = 1;
        }
        if (!key[KEY_SPACE]) {
            touche_espace_pressee = 0;
        }

        // Gestion de l'apparition des ennemis
        compteur_apparition_ennemis--;
        if (compteur_apparition_ennemis <= 0) {
            creer_nouvel_ennemi(ennemis, MAX_ENNEMIS);
            compteur_apparition_ennemis = delai_apparition_ennemis;
        }

        // Effacer le buffer
        clear_bitmap(buffer);

        // Mettre à jour la position du vaisseau
        deplacer_vaisseau(vaisseau);

        // Mettre à jour les ennemis
        mettre_a_jour_ennemis(ennemis, MAX_ENNEMIS, projectiles_ennemis, MAX_PROJECTILES_ENNEMIS, vaisseau);

        // Mettre à jour les projectiles
        mettre_a_jour_projectiles_joueur(projectiles_joueur, MAX_PROJECTILES_JOUEUR);
        mettre_a_jour_projectiles_ennemis(projectiles_ennemis, MAX_PROJECTILES_ENNEMIS);

        // Mettre à jour le bonus
        mettre_a_jour_bonus(&bonus);

        // Vérifier les collisions
        int nouveaux_ennemis_detruits = verifier_collisions_projectiles_joueur_ennemis(
            projectiles_joueur, MAX_PROJECTILES_JOUEUR, ennemis, MAX_ENNEMIS);

        ennemis_detruits += nouveaux_ennemis_detruits;
        score += nouveaux_ennemis_detruits * 100;  // 100 points par ennemi détruit

        // Vérifier si on doit créer un bonus (tous les 5 ennemis détruits)
        if (ennemis_detruits >= ENNEMIS_POUR_BONUS && !bonus.actif) {
            creer_bonus(&bonus);
            ennemis_detruits = 0;  // Réinitialiser le compteur
        }

        // Vérifier si le joueur attrape le bonus
        if (verifier_collision_vaisseau_bonus(vaisseau, &bonus)) {
            vaisseau->double_tir = 1;  // Activer le double tir
        }

        if (verifier_collisions_projectiles_ennemis_joueur(projectiles_ennemis, MAX_PROJECTILES_ENNEMIS, vaisseau)) {
            game_over = 1;  // Le joueur a été touché, fin du jeu
        }

        // Afficher le vaisseau
        afficher_vaisseau(buffer, vaisseau);

        // Afficher les ennemis
        afficher_ennemis(buffer, ennemis, MAX_ENNEMIS);

        // Afficher les projectiles
        afficher_projectiles(buffer, projectiles_joueur, MAX_PROJECTILES_JOUEUR, makecol(0, 255, 0));  // Projectiles du joueur en vert
        afficher_projectiles(buffer, projectiles_ennemis, MAX_PROJECTILES_ENNEMIS, makecol(255, 0, 0));  // Projectiles ennemis en rouge

        // Afficher le bonus
        afficher_bonus(buffer, &bonus);

        // Afficher les instructions et le score
        textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), -1,
                     "Utilisez les flèches HAUT et BAS pour déplacer le vaisseau");
        textprintf_ex(buffer, font, 10, 30, makecol(255, 255, 255), -1,
                     "Appuyez sur ESPACE pour tirer");
        textprintf_ex(buffer, font, 10, 50, makecol(255, 255, 255), -1,
                     "Appuyez sur ESC pour quitter");
        textprintf_ex(buffer, font, 10, 70, makecol(255, 255, 0), -1,
                     "Score: %d", score);

        // Afficher l'état du double tir
        if (vaisseau->double_tir) {
            textprintf_ex(buffer, font, 10, 90, makecol(255, 255, 0), -1,
                         "Double tir activé!");
        }

        // Afficher le message de game over si nécessaire
        if (game_over) {
            textprintf_centre_ex(buffer, font, SCREEN_W/2, SCREEN_H/2, makecol(255, 0, 0), -1,
                               "GAME OVER - Votre vaisseau a été détruit!");
            textprintf_centre_ex(buffer, font, SCREEN_W/2, SCREEN_H/2 + 20, makecol(255, 255, 255), -1,
                               "Score final: %d", score);
            textprintf_centre_ex(buffer, font, SCREEN_W/2, SCREEN_H/2 + 40, makecol(255, 255, 255), -1,
                               "Appuyez sur ESC pour quitter");
        }

        // Afficher le buffer à l'écran
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        // Petite pause pour limiter la vitesse du jeu
        rest(16);  // Environ 60 FPS
    }

    // Libération des ressources
    detruire_vaisseau(vaisseau);
    detruire_ennemis(ennemis, MAX_ENNEMIS);
    detruire_bonus(&bonus);
    destroy_bitmap(buffer);
    allegro_exit();

    return 0;
}
END_OF_MAIN();
