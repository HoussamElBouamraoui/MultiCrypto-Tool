#include <stdio.h>
#include "crypto_app.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

static int normaliser_k(int k){
    k %= 26 ;
    if(k<0) k+=26;
    return k ;
}

char* chiffrer_cesar(const char *clair,int k){
    if(clair==NULL){
        return NULL;
    }

    k = normaliser_k(k) ;

    size_t n = strlen(clair);
    char *chiffrer = (char *)malloc(( n +1) * sizeof(char));
    if(chiffrer == NULL){

        return NULL;
    }
    for(size_t i = 0 ;i < n;i++){

        char c = (char)clair[i];
        if( c >='A' && c  <= 'Z'){
            char chare= c + k ;
            if(chare > 'Z'){
                chare -= 26;
            }
            chiffrer[i] = chare;
        }else if(c >='a' && c  <= 'z'){
            char chare= c + k ;
            if(chare > 'z'){
                chare -= 26;
            }
            chiffrer[i] = chare;

        }
        else{
            chiffrer[i] = c ;
        }
    }
    chiffrer[n]='\0';
    return chiffrer;
}

void calculer_frequence(const char *texte,double *frequences){

    if(texte == NULL || frequences == NULL){
        return ;
    }
    size_t taillemss = strlen(texte);
    if(taillemss == 0){
        printf("Text vide");
        return;
    }
    const char lettre[26]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
    for (int i = 0; i < 26; i++) {
        frequences[i] = 0;
    }

    for(size_t i = 0;i<26;i++){
        int freq = 0;

        for(size_t j = 0;j<taillemss;j++){
            if(lettre[i]==texte[j]){
                freq++;
            }

        }
        frequences[i] = (double)(freq * 100) / taillemss;
        if(frequences[i] != 0){
            printf("%c = %d = (%.2f%%)\n",lettre[i],freq,frequences[i]);
        }
    }
    double maxfrequence = frequences[0];
    int indice = 0;
    for(int i=0;i<26;i++){
        if(frequences[i]>maxfrequence){
            maxfrequence=frequences[i];
            indice = i;
        }

    }
    printf("la fraquence maximale est %c = (%.2f%%)\n",lettre[indice],maxfrequence);

}

int decalage(char p, char s) {
    if (p >= 'A' && p <= 'Z') p += 32;
    if (s >= 'A' && s <= 'Z') s += 32;

    return p - s;
}

char* decrypter_cesar(const char *chiffre,int k){
    if(chiffre==NULL){
        return NULL;
    }

    k = normaliser_k(k);
    int n = strlen(chiffre);
    char* res = (char*) malloc(n + 1);
    if(res == NULL){
        return NULL;
    }


    for(int i=0;i<n;i++){
        char c = chiffre[i];

        if( c >='A' && c  <= 'Z'){
            char chare = c - k;
            if(chare < 'A'){
                chare += 26;
            }

            res[i] = chare;
        }else if(c >='a' && c  <= 'z'){
            char chare = c - k;
            if(chare < 'a'){
                chare += 26;
            }

            res[i] = chare;
        }else{
            res[i] = c;
        }


    }
    res[n] = '\0';
    return res;
}

static void lire_ligne(char *buf, size_t size) {
    if (fgets(buf, (int)size, stdin)) {
        buf[strcspn(buf, "\n")] = '\0';  // enlever \n
    } else {
        buf[0] = '\0';
    }
}

static int lire_int(void) {
    char tmp[64];
    lire_ligne(tmp, sizeof(tmp));
    if (tmp[0] == '\0') {
        return INT_MIN; // entrée vide => invalide
    }
    char *endptr = NULL;
    long v = strtol(tmp, &endptr, 10);
    if (endptr == tmp) {
        return INT_MIN; // pas de nombre parsé
    }
    return (int)v;
}

void run_cesar(void) {
    int choix = -1;

    while (1) {
        printf("\n==============================\n");
        printf("        MENU - CESAR\n");
        printf("==============================\n");
        printf("1) Chiffrer un message\n");
        printf("2) Dechiffrer un message\n");
        printf("3) Calculer frequence (a..z)\n");
        printf("0) Quitter\n");
        printf("------------------------------\n");
        printf("Votre choix : ");

        choix = lire_int();
        if (choix == INT_MIN) {
            printf("Entree vide ou invalide. Reessayez.\n");
            continue;
        }

        switch (choix) {

            case 1: {
                char clair[2048];
                int k;

                printf("\n--- Chiffrement Cesar ---\n");
                printf("Entrer le message clair : ");
                lire_ligne(clair, sizeof(clair));

                printf("Entrer la cle k : ");
                k = lire_int();

                char *chiffre = chiffrer_cesar(clair, k);
                if (chiffre == NULL) {
                    printf("Erreur: chiffrement (allocation ou entree invalide)\n");
                } else {
                    printf("Message chiffre : %s\n", chiffre);
                    free(chiffre);
                }
                break;
            }

            case 2: {
                char chiffre[2048];
                int k;

                printf("\n--- Dechiffrement Cesar ---\n");
                printf("Entrer le message chiffre : ");
                lire_ligne(chiffre, sizeof(chiffre));

                printf("Entrer la cle k : ");
                k = lire_int();

                char *clair = decrypter_cesar(chiffre, k);
                if (clair == NULL) {
                    printf("Erreur: dechiffrement (allocation ou entree invalide)\n");
                } else {
                    printf("Message dechiffre : %s\n", clair);
                    free(clair);
                }
                break;
            }

            case 3: {
                char texte[4096];
                double freq[26];

                printf("\n--- Analyse frequentielle ---\n");
                printf("Entrer le texte (minuscules conseille) : ");
                lire_ligne(texte, sizeof(texte));

                calculer_frequence(texte, freq);
                break;
            }

            case 0:
                printf("Au revoir.\n");
                return;

            default:
                printf("Choix invalide. Reessayez.\n");
                break;
        }
    }
}
