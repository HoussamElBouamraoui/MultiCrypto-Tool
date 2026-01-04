#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crypto_app.h"

/* ========= Helpers ========= */
static void lire_ligne(char *buf, size_t n){
    if(fgets(buf, (int)n, stdin)){
        size_t L = strlen(buf);
        if(L > 0 && buf[L-1] == '\n') buf[L-1] = '\0';
    }
}

static void vider_stdin(void){
    int c;
    while((c = getchar()) != '\n' && c != EOF) {}
}

/* ========= Chiffrement ========= */
char* chiffrer_transposition(const char *clair,int k){
    if(clair == NULL){
        return NULL;
    }
    if(k <= 0){
        return NULL;
    }

    int len = (int)strlen(clair);

    // IMPORTANT: ton tableau est [20][20] -> on s�curise
    if(k > 20){
        printf("[ERREUR] k > 20 (ton tableau tab[20][20] ne supporte pas)\n");
        return NULL;
    }

    int lignes;
    if(len % k == 0){
        lignes = len / k ;
    }else{
        lignes = (len / k) + 1;
    }

    if(lignes > 20){
        printf("[ERREUR] lignes > 20 (ton tableau tab[20][20] ne supporte pas)\n");
        return NULL;
    }

    char *phrase = (char *)malloc((size_t)lignes * (size_t)k + 1); // taille exacte chiffr�e
    if(phrase==NULL){
        return NULL;
    }

    char tab[20][20];

    // init
    for(int i=0;i<lignes;i++){
        for(int j=0;j<k;j++){
            tab[i][j]=' ';
        }
    }

    // remplir ligne par ligne + padding 'x'
    int index=0;
    for(int i=0;i<lignes;i++){
        for(int j=0;j<k;j++){
            if(index < len){
                tab[i][j]=clair[index];
                index++;
            }else{
                tab[i][j]='x';
            }
        }
    }

    // affichage matrice
    printf("\n===== MATRICE (clair) =====\n");
    for(int i=0;i<lignes;i++){
        for(int j=0;j<k;j++){
            printf("%c ",tab[i][j]);
        }
        printf("\n");
    }

    // lecture colonne par colonne
    index=0;
    printf("\ntext chiffre : ");
    for(int j=0;j<k;j++){
        for(int i=0;i<lignes;i++){
            phrase[index]=tab[i][j];
            index++;
        }
    }

    phrase[index]='\0';
    printf("%s\n", phrase);
    return phrase;
}

/* ========= D�chiffrement ========= */
char* dechiffrer_transposition(const char *chiffre, int k, int afficher_matrice){
    if(chiffre == NULL) return NULL;
    if(k <= 0) return NULL;

    int len = (int)strlen(chiffre);
    char *phrase = (char*)malloc((size_t)len + 1);
    if(phrase == NULL) return NULL;

    int lignes = len / k;
    if(len % k != 0) lignes++;

    // s�curit� tableau
    if(k > 100 || lignes > 100){
        printf("[ERREUR] k ou lignes trop grand pour tab[100][100]\n");
        free(phrase);
        return NULL;
    }

    char tab[100][100];
    int pos = 0;

    // remplir colonne par colonne
    for(int j = 0; j < k; j++){
        for(int i = 0; i < lignes; i++){
            if(pos < len){
                tab[i][j] = chiffre[pos];
                pos++;
            } else {
                tab[i][j] = ' ';
            }
        }
    }

    // afficher matrice seulement si demand�
    if(afficher_matrice){
        printf("\n===== MATRICE (remplie) =====\n");
        for(int i = 0; i < lignes; i++){
            for(int j = 0; j < k; j++){
                printf("%c ", tab[i][j]);
            }
            printf("\n");
        }
    }

    // reconstruire phrase ligne par ligne
    int idx = 0;
    for(int i = 0; i < lignes; i++){
        for(int j = 0; j < k; j++){
            if(idx < len){
                phrase[idx] = tab[i][j];
                idx++;
            }
        }
    }

    phrase[idx] = '\0';
    return phrase;
}

/* ========= Bruteforce ========= */
void bruteforce_transposition(const char *chiffre) {
    if(chiffre == NULL) return;

    int len = (int)strlen(chiffre);

    printf("\n========== BRUTEFORCE ==========\n");
    printf("Texte: %s\n\n", chiffre);

    // 1) Afficher seulement k + resultat (SANS matrice)
    for(int k = 2; k <= len; k++){
        char *resultat = dechiffrer_transposition(chiffre, k, 0); // 0 = pas de matrice
        if(resultat == NULL) continue;

        printf("k = %-2d -> %s\n", k, resultat);

        free(resultat);
    }

    // 2) Demander la cl� correcte
    int k_choisi;
    printf("\nQuelle est la cle correcte ? (k) : ");
    if(scanf("%d", &k_choisi) != 1){
        printf("Entree invalide.\n");
        vider_stdin();
        return;
    }
    vider_stdin();

    // 3) Afficher seulement le r�sultat pour cette cl� (sans matrice)
    char *final = dechiffrer_transposition(chiffre, k_choisi, 0);
    if(final){
        printf("\n===== RESULTAT FINAL =====\n");
        printf("k = %d -> %s\n", k_choisi, final);
        free(final);
    } else {
        printf("Impossible de dechiffrer avec k=%d\n", k_choisi);
    }

    printf("===============================\n");
}

/* ========= Main + Menu ========= */
void run_transposition(void) {
    printf("\n=== CHIFFREMENT TRANSPOSITION ===\n");
    int choix;
    int k;
    char texte[512];
    char buf[64];

    do{
        printf("\n=========== MENU ===========\n");
        printf("1) Chiffrer (transposition)\n");
        printf("2) Dechiffrer (transposition)\n");
        printf("3) Bruteforce (tester k)\n");
        printf("0) Quitter\n");
        printf("============================\n");
        printf("Votre choix: ");

        lire_ligne(buf, sizeof(buf));
        choix = (int)strtol(buf, NULL, 10);

        switch(choix){
            case 1:
                printf("Donner texte clair: ");
                lire_ligne(texte, sizeof(texte));

                printf("Donner k: ");
                lire_ligne(buf, sizeof(buf));
                k = (int)strtol(buf, NULL, 10);

                {
                    char *chiffre = chiffrer_transposition(texte, k);
                    if(chiffre){
                        printf("\n[OK] Chiffre: %s\n", chiffre);
                        free(chiffre);
                    }else{
                        printf("[ERREUR] chiffrement impossible.\n");
                    }
                }
                break;

            case 2:
                printf("Donner texte chiffre: ");
                lire_ligne(texte, sizeof(texte));

                printf("Donner k: ");
                lire_ligne(buf, sizeof(buf));
                k = (int)strtol(buf, NULL, 10);

                {
                    char *clair = dechiffrer_transposition(texte, k, 1); // 1 => afficher matrice
                    if(clair){
                        printf("\n[OK] Clair: %s\n", clair);
                        free(clair);
                    }else{
                        printf("[ERREUR] dechiffrement impossible.\n");
                    }
                }
                break;

            case 3:
                printf("Donner texte chiffre: ");
                lire_ligne(texte, sizeof(texte));
                bruteforce_transposition(texte);
                break;

            case 0:
                printf("Au revoir.\n");
                break;

            default:
                printf("Choix invalide.\n");
                break;
        }

    }while(choix != 0);

    return;
}
