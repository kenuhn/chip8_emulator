Présentation de Mon Projet d'Émulateur Chip8 en C

Je suis ravi de vous présenter mon projet : un émulateur Chip8 développé en C, visant à recréer une des plus anciennes machines virtuelles. Ce projet a pour but de permettre l'exécution de jeux classiques comme "Pong" et "Space Invaders" sur des systèmes modernes.

En optant pour le C, j'assure une exécution rapide et une grande portabilité, rendant mon émulateur accessible sur différentes plateformes. Le projet comprend également un débogueur intégré pour faciliter le développement et la compréhension des jeux.

Démarrage de l'Émulateur Chip8
Prérequis

Avant de compiler et d'exécuter l'émulateur Chip8, assurez-vous que les outils suivants sont installés sur votre système :

    GCC (GNU Compiler Collection)
    SDL2 (Simple DirectMedia Layer 2)

Vous pouvez installer SDL2 sur la plupart des systèmes d'exploitation via le gestionnaire de paquets. Par exemple, sous Ubuntu, vous pouvez utiliser :

bash

sudo apt-get install libsdl2-dev

Compilation

Le projet utilise un Makefile pour simplifier le processus de compilation. Pour compiler l'émulateur, naviguez dans le répertoire racine du projet où se trouve le Makefile et exécutez la commande suivante :

bash

make all

Cette commande va compiler tous les fichiers sources nécessaires et lier les objets avec les bibliothèques SDL2 pour créer l'exécutable chip8.
Exécution

Une fois la compilation réussie, vous pouvez lancer l'émulateur en exécutant :

bash

./chip8

Assurez-vous de placer les jeux Chip8 dans un répertoire accessible ou de spécifier le chemin correct vers le fichier de jeu lors du lancement de l'émulateur.
Nettoyage

Si vous souhaitez nettoyer votre projet (par exemple, pour recompiler entièrement l'application), vous pouvez utiliser la commande suivante :

bash

make clean

Cette commande supprimera tous les fichiers objets et l'exécutable généré, vous permettant de repartir de zéro.
