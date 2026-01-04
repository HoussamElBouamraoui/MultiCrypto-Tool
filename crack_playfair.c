#include "crypto_app.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAXN 200
#define MAX_SOL 10

/* Alphabet Playfair 25 lettres (W supprimé) */
const char A25[] = "ABCDEFGHIJKLMNOPQRSTUVXYZ";

typedef struct {
    char p[3]; /* plaintext bigram */
    char c[3]; /* cipher bigram */
} Pair;

/* -------------------- Outils de base -------------------- */
static void vider_stdin(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
}

static int longueur(const char s[]) {
    int i = 0;
    while (s[i] != '\0') i++;
    return i;
}

static char norm(char ch) {
    ch = (char)toupper((unsigned char)ch);
    if (ch < 'A' || ch > 'Z') return 0;
    if (ch == 'W') return 'V'; /* convention 25 lettres */
    return ch;
}

static void norm_bigramme(char b[3]) {
    b[0] = norm(b[0]); if (b[0] == 0) b[0] = 'X';
    b[1] = norm(b[1]); if (b[1] == 0) b[1] = 'X';
    b[2] = '\0';
}

static int row_of(int pos) { return pos / 5; }
static int col_of(int pos) { return pos % 5; }

/* -------------------- Données globales -------------------- */
Pair pairs[MAXN];
int N;

int posLet[26];     /* position de chaque lettre (0..24) ou -1 */
int occ[25];        /* lettre occupant la position p (0..25) ou -1 */

int letters[26];    /* lettres impliquées dans les contraintes */
int mLetters;
int countLet[26];   /* fréquence d'apparition dans les contraintes */
int solutions = 0;

/* -------------------- Vérification de contraintes -------------------- */
static int check_req(int L, int reqPos) {
    if (posLet[L] == -1) {
        if (occ[reqPos] != -1) return 0; /* case déjà occupée */
        return 1;
    }
    return (posLet[L] == reqPos);
}

static int case_ligne_possible(int A, int B, int C, int D) {
    if (posLet[A] != -1 && posLet[B] != -1) {
        if (row_of(posLet[A]) != row_of(posLet[B])) return 0;
    }

    if (posLet[A] != -1) {
        int reqC = row_of(posLet[A]) * 5 + ((col_of(posLet[A]) + 1) % 5);
        if (!check_req(C, reqC)) return 0;
    }
    if (posLet[B] != -1) {
        int reqD = row_of(posLet[B]) * 5 + ((col_of(posLet[B]) + 1) % 5);
        if (!check_req(D, reqD)) return 0;
    }

    if (posLet[C] != -1) {
        int reqA = row_of(posLet[C]) * 5 + ((col_of(posLet[C]) + 4) % 5);
        if (!check_req(A, reqA)) return 0;
    }
    if (posLet[D] != -1) {
        int reqB = row_of(posLet[D]) * 5 + ((col_of(posLet[D]) + 4) % 5);
        if (!check_req(B, reqB)) return 0;
    }

    return 1;
}

static int case_colonne_possible(int A, int B, int C, int D) {
    if (posLet[A] != -1 && posLet[B] != -1) {
        if (col_of(posLet[A]) != col_of(posLet[B])) return 0;
    }

    if (posLet[A] != -1) {
        int reqC = ((row_of(posLet[A]) + 1) % 5) * 5 + col_of(posLet[A]);
        if (!check_req(C, reqC)) return 0;
    }
    if (posLet[B] != -1) {
        int reqD = ((row_of(posLet[B]) + 1) % 5) * 5 + col_of(posLet[B]);
        if (!check_req(D, reqD)) return 0;
    }

    if (posLet[C] != -1) {
        int reqA = ((row_of(posLet[C]) + 4) % 5) * 5 + col_of(posLet[C]);
        if (!check_req(A, reqA)) return 0;
    }
    if (posLet[D] != -1) {
        int reqB = ((row_of(posLet[D]) + 4) % 5) * 5 + col_of(posLet[D]);
        if (!check_req(B, reqB)) return 0;
    }

    return 1;
}

static int case_rectangle_possible(int A, int B, int C, int D) {
    if (posLet[A] != -1 && posLet[B] != -1) {
        int rA = row_of(posLet[A]), cA = col_of(posLet[A]);
        int rB = row_of(posLet[B]), cB = col_of(posLet[B]);

        if (rA == rB || cA == cB) return 0; /* si même ligne/col, ce n'est pas un rectangle */

        int reqC = rA * 5 + cB;
        int reqD = rB * 5 + cA;

        if (!check_req(C, reqC)) return 0;
        if (!check_req(D, reqD)) return 0;
    }

    if (posLet[C] != -1 && posLet[D] != -1) {
        int rC = row_of(posLet[C]), cC = col_of(posLet[C]);
        int rD = row_of(posLet[D]), cD = col_of(posLet[D]);

        if (rC == rD || cC == cD) return 0;

        int reqA = rC * 5 + cD;
        int reqB = rD * 5 + cC;

        if (!check_req(A, reqA)) return 0;
        if (!check_req(B, reqB)) return 0;
    }

    return 1;
}

static int pair_possible(Pair *pr) {
    int A = pr->p[0] - 'A';
    int B = pr->p[1] - 'A';
    int C = pr->c[0] - 'A';
    int D = pr->c[1] - 'A';

    int okL = case_ligne_possible(A, B, C, D);
    int okC = case_colonne_possible(A, B, C, D);
    int okR = case_rectangle_possible(A, B, C, D);

    return (okL || okC || okR);
}

static int contraintes_ok(void) {
    for (int i = 0; i < N; i++) {
        if (!pair_possible(&pairs[i])) return 0;
    }
    return 1;
}

/* -------------------- Construction matrice candidate -------------------- */
static void construire_matrice(char mat[5][5]) {
    int used[26] = {0};

    for (int p = 0; p < 25; p++) {
        if (occ[p] != -1) used[occ[p]] = 1;
    }

    /* Liste des lettres restantes selon A25 (donc sans W) */
    int idxRem = 0;
    char rem[25];
    for (int i = 0; A25[i] != '\0'; i++) {
        int L = A25[i] - 'A';
        if (!used[L]) rem[idxRem++] = A25[i];
    }

    int t = 0;
    for (int p = 0; p < 25; p++) {
        int r = p / 5;
        int c = p % 5;
        if (occ[p] != -1) {
            mat[r][c] = (char)('A' + occ[p]);
        } else {
            mat[r][c] = rem[t++];
        }
    }
}

static void build_posmap(char mat[5][5], int posMap[26]) {
    for (int i = 0; i < 26; i++) posMap[i] = -1;
    for (int r = 0; r < 5; r++) {
        for (int c = 0; c < 5; c++) {
            posMap[mat[r][c] - 'A'] = r * 5 + c;
        }
    }
}

/* -------------------- Déchiffrement Playfair -------------------- */
static void dechiffrer_bigramme(const char *bg, char mat[5][5], int posMap[26], char out[3]) {
    int A = bg[0] - 'A';
    int B = bg[1] - 'A';

    int pA = posMap[A];
    int pB = posMap[B];

    int rA = row_of(pA), cA = col_of(pA);
    int rB = row_of(pB), cB = col_of(pB);

    if (rA == rB) {
        /* même ligne: décalage à gauche */
        out[0] = mat[rA][(cA + 4) % 5];
        out[1] = mat[rB][(cB + 4) % 5];
    } else if (cA == cB) {
        /* même colonne: décalage vers le haut */
        out[0] = mat[(rA + 4) % 5][cA];
        out[1] = mat[(rB + 4) % 5][cB];
    } else {
        /* rectangle: échange colonnes */
        out[0] = mat[rA][cB];
        out[1] = mat[rB][cA];
    }
    out[2] = '\0';
}

/* -------------------- Affichage amélioré -------------------- */
static void afficher_matrice_jolie(char mat[5][5]) {
    printf("   +---+---+---+---+---+\n");
    for (int i = 0; i < 5; i++) {
        printf("   |");
        for (int j = 0; j < 5; j++) {
            printf(" %c |", mat[i][j]);
        }
        printf("\n");
        printf("   +---+---+---+---+---+\n");
    }
}

static void afficher_par_blocs(const char *s, int bloc) {
    int i = 0;
    while (s[i] != '\0') {
        putchar(s[i]);
        i++;
        if (bloc > 0 && i % bloc == 0) putchar(' ');
    }
}

static void afficher_solution(const char *crypt) {
    char mat[5][5];
    int posMap[26];

    construire_matrice(mat);
    build_posmap(mat, posMap);

    printf("\n=================================================\n");
    printf(" Solution #%d\n", solutions);
    printf("=================================================\n");

    printf("Matrice 5x5 (W remplace par V) :\n");
    afficher_matrice_jolie(mat);

    int n = longueur(crypt);
    printf("\nCryptogramme (%d chars) : ", n);
    afficher_par_blocs(crypt, 2);
    printf("\n");

    printf("Dechiffrement           : ");

    int outCount = 0;
    for (int i = 0; i < n; i += 2) {
        char bg[3], cl[3];
        bg[0] = crypt[i];
        bg[1] = (i + 1 < n) ? crypt[i + 1] : 'X';
        bg[2] = '\0';

        dechiffrer_bigramme(bg, mat, posMap, cl);

        printf("%c%c", cl[0], cl[1]);
        outCount += 2;

        putchar(' ');

        /* retour à la ligne pour lisibilité */
        if (outCount % 50 == 0) {
            printf("\n                         ");
        }
    }
    printf("\n");
}

/* -------------------- Backtracking -------------------- */
static void backtrack(int idx, const char *crypt) {
    if (solutions >= MAX_SOL) return;

    if (idx == mLetters) {
        solutions++;
        afficher_solution(crypt);
        return;
    }

    int L = letters[idx];

    for (int p = 0; p < 25; p++) {
        if (occ[p] == -1) {
            posLet[L] = p;
            occ[p] = L;

            if (contraintes_ok()) {
                backtrack(idx + 1, crypt);
            }

            occ[p] = -1;
            posLet[L] = -1;
        }
    }
}

/* tri simple (bulle) par fréquence décroissante */
static void trier_lettres(void) {
    for (int i = 0; i < mLetters; i++) {
        for (int j = i + 1; j < mLetters; j++) {
            if (countLet[letters[j]] > countLet[letters[i]]) {
                int tmp = letters[i];
                letters[i] = letters[j];
                letters[j] = tmp;
            }
        }
    }
}

/* -------------------- Programme principal -------------------- */
void run_playfair_crack(void) {
    printf("\n=== PLAYFAIR : CRYPTANALYSE / CRACK ===\n");
    printf("=============================================\n");
    printf("  Alphabet: 25 lettres (W -> V)\n");
    printf("  Max solutions affichees: %d\n", MAX_SOL);
    printf("=============================================\n\n");

    printf("Combien de correspondances (N) ? ");
    if (scanf("%d", &N) != 1 || N < 0 || N > MAXN) {
        printf("Erreur: N invalide.\n");
        return;
    }

    for (int i = 0; i < 26; i++) { posLet[i] = -1; countLet[i] = 0; }
    for (int i = 0; i < 25; i++) occ[i] = -1;

    printf("\nDonner les correspondances (ex: TH HM)\n");
    for (int i = 0; i < N; i++) {
        if (scanf(" %2s %2s", pairs[i].p, pairs[i].c) != 2) {
            printf("Erreur de lecture sur la paire #%d.\n", i + 1);
            return;
        }

        norm_bigramme(pairs[i].p);
        norm_bigramme(pairs[i].c);

        countLet[pairs[i].p[0] - 'A']++;
        countLet[pairs[i].p[1] - 'A']++;
        countLet[pairs[i].c[0] - 'A']++;
        countLet[pairs[i].c[1] - 'A']++;
    }

    printf("\nPaires normalisees:\n");
    for (int i = 0; i < N; i++) {
        printf("  %2s -> %2s\n", pairs[i].p, pairs[i].c);
    }

    char crypt[2048];
    printf("\nDonner le cryptogramme final : ");
    if (scanf(" %2047s", crypt) != 1) {
        printf("Erreur de lecture du cryptogramme.\n");
        return;
    }
    vider_stdin();

    /* normaliser le cryptogramme */
    for (int i = 0; crypt[i] != '\0'; i++) {
        crypt[i] = norm(crypt[i]);
        if (crypt[i] == 0) crypt[i] = 'X';
    }

    /* construire la liste des lettres impliquées dans les paires */
    int used[26] = {0};
    for (int i = 0; i < N; i++) {
        used[pairs[i].p[0] - 'A'] = 1;
        used[pairs[i].p[1] - 'A'] = 1;
        used[pairs[i].c[0] - 'A'] = 1;
        used[pairs[i].c[1] - 'A'] = 1;
    }

    mLetters = 0;
    for (int i = 0; i < 26; i++) {
        if (used[i]) letters[mLetters++] = i;
    }

    trier_lettres();

    printf("\nLettres impliquees dans les contraintes: %d\n", mLetters);
    printf("Ordre de placement (heuristique frequence): ");
    for (int i = 0; i < mLetters; i++) {
        printf("%c ", (char)('A' + letters[i]));
    }
    printf("\n");

    if (!contraintes_ok()) {
        printf("\nTes correspondances sont contradictoires (aucune matrice possible).\n");
        return;
    }

    printf("\nRecherche de matrices candidates...\n");
    backtrack(0, crypt);

    if (solutions == 0) {
        printf("\nAucune matrice trouvee (paires incoherentes ou trop fortes).\n");
    } else {
        printf("\nTotal solutions affichees: %d (max %d)\n", solutions, MAX_SOL);
    }

    return;
}
