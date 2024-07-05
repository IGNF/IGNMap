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
Il faut copier le fichier laszip_api.h se trouvant dans include/laszip dans le répertoire src.
Il faut modifier le fichier laszip_dll.cpp :
* supprimer la ligne #include <laszip/laszip_api.h>
* ajouter la ligne #include "laszip_api.h" après la ligne #include "lasindex.hpp"

Ligne 4337 : #ifdef _MSC_VER à remplacer par #ifdef _MSC_VER666
Le code de LASzip exécute un code particulier sous Windows pour l'ouverture des fichiers (sous LINUX et MacOS, les noms de fichiers sont en UTF8. Sous Windows, ce n'est pas le cas pour garder la compatibilité avec les anciennes versions de Windows). Toutefois, IGNMap gère cela en amont (utilisation de std::filesystem::path pour gérer les chemins de manière multiplateforme, ce qui est la méthode moderne pour le faire).