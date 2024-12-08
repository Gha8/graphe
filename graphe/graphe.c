/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*   p_graphe.c                                                               */
/*                                                                            */
/*   By: gmohamed <marvin@42.fr>                                              */
/*                                                                            */
/*   Created: 2024/11/20 01:10:51 by gmohamed,                                */
/*   Updated: 2024/12/07 12:01:58 by gmohamed         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_TACHES 100
#define MAX_NOM_FICHIER 20
#define MAX_LIGNE 100

int datePlusTot[MAX_TACHES];
int datePlusTard[MAX_TACHES]; 
int marge[MAX_TACHES];       


typedef struct 
{
	int tache;
	int duree;
	int predecesseurs[MAX_TACHES];
	int nb_predecesseurs;
}	Tache;

typedef struct 
{
	int source;
	int destination;
	int poids;
}	Arc;

int lireTaches(const char* nomFichier, Tache taches[]) {
    int i = 0;
    char ligne[MAX_LIGNE];
    
    FILE *fichier = fopen(nomFichier, "r");
    if (fichier == NULL) {
        printf("Erreur d'ouverture du fichier %s\n", nomFichier);
        return 0;
    }

    while (fgets(ligne, sizeof(ligne), fichier)) {
        char *token = strtok(ligne, " \n");
        taches[i].tache = atoi(token);
        token = strtok(NULL, " \n");
        taches[i].duree = atoi(token);
        taches[i].nb_predecesseurs = 0;
        while ((token = strtok(NULL, " \n")) != NULL) {
            int pred = atoi(token);
            if (pred != 0) {
                taches[i].predecesseurs[taches[i].nb_predecesseurs++] = pred;
            }
        }
        i++;
    }

    fclose(fichier);
    return i;
}

void afficherGraphe(Tache taches[], int nbTaches) {
    int nbSommets = nbTaches + 2;  // 2 sommets fictifs (α et ω)
    int nbArcs = 0;

    // Compter les arcs
    for (int i = 0; i < nbTaches; i++) {
        if (taches[i].nb_predecesseurs == 0) {
            nbArcs++;
        }
    }

    for (int i = 0; i < nbTaches; i++) {
        nbArcs += taches[i].nb_predecesseurs;
    }

    for (int i = 0; i < nbTaches; i++) {
        int hasSuccesseur = 0;
        for (int j = 0; j < nbTaches; j++) {
            for (int k = 0; k < taches[j].nb_predecesseurs; k++) {
                if (taches[j].predecesseurs[k] == taches[i].tache) {
                    hasSuccesseur = 1;
                    break;
                }
            }
            if (hasSuccesseur) break;
        }
        if (!hasSuccesseur) {
            nbArcs++;
        }
    }

    // Afficher les informations sur le graphe
    printf("****Création du graphe d’ordonnancement****\n\n");
    printf("%d sommets\n", nbSommets);
    printf("%d arcs\n\n", nbArcs);

    // Afficher les arcs du sommet fictif α (0)
    for (int i = 0; i < nbTaches; i++) {
        if (taches[i].nb_predecesseurs == 0) {
            printf("0  -> %-2d = %-2d\n", taches[i].tache, 0);
        }
    }

    // Afficher les arcs pour les tâches (arc entre prédécesseur et tâche)
    for (int i = 0; i < nbTaches; i++) {
        for (int j = 0; j < taches[i].nb_predecesseurs; j++) {
            int pred = taches[i].predecesseurs[j];
            printf("%-2d -> %-2d = %-2d\n", pred, taches[i].tache, taches[i].duree);
        }
    }

    // Afficher les arcs du sommet fictif ω (N+1)
    for (int i = 0; i < nbTaches; i++) {
        int hasSuccesseur = 0;
        for (int j = 0; j < nbTaches; j++) {
            for (int k = 0; k < taches[j].nb_predecesseurs; k++) {
                if (taches[j].predecesseurs[k] == taches[i].tache) {
                    hasSuccesseur = 1;
                    break;
                }
            }
            if (hasSuccesseur) break;
        }
        if (!hasSuccesseur) {
            printf("%-2d -> %-2d = %-2d\n", taches[i].tache, nbTaches + 1, taches[i].tache);
        }
    }
    
}

// Affichage de la matrice de dépendance
void afficherMatrice(Tache taches[], int nbTaches) {
    int nbSommets = nbTaches + 2;  // 2 sommets fictifs (α et ω)
    
    // Initialisation de la matrice
    char matrice[nbSommets][nbSommets];
    
    // Remplir la matrice avec des '*'
    for (int i = 0; i < nbSommets; i++) {
        for (int j = 0; j < nbSommets; j++) {
            matrice[i][j] = '*';
        }
    }

    // Remplir la matrice avec les durées des arcs
    for (int i = 0; i < nbTaches; i++) {
        for (int j = 0; j < taches[i].nb_predecesseurs; j++) {
            int pred = taches[i].predecesseurs[j];
            matrice[pred][taches[i].tache] = taches[i].duree + '0';  // Conversion en char pour l'affichage
        }
    }

    // Ajouter les arcs du sommet fictif α (0) vers les tâches sans prédécesseurs
    for (int i = 0; i < nbTaches; i++) {
        if (taches[i].nb_predecesseurs == 0) {
            matrice[0][taches[i].tache] = '0';  // Arc de 0 vers cette tâche
        }
    }

    // Ajouter les arcs vers le sommet fictif ω (N + 1) pour les tâches sans successeurs
    for (int i = 0; i < nbTaches; i++) {
        int hasSuccesseur = 0;
        for (int j = 0; j < nbTaches; j++) {
            for (int k = 0; k < taches[j].nb_predecesseurs; k++) {
                if (taches[j].predecesseurs[k] == taches[i].tache) {
                    hasSuccesseur = 1;
                    break;
                }
            }
            if (hasSuccesseur) break;
        }
        if (!hasSuccesseur) {
            matrice[taches[i].tache][nbTaches + 1] = taches[i].tache + '0';  // Arc de cette tâche vers ω
        }
    }

    // Affichage de la matrice
    printf("\n       ****Matrice des valeurs****\n\n");
    printf("   ");
    for (int i = 0; i < nbSommets; i++) {
        printf("%-3d", i);  // Affichage de l'entête des colonnes
    }
    printf("\n");

    for (int i = 0; i < nbSommets; i++) {
        printf("%-3d", i);  // Affichage de l'entête des lignes
        for (int j = 0; j < nbSommets; j++) {
            printf("%-3c", matrice[i][j]);  // Affichage des valeurs de la matrice
        }
        printf("\n");
    }
}
int detecterCycle(Tache taches[], int nbTaches) {
    Tache copieTaches[MAX_TACHES];
    memcpy(copieTaches, taches, sizeof(Tache) * nbTaches); // Créer une copie locale des tâches

    int flag = 1;
    int sommetSupprime[MAX_TACHES] = {0};
    int nbRestants = nbTaches;

    printf("\n****Détection de circuit (Avec la méthode de suppression des points d'entrée)****\n\n");

    // Affichage des points d'entrée au début
    printf("Points d’entrée : ");
    for (int i = 0; i < nbTaches; i++) {
        if (copieTaches[i].nb_predecesseurs == 0) {
            printf("%d ", copieTaches[i].tache);
        }
    }
    printf("\n");

    while (nbRestants > 0) 
    {
        int trouve = 0;
        for (int i = 0; i < nbTaches; i++) 
        {
            flag = 1;	
            if (!sommetSupprime[i] && copieTaches[i].nb_predecesseurs == 0) 
            {
                sommetSupprime[i] = 1;
                nbRestants--;

                printf("Suppression d'un point d’entrée\n");
                printf("Sommets restant : ");
                for (int j = 0; j < nbTaches; j++) 
                {
                    if (!sommetSupprime[j]) 
                    {
                        printf("%d ", copieTaches[j].tache);
                        flag = 0;
                    }
                }    
                if (flag)
			printf("Aucun");    
                printf("\n");
                for (int j = 0; j < nbTaches; j++) 
                {
                    for (int k = 0; k < copieTaches[j].nb_predecesseurs; k++) 
                    {
                        if (copieTaches[j].predecesseurs[k] == copieTaches[i].tache) 
                        {
                            copieTaches[j].predecesseurs[k] = copieTaches[j].predecesseurs[copieTaches[j].nb_predecesseurs - 1];
                            copieTaches[j].nb_predecesseurs--;
                            break;
                        }
                    }
                }
                trouve = 1;
                break;
            }
        }
        if (!trouve) {
            printf("Le graphe contient un cycle.\n");
            return 1; // Cycle détecté
        }
	flag = 1;
        printf("Points d’entrée : ");
        for (int i = 0; i < nbTaches; i++) 
        {
            if (copieTaches[i].nb_predecesseurs == 0 && !sommetSupprime[i]) 
            {
                printf("%d ", copieTaches[i].tache);
                flag =0;
            }
        }
        if (flag)
        	printf("Aucun");
        printf("\n");
    }

    printf("Il n’y a pas de circuit :D .\n");
    return 0; // Pas de cycle
}

void calculerEtAfficherRang(Tache taches[], int nbTaches) {
    int rang[MAX_TACHES] = {0};  // Tableau pour stocker le rang de chaque tâche

    // Calcul des rangs
    for (int i = 0; i < nbTaches; i++) {
        for (int j = 0; j < taches[i].nb_predecesseurs; j++) {
            int pred = taches[i].predecesseurs[j];
            rang[taches[i].tache - 1] = (rang[taches[i].tache - 1] > rang[pred - 1] + 1)
                                        ? rang[taches[i].tache - 1]
                                        : rang[pred - 1] + 1;
        }
    }

    // Affichage des rangs
    printf("\n****Rangs des tâches****\n");
    for (int i = 0; i < nbTaches; i++) {
        printf("Tâche %d : Rang %d\n", taches[i].tache, rang[taches[i].tache - 1]);
    }
}

void calculerEtAfficherCalendrier(Tache taches[], int nbTaches) {
    // Initialisation des dates au plus tôt et au plus tard
    for (int i = 0; i < nbTaches; i++) {
        datePlusTot[i] = 0;  // Initialisation à 0
        datePlusTard[i] = INT_MAX;  // Initialisation à une valeur très grande
        marge[i] = 0;  // Initialisation de la marge à 0
    }

    // Initialisation de la date au plus tard pour la tâche finale (sommet fictif ω)
    datePlusTard[nbTaches] = 0;

    // Calcul des dates au plus tôt (en avançant dans le graphe)
    for (int i = 0; i < nbTaches; i++) {
        for (int j = 0; j < taches[i].nb_predecesseurs; j++) {
            int pred = taches[i].predecesseurs[j];
            if (datePlusTot[i] < datePlusTot[pred] + taches[pred].duree) {
                datePlusTot[i] = datePlusTot[pred] + taches[pred].duree;
            }
        }
    }

    // Calcul des dates au plus tard (en reculant dans le graphe)
    for (int i = nbTaches - 1; i >= 0; i--) {
        for (int j = 0; j < nbTaches; j++) {
            for (int k = 0; k < taches[j].nb_predecesseurs; k++) {
                if (taches[j].predecesseurs[k] == taches[i].tache) {
                    // Mise à jour de la date au plus tard pour la tâche i
                    int possible_date_tard = datePlusTard[taches[i].tache] - taches[i].duree;
                    if (possible_date_tard < datePlusTard[j]) {
                        datePlusTard[j] = possible_date_tard;
                    }
                }
            }
        }
    }

    // Calcul des marges
    for (int i = 0; i < nbTaches; i++) {
        marge[i] = datePlusTard[i] - datePlusTot[i];
    }

    // Affichage des résultats
    printf("\n**** Calendrier des tâches ****\n");
    for (int i = 0; i < nbTaches; i++) {
        printf("Tâche %d : Date au plus tôt = %d, Date au plus tard = %d, Marge = %d\n",
               taches[i].tache, datePlusTot[i], datePlusTard[i], marge[i]);
    }
}




int main(int argc, char **argv) {
    Tache taches[MAX_TACHES];
    char *nomFichier = argv[1];
    
    // Lecture des tâches
    int nbTaches = lireTaches(nomFichier, taches);
    if (nbTaches == 0) {
        return 1;  // Erreur de lecture du fichier
    }
    
    // Affichage du graphe
    afficherGraphe(taches, nbTaches);
    afficherMatrice(taches, nbTaches);
    // Vérification de la présence d'un cycle
    if (detecterCycle(taches, nbTaches) == 1) {
        printf("Le graphe n'est pas valide pour l'ordonnancement (cycle détecté).\n");
        return 1;  // Quitter si un cycle est détecté
    }

    // Vérification que toutes les durées des arcs sont positives
    int dureesPositives = 1;  // On suppose qu'elles sont positives
    for (int i = 0; i < nbTaches; i++) {
        if (taches[i].duree <= 0) {  // Si la durée est inférieure ou égale à 0
            dureesPositives = 0;
            break;
        }
    }

    // Si les durées ne sont pas toutes positives
    if (!dureesPositives) {
        printf("Erreur: Certaines durées sont négatives ou nulles.\n");
        return 1;  // Quitter si une durée est négative ou nulle
    }
    

    // Si pas de cycle et que toutes les durées sont positives
    printf("Pas de durée négative :D .\nC’est bien un graphe d’ordonnancement.\n\n");
    
    // Calcul et affichage des rangs
    calculerEtAfficherRang(taches, nbTaches);
     // Calculer les calendriers
     
    // Calculer et afficher les calendriers
    calculerEtAfficherCalendrier(taches, nbTaches);

    return 0;
} 
