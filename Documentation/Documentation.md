# Documentation IGNMap v3

## Installation du logiciel
IGNMap est fourni sur Windows, MacOS ou Linux sous la forme d’un fichier unique. Il n’y a aucune dépendance à installer en plus.
IGNMap peut s’exécuter depuis une clef USB.

## Interface générale

<https://raw.githubusercontent.com/IGNF/IGNMap/master/Documentation/Images/Interface.png>

L’interface d’IGNMap est composée :
* d’une barre de menus
* d’une barre d’outils
* d’une vue principale qui sert à visualiser les données
* d’un panneau d’information qui contient lui-même des sous-panneaux. Ces sous-panneaux servent à configurer la manière dont sont visualisées les données et à lancer des traitements sur les données.

L’interface d’IGNMap peut être en langue anglaise ou française : pour basculer d’une langue à l’autre, on utilise le menu **Édition – Traduire** (ou **Edit – Translate** en anglais).

## Charger des données
**IGNMap ne modifie jamais les données que vous chargez ! On peut éventuellement exporter de nouveaux fichiers à partir des données chargées mais on ne risque jamais de les modifier.**

Pour importer des données, on utilise le menu **Fichier** et les sous-menus en fonction du type de données à charger.
IGNMap peut importer soit un fichier unique, soit tous les fichiers se trouvant dans un répertoire.
Les formats reconnus par IGNMap sont :
* pour les données vectorielles : Shapefile, GeoPackage, MIF/MID, DXF
* pour les données images : TIFF, COG, JPEG2000, WEBP
* pour les modèles numériques de terrain : TIFF 32 bits, ASC
* pour les nuages de points : LAS, LAZ, COPC

De plus, IGNMap peut importer des flux WMTS ou TMS. Certains flux sont déjà préconfigurés, comme les flux OSM ou certains flux de la Géoplateforme.

## Visualiser les données
