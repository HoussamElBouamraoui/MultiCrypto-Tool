#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#define LLFMT "%I64d"
#include <stdio.h>
#include <stdlib.h>
#include "crypto_app.h"

long long euclide_etendu(long long a,long long b,long long *u ,long long *v){
    long long r1 = a, r2=b ,u1 = 1, u2 = 0, v1 = 0 , v2 = 1 ;


    while (r2 != 0){
        long long q = r1 / r2 ;
        long long r = r1 % r2 ;

        long long utmp = u1 - q * u2 ;
        long long vtmp = v1 - q * v2 ;

        r1 = r2 ;
        r2 = r ;
        u1 = u2 ;
        u2 = utmp;

        v1 = v2 ;
        v2 = vtmp;
    }

    *u = u1;
    *v = v1 ;

    return r1;

}

long long exp_modulaire(long long base, long long exp, long long mod){
    long long r = 1 ;

    base = base % mod ;
    while(exp > 0){
        if(exp % 2 == 1){
            r = (r * base) % mod ;
        }

        base = ( base * base ) % mod ;
        exp = exp / 2;
    }

    return r ;
}
int factoriser_n(long long n, long long *p, long long *q){
    for (long long i = 2 ;i * i <= n;i++){
        if( n % i == 0 ){
        *p = i ;
        *q = n /i ;
        return 1 ;
        }
    }
    return 0 ;
}

long long calcule_trape(long long p,long long q){
    return (p - 1) * (q - 1) ;
}

long long extr_cle(long long e,long long phi){
    long long u,v;
    long long pg = euclide_etendu(e,phi,&u,&v);

    if(pg != 1){
        printf("inverse modulaire n existe pas\n");
        return -1;
    }

    long long d ;
    d = u % phi ;
    if (d < 0){
        d = d + phi;
    }

    return d ;


}

long long dechiff_rsa(long long C, long long d, long long n){
    return exp_modulaire(C,d,n);
}

void run_rsa(void) {
    int choix;

    while (1) {
        printf("\n================= MENU RSA (C) =================\n");
        printf("1) Dechiffrer RSA (C, e, n) -> factoriser n, calculer phi, trouver d, puis M\n");
        printf("2) Factoriser n (trouver p et q)\n");
        printf("3) Calculer phi(n) a partir de p et q\n");
        printf("4) Trouver d = inverse modulaire de e modulo phi(n)\n");
        printf("5) Calculer exp_modulaire(base, exp, mod)\n");
        printf("0) Quitter\n");
        printf("Votre choix: ");

        if (scanf("%d", &choix) != 1) {
            printf("Erreur de saisie.\n");
            return;
        }

        switch (choix) {
            case 1: {
                long long C, e, n;
                printf("\nEntrer C (ciphertext): ");
                scanf(LLFMT, &C);
                printf("Entrer e (cle publique): ");
                scanf(LLFMT, &e);
                printf("Entrer n (module): ");
                scanf(LLFMT, &n);

                printf("\nLes donnees : C = " LLFMT " , e = " LLFMT " , n = " LLFMT "\n", C, e, n);

                long long p, q;
                if (!factoriser_n(n, &p, &q)) {
                    printf("Factorisation impossible pour n = " LLFMT "\n", n);
                    break;
                }
                printf("Factorisation: n = " LLFMT " = " LLFMT " * " LLFMT "\n", n, p, q);

                long long phi = calcule_trape(p, q);
                printf("phi(n) = " LLFMT "\n", phi);

                long long d = extr_cle(e, phi);
                if (d == -1) { printf("Impossible de trouver d.\n"); break; }
                printf("d = " LLFMT "\n", d);

                long long M = dechiff_rsa(C, d, n);
                printf("Message clair M = " LLFMT "\n", M);
                break;
            }

            case 2: {
                long long n, p, q;
                printf("Entrer n: ");
                scanf(LLFMT, &n);
                if (factoriser_n(n, &p, &q)) {
                    printf("n = " LLFMT " = " LLFMT " * " LLFMT "\n", n, p, q);
                } else {
                    printf("Factorisation impossible pour n = " LLFMT "\n", n);
                }
                break;
            }

            case 3: {
                long long p, q;
                printf("Entrer p: "); scanf(LLFMT, &p);
                printf("Entrer q: "); scanf(LLFMT, &q);
                printf("phi(n) = " LLFMT "\n", calcule_trape(p, q));
                break;
            }

            case 4: {
                long long e, phi;
                printf("Entrer e: "); scanf(LLFMT, &e);
                printf("Entrer phi(n): "); scanf(LLFMT, &phi);
                long long d = extr_cle(e, phi);
                if (d == -1) printf("Pas d'inverse modulaire.\n");
                else printf("d = " LLFMT "\n", d);
                break;
            }

            case 5: {
                long long base, exp, mod;
                printf("Entrer base: "); scanf(LLFMT, &base);
                printf("Entrer exp: "); scanf(LLFMT, &exp);
                printf("Entrer mod: "); scanf(LLFMT, &mod);
                printf("Resultat = " LLFMT "\n", exp_modulaire(base, exp, mod));
                break;
            }

            case 0:
                printf("Au revoir, à la prochaine.\n");
                return;

            default:
                printf("Choix invalide.\n");
        }
    }
}
