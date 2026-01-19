# Suivi de Progression - [CUINET Louis]
**Rôle :** Contrôleur & Réseau
**Projet :** Santorini (C++)

## 📅 Journal de bord

### [01/12/2026 - Architecture et Couche Réseau
**Objectifs :**
- Mettre en place l'architecture du Contrôleur.
- Créer la communication réseau (Client/Serveur) pour le 1v1.

**Réalisations :**
1. **Refonte UML :**
   - Extraction de la logique réseau hors du Contrôleur (création de la classe `NetworkManager`) pour respecter le principe de responsabilité unique (SRP).
   - Simplification de la gestion des Pions (`Slave`) : suppression des coordonnées redondantes, ajout d'un `owner_id`.
2. **Implémentation Réseau (`NetworkManager`) :**
   - Utilisation des Sockets TCP (Linux/Standard).
   - Gestion des modes "Host" (Serveur) et "Join" (Client).
   - Création d'un protocole commun (`common.hpp`) avec une structure `Packet` sérialisée pour l'envoi des ordres (Move/Build).
3. **Implémentation Contrôleur (`Controller`) :**
   - Squelette de la classe et gestion de la mémoire avec `std::unique_ptr` (RAII).
   - Logique d'initialisation (`createGame`) qui s'adapte au mode Local ou Réseau.

**Difficultés / Solutions :**
- *Problème :* Dépendances circulaires entre `Controller` et `NetworkManager` pour l'échange de données.
- *Solution :* Création du fichier `common.hpp` indépendant pour définir les structures de données (`Packet`, `ActionType`).
- *Problème :* Impossible de compiler le `Main` car le `Model` des collègues n'est pas encore prêt.
- *Solution :* Création d'un "Mock" (faux modèle temporaire) pour valider la compilation et tester la connexion réseau isolément.

**Prochaines étapes :**
- Implémenter la logique métier `selectMove` et `selectBuild`.
- Gérer la réception des paquets dans la boucle de jeu.

---