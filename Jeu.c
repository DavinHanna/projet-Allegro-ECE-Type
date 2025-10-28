//
// Created by Mazouz Ilias on 16/05/2025.
//

#include <allegro.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "definition.h"
#include "Graphique.h"
#include "Animation.h"
#include "Vaisseau.h"


void initialiser_allegro(void) {
    //Initialise Allegro et configure le mode graphique//
    allegro_init();
    install_keyboard();
    install_timer();
    set_color_depth(32);
    srand(time(NULL));

    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, LARGEUR, HAUTEUR, 0, 0) != 0) {
        allegro_message("Erreur : %s", allegro_error);
        exit(1);
    }
}


BITMAP* initialiser_buffer(void) {
    //Crée un bitmap buffer//
    BITMAP *buffer = create_bitmap(LARGEUR, HAUTEUR);
    if (!buffer) {
        allegro_message("Erreur création ");
        exit(1);
    }
    return buffer;
}



void charger_ressources_jeu(BITMAP **sprite_vaisseau, BITMAP *sprites_ennemis[], BITMAP **fond) {
    // Charge les sprites du vaisseau, des ennemis et le fond du jeu//
    // Gère les erreurs de chargement et effectue les transformations nécessaires//
    *sprite_vaisseau = lancer_sprite("Ship.bmp", 1);
    if (!*sprite_vaisseau) {
        allegro_message("Erreur: Impossible de charger Ship.bmp");
        allegro_exit();
        exit(1);
    }

    //Charger les sprites des ennemis pour chaque niveau//
    BITMAP *sprite_temp;

    // Niveau 1 : Ship4
    sprite_temp = lancer_sprite("Ship4.bmp", 1);
    if (sprite_temp) {
        sprites_ennemis[0] = flip(sprite_temp);
        destroy_bitmap(sprite_temp);
    } else {
        sprites_ennemis[0] = NULL;
    }

    // Niveau 2 : Ship3
    sprite_temp = lancer_sprite("Ship3.bmp", 1);
    if (sprite_temp) {
        sprites_ennemis[1] = flip(sprite_temp);
        destroy_bitmap(sprite_temp);
    } else {
        sprites_ennemis[1] = NULL;
    }

    // Niveau 3 : Ship2
    sprite_temp = lancer_sprite("Ship2.bmp", 1);
    if (sprite_temp) {
        sprites_ennemis[2] = flip(sprite_temp);
        destroy_bitmap(sprite_temp);
    } else {
        sprites_ennemis[2] = NULL;
    }

    //Vérifier si les sprites ont été chargés correctement//
    for (int i = 0; i < 3; i++) {
        if (!sprites_ennemis[i]) {
            char nom_fichier[20];
            sprintf(nom_fichier, "Ship%d.bmp", 4 - i);
            allegro_message("Erreur: Impossible de charger %s", nom_fichier);
        }
    }

    //Charger le fond//
    *fond = load_bitmap("fond1.bmp", NULL);
    if (!*fond) {
        allegro_message("Erreur: Impossible de charger fond1.bmp");
    }
    if (*fond && (*fond)->h != HAUTEUR) {
        BITMAP *fond_redimensionne = create_bitmap(LARGEUR, HAUTEUR);
        stretch_blit(*fond, fond_redimensionne, 0, 0, (*fond)->w, (*fond)->h, 0, 0, LARGEUR, HAUTEUR);
        destroy_bitmap(*fond);
        *fond = fond_redimensionne;
    }
}


void initialiser_jeu(Vaisseau *vaisseau, Tir tirs[], Effet effets[], Ennemi ennemis[], Bonus *bonus, Meteor meteors[]) {
    // Initialise toutes les structures de données du jeu et charge les animations
    initialiser_vaisseau(vaisseau);
    initialiser_tirs(tirs);
    initialiser_effets_impact(effets);
    initialiser_ennemis(ennemis);
    initialiser_bonus(bonus);
    initialiser_meteors(meteors);

    // Charger les animations
    charger_animation_tir(tirs);
    charger_images_impact(effets);
    charger_animation_tir_ennemi(tirs);
    charger_impact_ennemi(effets);
    charger_sprites_meteors(meteors);
}


void gerer_controles(Vaisseau *vaisseau, Tir tirs[], Effet effets[], int *espace_relache) {
    // Gère les contrôles du joueur: déplacements, tirs et utilisation du bouclier//
    // Implémente le système de tir chargé avec 3 niveaux de puissance//

    if (key[KEY_UP] && vaisseau->y > TAILLE_VAISSEAU/2) {
        vaisseau->y -= 5;
    }
    if (key[KEY_DOWN] && vaisseau->y < HAUTEUR - TAILLE_VAISSEAU/2) {
        vaisseau->y += 5;
    }
    if (key[KEY_LEFT] && vaisseau->x > TAILLE_VAISSEAU/2) {
        vaisseau->x -= 5;
    }
    if (key[KEY_RIGHT] && vaisseau->x < LARGEUR - TAILLE_VAISSEAU) {
        vaisseau->x += 5;
    }

    //Gestion du bouclier//
    update_bouclier(vaisseau);

    //Mise à jour du compteur d'immunité//
    if (vaisseau->compteur_immunite > 0) {
        vaisseau->compteur_immunite--;
    }

    //Gestion tir double//
    if (vaisseau->double_tir) {
        vaisseau->compteur_double_tir--;
        if (vaisseau->compteur_double_tir <= 0) {
            vaisseau->double_tir = 0;
        }
    }

    //Gestion tir triple//
    if (vaisseau->triple_tir) {
        vaisseau->compteur_triple_tir--;
        if (vaisseau->compteur_triple_tir <= 0) {
            vaisseau->triple_tir = 0;
        }
    }

    //Tir chargé//
    if (key[KEY_SPACE]) {
        if (*espace_relache) {
            vaisseau->charge = 1;
            vaisseau->compteur_charge = 0;
            *espace_relache = 0;
        } else {
            if (vaisseau->charge) {
                vaisseau->compteur_charge++;
            }
        }
    } else {
        if (!*espace_relache) {
            *espace_relache = 1;

            if (vaisseau->charge) {
                int niveau_puissance = 0;
                if (vaisseau->compteur_charge >= NIVEAU_2) {
                    niveau_puissance = 2;
                } else if (vaisseau->compteur_charge >= NIVEAU_1) {
                    niveau_puissance = 1;
                }

                tirer_tir_charge(tirs, vaisseau->x + TAILLE_VAISSEAU, vaisseau->y, niveau_puissance, vaisseau->double_tir, vaisseau->triple_tir, effets);
                vaisseau->charge = 0;
            }
        }
    }
}

void update_jeu(Tir tirs[], Effet effets[], Ennemi ennemis[], Vaisseau *vaisseau, Bonus *bonus, Meteor meteors[], int *score, int *ennemis_tues, int *ennemis_pour_vie) {
    //Met à jour tous les éléments du jeu//
    // Mise à jour des tirs et effets/
    update_tirs(tirs, effets);
    update_effets_impact(effets);

    //Mise à jour des ennemis et bonus//
    update_ennemis(ennemis, tirs, effets);
    update_bonus(bonus);

    //Mise à jour des météores//
    update_meteors(meteors);

    // Gestion des collisions
    int touches = verifier_collisions(tirs, ennemis, vaisseau, bonus, effets);
    verifier_collision_meteors(meteors, vaisseau, effets);

    // Mise à jour du score et compteurs
    *score += touches * 100;
    *ennemis_tues += touches;

    // Gestion des bonus de vie
    if (*ennemis_tues >= *ennemis_pour_vie) {
        vaisseau->vies++;
        *ennemis_pour_vie += 15; // Augmenter le seuil pour la prochaine vie
    }

    // Gestion du bouclier
    update_bouclier(vaisseau);

    // Gestion de l'immunité
    if (vaisseau->compteur_immunite > 0) {
        vaisseau->compteur_immunite--;
    }

    // Gestion des bonus de tir
    if (vaisseau->double_tir) {
        if (--vaisseau->compteur_double_tir <= 0) {
            vaisseau->double_tir = 0;
        }
    }

    if (vaisseau->triple_tir) {
        if (--vaisseau->compteur_triple_tir <= 0) {
            vaisseau->triple_tir = 0;
        }
    }

    // Apparition aléatoire de météores (1% de chance par frame)
    if (rand() % 100 == 0) {
        apparition_meteor(meteors);
    }
}


int compter_ennemis_actifs(Ennemi ennemis[], int *boss_vivant) {
    // Fonction pour compter les ennemis actifs et vérifier si le boss est vivant//
    int ennemis_actifs = 0;
    *boss_vivant = 0;

    for (int i = 0; i < MAX_ENNEMIS; i++) {
        if (ennemis[i].actif) {
            ennemis_actifs++;
            if (ennemis[i].boss) {
                *boss_vivant = 1;
            }
        }
    }

    return ennemis_actifs;
}






void gerer_niveau(int *timer_niveau, int *niveau_actuel, int *generer_ennemis, int *niveau_termine,
                 int *boss_genere, Ennemi ennemis[], BITMAP *buffer, int niveau_choisi, int *score,
                 Vaisseau *vaisseau, int *partie_terminee, int *ennemis_actifs) {
    //Fonction pour gérer le niveau et sa progression//
    //Incrémenter le timer du niveau//
    (*timer_niveau)++;

    //Après 30 secondes, arrêter l'apparition d'ennemis//
    if (*timer_niveau >= DUREE_NIVEAU && *generer_ennemis) {
        *generer_ennemis = 0;

        //Au niveau 3, faire apparaître le boss quand le temps est écoulé//
        if (*niveau_actuel == 3 && !*boss_genere) {
            //Vérifier s'il y a déjà un boss//
            int boss_existe = 0;
            for (int i = 0; i < MAX_ENNEMIS; i++) {
                if (ennemis[i].actif && ennemis[i].boss) {
                    boss_existe = 1;
                    break;
                }
            }

            if (!boss_existe) {
                apparition_boss(ennemis);
                *boss_genere = 1;

                //Afficher un message d'avertissement//
                textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR/2,
                                   makecol(255, 0, 0), makecol(0, 0, 0),
                                   "ATTENTION! BOSS EN APPROCHE!");
                blit(buffer, screen, 0, 0, 0, 0, LARGEUR, HAUTEUR);
                rest(2000); // Pause de 2 secondes
            }
        }
    }

    //Si le boss a été vaincu ou tous les ennemis sont éliminés après la fin du temps de niveau//
    if (!*generer_ennemis && *ennemis_actifs == 0 && !*niveau_termine) {
        *niveau_termine = 1;

        //Passer au niveau suivant//
        if (*niveau_actuel < NIVEAU_MAX && niveau_choisi != 0) {
            //Afficher l'écran de fin de niveau//
            dessiner_niveau_termine(*niveau_actuel, *score, buffer);
            rest(3000);

            (*niveau_actuel)++;
            *timer_niveau = 0;
            *generer_ennemis = 1;
            *niveau_termine = 0;
            *boss_genere = 0;

            //Bonus pour avoir complété le niveau//
            vaisseau->vies = vaisseau->vies < 5 ? vaisseau->vies + 1 : 5; //Vie supplémentaire//
            vaisseau->energie_bouclier = vaisseau->energie_max_bouclier; //Recharge complète du bouclier//

        } else if ((*niveau_actuel == NIVEAU_MAX || niveau_choisi == 0) && *boss_genere) {
            // Le joueur a terminé le dernier niveau et vaincu le boss
            textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR/2 - 40,
                               makecol(0, 255, 0), -1, "FÉLICITATIONS!");
            textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR/2,
                               makecol(255, 255, 255), -1, "Vous avez vaincu le boss final!");
            textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR/2 + 40,
                               makecol(255, 255, 0), -1, "Score final: %d", *score);
            blit(buffer, screen, 0, 0, 0, 0, LARGEUR, HAUTEUR);
            // Sauvegarder le score dans le leaderboard//
            rest(5000);
            *partie_terminee = 1;
        }
    }
}

void gerer_apparition_ennemis(int *timer_apparition_ennemi, Ennemi ennemis[], int niveau_actuel, int generer_ennemis) {
    //Apparition des ennemis seulement si autorisé//
    if (generer_ennemis && ++(*timer_apparition_ennemi) > 60 && rand() % 100 < 60) {
        *timer_apparition_ennemi = 0;
        apparition_ennemi(ennemis, niveau_actuel);
    }
}

void dessiner_jeu(BITMAP *buffer, BITMAP *fond, int position_fond, Vaisseau *vaisseau,
                 BITMAP *sprite_vaisseau, Tir tirs[], Effet effets[], Ennemi ennemis[],
                 BITMAP *sprites_ennemis[], Bonus *bonus, Meteor meteors[], int score, char *pseudo,
                 int niveau_choisi, int niveau_actuel, int timer_niveau, int generer_ennemis,
                 int ennemis_actifs, int niveau_termine, int boss_vivant, int ennemis_pour_vie) {
    //Dessiner le jeu dans le buffer//
    clear_bitmap(buffer);
    //Dessiner le fond//
    if (fond) {
        dessiner_fond(buffer, fond, position_fond);
    }
    //Dessiner les éléments du jeu//

    // Dessiner les météores avant le vaisseau pour qu'ils apparaissent derrière
    dessiner_meteors(buffer, meteors);

    dessiner_vaisseau(vaisseau, buffer, sprite_vaisseau);
    dessiner_tirs(tirs, buffer);
    dessiner_effets_impact(effets, buffer);
    dessiner_ennemis(ennemis, buffer, sprites_ennemis);
    dessiner_bonus(bonus, buffer);
    dessiner_barre_charge(vaisseau, buffer);
    dessiner_barre_bouclier(vaisseau, buffer);
    dessiner_vaisseau(vaisseau, buffer, sprite_vaisseau);

    //Afficher le score//
    textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), -1,
                "Score: %d", score);
    dessiner_vies(vaisseau, buffer);

    //Afficher le pseudo du joueur//
    textprintf_ex(buffer, font, LARGEUR - 150, 10, makecol(255, 255, 255), -1,
                "Joueur: %s", pseudo);

    //Afficher le niveau actuel//
    textprintf_ex(buffer, font, LARGEUR - 150, 30, makecol(255, 255, 255), -1,
                "Niveau: %s", niveau_choisi == 0 ? "Boss" : (char[]){niveau_actuel + '0', '\0'});

    //Afficher le temps restant avant la fin du niveau//
    if (generer_ennemis) {
        int secondes_restantes = (DUREE_NIVEAU - timer_niveau) / 60;
        textprintf_ex(buffer, font, LARGEUR - 150, 50, makecol(255, 255, 255), -1,
                    "Temps: %d", secondes_restantes);
    } else if (ennemis_actifs > 0) {
        if (boss_vivant) {
            textprintf_centre_ex(buffer, font, LARGEUR / 2, 30, makecol(255, 0, 0), -1,
                        "BOSS FIGHT!");
        } else {
            textprintf_centre_ex(buffer, font, LARGEUR / 2, 30, makecol(255, 200, 0), -1,
                        "Éliminez tous les ennemis!");
            textprintf_ex(buffer, font, LARGEUR - 150, 50, makecol(255, 200, 0), -1,
                        "Restants: %d", ennemis_actifs);
        }
    } else if (niveau_actuel == NIVEAU_MAX && niveau_termine) {
        textprintf_centre_ex(buffer, font, LARGEUR / 2, 30, makecol(0, 255, 0), -1,
                    "Félicitations! Vous avez terminé tous les niveaux!");
    } else if (niveau_termine) {
        textprintf_centre_ex(buffer, font, LARGEUR / 2, 30, makecol(0, 255, 0), -1,
                    "Niveau %d terminé! Passage au niveau %d", niveau_actuel, niveau_actuel + 1);
    }

    //Afficher le statut du double tir//
    if (vaisseau->double_tir) {
        textprintf_ex(buffer, font, 10, 70, makecol(255, 255, 0), -1,
                    "Double tir: %d", vaisseau->compteur_double_tir / 60 + 1);
    }

    //Afficher le statut du triple tir//
    if (vaisseau->triple_tir) {
        textprintf_ex(buffer, font, 10, 90, makecol(0, 255, 255), -1,
                    "Triple tir: %d", vaisseau->compteur_triple_tir / 60 + 1);
    }

    //Afficher le compteur pour la prochaine vie//
    textprintf_ex(buffer, font, 10, 110, makecol(255, 100, 100), -1,
                "Prochaine vie: %d/%d", ennemis_pour_vie, ENNEMIS_POUR_VIE);

    blit(buffer, screen, 0, 0, 0, 0, LARGEUR, HAUTEUR);
}



void liberer(BITMAP *buffer, BITMAP *sprite_vaisseau, BITMAP *sprites_ennemis[], BITMAP *fond) {
    //Libération de la mémoire//
    for (int i = 0; i < 3; i++) {
        if (sprites_ennemis[i]) destroy_bitmap(sprites_ennemis[i]);
    }
    if (fond) destroy_bitmap(fond);
    destroy_bitmap(sprite_vaisseau);
    destroy_bitmap(buffer);
}





void sauvegarder_score(char *pseudo, int score) {
    //Sauvegarde le score du joueur dans le fichier de leaderboard//
    FILE *fichier;
    Leaderboard leaderboard[10]; //Maximum 10 entrées dans le leaderboard//
    int nb_scores = 0;

    //Charger les scores existants//
    fichier = fopen("leaderboard.dat", "rb");
    if (fichier) {
        nb_scores = fread(leaderboard, sizeof(Leaderboard), 10, fichier);
        fclose(fichier);
    }

    //Vérifier si le score mérite d'être dans le leaderboard//
    int position = -1;
    for (int i = 0; i < nb_scores; i++) {
        if (score > leaderboard[i].score) {
            position = i;
            break;
        }
    }

    //Si le score est trop bas et que le leaderboard est plein//
    if (position == -1 && nb_scores >= 10) {
        return;
    }

    //Si le leaderboard n'est pas plein, ajouter à la fin//
    if (position == -1 && nb_scores < 10) {
        position = nb_scores;
        nb_scores++;
    }

    //Décaler les scores inférieurs//
    for (int i = nb_scores - 1; i > position; i--) {
        strcpy(leaderboard[i].pseudo, leaderboard[i-1].pseudo);
        leaderboard[i].score = leaderboard[i-1].score;
    }

    //Insérer le nouveau score//
    strcpy(leaderboard[position].pseudo, pseudo);
    leaderboard[position].score = score;

    //Sauvegarder le leaderboard mis à jour//
    fichier = fopen("leaderboard.dat", "wb");
    if (fichier) {
        fwrite(leaderboard, sizeof(Leaderboard), nb_scores, fichier);
        fclose(fichier);
    }
}

void charger_leaderboard(char pseudo_list[][20], int scores[], int *nb_scores) {
    //Charge les données du leaderboard depuis le fichier//
    FILE *fichier;
    Leaderboard leaderboard[10];
    *nb_scores = 0;

    fichier = fopen("leaderboard.dat", "rb");
    if (fichier) {
        *nb_scores = fread(leaderboard, sizeof(Leaderboard), 10, fichier);
        fclose(fichier);

        // Copier les données dans les tableaux fournis
        for (int i = 0; i < *nb_scores; i++) {
            strcpy(pseudo_list[i], leaderboard[i].pseudo);
            scores[i] = leaderboard[i].score;
        }
    }
}

void afficher_leaderboard(BITMAP *buffer) {
    //Affiche le leaderboard dans le buffer//
    char pseudo_list[10][20];
    int scores[10];
    int nb_scores = 0;

    //Charger les données du leaderboard//
    charger_leaderboard(pseudo_list, scores, &nb_scores);

    //Dessiner le fond du leaderboard//
    rectfill(buffer, LARGEUR/4, HAUTEUR/6, LARGEUR*3/4, HAUTEUR*5/6, makecol(0, 0, 100));
    rect(buffer, LARGEUR/4, HAUTEUR/6, LARGEUR*3/4, HAUTEUR*5/6, makecol(255, 255, 255));

    // Titre
    textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR/6 + 20,
                       makecol(255, 255, 0), -1, "MEILLEURS SCORES");

    //Afficher les entrées du leaderboard//
    for (int i = 0; i < nb_scores; i++) {
        int y = HAUTEUR/6 + 60 + i * 30;
        textprintf_ex(buffer, font, LARGEUR/4 + 20, y,
                    makecol(255, 255, 255), -1, "%d. %s", i+1, pseudo_list[i]);
        textprintf_right_ex(buffer, font, LARGEUR*3/4 - 20, y,
                          makecol(255, 255, 0), -1, "%d", scores[i]);
    }

    //Message pour revenir au menu//
    textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR*5/6 - 30,
                       makecol(200, 200, 200), -1, "Appuyez sur une touche pour revenir au menu");

    blit(buffer, screen, 0, 0, 0, 0, LARGEUR, HAUTEUR);

    //Attendre qu'une touche soit pressée//
    readkey();
}


int gerer_menu(BITMAP *buffer, int *niveau_choisi, char *pseudo) {
    //Gère l'interface du menu principal et les interactions utilisateur//
    //Retourne 1 si le joueur commence une partie, 0 pour quitter//
    int jeu = 0;
    int selection_menu = 0;
    int saisie_pseudo = 0;
    int affichage_leaderboard = 0;

    while (!key[KEY_ESC]) {
        if (saisie_pseudo) {
            //saisie de pseudo//
            saisir_pseudo(buffer, pseudo);

            //Gestion des touches pour la saisie//
            int touche = readkey() >> 8;
            if (touche == KEY_ENTER) {
                saisie_pseudo = 0;
                if (strlen(pseudo) > 0) {
                    jeu = 1;
                    break;
                }
            } else if (touche == KEY_BACKSPACE) {
                int len = strlen(pseudo);
                if (len > 0) pseudo[len-1] = '\0';
            } else {
                char c = readkey() & 0xFF;
                if (c >= 32 && c <= 126 && strlen(pseudo) < 19) {
                    int len = strlen(pseudo);
                    pseudo[len] = c;
                    pseudo[len+1] = '\0';
                }
            }
        } else if (affichage_leaderboard) {
            //Afficher le leaderboard//
            afficher_leaderboard(buffer);
            affichage_leaderboard = 0;
        } else {
            //Affichage du menu//
            dessiner_menu(buffer, selection_menu, pseudo, niveau_choisi);

            //Navigation dans le menu//
            if (keypressed()) {
                int touche = readkey() >> 8;
                if (touche == KEY_UP && selection_menu > 0) {
                    selection_menu--;
                } else if (touche == KEY_DOWN && selection_menu < 3) {
                    selection_menu++;
                } else if (touche == KEY_ENTER) {
                    if (selection_menu == 0) {
                        //Option Jouer//
                        saisie_pseudo = 1;
                    } else if (selection_menu == 1) {
                        //Option Niveau//
                        *niveau_choisi = *niveau_choisi % 4 + 1;

                        //0 => mode boss uniquement//
                        if (*niveau_choisi == 4) *niveau_choisi = 0;
                    } else if (selection_menu == 2) {
                        //Option Leaderboard//
                        affichage_leaderboard = 1;
                    } else if (selection_menu == 3) {
                        //Option Quitter//
                        return 0;
                    }
                }
            }
        }

        //Limiter la vitesse de la boucle//
        rest(100);
    }

    return jeu;
}

void afficher_ecran_fin(BITMAP *buffer, int score, char *pseudo) {
    //Fonction pour afficher l'écran de fin de partie//
    dessiner_fin(score, buffer);
    //Afficher aussi le pseudo du joueur sur l'écran de fin//
    textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR/2 + 60,
                       makecol(200, 200, 255), -1, "Joueur: %s", pseudo);

    //Ajouter un message pour indiquer comment revenir au menu//
    textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR/2 + 90,
                       makecol(200, 200, 200), -1, "Appuyez sur une touche pour revenir au menu");

    //Sauvegarder le score dans le leaderboard//
    sauvegarder_score(pseudo, score);

    //Afficher un message indiquant que le score a été sauvegardé//
    textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR/2 + 120,
                       makecol(0, 255, 0), -1, "Score sauvegardé dans le leaderboard!");

    blit(buffer, screen, 0, 0, 0, 0, LARGEUR, HAUTEUR);
    //Attendre qu'une touche soit pressée//
    readkey();
}