# MultiCrypto Tool – Application console

Application C tout-en-un regroupant plusieurs démos d’algorithmes classiques : César, transposition, Playfair (génération et attaque), AES MixColumns, analyse d’avalanche (DES) et petits utilitaires RSA. L’exécutable principal est `app.exe`.

## Prérequis
- Windows avec PowerShell
- gcc (MinGW/MinGW-w64) disponible dans le `PATH` ou chemin fourni via `-GccPath`

## Construction
Depuis le dossier du projet :

```powershell
# Build debug (par défaut)
./build.ps1

# Build release (optimisé)
./build.ps1 -Release

# Spécifier un gcc particulier
./build.ps1 -GccPath "C:\chemin\vers\gcc.exe"
```

Le script compile toutes les sources listées (`app.c`, `cesar.c`, `transposition.c`, `generateur_playfair.c`, `crack_playfair.c`, `AESmixcolumns.c`, `analyse_avalanche.c`, `RSA.c`) et produit `app.exe`.

## Exécution
```powershell
./app.exe
```
Un menu s’affiche. Saisir le numéro voulu puis valider. Les modules disponibles :
- 1 : Chiffre de César
- 2 : Transposition
- 3 : Génération de clé Playfair
- 4 : Cassage Playfair
- 5 : AES MixColumns (démonstration)
- 6 : Analyse d’avalanche (DES)
- 7 : Utilitaires RSA (petits calculs)
- 0 : Quitter

## Notes
- Le script ne génère qu’un seul exécutable (`app.exe`) intégrant tous les modules. Les `.exe` individuels ne sont pas nécessaires.
- En cas d’absence de gcc, installez MinGW/MinGW-w64 ou passez `-GccPath` avec le chemin complet de `gcc.exe`.
