# Santorini
Projet de l'année, le JOAT (jeu of all time)

## Dépendances
CMake <br>
Doxygen <br>
Google Test <br>

## Organisation
Les fichiers executables dans le dossier build/ <br>
Les headers dans include/<br>
Les fichiers sources dans src/<br>
Les tests dans tests/<br>
Les bibliothèques externes dans lib/<br>
La documentation dans doc/<br>
Les suivis de progression de chacun dans README/<br>
1 classe par fichier<br>

## Conventions de nomage
Les classes sont en PascalCase<br>
Les fonctions et variables sont en camelCase<br>
Les fichiers sont camelCase et reprenne le nom de la classe qu'ils définissent<br>
Les constantes sont en UPPER_SNAKE_CASE<br>
Les attribut privés sont finissent par _<br>
Les espaces de noms en miniscules et en 1 seul mot<br>

## Best-practices
Utiliser des includes guards dans les headers<br>
Laisser les bibliothèque gérer la mémoire au maximum<br>
Utiliser les exeptions que dans des cas exeptionnel<br>
Mettre les attributs et méthodes publiques avant celles privés<br>
