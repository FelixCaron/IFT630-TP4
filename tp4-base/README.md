# TP3

# Commandes utiles :

## Information pour compiler le projet :
- C++ 17 et plus pour utiliser le filesystem
- Pour compiler, naviguer dans le répertoire build du projet, éxécuter ces commandes :
```bash
cmake ..
```
Ensuite :
```bash
make
```

Afficher des informations sur les installations IPC (canaux ouvert, segments mémoire partagés, etc) :

```bash
ipcs
```

Afficher des informations spécifique d'une ressource partagée :

```bash
ipcs -i <id de la ressource> -a -p
```

Afficher des informations spécifique des canaux d'une ressource partagée :

```bash
ipcs -i <id de la ressource> -q -p
```

# Utilisation du programme «serveur»

-   Vérifier que le dossier à partager est existant, voir racine du projet. Le nom devrait être « transfer_folder »
-   Naviguer jusqu'au dossier build où cmake a contruit le projet
-   éxécuter ./server <port>

# Utilisation du programme «client»

-   S'assurer que le serveur est en marche
-   éxécuter ./client <id> <portServeur> <nom du fichier à transférer>
