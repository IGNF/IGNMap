# IGNMap


## Description/Résumé du projet

IGNMap est un logiciel de visualisation de données géographiques. Il permet de visualiser des données vectorielles, des images, des modèles numériques de terrain, des nuages de points LIDAR.  
IGNMap n’est pas à proprement parler un système d’informations géographiques (SIG) mais plutôt un outil léger pour visualiser et analyser des données.  
Quelques exemples de visualisation :  
<https://raw.githubusercontent.com/IGNF/IGNMap/main/images/IGNMap01.png>

<https://raw.githubusercontent.com/IGNF/IGNMap/main/images/IGNMap02.png>

<https://raw.githubusercontent.com/IGNF/IGNMap/main/images/IGNMap03.png>

## Historique

IGNMap était à la base un outil interne de l’institut géographique national (IGN). La version 1 était développé avec la bibliothèque MFC de Microsoft pour la création de l’interface homme-machine.  

La version 2 a été distribuée à l’extérieur de l’IGN, principalement pour aider au passage au RGF 93, cette version proposant des outils pour la conversion des données des anciennes projections légales (Lambert 2 étendu pour la France métropolitaine ; Saint-Anne, Fort Marigot et Fort Desaix pour les Antilles, CSG1967 pour la Guyane ; Piton des Neiges pour la Réunion ; …) vers les nouvelles projections (Lambert 93, RGAF09, RGRG95, RGR92, …). Cette version 2 était développée avec QT4 pour la création de l’interface homme-machine. La version 2 est disponible sur le site [IGNMap](https://ignmap.ign.fr/).  

Cette nouvelle version, donc la version 3, est développée avec [JUCE](https://juce.com/).

## JUCE

[JUCE](https://juce.com/) est une environnement de développement multi-plateforme destiné à l'origine pour le développement d'applications musicales. Il permet de réaliser des applications ou des plugins sous Windows, macOS, LINUX, iOS, Android.  
Le développement d'IGNMap se fait principalement sous Windows, mais l'objectif est bien d'avoir une version LINUX et éventuellement une version macOS.

## Dépendances

IGNMap utilise un certain nombre de bibliothèques :
* [JUCE](https://juce.com/) : pour le développement de l'interface homme-machine
* [OpenJPEG](https://github.com/uclouvain/openjpeg/) : pour la lecture des images au format JPEG2000
* [LibJPEG](https://www.ijg.org/) : pour la lecture des images JPEG
* [LibWebP](https://chromium.googlesource.com/webm/libwebp) : pour la lecture des images WebP
* [SQLite](https://www.sqlite.org/index.html) : pour la lecture du format GeoPackage
* [ZLib](https://www.zlib.net/) : pour la décompression ZLib
* [LASzip](https://github.com/LASzip/LASzip) : pour lecture des nuages de points au format LAS/LAZ

De plus IGNMap utilise du code C++ "pur" qui permet la gestion de données et un certain nombre de données :
* XTool : les outils de base pour organiser des données géographiques sous une forme d'une base de données
* XToolAlgo : des algorithmes en général destinés à la géomatique
* XToolGeod : des algorithmes pour la géodésie
* XToolImage : des outils pour lire des images
* XToolVector : des outils pour lire différents formats de données géographiques vectorielles

## Documentation développeurs

Le projet est géré via le Projucer de [JUCE](https://juce.com/) : cet outil permet de gérer les sources, les options de compilation et génère pour chaque système 
d'exploitation les projets nécessaires : Makefile pour LINUX, projet Visual Studio pour Windows.  
Un tutoriel est présent sur le site de JUCE : <https://docs.juce.com/master/tutorial_new_projucer_project.html>  
Avant toute première compilation, il est nécessaire d'ouvrir le projet avec le Projucer pour que celui-ci configure bien les chemins vers les modules JUCE !

Pour la compilation sous Windows, on utilise Visual Studio, la version gratuite (Community) est suffisante :
<https://visualstudio.microsoft.com/fr/vs/community/>


## L'arborescence du projet

Exemple d'arborescence de projet :

* `.github/` : dossier contenant les modèles d'issues et github actions ;
* `doc/` : dossier contenant des fichiers .md de documentation (ex: install.md) ;
* `tests/`: scripts et explications pour lancer les tests ;
* `README.md` : ce fichier

## Contacts du projets

Ici on met la listes des personnes qui travaillent sur ce projet et le maintiennent à jour.
