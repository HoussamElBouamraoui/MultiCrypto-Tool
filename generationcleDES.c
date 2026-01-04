#include <stdio.h>

void run_generationcleDES(void);

void left_shift(int tab[28], int shifts) {
    int temp[2];

    for (int s = 0; s < shifts; s++) {
        temp[0] = tab[0];
        for (int i = 0; i < 27; i++)
            tab[i] = tab[i + 1];
        tab[27] = temp[0];
    }
}

void generer_sous_cles(int cle_maitre[64], int sous_cles[16][48]) {


int CP1[8][7] = {
    {57,49,41,33,25,17,9},
    {1,58,50,42,34,26,18},
    {10,2,59,51,43,35,27},
    {19,11,3,60,52,44,36},
    {63,55,47,39,31,23,15},
    {7,62,54,46,38,30,22},
    {14,6,61,53,45,37,29},
    {21,13,5,28,20,12,4}
};

int CP2[48] = {
    14,17,11,24,1,5,3,28,
    15,6,21,10,23,19,12,4,
    26,8,16,7,27,20,13,2,
    41,52,31,37,47,55,30,40,
    51,45,33,48,44,49,39,56,
    34,53,46,42,50,36,29,32
};

int R[16] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

    int tab56[8][7];

    /* ---- CP-1 ---- */
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 7; j++) {
            tab56[i][j] = cle_maitre[ CP1[i][j] - 1 ];
        }
    }

    /* ---- Split into C and D ---- */
    int C[4][7], D[4][7];

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 7; j++)
            C[i][j] = tab56[i][j];

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 7; j++)
            D[i][j] = tab56[i + 4][j];

    /* ---- Convert to 1D ---- */
    int C1D[28], D1D[28];
    int idx = 0;

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 7; j++)
            C1D[idx++] = C[i][j];

    idx = 0;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 7; j++)
            D1D[idx++] = D[i][j];

    /* ---- 16 rounds ---- */
    for (int r = 0; r < 16; r++) {

        left_shift(C1D, R[r]);
        left_shift(D1D, R[r]);

        int CD[56];
        for (int i = 0; i < 28; i++) {
            CD[i] = C1D[i];
            CD[i + 28] = D1D[i];
        }

        /* ---- CP-2 ---- */
        for (int i = 0; i < 48; i++) {
            sous_cles[r][i] = CD[ CP2[i] - 1 ];
        }
    }
}

static int hexval(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return -1;
}

static int parse_key_hex16_to_bits(const char *hex16, int bits64[64]) {
    // hex16 = 16 caract�res hex -> 8 octets -> 64 bits
    for (int i = 0; i < 16; i++) {
        if (hexval(hex16[i]) < 0) return 0;
    }

    int k = 0;
    for (int byte = 0; byte < 8; byte++) {
        int hi = hexval(hex16[2*byte]);
        int lo = hexval(hex16[2*byte + 1]);
        unsigned char b = (unsigned char)((hi << 4) | lo);

        // bits du MSB vers LSB
        for (int bit = 7; bit >= 0; bit--) {
            bits64[k++] = (b >> bit) & 1;
        }
    }
    return 1;
}

static void print_subkey_bits(const int sk[48]) {
    for (int i = 0; i < 48; i++) printf("%d", sk[i]);
    printf("\n");
}

void run_generationcleDES(void) {
    int choix;

    while (1) {
        printf("\n================= DES Key Schedule MENU =================\n");
        printf("1) Generer et afficher les 16 sous-cles (cle en hex)\n");
        printf("2) Afficher une sous-cle specifique (round 1..16)\n");
        printf("0) Quitter\n");
        printf("Votre choix: ");

        if (scanf("%d", &choix) != 1) return;

        switch (choix) {
            case 1: {
                char hex16[17];
                int cle_maitre[64];
                int sous_cles[16][48];

                printf("Entrer la cle DES en hex (16 caracteres) ex: 133457799BBCDFF1\n");
                printf("Cle: ");
                scanf("%16s", hex16);

                if (!parse_key_hex16_to_bits(hex16, cle_maitre)) {
                    printf("Cle invalide (il faut exactement 16 hex).\n");
                    break;
                }

                generer_sous_cles(cle_maitre, sous_cles);

                for (int r = 0; r < 16; r++) {
                    printf("Sous-cle %2d : ", r + 1);
                    print_subkey_bits(sous_cles[r]);
                }
                break;
            }

            case 2: {
                char hex16[17];
                int cle_maitre[64];
                int sous_cles[16][48];
                int round;

                printf("Entrer la cle DES en hex (16 caracteres) ex: 133457799BBCDFF1\n");
                printf("Cle: ");
                scanf("%16s", hex16);

                if (!parse_key_hex16_to_bits(hex16, cle_maitre)) {
                    printf("Cle invalide (il faut exactement 16 hex).\n");
                    break;
                }

                printf("Round (1..16): ");
                scanf("%d", &round);
                if (round < 1 || round > 16) {
                    printf("Round invalide.\n");
                    break;
                }

                generer_sous_cles(cle_maitre, sous_cles);
                printf("Sous-cle %2d : ", round);
                print_subkey_bits(sous_cles[round - 1]);
                break;
            }

            case 0:
                printf("Fin.\n");
                return;

            default:
                printf("Choix invalide.\n");
        }
    }
}

#ifdef GENERATIONCLEDES_STANDALONE
int main(void) {
    run_generationcleDES();
    return 0;
}
#endif
