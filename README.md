# IGNMap


## Description/Résumé du projet

IGNMap est un logiciel de visualisation de données géographiques. Il permet de visualiser des données vectorielles, des images, des modèles numériques de terrain, des nuages de points LIDAR.  
IGNMap n’est pas à proprement parler un système d’informations géographiques (SIG) mais plutôt un outil léger pour visualiser et analyser des données.  
Quelques exemples de visualisation :  
<div align="center">
<img alt="IGNMap" src="https://raw.githubusercontent.com/IGNF/IGNMap/master/Documentation/Images/IGNMap01.png">

<img alt="IGNMap" src="https://raw.githubusercontent.com/IGNF/IGNMap/master/Documentation/Images/IGNMap02.png">

<img alt="IGNMap" src="https://raw.githubusercontent.com/IGNF/IGNMap/master/Documentation/Images/IGNMap03.png">
</div><br>

Une chaîne vidéo YouTube propose quelques tutoriels : [www.youtube.com/@IGNF_IGNMap](https://www.youtube.com/@IGNF_IGNMap)

## Historique

IGNMap était à la base un outil interne de l’institut géographique national (IGN). La version 1 était développé avec la bibliothèque MFC de Microsoft pour la création de l’interface homme-machine.  

La version 2 a été distribuée à l’extérieur de l’IGN, principalement pour aider au passage au RGF 93, cette version proposant des outils pour la conversion des données des anciennes projections légales (Lambert 2 étendu pour la France métropolitaine ; Saint-Anne, Fort Marigot et Fort Desaix pour les Antilles, CSG1967 pour la Guyane ; Piton des Neiges pour la Réunion ; …) vers les nouvelles projections (Lambert 93, RGAF09, RGRG95, RGR92, …). Cette version 2 était développée avec QT4 pour la création de l’interface homme-machine. La version 2 est disponible sur le site [IGNMap](https://ignmap.ign.fr/).  

Cette nouvelle version, donc la version 3, est développée avec [JUCE](https://juce.com/). Elle est open-source, sous licence AGPL-3.

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
* [TinyEXIF] (https://github.com/cdcseacave/TinyEXIF) : pour la lecture des champs EXIF dans les images JPEG
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

Pour pouvoir compiler le projet, il est nécessaire de récupérer les bibliothèques tierces et d'installer l'ensemble des bibliothèques dans
un même répertoire :

* `IGNMap/` : dossier contenant les sources du projet ;
* `jpeg-9f/` : dossier contenant la bibliothèque [LibJPEG](https://www.ijg.org/) ;
* `zlib-1.3.1/` : dossier contenant la bibliothèque [ZLib](https://www.zlib.net/) ;
* `JUCE/` : dossier contenant [JUCE](https://juce.com/) ;
* `LASzip/` : dossier contenant la bibliothèque [LASzip](https://github.com/LASzip/LASzip) ;
* `libwebp-1.3.2/` : dossier contenant la bibliothèque [LibWebP](https://chromium.googlesource.com/webm/libwebp) ;
* `openjpeg/` : dossier contenant la bibliothèque [OpenJPEG](https://github.com/uclouvain/openjpeg/) ;
* `Sqlite/` : dossier contenant la bibliothèque [SQLite](https://www.sqlite.org/index.html) ;
* `TinyEXIF/` : dossier contenant la bibliothèque [TinyEXIF](https://github.com/cdcseacave/TinyEXIF) ;
* `XTool/` : bibliothèque de base pour la gestion de données géographiques ;
* `XToolAlgo/` : bibliothèque pour les algorithmes géomatiques ;
* `XToolGeod/` : bibliothèque pour les calculs géodésiques ;
* `XToolImage/` : bibliothèque pour la gestion des images ;
* `XToolVector/` : bibliothèque pour la gestion des données vectorielles ;
  
Le répertoire IGNMap contient :
* `Builds/` : ce répertoire contient les différentes cibles de compilation comme les makefile LINUX ou le projet Visual Studio ;
* `Images/` : les images nécessaires pour l'interface homme-machine ;
* `JuceLibraryCode/` : le code JUCE généré par le Projucer de JUCE. Ne pas y toucher !!!
* `Source/` : les sources de l'interface homme-machine d'IGNMap ;
* `IGNMap.jucer` : le fichier projet du Projucer. Ne pas éditer ce fichier à la main ! Il faut l'ouvrir avec le Projucer.
  
De plus, on trouvera :
* `Documentation/` : dossier contenant la documentation ;
* `LICENSE` : le fichier de license AGPL ;
* `LICENSE.md` : fichier explicitant la licence ;
* `README.md` : ce fichier

## Contacts du projets

IGNMap a un site : [ignmap.ign.fr](htpps://ignmap.ign.fr)
On peut contacter le projet via [la page Contact](https://ignmap.ign.fr/contact.html)
