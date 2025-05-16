# TP DPS 310 - Projet

## Description
Ce projet implémente un programme pour lire les données d'un capteur DPS310, calculer la température et la pression, et envoyer les données via MQTT.

## Fonctionnalités
- Lecture de la température et de la pression simulées à partir d'un registre DPS310.
- Publication des données sur un serveur MQTT (`test.mosquitto.org`).
- Support de topics dynamiques :
  - **Données** : `thprobe/[NAME]/data/temperature` et `thprobe/[NAME]/data/pressure`.
  - **Commandes** : `thprobe/[NAME]/cmd`.
- Fréquence configurable pour l'envoi des messages MQTT.

## Arborescence du projet
```
.
├── include
│   ├── dps310.h
│   ├── mqtt_client.h
│   └── fakedps310.h
├── lib
│   ├── libdps310.c
│   └── libdps310.a
├── Makefile
├── README.md
└── src
    ├── dps310.c
    ├── fakedps310.c
    ├── main.c
    └── mqtt_client.c
```

## Commandes Makefile

Le projet utilise un `Makefile` pour simplifier la compilation et l'exécution. Voici les commandes disponibles :

### Commandes principales

1. **`make` ou `make all`**  
   Compile l'exécutable principal du projet en utilisant tous les fichiers sources.  
   L'exécutable est généré dans le dossier `bin` sous le nom `thermalprobe`.

2. **`make run`**  
   Compile le projet si nécessaire et exécute l'exécutable principal `bin/thermalprobe` avec des arguments par défaut (`sensor1` comme nom de capteur et une fréquence de 5 secondes).

3. **`make clean`**  
   Supprime tous les fichiers générés, y compris l'exécutable dans le dossier `bin`.

4. **`make check-deps`**  
   Vérifie si les dépendances nécessaires (compilateur GCC et bibliothèque MQTT Paho) sont installées.

## Exemple d'utilisation

1. **Compiler le projet** :
   ```bash
   make
   ```

2. **Exécuter le programme avec des arguments personnalisés** :
   ```bash
   ./bin/thermalprobe [NAME] [FREQ]
   ```
   - **`NAME`** : Nom du capteur utilisé pour construire les topics MQTT (par exemple, `sensor1`).
   - **`FREQ`** : Fréquence d'envoi des messages MQTT en secondes (par exemple, `5`).

   Exemple :
   ```bash
   ./bin/thermalprobe sensor1 5
   ```

3. **Observer les messages publiés** :
   Utilisez la commande suivante pour vous abonner à tous les sous-topics de `thprobe` :
   ```bash
   mosquitto_sub -h test.mosquitto.org -t "thprobe/#"
   ```

4. **Nettoyer les fichiers générés** :
   ```bash
   make clean
   ```

## Dépendances
### Compilateur GCC
Assurez-vous que GCC est installé pour compiler le code C :
```bash
sudo apt-get install build-essential
```

### Bibliothèque MQTT Paho
Le projet utilise la bibliothèque MQTT Paho. Assurez-vous qu'elle est installée sur votre système avant de compiler le projet.  
Pour installer la bibliothèque sur une distribution basée sur Debian, utilisez la commande suivante :
```bash
sudo apt-get install libpaho-mqtt3c-dev
```

## Notes
- Les messages sont publiés sur des topics au format `thprobe/[NAME]/data/temperature` pour la température et `thprobe/[NAME]/data/pressure` pour la pression.
- Les commandes peuvent être envoyées sur le topic `thprobe/[NAME]/cmd`.
- Pour observer les messages de plusieurs capteurs, utilisez le caractère générique `#` dans la commande `mosquitto_sub` :
  ```bash
  mosquitto_sub -h test.mosquitto.org -t "thprobe/#"
  ```
- Le serveur MQTT utilisé est `test.mosquitto.org` sur le port 1883.

## Bibliothèques et fichiers spécifiques
- **`libdps310`** : Bibliothèque pour simuler les lectures de température et de pression du capteur DPS310.
- **`fakedps310`** : Simule le registre du capteur DPS310 pour les tests.
- **`mqtt_client`** : Gère la connexion et la publication des messages MQTT.