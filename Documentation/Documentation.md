# Documentation IGNMap v3

* <a href="https://github.com/IGNF/IGNMap/blob/master/Documentation/Documentation.md#installation-du-logiciel">Installation du logiciel</a>
* <a href="https://github.com/IGNF/IGNMap/blob/master/Documentation/Documentation.md#charger-des-donn%C3%A9es">Charger des données</a>
* <a href="https://github.com/IGNF/IGNMap/blob/master/Documentation/Documentation.md#visualiser-les-donn%C3%A9es">Visualiser les données</a>


## Installation du logiciel
IGNMap est fourni sur Windows, MacOS ou Linux sous la forme d’un fichier unique. Il n’y a aucune dépendance à installer en plus.
IGNMap peut s’exécuter depuis une clef USB.
Sous Linux, il faut éventuellement rendre le fichier exécutable (`chmod a+x`).

## Interface générale
<div align="center">
<img alt="Interface" src="https://raw.githubusercontent.com/IGNF/IGNMap/master/Documentation/Images/Interface.png">
</div><br>

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
* pour les données vectorielles : Shapefile, GeoPackage, GEOJSON, MIF/MID, DXF
* pour les données images : TIFF, COG, JPEG2000, WEBP
* pour les modèles numériques de terrain : TIFF 32 bits, ASC
* pour les nuages de points : LAS, LAZ, COPC

De plus, IGNMap peut importer des flux WMTS ou TMS. Certains flux sont déjà préconfigurés, comme les flux OSM ou certains flux de la Géoplateforme.

On peut aussi charger directement des fichiers de données en faisant un drag & drop du fichier vers la fenêtre d’IGNMap.

## Visualiser les données
Dans la vue principale, on peut être dans 3 modes :
* le mode déplacement
* le mode sélection
* le mode zoom
Quel que soit le mode, la molette de la souris permet de zoomer / dézoomer.

### Le mode déplacement
C’est le mode par défaut, accessible via le bouton <img alt="Interface" src="https://raw.githubusercontent.com/IGNF/IGNMap/master/Documentation/Images/Move.png"> de la barre d’outils.
En faisant un click & drag dans la vue principale, on peut se déplacer.

Dans le mode déplacement, on peut accéder directement aux deux autres modes en utilisant des touches du clavier :
* en maintenant la touche `Shift`, on passe en mode sélection (voir ci-dessous)
* en maintenant la touche `Ctrl`, on passe en mode zoom (voir ci-dessous)

### Le mode sélection
Ce mode est accessible via le bouton <img alt="Interface" src="https://raw.githubusercontent.com/IGNF/IGNMap/master/Documentation/Images/Select.png"> de la barre d’outils.
Un clic souris bouton gauche permet de sélectionner le ou les données se trouvant sous la souris.
En faisant un click & drag, on peut sélectionner toutes les données se trouvant dans une zone rectangulaire.
La sélection d’un objet permet de visualiser ses points de construction dans la vue principale et d’afficher ses attributs dans le panneau **Sélection** :
<div align=center>
<img alt="Interface" src="https://raw.githubusercontent.com/IGNF/IGNMap/master/Documentation/Images/Selection.png">
</div><br>

### Le mode zoom
Ce mode est accessible via le bouton <img alt="Interface" src="https://raw.githubusercontent.com/IGNF/IGNMap/master/Documentation/Images/Zoom.png"> de la barre d’outils.
Un clic souris bouton gauche permet de faire un zoom avant.
Un clic souris bouton droit permet de faire un zoom arrière.
En faisant un click & drag, on peut zoomer sur une zone rectangulaire donnée.

## Les panneaux d’information
Ces panneaux permettent de voir quelles sont les couches de données chargées dans IGNMap et d’agir sur ces couches. Il y a 4 panneaux, chacun permettant de travailler sur un type de données :
* le panneau Couches vectorielles : pour les données vectorielles (Shapefile, MIF/MID, …)
* le panneau Couches images : pour les données images (TIFF, COG, JPEG2000, …)
* le panneau Couches MNT : pour les modèles numériques de terrain
* le panneau Couches LAS : pour les nuages de points
Chaque panneau présente la liste des couches, dans leur ordre d’affichage (la première couche de la liste est la couche qui sera affichée en premier, donc susceptible d’être recouverte par les autres). L’ordre des couches peut-être modifié en sélectionnant les couches que l’on veut déplacer et en faisant un glisser / déposer (*drag & drop*).
L’icône <img alt="View" src="https://raw.githubusercontent.com/IGNF/IGNMap/master/Documentation/Images/View.png"> permet de rendre visible / invisible une couche.
L’icône <img alt="Select" src="https://raw.githubusercontent.com/IGNF/IGNMap/master/Documentation/Images/Selectable.png"> permet de rendre sélectionnable / non-sélectionnable une couche.
Si on sélectionne plusieurs couches dans la liste, alors on peut rendre visible / invisible plusieurs couches à la fois en un seul clic souris.
A côté de chaque couche, l’icône <img alt="Select" src="https://raw.githubusercontent.com/IGNF/IGNMap/master/Documentation/Images/Options.png"> permet d’accéder à différentes **actions** :
* Centre de la couche : permet de recentre la vue principale au centre de la couche,
* Emprise de la couche : permet de centrer et de mettre à l’échelle la vue principale pour afficher l’intégralité de la couche,
* Visualiser les objets : permet d’affiche la liste de tous les objets de la couche,
* Supprimer : permet de retirer la couche d’IGNMap.

En fonction du type de la couche, on retrouvera d’autres actions :

### Actions spécifiques pour les couches vectorielles
#### Export de la classe

### Actions spécifiques pour les couches vectorielles
#### Résolution de la couche

### Actions spécifiques pour les couches MNT
#### Résolution de la couche

### Actions spécifiques pour les couches LAS
#### Calcul de MNT

#### Statistiques

