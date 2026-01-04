cmake_minimum_required(VERSION 3.10)
project(MultiCryptoTool C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(app
    app.c
    cesar.c
    transposition.c
    generateur_playfair.c
    crack_playfair.c
    AESmixcolumns.c
    analyse_avalanche.c
    RSA.c
    generationcleDES.c
)
