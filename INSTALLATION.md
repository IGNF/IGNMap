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