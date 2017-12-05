# Pendu
Simple client/server pendu program written in c

## Résumé

Je vous présente mon petit programme de modèle client-serveur avec le jeu du pendu.
Il est fait avec les tubes nommés en langage C, dans lequel j'ai implémenté un serveur
qui peut gérer un nombre infini de client, chaque client est identifié par un pseudo, le
nom de son tube et un numéro de client gérée par le serveur.
Comment se passe la communication entre le serveur et le(s) clients ?

![screenshot2](https://user-images.githubusercontent.com/26382145/33632581-720c6b6e-d9e4-11e7-8d4c-48488bd4432a.jpg)

Il est possible que plus d’un joueur communique avec le serveur en même temps. Tant
que l’atomicité des écritures des joueurs est garantie, ils peuvent tous utiliser le même
tube nommé pour envoyer des données au serveur.
Par contre, le serveur ne peut pas utiliser un seul tube pour communiquer avec les
joueurs. Si plus d’un joueur lit le même tube, il n’est pas possible de garantir que le bon
joueur reçoive les données envoyées par le serveur.
Chaque joueur possède son propre tube en lecture et écriture avant d’envoyer des
données au serveur (Figure. 1). Utilisation du pid du processus du joueur est une bonne
façon de créer des tubes ayant un nom unique et facilement identifiable. Le client
transmet donc par le tube serveur son pid qui permettra au serveur de savoir dans quel
tube lui retourner les informations ainsi que les informations reçu du client pour le jeu
du pendu.
Pour le serveur j'ai deux processus qui tourne, le principale qui s'occupe de lire sur le
tube serveur et l'autre ( le fils ) qui s'occupe de le jeu du pendu (les tests) qui écrit et lit
sur le tube du client.

## Licence
Le code peuvent être utilisés et copiés librement à des fins non commerciales à condition que cet avis soit inclus. Ces fichiers sont fournis "tels quels" sans aucune garantie. Le téléchargement des fichiers signifie l'acceptation de ces conditions.
