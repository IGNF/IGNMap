# Installation des bibliotheques externes

Pour compiler IGNMap, il est nécessaire de configurer les bibliotheques externes :

## LibJPEG
Il faut copier le fichier jconfig.vc en jconfig.h

## ZLib
Rien à faire !

## LibWebP
Rien à faire !

## OpenJPEG
Il faut créer les fichiers opj_config.h et opj_config_private.h.

## LASzip
Il faut modifier le fichier laszip_dll.cpp :
* rajouter #define NOMINMAX après #define LASZIP_SOURCE (ligne 50)
* supprimer la ligne #include "../dll/laszip_api.h"
* ajouter la ligne #include "../dll/laszip_api.h" après la ligne #include "lasmessage.hpp"

Ligne 4294 : #ifdef _MSC_VER à remplacer par #ifdef _MSC_VER666 (dans la méthode laszip_open_reader)
Le code de LASzip exécute un code particulier sous Windows pour l'ouverture des fichiers (sous LINUX et MacOS, les noms de fichiers sont en UTF8. Sous Windows, ce n'est pas le cas pour garder la compatibilité avec les anciennes versions de Windows). Toutefois, IGNMap gère cela en amont (utilisation de std::filesystem::path pour gérer les chemins de manière multiplateforme, ce qui est la méthode moderne pour le faire).