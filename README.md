# 🔐 MultiCrypto-Tool

MultiCrypto-Tool est un projet académique développé en **C** regroupant plusieurs algorithmes de cryptographie (classique et moderne) accessibles via un **menu interactif en ligne de commande**. Un seul `main` existe, situé dans `app.c`.

## 📚 Fonctionnalités
- 🔁 Chiffrement de **César**
- 🔁 Chiffrement par **Transposition**
- 🔐 **RSA** (cryptographie asymétrique)
- 🔑 Génération de clé **DES**
- 🔍 Analyse de l’**effet avalanche**
- 🧮 **AES – MixColumns**
- 🔤 Chiffrement et attaque **Playfair**

Chaque algorithme est implémenté dans un fichier `.c` séparé et appelé depuis un menu central.

## 🗂️ Structure du projet
```text
MultiCrypto-Tool/
│
├── app.c                   # Point d’entrée (SEUL main)
├── cesar.c
├── transposition.c
├── RSA.c
├── generationcleDES.c
├── analyse_avalanche.c
├── AESmixcolumns.c
├── generateur_playfair.c
├── crack_playfair.c
├── crypto_app.h            # Prototypes des fonctions
├── CMakeLists.txt          # (optionnel)
└── README.md
```
⚠️ Règle fondamentale : il doit y avoir un seul `main()`, situé dans `app.c`.

---

## 🖥️ Installation (Linux)
1) Mettre à jour et installer les outils :
```bash
sudo apt update && sudo apt upgrade -y
sudo apt install -y gcc make build-essential git
```

2) Cloner le projet :
```bash
git clone https://github.com/HoussamElBouamraoui/MultiCrypto-Tool.git
cd MultiCrypto-Tool
```

3) Compilation rapide avec gcc :
```bash
gcc app.c \
    cesar.c \
    transposition.c \
    RSA.c \
    generateur_playfair.c \
    crack_playfair.c \
    generationcleDES.c \
    analyse_avalanche.c \
    AESmixcolumns.c \
    -o app
```

4) Exécution :
```bash
./app
```
Un menu interactif s’affiche pour choisir l’algorithme.

### Erreurs courantes
- `multiple definition of main` : il y a plus d’un `main()`. Garder uniquement celui de `app.c`.
- `undefined reference to run_xxx` : un fichier `.c` manque dans la commande `gcc`.
- `./app: No such file or directory` : la compilation a échoué. Corriger les erreurs puis recompiler.

### Vérification rapide
```bash
grep -R "int main" *.c
```
Attendu : `app.c:int main(...)` seulement.

---

## 🖥️ Installation (Windows, MinGW via build.ps1)
Prérequis : PowerShell + gcc (MinGW/MinGW-w64) dans le `PATH`.

1) Lancer le script de build :
```powershell
./build.ps1          # Debug
./build.ps1 -Release # Optimisé
```

2) Exécuter :
```powershell
./app.exe
```

Modules dans le menu :
- 1: César
- 2: Transposition
- 3: Playfair Générateur
- 4: Playfair Crack
- 5: AES MixColumns
- 6: Analyse avalanche (DES)
- 7: RSA utilitaires
- 8: Generation sous-cles DES
- 0: Quitter

---

## ⚙️ Compilation avec CMake (optionnel)
```bash
mkdir -p build
cd build
cmake ..
make
./app
```

---

## 🎓 Contexte
Projet pédagogique (2024–2025) visant à pratiquer la structuration d’un projet C multi-fichiers et la compilation sur Linux/Windows.
