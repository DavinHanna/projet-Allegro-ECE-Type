cmake_minimum_required(VERSION 3.18)
project(ProjetAllegro C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-L/usr/local/lib -lalleg -lm")

add_executable(ProjetAllegro main.c
        Animation.c
        Graphique.c
        Vaisseau.c
        Animation.h
        Graphique.h
        Vaisseau.h
        definition.h
        Jeu.c
        Jeu.h
)
