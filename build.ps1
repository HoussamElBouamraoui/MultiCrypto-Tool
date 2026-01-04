# build.ps1 - Compile le projet TP1 avec gcc (MinGW) sous Windows
# Usage:
#   .\build.ps1           -> build debug (avec -O0 -g)
#   .\build.ps1 -Release  -> build release (avec -O2)
#   .\build.ps1 -GccPath "C:\\path\\to\\gcc.exe"  -> préciser le chemin si gcc n'est pas dans le PATH

param(
    [switch]$Release,
    [string]$GccPath = "gcc"
)

# Se placer dans le répertoire du script (racine du projet)
if ($PSScriptRoot) { Set-Location $PSScriptRoot }

# Vérifier la présence de gcc
& $GccPath --version > $null 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Error "gcc introuvable. Installez MinGW/MinGW-w64 ou ajoutez gcc au PATH, ou précisez -GccPath."
    exit 1
}

# Flags de compilation
$args = @("-std=c11","-Wall","-Wextra")
if ($Release) { $args += "-O2" } else { $args += "-O0"; $args += "-g" }

# Liste des sources attendues (modifiez si nécessaire)
$sources = @("app.c","cesar.c","transposition.c","generateur_playfair.c","crack_playfair.c","AESmixcolumns.c","analyse_avalanche.c","RSA.c","generationcleDES.c")
$existing = $sources | Where-Object { Test-Path $_ }
if ($existing.Count -eq 0) {
    Write-Error "Aucun fichier source trouvé dans $PWD"
    exit 1
}

# Nom de l'exécutable principal
$out = "app.exe"

# Construire les arguments pour gcc
$gccArgs = @()
$gccArgs += $args
$gccArgs += $existing
$gccArgs += "-o"
$gccArgs += $out

Write-Output "Compiling: $GccPath $($gccArgs -join ' ')"
& $GccPath @gccArgs
if ($LASTEXITCODE -ne 0) {
    Write-Error "Compilation échouée (code: $LASTEXITCODE)."
    exit $LASTEXITCODE
}

Write-Output "Compilation OK -> $out"

Write-Output "\nPour exécuter : .\$out"
exit 0
