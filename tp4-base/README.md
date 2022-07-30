# TP3

## Commandes utiles :

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

## Utilisation du programme «serveur»

-   Build avec CMake
-   Vérifier que le dossier à partager est existant, voir racine du projet
-   se rendre dans le dossier build ou cmake a contruit le projet
-   executer ./server <port>




## Utilisation du programme «client»

-   S'assurer aue le serveur est en marche
-   executer ./client <id> <portServeur> <nom du fichier à transférer>