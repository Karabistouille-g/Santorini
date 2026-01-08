# Santorini
Projet de l'année, le JOAT (jeu of all time)

## Dépendances
CMake
Doxygen
Google Test

## Organisation
Les fichiers executables dans le dossier build/
Les headers dans include/
Les fichiers sources dans src/
Les tests dans tests/
Les bibliothèques externes dans lib/
La documentation dans doc/
1 classe par fichier

## Conventions de nomage
Les classes sont en PascalCase
Les fonctions et variables sont en camelCase
Les fichiers sont camelCase et reprenne le nom de la classe qu'ils définissent
Les constantes sont en UPPER_SNAKE_CASE
Les attribut privés sont finissent par _
Les espaces de noms en miniscules et en 1 seul mot

## Best-practices
Utiliser des includes guards dans les headers
Laisser les bibliothèque gérer la mémoire au maximum
Utiliser les exeptions que dans des cas exeptionnel
