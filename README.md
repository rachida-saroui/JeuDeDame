# JeuDeDame
Partie de dames Humain contre IA sur STM32F746G avec la bibliothèque StemWin. Ce référentiel héberge une implémentation du jeu classique de dames, où les joueurs peuvent défier un adversaire IA sur la plateforme microcontrôleur STM32F746G

# StemWin
-Téléchargez la bibliothèque StemWin : Rendez-vous sur le site officiel de Segger et téléchargez la bibliothèque StemWin pour votre microcontrôleur STM32F746G.

-Intégrez la bibliothèque StemWin dans votre projet : Copiez les fichiers de la bibliothèque StemWin dans le répertoire de votre projet STM32F746G.

-Configurez votre projet : Ajoutez les fichiers de la bibliothèque StemWin à votre projet et assurez-vous de les inclure dans la compilation.

-Initialisez l'interface graphique : Utilisez les fonctions fournies par la bibliothèque StemWin pour initialiser l'interface graphique sur votre microcontrôleur STM32F746G.

-Créez les fenêtres et les contrôles : Utilisez les fonctions de la bibliothèque StemWin pour créer des fenêtres, des boutons et d'autres contrôles graphiques nécessaires à votre application.

-Gérez les événements : Implémentez des fonctions de rappel pour gérer les événements tels que les clics ou les appuis de bouton.

-Affichez et mettez à jour l'interface utilisateur : Utilisez les fonctions fournies par la bibliothèque StemWin pour afficher et mettre à jour l'interface utilisateur en fonction des interactions de l'utilisateur.

# Les fonction définies dans main 

Voici une brève explication des différentes fonctions présentes dans le code :

-GUI_Init() : Cette fonction initialise la bibliothèque graphique GUI.

-GUI_DispStringAt("Starting...", 0, 0) : Cette fonction affiche le texte "Starting..." à la position (0, 0) de l'écran.

-WM_SetCreateFlags(WM_CF_MEMDEV) : Cette fonction active l'utilisation de la fonctionnalité de mémoire de périphérique.

-init_board(&board) : Cette fonction initialise le plateau de jeu en plaçant les pions aux positions de départ.

-draw_board(&board) : Cette fonction dessine le plateau de jeu en utilisant la bibliothèque graphique GUI.
-is_valid_move(&board, departureX, departureY, arriveeX, arriveeY) : Cette fonction vérifie si le mouvement de la case de départ à la case d'arrivée est un mouvement valide dans le jeu.

-update_score(&board, departureX, departureY, arriveeX, arriveeY) : Cette fonction met à jour le score du jeu en fonction des pions capturés lors d'un mouvement.

-decide(&board) : Cette fonction permet à l'ordinateur de décider du prochain mouvement à jouer en analysant l'état actuel du plateau de jeu.

-check_win(blackPieces, whitePieces) : Cette fonction vérifie si l'une des deux couleurs de pions a gagné le jeu.

Ces fonctions sont utilisées dans une boucle while(1) pour exécuter le jeu jusqu'à ce qu'une condition de fin soit atteinte.
