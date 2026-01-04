#include <stdio.h>
#include <stdint.h>
#include "crypto_app.h"

// Multiplication par 02 dans GF(2^8)
unsigned char xtime(unsigned char b) {
    // Décalage à gauche (équivaut à multiplication par x)
    unsigned char res = b << 1;
    // Si le bit de poids fort était 1, on réduit modulo le polynôme P(X)=x^8+x^4+x^3+x+1 (0x1B)
    if (b & 0x80) res ^= 0x1B;
    return res;
}

// Multiplication dans GF(2^8) par 01, 02 ou 03
unsigned char mul_gf(unsigned char b, int facteur) {
    switch(facteur) {
        case 1: return b;
        case 2: return xtime(b);
        case 3: return xtime(b) ^ b; // 3*b = 2*b + b
        default: return 0;
    }
}

// Transformation MixColumns pour une colonne (4 octets)
void mix_column(unsigned char col[4], unsigned char res[4]) {
    // Matrice standard AES pour MixColumns
    // [02 03 01 01]
    // [01 02 03 01]
    // [01 01 02 03]
    // [03 01 01 02]
    res[0] = mul_gf(col[0],2) ^ mul_gf(col[1],3) ^ mul_gf(col[2],1) ^ mul_gf(col[3],1);
    res[1] = mul_gf(col[0],1) ^ mul_gf(col[1],2) ^ mul_gf(col[2],3) ^ mul_gf(col[3],1);
    res[2] = mul_gf(col[0],1) ^ mul_gf(col[1],1) ^ mul_gf(col[2],2) ^ mul_gf(col[3],3);
    res[3] = mul_gf(col[0],3) ^ mul_gf(col[1],1) ^ mul_gf(col[2],1) ^ mul_gf(col[3],2);
}

void run_aes_mixcolumns(void) {
    int choix;

    while (1) {
        printf("\n================= AES MixColumns MENU =================\n");
        printf("1) Tester xtime(b)  (multiplication par 02)\n");
        printf("2) Tester mul_gf(b, facteur)\n");
        printf("3) Tester mix_column sur une colonne de 4 octets\n");
        printf("0) Quitter\n");
        printf("Votre choix: ");

        if (scanf("%d", &choix) != 1) return;

        if (choix == 0) break;

        switch (choix) {
            case 1: {
                unsigned int b;
                printf("b (00..FF en hex): ");
                scanf("%x", &b);
                printf("xtime(0x%02X) = 0x%02X\n", b & 0xFF, xtime((unsigned char)b));
                break;
            }
            case 2: {
                unsigned int b; int f;
                printf("b (00..FF en hex): ");
                scanf("%x", &b);
                printf("facteur (1,2,3): ");
                scanf("%d", &f);
                printf("mul_gf(0x%02X, %d) = 0x%02X\n", b & 0xFF, f, mul_gf((unsigned char)b, f));
                break;
            }
            case 3: {
                unsigned char col[4];
                unsigned char res[4];
                unsigned int x;
                printf("Entrer 4 octets en hex (ex: 87 0B 7B 4D): ");
                for (int i = 0; i < 4; i++) {
                    if (scanf("%x", &x) != 1) return;
                    col[i] = (unsigned char)x;
                }
                mix_column(col, res);
                printf("Resultat: %02X %02X %02X %02X\n", res[0], res[1], res[2], res[3]);
                break;
            }
            default:
                printf("Choix invalide.\n");
        }
    }
}
