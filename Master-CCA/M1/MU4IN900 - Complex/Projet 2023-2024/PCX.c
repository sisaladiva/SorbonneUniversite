#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//////////////////////////Exercice 1 //////////////////////////////
//Structure d'un noeud
typedef struct Node {
    char cle;
    int priorite;
    struct Node* droite;
    struct Node* gauche;
     struct Node* parent;
} Node;

void afficherNoeud(Node* noeud){
     printf("Clé %c \n Priorité %d\n", noeud->cle, noeud->priorite);
}


//Constructeurs Node
Node* creerNode(int cle, int priorite) {
    Node* nouveauNoeud = (Node*) malloc(sizeof(Node));
    nouveauNoeud->cle = cle;
    nouveauNoeud->priorite = priorite;
    nouveauNoeud->droite = NULL;
    nouveauNoeud->gauche = NULL;
    nouveauNoeud->parent = NULL;
    return nouveauNoeud;
}





typedef struct Arbre {
   Node * racine;
}Arbre;


Arbre* creerArbreVide() {
    Arbre* nouvelarbre= (Arbre*) malloc(sizeof(Arbre));
  
    if( nouvelarbre != NULL)  nouvelarbre->racine= NULL;
    return nouvelarbre;
}

int EstVide(Node* Node) {
    return (Node == NULL);
}

Node* obtenirSAGauche(Node* Node) {
    if (Node == NULL) return NULL;  
    return Node->gauche;  
}

Node* obtenirSADroit(Node* Node) {
    if (Node == NULL)  return NULL; 
    return Node->droite;  
}







//////////////////////////Exercice 2 //////////////////////////////
//Rechercher un noeud
Node* rechercherNoeudinterne(Node* racine, int cle) {
    if (racine == NULL) {
        printf("L'arbre est vide");
        return NULL;
    }

    if (racine->cle == cle) {
        printf("La clé %c a été trouvée avec la priorité %d.\n", racine->cle, racine->priorite);
        return racine;
    }

    // Si la clé recherchée est inférieure à la clé du nœud courant, rechercher à gauche
    if (cle < racine->cle) 
        return rechercherNoeudinterne(racine->gauche, cle);

    // Sinon, rechercher à droite
    return rechercherNoeudinterne(racine->droite, cle);
}

Node* rechercherNoeud(Arbre* arbre, int cle) {
return rechercherNoeudinterne(arbre->racine, cle);
}


// Afficher l'arbre en parcours pré-ordre 
void preordre(Node* racine) {
    if (racine != NULL) {
        printf("(%c : %d) ", racine->cle, racine->priorite);
        preordre( racine->gauche);
        preordre( racine->droite);
    }
}

//////////////////////////Exercice 3 //////////////////////////////

Node* RotationDroite(Node* y) {
    Node* x = y->gauche;
    y->gauche = x->droite;
    x->droite = y;

if (x->droite != NULL) x->droite->parent = y;
    x->droite = y;

    // Mettre à jour le parent
    x->parent = y->parent;
    y->parent = x;
    return x;
}

Node* RotationGauche(Node* x) {
    Node* y = x->droite;
    x->droite = y->gauche;
    y->gauche = x;

      if (y->gauche != NULL) y->gauche->parent = x;
    y->gauche = x;

    // Mettre à jour le parent
    y->parent = x->parent;
    x->parent = y;
    return y;
}

// Insérer un noeud dans l'arbre cartésien
int rotations;
Node* insererNodeinterne(Node* arbre, Node* noeud) {
    if (arbre == NULL) {
        return noeud;
    }

    // Insertion du noeud dans le sous-arbre approprié
    if (noeud->cle < arbre->cle) {
        arbre->gauche = insererNodeinterne(arbre->gauche, noeud);
        arbre->gauche->parent = arbre;
    } else {
        arbre->droite = insererNodeinterne(arbre->droite, noeud);
        arbre->droite->parent = arbre;
    }

    // Vérifier et effectuer les rotations nécessaires
    if (arbre->gauche != NULL && arbre->gauche->priorite < arbre->priorite) {
        rotations += 1;  // Incrémenter le compteur de rotations
        arbre = RotationDroite(arbre);  // Effectuer une rotation à droite
    }

    if (arbre->droite != NULL && arbre->droite->priorite < arbre->priorite) {
        rotations += 1;  // Incrémenter le compteur de rotations
        arbre = RotationGauche(arbre);  // Effectuer une rotation à gauche
    }

    return arbre;
}


Arbre* insererNode(Arbre* arbre, Node* noeud) {
     // Déclaration du compteur de rotations local
  rotations = 0;
    // Insertion du noeud dans l'arbre
    arbre->racine = insererNodeinterne(arbre->racine, noeud);
    
    return arbre;
}



//////////////////////////Exercice 4 //////////////////////////////

Node* supprimerNoeudinterne(Node* arbre, Node* noeud) {
    if (arbre == NULL) {
        return NULL; 
    }
    
    // Recherche du nœud à supprimer
    if (noeud->cle < arbre->cle) {
        arbre->gauche = supprimerNoeudinterne(arbre->gauche, noeud);
    } else if (noeud->cle > arbre->cle) {
        arbre->droite = supprimerNoeudinterne(arbre->droite, noeud);
    } else {
        // Nœud trouvé
        if (arbre->gauche == NULL && arbre->droite == NULL) {
            // Cas de feuille : supprimer directement
            free(arbre);
            return NULL;
        } else if (arbre->gauche == NULL || (arbre->droite != NULL && arbre->droite->priorite < arbre->gauche->priorite)) {
            // Rotation à gauche avec le fils droit ayant la priorité la plus forte (numériquement plus faible)
            arbre = RotationGauche(arbre);
            arbre->gauche = supprimerNoeudinterne(arbre->gauche, noeud);
        } else {
            // Rotation à droite avec le fils gauche ayant la priorité la plus forte (numériquement plus faible)
            arbre = RotationDroite(arbre);
            arbre->droite = supprimerNoeudinterne(arbre->droite, noeud);
        }
    }
    return arbre;
}

Arbre* supprimerNoeud(Arbre* arbre, Node* noeud) {
    arbre->racine = supprimerNoeudinterne(arbre->racine, noeud); // Mise à jour de la racine
    return arbre; 
}



//////////////////////////Exercice 5 //////////////////////////////

Node* genererNoeudAleatoire() {
    char cle = 'A' + rand() % 26;  // Clé aléatoire entre 'A' et 'Z'
    int priorite = rand() % 100;   // Priorité aléatoire entre 0 et 99
    return creerNode(cle, priorite);  // Crée et retourne un nouveau nœud avec la clé et la priorité générées
}

void enregistrerResultatsCSV(int nbNoeuds, int rotationsTotales) {
    FILE *fichier = fopen("resultats_rotations.csv", "a");  // Ouvrir le fichier en mode append
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    // Sauvegarder les résultats sous forme de ligne CSV
    fprintf(fichier, "%d,%d\n", nbNoeuds, rotationsTotales);
    fclose(fichier);
}
void testerArbreCartesien(int nbNoeuds) {
    Arbre* arbre = creerArbreVide();  // Créer un arbre vide
    int rotationsTotales = 0;  // Initialiser le compteur des rotations
rotations=0;
    // Insérer nbNoeuds nœuds dans l'arbre et compter les rotations
    for (int i = 0; i < nbNoeuds; i++) {
        Node* noeud = genererNoeudAleatoire();  // Générer un nœud aléatoire
        arbre = insererNode(arbre, noeud);  // Insérer le nœud dans l'arbre
        rotationsTotales+=rotations;
    }

    // Enregistrer les résultats dans un fichier CSV
    enregistrerResultatsCSV(nbNoeuds, rotationsTotales);
    printf("Nombre de nœuds : %d, Rotations totales : %d\n", nbNoeuds, rotationsTotales);
}


//HAUTEUR
// Fonction récursive pour calculer la hauteur d'un arbre binaire à partir d'un nœud donné
int hauteurRec(Node* racine) {
    if (racine == NULL) {
        return 0;  // Si le nœud est NULL, la hauteur est 0
    }

    // Calculer la hauteur des sous-arbres gauche et droit
    int hauteurGauche = hauteurRec(racine->gauche);
    int hauteurDroite = hauteurRec(racine->droite);

    // La hauteur de l'arbre est la hauteur maximale entre les sous-arbres gauche et droit + 1
    return (hauteurGauche > hauteurDroite ? hauteurGauche : hauteurDroite) + 1;
}

// Fonction pour calculer la hauteur d'un arbre en utilisant la struct Arbre
int hauteur(Arbre* arbre) {
    if (arbre == NULL || arbre->racine == NULL) {
        return 0;  // Si l'arbre est vide, la hauteur est 0
    }
    return hauteurRec(arbre->racine);  // Appel à la fonction récursive pour calculer la hauteur de l'arbre
}
Arbre* genererArbre(int n) {
    Arbre* arbre = creerArbreVide();
    for (int i = 0; i < n; i++) {
        Node* noeud = genererNoeudAleatoire();
        arbre = insererNode(arbre, noeud);
    }
    return arbre;
}

/*// Fonction pour générer un arbre cartésien avec n nœuds
Arbre* genererArbre(int n) {
    Arbre* arbre = creerArbreVide();
    for (int i = 0; i < n; i++) {
        Node* noeud = genererNoeudAleatoire();  // Vous pouvez adapter cette fonction pour générer des nœuds
        arbre = insererNode(arbre, noeud);
    }
    return arbre;
}
void enregistrerResultatsCSVgen(const char* filename, int nbNoeuds, int hauteur) {
    FILE* fichier = fopen(filename, "a");  // Ouvre le fichier en mode append
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    // Sauvegarde les résultats sous forme de ligne CSV
    fprintf(fichier, "%d,%d\n", nbNoeuds, hauteur);  // Écrire les données dans le fichier
    fclose(fichier);  // Fermer le fichier après l'écriture
}
void testerHauteurArbres(const char* filename, int g) {
    // Tester pour plusieurs tailles d'arbres
    for (int n = 10; n <= g; n += 1) {  // Tester de 10 à 1000 nœuds (ajuster selon vos besoins)
        Arbre* arbre = genererArbre(n);  // Créer l'arbre avec n nœuds
        int h = hauteur(arbre);  // Calculer la hauteur de l'arbre
        enregistrerResultatsCSVgen(filename, n, h);  // Sauvegarder le résultat dans le fichier CSV
    }
}*/

void libererArbreRec(Node* noeud) {
    if (noeud == NULL) {
        return;
    }
    libererArbreRec(noeud->gauche);
    libererArbreRec(noeud->droite);
    free(noeud);
}

void libererArbreComplet(Arbre* arbre) {
    if (arbre == NULL) {
        return;
    }
    libererArbreRec(arbre->racine);
    free(arbre);
}

//tempsss cartésien 
/*void enregistrerResultatsCSVtemps(const char* filename, int nbNoeuds, double temps) {
    FILE* fichier = fopen(filename, "a");  // Ouvre le fichier en mode append
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    // Sauvegarde les résultats sous forme de ligne CSV
    fprintf(fichier, "%d,%.6f\n", nbNoeuds, temps);  // Enregistrer le temps en secondes
    fclose(fichier);  // Fermer le fichier après l'écriture
}*/
void enregistrerTempsMoyen(const char* filename, int nbNoeuds, double tempsMoyen) {
    FILE* fichier = fopen(filename, "a");  // Ouvrir en mode append
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    fprintf(fichier, "%d,%.6f\n", nbNoeuds, tempsMoyen);  // Sauvegarder les résultats
    fclose(fichier);
}


// Fonction pour mesurer le temps total d'insertion pour n nœuds
/*void tempsinsertion(const char* filename, int m) {
    for (int n = 10; n <= m; n += 1) {  // Tester de 10 à m nœuds
        Arbre* arbre = (Arbre*)malloc(sizeof(Arbre));
        arbre->racine = NULL;
        
        // Chronométrer le temps d'insertion pour tous les nœuds
        int debut = clock();

        // Insérer n nœuds dans l'arbre
        for (int i = 0; i < n; i++) {
            Node* noeud = genererNoeudAleatoire();  // Générer un nœud aléatoire
            arbre = insererNode(arbre, noeud);  // Insérer le nœud dans l'arbre
        }

        int fin = clock();

        // Calculer le temps total d'insertion
        double tempsEnSecondes = (double)(fin - debut) / CLOCKS_PER_SEC;
        printf("Temps d'insertion pour %d nœuds : %f secondes\n", n, tempsEnSecondes);

        // Enregistrer le résultat dans le fichier CSV
        enregistrerResultatsCSVtemps(filename, n, tempsEnSecondes);
    }
}*/
void tempsMoyenInsertionCartesien(const char* filename, int maxNoeuds, int repetitions) {
    FILE* fichier = fopen(filename, "w");  // Créer un fichier avec en-tête
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    fprintf(fichier, "Nombre de nœuds,Temps moyen d'insertion (secondes)\n");  // En-tête
    fclose(fichier);
    for (int n = 10; n <= maxNoeuds; n += 10) {  // Parcourir différentes tailles d'arbres
        double tempsTotal = 0.0;

        for (int r = 0; r < repetitions; r++) {  // Répéter pour calculer la moyenne
            Arbre* arbre = (Arbre*)malloc(sizeof(Arbre));
            arbre->racine = NULL;

            int debut = clock();  // Début du chronométrage
            for (int i = 0; i < n; i++) {
                Node* noeud = genererNoeudAleatoire();
                arbre = insererNode(arbre, noeud);  // Insertion dans l'arbre cartésien
            }
            int fin = clock();  // Fin du chronométrage

            tempsTotal += (double)(fin - debut) / CLOCKS_PER_SEC;
            free(arbre);  // Libérer la mémoire
        }

        double tempsMoyen = tempsTotal / repetitions;  // Calculer la moyenne
        enregistrerTempsMoyen(filename, n, tempsMoyen);
        printf("Arbre cartésien - Temps moyen pour %d nœuds : %.6f secondes\n", n, tempsMoyen);
    }
}







//----------BST

// Fonction pour insérer un nœud dans un arbre binaire de recherche
Node* insererBST(Node* racine, Node* noeud) {
    // Si l'arbre est vide, retourner ce nœud
    if (racine == NULL)
        return noeud;

    // Sinon, insérer récursivement à gauche ou à droite
    if (noeud->cle < racine->cle) {
        racine->gauche = insererBST(racine->gauche, noeud);
        racine->gauche->parent = racine;
    } else {
        racine->droite = insererBST(racine->droite, noeud);
        racine->droite->parent = racine;
    }

    return racine;
}

// Fonction pour insérer un nœud dans l'arbre
Arbre* insererArbreBST(Arbre* arbre, Node* noeud) {
    if (arbre->racine == NULL) {
        arbre->racine = noeud;
    } else {
        arbre->racine = insererBST(arbre->racine, noeud);
    }
    return arbre;
}
// Fonction récursive pour calculer la hauteur de l'arbre
int hauteurBST(Node* racine) {
    if (racine == NULL)
        return 0;
    int hauteurGauche = hauteurBST(racine->gauche);
    int hauteurDroite = hauteurBST(racine->droite);
    return (hauteurGauche > hauteurDroite ? hauteurGauche : hauteurDroite) + 1;
}

// Fonction pour obtenir la hauteur de l'arbre
int hauteurArbreBST(Arbre* arbre) {
    return hauteurBST(arbre->racine);
}


// Fonction pour générer un arbre BST avec n nœuds
Arbre* genererArbreBST(int n) {
    Arbre* arbre = (Arbre*)malloc(sizeof(Arbre));
    arbre->racine = NULL;

    for (int i = 0; i < n; i++) {
        Node* noeud = genererNoeudAleatoire();
        arbre = insererArbreBST(arbre, noeud);
    }

    return arbre;
}
// Fonction pour rechercher une clé dans un BST
int rechercherBST(Node* racine, char cle) {
    if (racine == NULL) {
        return 0;  // La clé n'est pas trouvée
    }

    if (cle == racine->cle) {
        return 1;  // La clé est trouvée
    }

    if (cle < racine->cle) {
        return rechercherBST(racine->gauche, cle);  // Recherche dans le sous-arbre gauche
    } else {
        return rechercherBST(racine->droite, cle);  // Recherche dans le sous-arbre droit
    }
}
Node* trouverMin(Node* noeud) {
    while (noeud->gauche != NULL) {
        noeud = noeud->gauche;
    }
    return noeud;
}

Node* supprimerBST(Node* racine, char cle) {
    // Si l'arbre est vide
    if (racine == NULL) {
        return NULL;
    }

    // Si la clé est plus petite, aller dans le sous-arbre gauche
    if (cle < racine->cle) {
        racine->gauche = supprimerBST(racine->gauche, cle);
    }
    // Si la clé est plus grande, aller dans le sous-arbre droit
    else if (cle > racine->cle) {
        racine->droite = supprimerBST(racine->droite, cle);
    }
    // Si la clé correspond au nœud courant
    else {
        // Cas 1 : Nœud sans enfant ou avec un seul enfant
        if (racine->gauche == NULL) {
            Node* temp = racine->droite;
            free(racine);
            return temp;
        }
        else if (racine->droite == NULL) {
            Node* temp = racine->gauche;
            free(racine);
            return temp;
        }

        // Cas 2 : Nœud avec deux enfants
        Node* temp = trouverMin(racine->droite);  // Trouver le successeur
        racine->cle = temp->cle;                  // Remplacer la clé
        racine->droite = supprimerBST(racine->droite, temp->cle);  // Supprimer le successeur
    }

    return racine;
}
void enregistrerResultatsHauteur(const char* filename, int nbNoeuds, int hauteur) {
    FILE* fichier = fopen(filename, "a");  // Ouvre le fichier en mode append
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    // Sauvegarde les résultats sous forme de ligne CSV
    fprintf(fichier, "%d,%d\n", nbNoeuds, hauteur);  // Écrire les données dans le fichier
    fclose(fichier);  // Fermer le fichier après l'écriture
}
void hauteurMoyenneCartesien(const char* filename, int maxNoeuds, int repetitions) {
    FILE* fichier = fopen(filename, "w");  // Crée un fichier avec un en-tête
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    fprintf(fichier, "Nombre de nœuds,Hauteur moyenne\n");

    for (int n = 10; n <= maxNoeuds; n += 10) {  // Parcourir différentes tailles d'arbres
        double hauteurTotale = 0.0;

        for (int r = 0; r < repetitions; r++) {  // Répéter pour calculer la moyenne
            Arbre* arbre = creerArbreVide();

            // Construire un arbre cartésien avec `n` nœuds
            for (int i = 0; i < n; i++) {
                Node* noeud = genererNoeudAleatoire();
                arbre = insererNode(arbre, noeud);
            }

            // Ajouter la hauteur actuelle
            hauteurTotale += hauteur(arbre);

            // Libérer l'arbre pour éviter les fuites de mémoire
            libererArbreComplet(arbre);
        }

        double hauteurMoyenne = hauteurTotale / repetitions;  // Calculer la moyenne
        fprintf(fichier, "%d,%.2f\n", n, hauteurMoyenne);
        printf("Arbre cartésien - Nœuds : %d, Hauteur moyenne : %.2f\n", n, hauteurMoyenne);
    }

    fclose(fichier);
}
void hauteurMoyenneBST(const char* filename, int maxNoeuds, int repetitions) {
    FILE* fichier = fopen(filename, "w");  // Crée un fichier avec un en-tête
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    fprintf(fichier, "Nombre de nœuds,Hauteur moyenne\n");

    for (int n = 10; n <= maxNoeuds; n += 10) {  // Parcourir différentes tailles d'arbres
        double hauteurTotale = 0.0;

        for (int r = 0; r < repetitions; r++) {  // Répéter pour calculer la moyenne
            Arbre* arbre = creerArbreVide();

            // Construire un arbre BST avec `n` nœuds
            for (int i = 0; i < n; i++) {
                Node* noeud = genererNoeudAleatoire();
                arbre->racine = insererBST(arbre->racine, noeud);
            }

            // Ajouter la hauteur actuelle
            hauteurTotale += hauteur(arbre);

            // Libérer l'arbre pour éviter les fuites de mémoire
            libererArbreComplet(arbre);
        }

        double hauteurMoyenne = hauteurTotale / repetitions;  // Calculer la moyenne
        fprintf(fichier, "%d,%.2f\n", n, hauteurMoyenne);
        printf("BST - Nœuds : %d, Hauteur moyenne : %.2f\n", n, hauteurMoyenne);
    }

    fclose(fichier);
}

// Fonction pour tester la hauteur des arbres et enregistrer les résultats dans un fichier CSV
/*void testerHauteurArbresbst(const char* filename, int n) {
    FILE* fichier = fopen(filename, "a");  // Ouvre le fichier en mode ajout
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    // Tester avec différentes tailles d'arbres
    for (int taille = 10; taille <= n; taille++) {  // Tester avec 2 à 6 nœuds
        Arbre* arbreBST = genererArbreBST(taille);  // Générer un arbre BST avec `taille` nœuds
        int hauteur = hauteurBST(arbreBST->racine);  // Calculer la hauteur de l'arbre

        // Enregistrer les résultats dans le fichier CSV
        fprintf(fichier, "%d,%d\n", taille, hauteur);
    }

    fclose(fichier);  // Fermer le fichier après avoir ajouté les résultats
}*/


//temps bst
/*void tempsinsertionbst(const char* filename, int m) {
    for (int n = 10; n <= m; n += 1) {  // Tester de 10 à m nœuds
        Arbre* arbre = (Arbre*)malloc(sizeof(Arbre));
        arbre->racine = NULL;
        
        // Chronométrer le temps d'insertion pour tous les nœuds
        int debut = clock();

        // Insérer n nœuds dans l'arbre
        for (int i = 0; i < n; i++) {
            Node* noeud = genererNoeudAleatoire();  // Générer un nœud aléatoire
            arbre = insererArbreBST(arbre, noeud);  // Insérer le nœud dans l'arbre
        }

        int fin = clock();

        // Calculer le temps total d'insertion
        double tempsEnSecondes = (double)(fin - debut) / CLOCKS_PER_SEC;
        printf("Temps d'insertion pour %d nœuds dans bst : %f secondes\n", n, tempsEnSecondes);

        // Enregistrer le résultat dans le fichier CSV
        enregistrerResultatsCSVtemps(filename, n, tempsEnSecondes);
    }
}*/
void tempsMoyenInsertionBST(const char* filename, int maxNoeuds, int repetitions) {
    FILE* fichier = fopen(filename, "w");  // Créer un fichier avec en-tête
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    fprintf(fichier, "Nombre de nœuds,Temps moyen d'insertion (secondes)\n");  // En-tête
    fclose(fichier);

    for (int n = 10; n <= maxNoeuds; n += 10) {  // Parcourir différentes tailles d'arbres
        double tempsTotal = 0.0;

        for (int r = 0; r < repetitions; r++) {  // Répéter pour calculer la moyenne
            Arbre* arbre = (Arbre*)malloc(sizeof(Arbre));
            arbre->racine = NULL;

            int debut = clock();  // Début du chronométrage
            for (int i = 0; i < n; i++) {
                Node* noeud = genererNoeudAleatoire();
                arbre = insererArbreBST(arbre, noeud);  // Insertion dans l'arbre BST
            }
            int fin = clock();  // Fin du chronométrage

            tempsTotal += (double)(fin - debut) / CLOCKS_PER_SEC;
            free(arbre);  // Libérer la mémoire
        }

        double tempsMoyen = tempsTotal / repetitions;  // Calculer la moyenne
        enregistrerTempsMoyen(filename, n, tempsMoyen);
        printf("Arbre BST - Temps moyen pour %d nœuds : %.6f secondes\n", n, tempsMoyen);
    }
}

/*void tempsRechercheCartesien(const char* filename, int m) {
    for (int n = 10; n <= m; n += 1) {  // Tester de 10 à m nœuds
        Arbre* arbre = genererArbre(n);  // Générer un arbre cartésien avec n nœuds
        Node* noeudCherche = genererNoeudAleatoire();  // Générer un nœud pour la recherche

        // Chronométrer le temps de recherche
        int debut = clock();
        rechercherNoeud(arbre, noeudCherche->cle);  // Rechercher la clé dans l'arbre
        int fin = clock();

        // Calculer le temps de recherche
        double tempsEnSecondes = (double)(fin - debut) / CLOCKS_PER_SEC;
        printf("Temps de recherche pour %d nœuds dans l'arbre cartésien : %f secondes\n", n, tempsEnSecondes);

        // Enregistrer le résultat dans le fichier CSV
        enregistrerResultatsCSVtemps(filename, n, tempsEnSecondes);

        free(noeudCherche);  // Libérer le nœud généré pour la recherche
    }
}

void tempsRechercheBST(const char* filename, int m) {
    for (int n = 10; n <= m; n += 1) {  // Tester de 10 à m nœuds
        Arbre* arbreBST = genererArbreBST(n);  // Générer un arbre BST avec n nœuds
        Node* noeudCherche = genererNoeudAleatoire();  // Générer un nœud pour la recherche

        // Chronométrer le temps de recherche
        int debut = clock();
        rechercherNoeud(arbreBST, noeudCherche->cle);  // Rechercher la clé dans l'arbre BST
        int fin = clock();

        // Calculer le temps de recherche
        double tempsEnSecondes = (double)(fin - debut) / CLOCKS_PER_SEC;
        printf("Temps de recherche pour %d nœuds dans l'arbre BST : %f secondes\n", n, tempsEnSecondes);

        // Enregistrer le résultat dans le fichier CSV
        enregistrerResultatsCSVtemps(filename, n, tempsEnSecondes);

        free(noeudCherche);  // Libérer le nœud généré pour la recherche
    }
}
void tempsSuppressionCartesien(const char* filename, int m) {
    for (int n = 10; n <= m; n += 1) {  // Tester de 10 à m nœuds
        Arbre* arbre = genererArbre(n);  // Générer un arbre cartésien avec n nœuds
        Node* noeudSupprime = genererNoeudAleatoire();  // Générer un nœud pour la suppression

        // Chronométrer le temps de suppression
        int debut = clock();
        arbre = supprimerNoeud(arbre, noeudSupprime);  // Supprimer le nœud dans l'arbre
        int fin = clock();

        // Calculer le temps de suppression
        double tempsEnSecondes = (double)(fin - debut) / CLOCKS_PER_SEC;
        printf("Temps de suppression pour %d nœuds dans l'arbre cartésien : %f secondes\n", n, tempsEnSecondes);

        // Enregistrer le résultat dans le fichier CSV
        enregistrerResultatsCSVtemps(filename, n, tempsEnSecondes);

        free(noeudSupprime);  // Libérer le nœud généré pour la suppression
    }
}
void tempsSuppressionBST(const char* filename, int m) {
    for (int n = 10; n <= m; n += 1) {  // Tester de 10 à m nœuds
        Arbre* arbreBST = genererArbreBST(n);  // Générer un arbre BST avec n nœuds
        Node* noeudSupprime = genererNoeudAleatoire();  // Générer un nœud pour la suppression

        // Chronométrer le temps de suppression
        int debut = clock();
        arbreBST = supprimerNoeud(arbreBST, noeudSupprime);  // Supprimer le nœud dans l'arbre BST
        int fin = clock();

        // Calculer le temps de suppression
        double tempsEnSecondes = (double)(fin - debut) / CLOCKS_PER_SEC;
        printf("Temps de suppression pour %d nœuds dans l'arbre BST : %f secondes\n", n, tempsEnSecondes);

        // Enregistrer le résultat dans le fichier CSV
        enregistrerResultatsCSVtemps(filename, n, tempsEnSecondes);

        free(noeudSupprime);  // Libérer le nœud généré pour la suppression
    }
}

int calculerFacteurEquilibre(Node* noeud) {
    if (noeud == NULL) {
        return 0;
    }

    int hauteurGauche = hauteurRec(noeud->gauche);  // Hauteur du sous-arbre gauche
    int hauteurDroite = hauteurRec(noeud->droite);  // Hauteur du sous-arbre droit

    return abs(hauteurGauche - hauteurDroite);  // Différence absolue
}
void calculerFacteursEquilibre(Node* noeud, int* totalFacteurs, int* nbNoeuds) {
    if (noeud == NULL) {
        return;
    }

    // Calculer le facteur d'équilibre pour le nœud courant
    int facteur = calculerFacteurEquilibre(noeud);
    *totalFacteurs += facteur;  // Ajouter au total des facteurs
    (*nbNoeuds)++;  // Compter le nœud

    // Parcourir récursivement les sous-arbres gauche et droit
    calculerFacteursEquilibre(noeud->gauche, totalFacteurs, nbNoeuds);
    calculerFacteursEquilibre(noeud->droite, totalFacteurs, nbNoeuds);
}

double facteurEquilibreMoyen(Arbre* arbre) {
    if (arbre == NULL || arbre->racine == NULL) {
        return 0;
    }

    int totalFacteurs = 0;
    int nbNoeuds = 0;

    calculerFacteursEquilibre(arbre->racine, &totalFacteurs, &nbNoeuds);

    return (double)totalFacteurs / nbNoeuds;  // Moyenne des facteurs d'équilibre
}
void enregistrerFacteurEquilibreCSV(const char* filename, int nbNoeuds, double facteurMoyen) {
    FILE* fichier = fopen(filename, "a");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    fprintf(fichier, "%d,%.6f\n", nbNoeuds, facteurMoyen);  // Enregistrer le nombre de nœuds et le facteur moyen
    fclose(fichier);
}
void testerFacteurEquilibre(const char* filename, int maxNoeuds) {
    for (int n = 10; n <= maxNoeuds; n += 1) {
        Arbre* arbre = genererArbre(n);  // Générer un arbre cartésien avec n nœuds
        double facteurMoyen = facteurEquilibreMoyen(arbre);  // Calculer le facteur moyen

        enregistrerFacteurEquilibreCSV(filename, n, facteurMoyen);  // Enregistrer les résultats
    }

    printf("Facteurs d'équilibre calculés et enregistrés dans '%s'.\n", filename);
}
// Fonction pour calculer la taille d'un sous-arbre à partir d'un nœud
int tailleSousArbre(Node* racine) {
    if (racine == NULL) {
        return 0;
    }
    return 1 + tailleSousArbre(racine->gauche) + tailleSousArbre(racine->droite);
}

// Fonction pour calculer la balance des sous-arbres d'un nœud
int balanceSousArbres(Node* racine) {
    if (racine == NULL) {
        return 0;
    }

    // Calcul des tailles des sous-arbres gauche et droit
    int tailleGauche = tailleSousArbre(racine->gauche);
    int tailleDroite = tailleSousArbre(racine->droite);

    // Balance = différence absolue entre les tailles
    return abs(tailleGauche - tailleDroite);
}

// Fonction pour enregistrer les balances dans un fichier CSV
void enregistrerBalanceBST(Node* racine, const char* filename) {
    FILE* fichier = fopen(filename, "w");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    // En-tête pour le fichier CSV
    fprintf(fichier, "Clé,Balance\n");

    // Fonction récursive pour parcourir l'arbre et écrire dans le fichier
    void calculerBalance(Node* n) {
        if (n == NULL) {
            return;
        }

        // Calculer la balance du nœud courant
        int balance = balanceSousArbres(n);
        fprintf(fichier, "%c,%d\n", n->cle, balance);

        // Appel récursif pour les sous-arbres gauche et droit
        calculerBalance(n->gauche);
        calculerBalance(n->droite);
    }

    // Commencer le calcul depuis la racine
    calculerBalance(racine);

    fclose(fichier);
    printf("Les balances des sous-arbres ont été enregistrées dans '%s'.\n", filename);
}*/
void tempsMoyenRechercheCartesien(const char* filename, int m, int iterations) {
    FILE* fichier = fopen(filename, "w");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    fprintf(fichier, "Nombre de nœuds,Temps moyen (secondes)\n");

    for (int n = 10; n <= m; n += 10) {
        Arbre* arbre = (Arbre*)malloc(sizeof(Arbre));
        arbre->racine = NULL;

        // Construire l'arbre cartésien avec `n` nœuds
        for (int i = 0; i < n; i++) {
            Node* noeud = genererNoeudAleatoire();
            arbre = insererNode(arbre, noeud);

        }

        // Mesurer le temps moyen de recherche
        double totalTemps = 0;
        for (int i = 0; i < iterations; i++) {
            char cleAleatoire = 'A' + rand() % 26;  // Générer une clé aléatoire
            clock_t debut = clock();
            rechercherNoeud(arbre, cleAleatoire);
            clock_t fin = clock();
            totalTemps += (double)(fin - debut) / CLOCKS_PER_SEC;
        }

        double tempsMoyen = totalTemps / iterations;
        fprintf(fichier, "%d,%.6f\n", n, tempsMoyen);
        printf("Temps moyen de recherche pour %d nœuds (Cartésien) : %f secondes\n", n, tempsMoyen);
    }

    fclose(fichier);
}
void tempsMoyenRechercheBST(const char* filename, int m, int iterations) {
    FILE* fichier = fopen(filename, "w");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    fprintf(fichier, "Nombre de nœuds,Temps moyen (secondes)\n");

    for (int n = 10; n <= m; n += 10) {
        Arbre* arbre = (Arbre*)malloc(sizeof(Arbre));
        arbre->racine = NULL;

        // Construire le BST avec `n` nœuds
        for (int i = 0; i < n; i++) {
            Node* noeud = genererNoeudAleatoire();
            arbre->racine = insererBST(arbre->racine, noeud);
        }

        // Mesurer le temps moyen de recherche
        double totalTemps = 0;
        for (int i = 0; i < iterations; i++) {
            char cleAleatoire = 'A' + rand() % 26;  // Générer une clé aléatoire
            clock_t debut = clock();
            rechercherBST(arbre->racine, cleAleatoire);
            clock_t fin = clock();
            totalTemps += (double)(fin - debut) / CLOCKS_PER_SEC;
        }

        double tempsMoyen = totalTemps / iterations;
        fprintf(fichier, "%d,%.6f\n", n, tempsMoyen);
        printf("Temps moyen de recherche pour %d nœuds (BST) : %f secondes\n", n, tempsMoyen);
    }

    fclose(fichier);
}
void tempsMoyenSuppressionCartesien(const char* filename, int m, int iterations) {
    FILE* fichier = fopen(filename, "w");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    fprintf(fichier, "Nombre de nœuds,Temps moyen (secondes)\n");

    for (int n = 10; n <= m; n += 10) {
        Arbre* arbre = (Arbre*)malloc(sizeof(Arbre));
        arbre->racine = NULL;

        // Construire l'arbre cartésien avec `n` nœuds
        for (int i = 0; i < n; i++) {
            Node* noeud = genererNoeudAleatoire();
            arbre= insererNode(arbre, noeud);
        }

        // Mesurer le temps moyen de suppression
        double totalTemps = 0;
        for (int i = 0; i < iterations; i++) {
            char cleAleatoire = 'A' + rand() % 26;  // Générer une clé aléatoire
            clock_t debut = clock();
            Node* noeudASupprimer = rechercherNoeud(arbre, cleAleatoire);
            if (noeudASupprimer != NULL) {
                arbre = supprimerNoeud(arbre, noeudASupprimer);
                }

            clock_t fin = clock();
            totalTemps += (double)(fin - debut) / CLOCKS_PER_SEC;
        }

        double tempsMoyen = totalTemps / iterations;
        fprintf(fichier, "%d,%.6f\n", n, tempsMoyen);
        printf("Temps moyen de suppression pour %d nœuds (Cartésien) : %f secondes\n", n, tempsMoyen);
    }

    fclose(fichier);
}
void tempsMoyenSuppressionBST(const char* filename, int m, int iterations) {
    FILE* fichier = fopen(filename, "w");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    fprintf(fichier, "Nombre de nœuds,Temps moyen (secondes)\n");

    for (int n = 10; n <= m; n += 10) {
        Arbre* arbre = (Arbre*)malloc(sizeof(Arbre));
        arbre->racine = NULL;

        // Construire le BST avec `n` nœuds
        for (int i = 0; i < n; i++) {
            Node* noeud = genererNoeudAleatoire();
            arbre->racine = insererBST(arbre->racine, noeud);
        }

        // Mesurer le temps moyen de suppression
        double totalTemps = 0;
        for (int i = 0; i < iterations; i++) {
            char cleAleatoire = 'A' + rand() % 26;  // Générer une clé aléatoire
            clock_t debut = clock();
            arbre->racine = supprimerBST(arbre->racine, cleAleatoire);
            clock_t fin = clock();
            totalTemps += (double)(fin - debut) / CLOCKS_PER_SEC;
        }

        double tempsMoyen = totalTemps / iterations;
        fprintf(fichier, "%d,%.6f\n", n, tempsMoyen);
        printf("Temps moyen de suppression pour %d nœuds (BST) : %f secondes\n", n, tempsMoyen);
    }

    fclose(fichier);
}

//BALANCE
// Fonction pour calculer la taille d'un sous-arbre
int tailleSousArbre(Node* racine) {
    if (racine == NULL) {
        return 0;  // Aucun nœud
    }
    return 1 + tailleSousArbre(racine->gauche) + tailleSousArbre(racine->droite);
}
// Fonction pour calculer la balance des sous-arbres d'un nœud
int balanceSousArbres(Node* noeud) {
    if (noeud == NULL) {
        return 0;  // Si le nœud est NULL, balance = 0
    }

    // Taille des sous-arbres gauche et droit
    int tailleGauche = tailleSousArbre(noeud->gauche);
    int tailleDroite = tailleSousArbre(noeud->droite);

    // Balance = différence absolue entre les tailles
    return abs(tailleGauche - tailleDroite);
}
// Fonction récursive pour parcourir les nœuds et accumuler les balances
void calculerBalanceTotale(Node* noeud, int* totalBalance, int* nbNoeuds) {
    if (noeud == NULL) {
        return;  // Si le nœud est NULL, rien à faire
    }

    // Calculer la balance pour le nœud courant
    int balance = balanceSousArbres(noeud);
    *totalBalance += balance;  // Ajouter au total des balances
    (*nbNoeuds)++;            // Incrémenter le nombre de nœuds

    // Appel récursif pour les sous-arbres gauche et droit
    calculerBalanceTotale(noeud->gauche, totalBalance, nbNoeuds);
    calculerBalanceTotale(noeud->droite, totalBalance, nbNoeuds);
}

// Fonction pour calculer la balance moyenne d'un arbre
double balanceMoyenne(Arbre* arbre) {
    if (arbre == NULL || arbre->racine == NULL) {
        return 0.0;  // Si l'arbre est vide, balance moyenne = 0
    }

    int totalBalance = 0;  // Somme des balances
    int nbNoeuds = 0;      // Nombre total de nœuds

    // Calculer la balance totale et le nombre de nœuds
    calculerBalanceTotale(arbre->racine, &totalBalance, &nbNoeuds);
    // Retourner la moyenne des balances
    return (double)totalBalance / nbNoeuds;
}
// Fonction pour enregistrer les résultats dans un fichier CSV
void enregistrerBalanceMoyenneCSV(const char* filename, int nbNoeuds, double balanceMoyenne) {
    FILE* fichier = fopen(filename, "a");  // Ouvrir en mode append
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    // Enregistrer les résultats sous forme CSV
    fprintf(fichier, "%d,%.6f\n", nbNoeuds, balanceMoyenne);
    fclose(fichier);
}
void testerBalanceMoyenneCartesien(const char* filename, int maxNoeuds, int repetitions) {
    FILE* fichierCartesien = fopen(filename, "w");
    if (fichierCartesien == NULL) {
        printf("Erreur lors de l'ouverture du fichier %s.\n", filename);
        return;
    }
    fprintf(fichierCartesien, "Nombre de nœuds,Balance moyenne\n");
    fclose(fichierCartesien);

    for (int n = 10; n <= maxNoeuds; n += 10) {  // Tester pour différentes tailles d'arbres
        double sommeBalances = 0.0;

        for (int i = 0; i < repetitions; i++) {
            Arbre* arbre = genererArbre(n);  // Générer un arbre cartésien avec `n` nœuds
            sommeBalances += balanceMoyenne(arbre);  // Ajouter la balance moyenne
            libererArbreComplet(arbre);  // Libérer la mémoire de l'arbre
        }

        double moyenneBalance = sommeBalances / repetitions;  // Moyenne sur les répétitions
        // Afficher les résultats dans la console
        printf("Arbre Cartésien - Nombre de nœuds : %d, Balance moyenne : %.6f\n", n, moyenneBalance);
        // Enregistrer les résultats dans un fichier CSV
        enregistrerBalanceMoyenneCSV(filename, n, moyenneBalance);
    }
}

void testerBalanceMoyenneBST(const char* filename, int maxNoeuds, int repetitions) {
    FILE* fichierBST = fopen(filename, "w");
    if (fichierBST == NULL) {
        printf("Erreur lors de l'ouverture du fichier %s.\n", filename);
        return;
    }
    fprintf(fichierBST, "Nombre de nœuds,Balance moyenne\n");
    fclose(fichierBST);

    for (int n = 10; n <= maxNoeuds; n += 10) {  // Tester pour différentes tailles d'arbres
        double sommeBalances = 0.0;

        for (int i = 0; i < repetitions; i++) {
            Arbre* arbre = creerArbreVide();

            // Générer un BST avec `n` nœuds
            for (int j = 0; j < n; j++) {
                Node* noeud = genererNoeudAleatoire();
                arbre->racine = insererBST(arbre->racine, noeud);
            }

            sommeBalances += balanceMoyenne(arbre);  // Ajouter la balance moyenne
            libererArbreComplet(arbre);  // Libérer la mémoire de l'arbre
        }

        double moyenneBalance = sommeBalances / repetitions;  // Moyenne sur les répétitions
        // Afficher les résultats dans la console
        printf("Arbre BST - Nombre de nœuds : %d, Balance moyenne : %.6f\n", n, moyenneBalance);
        // Enregistrer les résultats dans un fichier CSV
        enregistrerBalanceMoyenneCSV(filename, n, moyenneBalance);
    }
}




int main() {
//exercice 1 

printf("Exercice 1:\n");
Arbre* arbre=creerArbreVide();
Node* H = creerNode('H', 1);
Node* D = creerNode('D', 2);
Node* B = creerNode('B', 3);
Node* A = creerNode('A', 5);
Node* C = creerNode('C', 8);
Node* E = creerNode('E', 6);
Node* F = creerNode('F', 7);
Node* G = creerNode('G', 9);
Node* I = creerNode('I', 10);
Node* J = creerNode('J', 12);
arbre->racine=H;
// H est la racine
    H->gauche = D; 
    D->parent = H;
    H->droite = I; 
    I->parent = H;
//D
    D->droite = E;
    E->parent = D;
    D->gauche = B;
    B->parent = D;
//B
    B->droite = C;
    C->parent = B;
    B->gauche = A;
    A->parent = B;
  
//F
     F->droite = G;
    G->parent = F;

//E   
    E->droite = F;
    F->parent = E;
 
//I 

    I->droite= J;
    J->parent=I;
   

    printf("Arbre en préordre: ");
    preordre(arbre->racine); 
    printf("\n");
    printf("\n");
    printf("\n");

// Exercice 2
printf("Exercice 2\n");

int cles[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
int priorites[] = {5, 3, 8, 2, 6, 7, 9, 1, 10, 12};
int taille = sizeof(cles) / sizeof(cles[0]);

Arbre* arbreexo2 = creerArbreVide();

for (int i = 0; i < 10; i++) {
    Node* noeudtmp = creerNode(cles[i], priorites[i]);
    insererNode(arbreexo2, noeudtmp);  // Correction ici
}

printf("arbre cartésien :\n");
preordre(arbreexo2->racine);

int cleRecherchee = 'G';
Node* resultat = rechercherNoeud(arbreexo2, cleRecherchee);


//exercice 3
 printf("\n");
  printf("\n");
 printf("Exercice 3\n");
    printf("Insertion Séquence 1 : (A:5), (B:3), (C:8), (D:2), (E:6), (F:7), (G:9), (H:1), (I:10), (J:12)\n");
Arbre* arbre1 = creerArbreVide();
    arbre1 = insererNode(arbre1, creerNode('A', 5));
    arbre1 = insererNode(arbre1, creerNode('B', 3));
    arbre1 = insererNode(arbre1, creerNode('C', 8));
    arbre1 = insererNode(arbre1, creerNode('D', 2));
    arbre1 = insererNode(arbre1, creerNode('E', 6));
    arbre1 = insererNode(arbre1, creerNode('F', 7));
    arbre1 = insererNode(arbre1, creerNode('G', 9));
    arbre1 = insererNode(arbre1, creerNode('H', 1));
    arbre1 = insererNode(arbre1, creerNode('I', 10));
    arbre1 = insererNode(arbre1, creerNode('J', 12));
    
    printf("Node après Insertion Séquence 1 : ");
    preordre(arbre1->racine);
    printf("\n\n");

    // Séquence d'insertion 2
    printf("Insertion Séquence 2 : (H:1), (G:9), (A:5), (B:3), (D:2), (F:7), (C:8), (J:12), (I:10), (E:6)\n");
Arbre* arbre2 = creerArbreVide();
    arbre2 = insererNode(arbre2, creerNode('H', 1));
    arbre2 = insererNode(arbre2, creerNode('G', 9));
    arbre2 = insererNode(arbre2, creerNode('A', 5));
    arbre2 = insererNode(arbre2, creerNode('B', 3));
    arbre2 = insererNode(arbre2, creerNode('D', 2));
    arbre2 = insererNode(arbre2, creerNode('F', 7));
    arbre2 = insererNode(arbre2, creerNode('C', 8));
    arbre2 = insererNode(arbre2, creerNode('J', 12));
    arbre2 = insererNode(arbre2, creerNode('I', 10));
    arbre2 = insererNode(arbre2, creerNode('E', 6));
    
    printf("Arbre après Insertion Séquence 2 : ");
    preordre(arbre2->racine);
    printf("\n\n");

    // Séquence d'insertion 3
    printf("Insertion Séquence 3 : (E:6), (H:1), (B:3), (D:2), (C:8), (F:7), (G:9), (J:12), (A:5), (I:10)\n");
Arbre* arbre3 = creerArbreVide();
    arbre3 = insererNode(arbre3, creerNode('E', 6));
    arbre3 = insererNode(arbre3, creerNode('H', 1));
    arbre3 = insererNode(arbre3, creerNode('B', 3));
    arbre3 = insererNode(arbre3, creerNode('D', 2));
    arbre3 = insererNode(arbre3, creerNode('C', 8));
    arbre3 = insererNode(arbre3, creerNode('F', 7));
    arbre3 = insererNode(arbre3, creerNode('G', 9));
    arbre3 = insererNode(arbre3, creerNode('J', 12));
    arbre3 = insererNode(arbre3, creerNode('A', 5));
    arbre3 = insererNode(arbre3, creerNode('I', 10));

    printf("Node après Insertion Séquence 3 : ");
    preordre(arbre3->racine);
    printf("\n");
    printf("\n");



//Exerice 4 
// Suppression des nœuds dans l'ordre donné
 printf("Exercice 4 \n");
 printf("Arbre après suppression de (%c), (%c), (%c) :\n",A->cle, J->cle, H->cle);
        supprimerNoeud(arbre, A);
        supprimerNoeud(arbre,J);
        supprimerNoeud(arbre,H);

        preordre(arbre->racine);
        printf("\n");
    

//Exerice 5------------------------------------------------------------------------------------
 /*// Rotations--------------------------------------------------------
srand(time(NULL));  // Initialiser le générateur de nombres aléatoires
    
    // Tester avec différents nombres de nœuds et enregistrer les résultats
    for (int taille = 2; taille <= 100; taille += 1) {
        testerArbreCartesien(taille);
    }
    printf("Les résultats ont été enregistrés dans le fichier 'resultats_rotations.csv'.\n");
    




 // hauteur Arbre cartésien--------------------------------------------------------

srand(time(NULL));  // Initialiser le générateur de nombres aléatoires

    const char* filename = "hauteurs_arbre.csv";  // Nom du fichier CSV où les résultats seront enregistrés

    // Créer un fichier avec en-tête
    FILE* fichier = fopen(filename, "w");  // Ouvre le fichier en mode écriture (écrase tout le contenu existant)
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return 1;
    }
    fprintf(fichier, "Nombre de nœuds,Hauteur\n");  // Ajouter un en-tête pour les colonnes
    fclose(fichier);  // Fermer le fichier après avoir ajouté l'en-tête

    testerHauteurArbres(filename, 1000);  // Tester et enregistrer la hauteur des arbres dans un fichier CSV




 // hauteur Arbre cartésien--------------------------------------------------------
srand(time(NULL));  // Initialiser le générateur de nombres aléatoires

    const char* filenamebst = "hauteurs_arbrebst.csv";  // Nom du fichier CSV où les résultats seront enregistrés

    // Créer un fichier avec en-tête
    FILE* fichierbst  = fopen(filenamebst, "w");  // Ouvre le fichier en mode écriture (écrase tout le contenu existant)
    if (fichierbst == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return 1;
    }
    fprintf(fichierbst, "Nombre de nœuds,Hauteur\n");  // Ajouter un en-tête pour les colonnes
    fclose(fichierbst);  // Fermer le fichier après avoir ajouté l'en-tête

    testerHauteurArbresbst(filenamebst, 1000);  // Tester et enregistrer la hauteur des arbres dans un fichier CSV

    printf("Les résultats ont été enregistrés dans le fichier '%s'.\n", filename);*/



//temps cartesien
 /*srand(time(NULL));

    const char* tempsinsertionss = "temps.csv";

    // Créer et ouvrir le fichier pour l'en-tête
    FILE* fichiertemps = fopen(tempsinsertionss, "w");
    if (fichiertemps == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return 1;
    }
    fprintf(fichiertemps, "Nombre de nœuds,Temps d'insertion (secondes)\n");
    fclose(fichiertemps);

    // Tester le temps d'insertion pour des arbres avec différentes tailles de nœuds
    tempsinsertion(tempsinsertionss, 1000);  // Tester jusqu'à 1000 nœuds

    printf("Tests d'insertion terminés. Les résultats ont été enregistrés dans '%s'.\n", tempsinsertionss);


//temps bst
 srand(time(NULL));

    const char* tempsinsertionssbst = "tempsbst.csv";

    // Créer et ouvrir le fichier pour l'en-tête
    FILE* fichiertempsbst = fopen(tempsinsertionssbst, "w");
    if (fichiertempsbst == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return 1;
    }
    fprintf(fichiertempsbst, "Nombre de nœuds,Temps d'insertion (secondes)\n");
    fclose(fichiertempsbst);

    // Tester le temps d'insertion pour des arbres avec différentes tailles de nœuds
    tempsinsertion(tempsinsertionssbst, 1000);  // Tester jusqu'à 1000 nœuds

    printf("Tests d'insertion bst terminés. Les résultats ont été enregistrés dans '%s'.\n", tempsinsertionssbst);*/
int maxNoeuds = 10000;      // Nombre maximal de nœuds
int repetitions = 50  ;      // Nombre de répétitions pour calculer la moyenne
/* /INSERTION
    srand(time(NULL));  // Initialiser le générateur aléatoire

    const char* fichierCartesien = "temps_moyen_cartesien.csv";
    const char* fichierBST = "temps_moyen_bst.csv";


    // Calculer le temps moyen pour un arbre cartésien
    printf("Calcul du temps moyen d'insertion pour un arbre cartésien...\n");
    tempsMoyenInsertionCartesien(fichierCartesien, maxNoeuds, repetitions);

    // Calculer le temps moyen pour un arbre BST
    printf("Calcul du temps moyen d'insertion pour un arbre BST...\n");
    tempsMoyenInsertionBST(fichierBST, maxNoeuds, repetitions);


//RECHERCHE
srand(time(NULL));  // Initialiser le générateur aléatoire

// Fichier pour les arbres cartésiens
const char* fichierCartesienRecherche = "temps_moyen_recherche_cartesien.csv";

// Calculer le temps moyen de recherche pour un arbre cartésien
printf("Calcul du temps moyen de recherche pour un arbre cartésien...\n");
tempsMoyenRechercheCartesien(fichierCartesienRecherche, maxNoeuds, repetitions);

// Fichier pour les arbres BST
const char* fichierBSTRecherche = "temps_moyen_recherche_bst.csv";

// Calculer le temps moyen de recherche pour un arbre BST
printf("Calcul du temps moyen de recherche pour un arbre BST...\n");
tempsMoyenRechercheBST(fichierBSTRecherche, maxNoeuds, repetitions);

//SUPPRESSION
// Initialiser le générateur aléatoire
    srand(time(NULL));

    // Fichiers CSV pour enregistrer les résultats
    const char* fichierSuppressionCartesien = "temps_moyen_suppression_cartesien.csv";
    const char* fichierSuppressionBST = "temps_moyen_suppression_bst.csv";


    // Calculer le temps moyen de suppression pour un arbre cartésien
    printf("Calcul du temps moyen de suppression pour un arbre cartésien...\n");
    tempsMoyenSuppressionCartesien(fichierSuppressionCartesien, maxNoeuds, repetitions);

    // Calculer le temps moyen de suppression pour un BST
    printf("Calcul du temps moyen de suppression pour un arbre BST...\n");
    tempsMoyenSuppressionBST(fichierSuppressionBST, maxNoeuds, repetitions);

    printf("Les résultats ont été enregistrés dans les fichiers CSV.\n");
//HAUTEUR

// Fichiers pour enregistrer les résultats
const char* fichierHauteurCartesien = "hauteur_moyenne_cartesien.csv";
const char* fichierHauteurBST = "hauteur_moyenne_bst.csv";

// Calculer la hauteur moyenne pour les arbres cartésiens
printf("Calcul de la hauteur moyenne pour les arbres cartésiens...\n");
hauteurMoyenneCartesien(fichierHauteurCartesien, maxNoeuds, repetitions);

// Calculer la hauteur moyenne pour les BST
printf("Calcul de la hauteur moyenne pour les arbres BST...\n");
hauteurMoyenneBST(fichierHauteurBST, maxNoeuds, repetitions);

printf("Les résultats ont été enregistrés dans les fichiers CSV.\n");
*/
//BALANCE
    // Initialiser le générateur de nombres aléatoires
    srand(time(NULL));

    // Fichiers CSV pour enregistrer les résultats
    const char* fichierBalanceCartesien = "balance_moyenne_cartesien.csv";
    const char* fichierBalanceBST = "balance_moyenne_bst.csv";


    // Tester la balance moyenne pour les arbres cartésiens
    printf("Calcul de la balance moyenne pour les arbres cartésiens...\n");
    testerBalanceMoyenneCartesien(fichierBalanceCartesien, maxNoeuds, repetitions);

    // Tester la balance moyenne pour les arbres BST
    printf("Calcul de la balance moyenne pour les arbres BST...\n");
    testerBalanceMoyenneBST(fichierBalanceBST, maxNoeuds, repetitions);

    printf("Les résultats ont été enregistrés dans les fichiers :\n");
    printf("- %s (Arbres Cartésiens)\n", fichierBalanceCartesien);
    printf("- %s (Arbres BST)\n", fichierBalanceBST);




/*
srand(time(NULL));  // Initialiser le générateur de nombres aléatoires

    const char* filenametemps = "tempsinsert.csv";  // Nom du fichier CSV où les résultats seront enregistrés

    // Créer un fichier avec en-tête
    FILE* fichiertemps = fopen(filenametemps, "w");  // Ouvre le fichier en mode écriture (écrase tout le contenu existant)
    if (fichiertemps == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return 1;
    }
    fprintf(fichiertemps, "Nombre de nœuds,temps\n");  // Ajouter un en-tête pour les colonnes
    fclose(fichiertemps);  // Fermer le fichier après avoir ajouté l'en-tête
tempsinsertion(filenametemps);
   printf("Les résultats ont été enregistrés dans le fichier '%s'.\n", filenametemps);
*/
// Temps de recherche dans l'arbre cartésien
/*srand(time(NULL));
const char* tempsRechercheCart = "temps_recherche_cartesien.csv";

// Créer un fichier avec en-tête
FILE* fichierCart = fopen(tempsRechercheCart, "w");
if (fichierCart == NULL) {
    printf("Erreur lors de l'ouverture du fichier.\n");
    return 1;
}
fprintf(fichierCart, "Nombre de nœuds,Temps de recherche (secondes)\n");
fclose(fichierCart);

tempsRechercheCartesien(tempsRechercheCart, 1000);  // Tester jusqu'à 1000 nœuds

// Temps de recherche dans le BST
srand(time(NULL));
const char* tempsRechercheBst = "temps_recherche_bst.csv";

// Créer un fichier avec en-tête
FILE* fichierBst = fopen(tempsRechercheBst, "w");
if (fichierBst == NULL) {
    printf("Erreur lors de l'ouverture du fichier.\n");
    return 1;
}
fprintf(fichierBst, "Nombre de nœuds,Temps de recherche (secondes)\n");
fclose(fichierBst);

tempsRechercheBST(tempsRechercheBst, 1000);  // Tester jusqu'à 1000 nœuds

// Temps de suppression dans l'arbre cartésien
srand(time(NULL));
const char* tempsSuppressionCart = "temps_suppression_cartesien.csv";

// Créer un fichier avec en-tête
FILE* fichierSuppCart = fopen(tempsSuppressionCart, "w");
if (fichierSuppCart == NULL) {
    printf("Erreur lors de l'ouverture du fichier.\n");
    return 1;
}
fprintf(fichierSuppCart, "Nombre de nœuds,Temps de suppression (secondes)\n");
fclose(fichierSuppCart);

tempsSuppressionCartesien(tempsSuppressionCart, 1000);  // Tester jusqu'à 1000 nœuds

// Temps de suppression dans le BST
srand(time(NULL));
const char* tempsSuppressionBst = "temps_suppression_bst.csv";

// Créer un fichier avec en-tête
FILE* fichierSuppBst = fopen(tempsSuppressionBst, "w");
if (fichierSuppBst == NULL) {
    printf("Erreur lors de l'ouverture du fichier.\n");
    return 1;
}
fprintf(fichierSuppBst, "Nombre de nœuds,Temps de suppression (secondes)\n");
fclose(fichierSuppBst);

tempsSuppressionBST(tempsSuppressionBst, 1000);  // Tester jusqu'à 1000 nœuds

srand(time(NULL));
const char* fichierEquilibreCartesien = "equilibre_cartesien.csv";

// Créer un fichier CSV avec en-tête
FILE* fichierEquilibre = fopen(fichierEquilibreCartesien, "w");
if (fichierEquilibre == NULL) {
    printf("Erreur lors de l'ouverture du fichier.\n");
    return 1;
}
fprintf(fichierEquilibre, "Nombre de nœuds,Facteur d'équilibre moyen\n");
fclose(fichierEquilibre);

testerFacteurEquilibre(fichierEquilibreCartesien, 1000);
// Générer un BST avec un nombre aléatoire de nœuds
    int nombreNoeuds = 100;  // Modifier selon vos besoins
    Arbre* arbreBST = genererArbreBST(nombreNoeuds);

    // Calculer et enregistrer la balance des sous-arbres
    const char* fichierBalance = "balance_bst.csv";
    enregistrerBalanceBST(arbreBST->racine, fichierBalance);

    printf("Les résultats de la balance des sous-arbres ont été enregistrés dans '%s'.\n", fichierBalance);*/
return 0;
}
