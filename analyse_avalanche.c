#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "crypto_app.h"




void des_encrypt(unsigned char message[8],
                 unsigned char key[8],
                 unsigned char ciphertext[8]) {
    for (int i = 0; i < 8; i++) {
        unsigned char x = message[i] ^ key[i];

        // mélange non linéaire (simulateur d'avalanche)
        x = (x << 3) | (x >> 5);
        x ^= (x * 29);

        ciphertext[i] = x;
    }
}

/*
void des_encrypt(unsigned char message[8], unsigned char key[8], unsigned char ciphertext[8]) {
    for(int i=0;i<8;i++) {
        ciphertext[i] = message[i] ^ key[i];
    }
}*/

// Inverse un bit dans un message de 64 bits
void flip_bit(unsigned char message[8], int bit_index) {
    int byte_index = bit_index / 8;
    int bit_pos = bit_index % 8;
    message[byte_index] ^= (1 << (7 - bit_pos));
}

// Calcul de la distance de Hamming
int hamming_distance(unsigned char c1[8], unsigned char c2[8]) {
    int dist = 0;
    for(int i=0;i<8;i++) {
        unsigned char xor = c1[i] ^ c2[i];
        for(int j=0;j<8;j++)
            if(xor & (1 << j)) dist++;
    }
    return dist;
}

static void print_block_hex(const unsigned char b[8]) {
    for (int i = 0; i < 8; i++) printf("%02X", b[i]);
}

void run_analyse_avalanche(void) {
    int choix;

    while (1) {
        printf("\n================= DES Avalanche MENU =================\n");
        printf("1) Lancer une campagne avalanche (N tests) + export CSV\n");
        printf("2) Un seul test detaille (message fixe, bit choisi)\n");
        printf("0) Quitter\n");
        printf("Votre choix: ");

        if (scanf("%d", &choix) != 1) return;

        switch (choix) {
            case 1: {
                int N;
                char filename[256];

                printf("Nombre de tests N (ex: 1000): ");
                if (scanf("%d", &N) != 1 || N <= 0) { printf("N invalide.\n"); break; }

                printf("Nom du fichier CSV (ex: avalanche.csv): ");
                if (scanf("%255s", filename) != 1) { printf("Nom invalide.\n"); break; }

                unsigned char key[8] = {0x13,0x34,0x57,0x79,0x9B,0xBC,0xDF,0xF1};
                unsigned char message[8], message2[8], C1[8], C2[8];
                long long total = 0;

                srand((unsigned int)time(NULL));

                FILE *f = fopen(filename, "w");
                if (!f) { printf("Impossible d'ouvrir %s\n", filename); break; }
                fprintf(f, "test;bit;hd\n");

                for (int t = 0; t < N; t++) {
                    for (int i = 0; i < 8; i++) message[i] = (unsigned char)(rand() % 256);
                    memcpy(message2, message, 8);

                    int bit = rand() % 64;
                    flip_bit(message2, bit);

                    des_encrypt(message, key, C1);
                    des_encrypt(message2, key, C2);

                    int hd = hamming_distance(C1, C2);
                    total += hd;
                    fprintf(f, "%d;%d;%d\n", t, bit, hd);
                }

                fclose(f);
                printf("Fini. Moyenne HD = %.2f\n", (double)total / N);
                break;
            }

            case 2: {
                unsigned char key[8] = {0x13,0x34,0x57,0x79,0x9B,0xBC,0xDF,0xF1};
                unsigned char message[8] = {0};
                unsigned char message2[8], C1[8], C2[8];
                int bit;

                printf("Saisir 8 octets du message en hex (ex: 01 02 03 04 05 06 07 08): ");
                unsigned int tmp;
                for (int i = 0; i < 8; i++) {
                    if (scanf("%x", &tmp) != 1) return;
                    message[i] = (unsigned char)tmp;
                }

                printf("Choisir le bit a inverser (0..63): ");
                if (scanf("%d", &bit) != 1 || bit < 0 || bit > 63) { printf("Bit invalide.\n"); break; }

                memcpy(message2, message, 8);
                flip_bit(message2, bit);

                des_encrypt(message, key, C1);
                des_encrypt(message2, key, C2);

                printf("Message1 : "); print_block_hex(message); printf("\n");
                printf("Message2 : "); print_block_hex(message2); printf("\n");
                printf("C1       : "); print_block_hex(C1); printf("\n");
                printf("C2       : "); print_block_hex(C2); printf("\n");

                printf("Distance de Hamming = %d\n", hamming_distance(C1, C2));
                break;
            }

            case 0:
                return;

            default:
                printf("Choix invalide.\n");
        }
    }
}
