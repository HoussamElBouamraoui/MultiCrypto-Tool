#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "crypto_app.h"

/* Alphabet 25 lettres SANS W */
const char ALPHABET25[] = "ABCDEFGHIJKLMNOPQRSTUVXYZ";

/* ---------- petites fonctions simples ---------- */
static int longueur(const char s[]) {
    int i = 0;
    while (s[i] != '\0') i++;
    return i;
}

char normaliser(char c) {
    c = (char)toupper((unsigned char)c);
    if (c < 'A' || c > 'Z') return 0;
    if (c == 'W') return 'V';
    return c;
}

/* ---------- affichage amélioré ---------- */

void ligne_sep(int n) {
    for (int i = 0; i < n; i++) putchar('=');
    putchar('\n');
}

void titre(const char *t) {
    putchar('\n');
    ligne_sep(52);
    printf("%s\n", t);
    ligne_sep(52);
}

static void afficher_matrice_jolie(char m[5][5]) {
    printf("Matrice 5x5 (W -> V)\n");
    printf("   +---+---+---+---+---+\n");
    for (int i = 0; i < 5; i++) {
        printf("   |");
        for (int j = 0; j < 5; j++) {
            printf(" %c |", m[i][j]);
        }
        printf("\n");
        printf("   +---+---+---+---+---+\n");
    }
}

/* affiche une chaîne par blocs (ex: bloc=2 pour Playfair) */
static void afficher_par_blocs(const char *s, int bloc) {
    int i = 0;
    while (s[i] != '\0') {
        putchar(s[i]);
        i++;
        if (bloc > 0 && i % bloc == 0) putchar(' ');
        if (i % 60 == 0) putchar('\n'); /* retour ligne pour ne pas dépasser */
    }
    if (i % 60 != 0) putchar('\n');
}

/* ---------- recherche position ---------- */

void trouver_pos(char m[5][5], char c, int *r, int *col) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (m[i][j] == c) {
                *r = i;
                *col = j;
                return;
            }
        }
    }
    *r = -1; *col = -1;
}

/* ---------- construction matrice ---------- */
/* remplit m[5][5] avec le mot-clé puis le reste de l'alphabet sans W */
static void construire_matrice(const char *cle, char m[5][5]) {
    int used[26] = {0};
    char seq[25];
    int pos = 0;

    /* 1) lettres du mot-clé (sans doublons) */
    for (int i = 0; cle[i] != '\0'; i++) {
        char c = normaliser(cle[i]);
        if (c == 0) continue;

        if (used[c - 'A'] == 0) {
            used[c - 'A'] = 1;
            seq[pos++] = c;
            if (pos == 25) break;
        }
    }

    /* 2) compléter avec l'alphabet 25 lettres sans W */
    for (int i = 0; ALPHABET25[i] != '\0'; i++) {
        char c = ALPHABET25[i];
        if (used[c - 'A'] == 0) {
            used[c - 'A'] = 1;
            seq[pos++] = c;
            if (pos == 25) break;
        }
    }

    /* 3) remplir la matrice */
    pos = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            m[i][j] = seq[pos++];
        }
    }
}

/* ---------- chiffrement / dechiffrement de 2 lettres ---------- */

void chiffrer_playfair(const char *bigramme_clair, char matrice[5][5], char *bigramme_chiffre) {
    char a = normaliser(bigramme_clair[0]);
    char b = normaliser(bigramme_clair[1]);

    if (a == 0) a = 'X';
    if (b == 0) b = 'X';

    if (a == b) b = 'X';

    int r1, c1, r2, c2;
    trouver_pos(matrice, a, &r1, &c1);
    trouver_pos(matrice, b, &r2, &c2);

    if (r1 == r2) {
        bigramme_chiffre[0] = matrice[r1][(c1 + 1) % 5];
        bigramme_chiffre[1] = matrice[r2][(c2 + 1) % 5];
    } else if (c1 == c2) {
        bigramme_chiffre[0] = matrice[(r1 + 1) % 5][c1];
        bigramme_chiffre[1] = matrice[(r2 + 1) % 5][c2];
    } else {
        bigramme_chiffre[0] = matrice[r1][c2];
        bigramme_chiffre[1] = matrice[r2][c1];
    }

    bigramme_chiffre[2] = '\0';
}

void dechiffrer_playfair(const char *bigramme_chiffre, char matrice[5][5], char *bigramme_clair) {
    char a = normaliser(bigramme_chiffre[0]);
    char b = normaliser(bigramme_chiffre[1]);

    if (a == 0) a = 'X';
    if (b == 0) b = 'X';

    int r1, c1, r2, c2;
    trouver_pos(matrice, a, &r1, &c1);
    trouver_pos(matrice, b, &r2, &c2);

    if (r1 == r2) {
        bigramme_clair[0] = matrice[r1][(c1 + 4) % 5];
        bigramme_clair[1] = matrice[r2][(c2 + 4) % 5];
    } else if (c1 == c2) {
        bigramme_clair[0] = matrice[(r1 + 4) % 5][c1];
        bigramme_clair[1] = matrice[(r2 + 4) % 5][c2];
    } else {
        bigramme_clair[0] = matrice[r1][c2];
        bigramme_clair[1] = matrice[r2][c1];
    }

    bigramme_clair[2] = '\0';
}

/* ---------- chiffrement d'un message complet (simple) ---------- */
char *chiffrer_message_playfair(const char *msg, char matrice[5][5]) {
    char clean[1024];
    int n = 0;

    for (int i = 0; msg[i] != '\0' && n < 1000; i++) {
        char c = normaliser(msg[i]);
        if (c != 0) clean[n++] = c;
    }
    clean[n] = '\0';

    char *out = (char*)malloc(2 * n + 3);
    if (out == NULL) return NULL;

    int i = 0;
    int pos = 0;
    while (i < n) {
        char a = clean[i];
        char b;

        if (i + 1 >= n) {
            b = 'X';
            i++;
        } else {
            b = clean[i + 1];
            if (a == b) {
                b = 'X';
                i++;
            } else {
                i += 2;
            }
        }

        char big[3], ch[3];
        big[0] = a; big[1] = b; big[2] = '\0';
        chiffrer_playfair(big, matrice, ch);

        out[pos++] = ch[0];
        out[pos++] = ch[1];
    }

    out[pos] = '\0';
    return out;
}

/* ---------- générer un bigramme clair aléatoire ---------- */
void bigramme_aleatoire(char *big) {
    int a = rand() % 25;
    int b = rand() % 25;
    while (b == a) b = rand() % 25;

    big[0] = ALPHABET25[a];
    big[1] = ALPHABET25[b];
    big[2] = '\0';
}

/* ---------- utilitaire: vider le reste de la ligne stdin ---------- */
static void vider_ligne(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) { }
}

/* ---------- main : générateur + test decrypt ---------- */
void run_playfair_generator(void) {
    printf("\n=== PLAYFAIR : GENERATEUR / CHIFFREMENT ===\n");
    srand((unsigned int)time(NULL));

    char cle[256];
    char matrice[5][5];

    titre("Playfair 5x5 - Chiffrement / Dechiffrement (Affichage propre)");

    printf("Donner le mot-cle secret : ");
    if (fgets(cle, sizeof(cle), stdin) == NULL) return;

    /* enlever le \n */
    for (int i = 0; cle[i] != '\0'; i++) {
        if (cle[i] == '\n') { cle[i] = '\0'; break; }
    }

    construire_matrice(cle, matrice);

    titre("1) Matrice generee");
    afficher_matrice_jolie(matrice);

    /* (3.1) Test déchiffrement bigramme simple */
    titre("2) Test dechiffrement bigramme");
    {
        char test_ch[3] = "HM"; /* tu peux changer */
        char test_cl[3];
        dechiffrer_playfair(test_ch, matrice, test_cl);
        printf("Bigramme chiffre : %s\n", test_ch);
        printf("Bigramme clair   : %s\n", test_cl);
    }

    /* (3.2) Génération des paires d'indices */
    titre("3) Generation de correspondances aleatoires");
    int N;
    printf("Combien de paires d'indices voulez-vous generer (N) ? ");
    if (scanf("%d", &N) != 1 || N < 0) {
        printf("Erreur: N invalide.\n");
        return;
    }
    vider_ligne();

    printf("\nCorrespondances (Clair -> Chiffre)\n");
    printf("---------------------------------\n");
    for (int i = 0; i < N; i++) {
        char bclair[3], bch[3];
        bigramme_aleatoire(bclair);
        chiffrer_playfair(bclair, matrice, bch);
        printf("%2d) %c%c  ->  %c%c\n", i + 1, bclair[0], bclair[1], bch[0], bch[1]);
    }

    /* Flag (message secret court) */
    titre("4) Chiffrement d'un message (flag)");
    char flag[512];
    printf("Donner un message secret (flag) : ");
    if (fgets(flag, sizeof(flag), stdin) == NULL) return;

    char *crypt = chiffrer_message_playfair(flag, matrice);
    if (crypt == NULL) {
        printf("Erreur: allocation memoire.\n");
        return;
    }

    printf("\nMessage nettoye (lettres uniquement, W->V, majuscules)\n");
    printf("Note: les X peuvent etre ajoutes si double lettre ou longueur impaire.\n");

    printf("\nCryptogramme final (par blocs de 2) :\n");
    afficher_par_blocs(crypt, 2);

    /* Bonus: afficher sans blocs */
    printf("\nCryptogramme final (brut) : %s\n", crypt);

    free(crypt);

    titre("Fin");
    printf("Termine.\n");
    return;
}
