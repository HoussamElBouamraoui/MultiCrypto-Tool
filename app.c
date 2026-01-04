#include <stdio.h>
#include <stdlib.h>
#include "crypto_app.h"

int main(void) {
    int choix;
    char line[32];

    do {
        printf("\n==============================\n");
        printf(" CRYPTO APP - MENU TERMINAL\n");
        printf("==============================\n");
        printf("1) Cesar\n");
        printf("2) Transposition\n");
        printf("3) Playfair Generateur\n");
        printf("4) Playfair Crack\n");
        printf("5) AES MixColumns\n");
        printf("6) Analyse avalanche (DES demo)\n");
        printf("7) RSA utilitaires\n");
        printf("0) Quitter\n");
        printf("Votre choix : ");

        if (!fgets(line, sizeof(line), stdin)) {
            break; // fin d'entrée
        }
        choix = atoi(line);

        switch (choix) {
            case 1: run_cesar(); break;
            case 2: run_transposition(); break;
            case 3: run_playfair_generator(); break;
            case 4: run_playfair_crack(); break;
            case 5: run_aes_mixcolumns(); break;
            case 6: run_analyse_avalanche(); break;
            case 7: run_rsa(); break;
            case 0: printf("Au revoir.\n"); break;
            default: printf("Choix invalide.\n");
        }
    } while (choix != 0);

    return 0;
}
