#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//////////////////////////Exercice 1 //////////////////////////////////////////////////
//Structure d'un noeud
typedef struct Node {
    char cle;
    int priorite;
    struct Node* droite;
    struct Node* gauche;
     struct Node* parent;
} Node;


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
Node* obtenirSAGauche(Node* Node) {
    if (Node == NULL) return NULL;  
    return Node->gauche;  
}

Node* obtenirSADroit(Node* Node) {
    if (Node == NULL)  return NULL; 
    return Node->droite;  
}

void afficherNoeud(Node* noeud){
     printf("Clé %c \n Priorité %d\n", noeud->cle, noeud->priorite);
}




//Structure d'un arbre
typedef struct Arbre {
   Node * racine;
}Arbre;

//Constructeurs d'arbres
Arbre* creerArbreVide() {
    Arbre* nouvelarbre= (Arbre*) malloc(sizeof(Arbre));
  
    if( nouvelarbre != NULL)  nouvelarbre->racine= NULL;
    return nouvelarbre;
}

int EstVide(Node* Node) {
    return (Node == NULL);
}


// Afficher l'arbre en parcours pré-ordre 
void preordre(Node* racine) {
    if (racine != NULL) {
        printf("(%c : %d) ", racine->cle, racine->priorite);
        preordre( racine->gauche);
        preordre( racine->droite);
    }
}






//////////////////////////Exercice 2 ////////////////////////////////////////////////////////////////////////////////////
//2a
//Rechercher un noeud dans un arbre cartésien 
Node* rechercherNoeudinterne(Node* racine, int cle) {
    if (racine == NULL) {   
        printf("Le noeud n'a pas été trouvé");
        return NULL;
    }

    if (racine->cle == cle) {  // si la clé du nœud courant correspond à la clé recherchée
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





//////////////////////////Exercice 3 ////////////////////////////////////////////////////////////////////////////////////////

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
        // Si on atteint un arbre vide, cela signifie que le nœud à supprimer n'est pas trouvé
        printf("Le nœud avec la clé %d n'a pas été trouvé dans l'arbre.\n", noeud->cle);
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

//nombre rotations
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
// Fonction pour générer un arbre cartésien avec n nœuds
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
}


//tempsss cartésien 
void enregistrerResultatsCSVtemps(const char* filename, int nbNoeuds, double temps) {
    FILE* fichier = fopen(filename, "a");  // Ouvre le fichier en mode append
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    // Sauvegarde les résultats sous forme de ligne CSV
    fprintf(fichier, "%d,%.6f\n", nbNoeuds, temps);  // Enregistrer le temps en secondes
    fclose(fichier);  // Fermer le fichier après l'écriture
}

// Fonction pour mesurer le temps total d'insertion pour n nœuds arbre cartésien 
void tempsinsertion(const char* filename, int m) {
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
}


//temps recherche
void tempsrecherchecartesien(const char* filename, int m) {
    for (int n = 10; n <= m; n += 1) {  // Tester de 10 à m nœuds
        Arbre* arbre = (Arbre*)malloc(sizeof(Arbre));
        arbre->racine = NULL;
        Node* noeud=genererNoeudAleatoire();
        insererNode(arbre, noeud);
        // Chronométrer le temps d'insertion pour tous les nœuds
        int debut = clock();
rechercherNoeud(arbre,noeud->cle);
        int fin = clock();

        // Calculer le temps total d'insertion
        double tempsEnSecondes = (double)(fin - debut) / CLOCKS_PER_SEC;
        printf("Temps de recherche du noeud:  %f secondes\n", tempsEnSecondes);

        // Enregistrer le résultat dans le fichier CSV
        enregistrerResultatsCSVtemps(filename, n, tempsEnSecondes);
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
// Fonction pour tester la hauteur des arbres et enregistrer les résultats dans un fichier CSV
void testerHauteurArbresbst(const char* filename, int n) {
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
}

void enregistrerTempsMoyen(const char* filename, int nbNoeuds, double tempsMoyen) {
    FILE* fichier = fopen(filename, "a");  // Ouvrir en mode append
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    fprintf(fichier, "%d,%.6f\n", nbNoeuds, tempsMoyen);  // Sauvegarder les résultats
    fclose(fichier);
}

//temps bst
void tempsinsertionbst(const char* filename, int m) {
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
}

// Fonction pour rechercher un nœud dans un BST
Node* rechercherNoeudBST(Node* racine, int valeur) {
    if (racine == NULL || racine->cle == valeur) {
        return racine; // Trouvé ou atteint une feuille sans trouver
    }

    if (valeur < racine->cle) {
        return rechercherNoeudBST(racine->gauche, valeur); // Rechercher dans le sous-arbre gauche
    } else {
        return rechercherNoeudBST(racine->droite, valeur); // Rechercher dans le sous-arbre droit
    }
}
// Fonction pour mesurer le temps de recherche d'un nœud dans un BST
void tempsRechercheBST(const char* filename, int m) {
    for (int n = 10; n <= m; n += 1) { // Tester avec des tailles de 10 à m, incrément de 10
        Arbre* arbre = genererArbreBST(n); // Générer un arbre avec n nœuds
        int valeurRecherchee = rand() % 10000; // Générer une valeur aléatoire pour la recherche

        // Chronométrer le temps de recherche
        clock_t debut = clock();
        rechercherNoeudBST(arbre->racine, valeurRecherchee);
        clock_t fin = clock();

        // Calculer le temps total de recherche
        double tempsEnSecondes = (double)(fin - debut) / CLOCKS_PER_SEC;
        printf("Temps de recherche pour un arbre de %d nœuds : %f secondes\n", n, tempsEnSecondes);

        // Enregistrer le résultat dans le fichier CSV
        enregistrerResultatsCSVtemps(filename, n, tempsEnSecondes);

        free(arbre); // Libérer la mémoire de l'arbre
    }
}


// Fonction pour mesurer le temps de suppression d'un nœud dans un arbre cartésien
void tempsSuppressionCartesien(const char* filename, int m) {
    for (int n = 10; n <= m; n += 1) { // Tester avec des tailles de 10 à m
        Arbre* arbre = genererArbre(n); // Générer un arbre cartésien avec n nœuds

        // Générer un nœud existant à supprimer (choisir la racine par simplicité ici)
        Node* noeudASupprimer = arbre->racine;

        // Chronométrer le temps de suppression
        clock_t debut = clock();
        supprimerNoeud(arbre, noeudASupprimer); // Supprimer le nœud
        clock_t fin = clock();

        // Calculer le temps total de suppression
        double tempsEnSecondes = (double)(fin - debut) / CLOCKS_PER_SEC;
        printf("Temps de suppression pour un arbre cartésien de %d nœuds : %f secondes\n", n, tempsEnSecondes);

        // Enregistrer le résultat dans le fichier CSV
        enregistrerResultatsCSVtemps(filename, n, tempsEnSecondes);

        free(arbre); // Libérer la mémoire de l'arbre
    }
}

//temps insertions
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
 // Rotations--------------------------------------------------------
srand(time(NULL));  // Initialiser le générateur de nombres aléatoires
    
    // Tester avec différents nombres de nœuds et enregistrer les résultats
    for (int taille = 2; taille <= 1000; taille += 1) {
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

    printf("Les résultats ont été enregistrés dans le fichier '%s'.\n", filename);


//temps insertion
    srand(time(NULL));  // Initialiser la graine pour les valeurs aléatoires
    const char* fichierCartesien = "temps_moyen_cartesien.csv";
    const char* fichierBST = "temps_moyen_bst.csv";

    int maxNoeuds = 10000;      // Nombre maximal de nœuds
    int repetitions = 1;      // Nombre de répétitions pour calculer la moyenne

    // Calculer le temps moyen pour un arbre cartésien
    printf("Calcul du temps moyen d'insertion pour un arbre cartésien...\n");
    tempsMoyenInsertionCartesien(fichierCartesien, maxNoeuds, repetitions);

    // Calculer le temps moyen pour un arbre BST
    printf("Calcul du temps moyen d'insertion pour un arbre BST...\n");
    tempsMoyenInsertionBST(fichierBST, maxNoeuds, repetitions);



//temps cartesien recherche
 srand(time(NULL));

    const char* tempsrecherche = "tempsrecherche.csv";

    // Créer et ouvrir le fichier pour l'en-tête
    FILE* fichiertempsrecherche = fopen(tempsrecherche, "w");
    if (fichiertempsrecherche == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return 1;
    }
    fprintf(fichiertempsrecherche, "Nombre de nœuds,Temps d'insertion (secondes)\n");
    fclose(fichiertempsrecherche);

    // Tester le temps d'insertion pour des arbres avec différentes tailles de nœuds
    tempsrecherchecartesien(tempsrecherche, 1000);  // Tester jusqu'à 1000 nœuds

    printf("Tests de recherche terminés. Les résultats ont été enregistrés dans '%s'.\n", tempsrecherche);



//temps recherche bst
srand(time(NULL)); // Initialiser la graine pour les valeurs aléatoires

    const char* tempsRechercheFichier = "temps_recherche_bst.csv";

    // Créer et ouvrir le fichier pour y écrire l'en-tête
    FILE* fichierTempsRecherche = fopen(tempsRechercheFichier, "w");
    if (fichierTempsRecherche == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return 1;
    }
    fprintf(fichierTempsRecherche, "Nombre de nœuds,Temps de recherche (secondes)\n");
    fclose(fichierTempsRecherche);

    // Tester le temps de recherche pour différentes tailles d'arbres
    tempsRechercheBST(tempsRechercheFichier, 1000); // Tester jusqu'à 1000 nœuds

    printf("Tests de recherche BST terminés. Les résultats ont été enregistrés dans '%s'.\n", tempsRechercheFichier);
    



//temps recherche bst
    srand(time(NULL)); // Initialiser la graine pour les valeurs aléatoires
    const char* tempssuppr = "tempssuppr.csv";
    FILE* fichiersuppr = fopen(tempssuppr, "w");
    if (fichiersuppr == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return 1;
    }
    fprintf(fichiersuppr, "Nombre de nœuds,Temps de suppr (secondes)\n");
    fclose(fichiersuppr);

    // Tester le temps de recherche pour différentes tailles d'arbres
    tempsRechercheBST(tempssuppr, 1000); // Tester jusqu'à 1000 nœuds

    printf("Tests de suppr terminés. Les résultats ont été enregistrés dans '%s'.\n", tempssuppr);
    




//temps suppression


return 0;
}
