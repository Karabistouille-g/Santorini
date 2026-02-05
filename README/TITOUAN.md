Création d'un premier diagramme UML pour définir les fonctionnalités des classes

Mise en place de la première version des méthodes de builder, case, board et model

Générer les tests avec IA, mais vérification de la conformité des tests

Commencement de l'IA

Chercher un moyen de virtualiser le plateau pour "jouer" les coups
-> Copier le plateau à chaque coup de l'IA : demande bcp de ressource + plateau en singleton
-> Créer un plateau virtuel : c'est un bourbier
-> Créer un historique de coup et undo à chaque tentive
-> Implémentation simple