//-----------------------------------------------------------------------------
//								XJson.cpp
//								=========
//
// Traitement sur les fichiers JSON
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 09/11/2026
//-----------------------------------------------------------------------------

#include "XJson.h"

//-----------------------------------------------------------------------------
// Lecture d'un token JSON
//-----------------------------------------------------------------------------
std::string XJson::ReadStringToken(std::istream* in)
{
  char sep;
  std::string token;
  *in >> sep;
  if (sep == '"') {
    do {
      if (sep != '"')
        token += sep;
      sep = (char)in->get();
    } while (sep != '"');
    return token;
  }
  if (sep == '{') {
    int count_open = 1;
    int count_close = 0;
    do {
      token += sep;
      sep = (char)in->get();
      if (sep == '{')
        count_open++;
      if (sep == '}')
        count_close++;
    } while (count_close != count_open);
    token += sep;
    return token;
  }

  if (sep == '[') {
    int count_open = 1;
    int count_close = 0;
    do {
      token += sep;
      sep = (char)in->get();
      if (sep == '[')
        count_open++;
      if (sep == ']')
        count_close++;
    } while (count_close != count_open);
    token += sep;
    return token;
  }
  token += sep;
  do {
    *in >> sep;
    if ((sep == ',') || (sep == '}') || (sep == ']')) {
      in->putback(sep);
      break;
    }
    token += sep;
  } while (in->good());
  return token;
}

//-----------------------------------------------------------------------------
// Lecture d'un objet JSON
//-----------------------------------------------------------------------------
bool XJson::ReadKeyValue(std::istream* in, std::vector<std::string>& keys, std::vector<std::string>& values)
{
  char sep;
  std::string key, value;

  *in >> sep;
  if (sep != '{')
    return false;

  while ((!in->eof()) && (in->good())) {
    key = ReadStringToken(in);
    *in >> sep;
    if (sep != ':')
      return false;

    value = ReadStringToken(in);

    *in >> sep;
    if (sep == '}') {
      keys.push_back(key);
      values.push_back(value);
      return true;
    }
    if (sep != ',')
      return false;
    keys.push_back(key);
    values.push_back(value);
  }
  return false;
}

//-----------------------------------------------------------------------------
// Lecture d'un objet JSON
//-----------------------------------------------------------------------------
bool XJson::ReadKeyValue(std::string source, std::vector<std::string>& keys, std::vector<std::string>& values)
{
  std::istringstream in(source);
  return ReadKeyValue(&in, keys, values);
}

//-----------------------------------------------------------------------------
// Recherche d'une valeur correspondant à un ensemble de clefs
//-----------------------------------------------------------------------------
std::string XJson::FindValue(std::vector<std::string> find_keys, const std::string& source)
{
  std::vector<std::string> keys, values;
  std::string new_source = source;
  for (size_t i = 0; i < find_keys.size(); i++) {
    if (!ReadKeyValue(new_source, keys, values))
      return "";
    bool find = false;
    for (size_t j = 0; j < keys.size(); j++) {
      if (find_keys[i] == keys[j]) {
        find = true;
        new_source = values[j];
        break;
      }
    }
    if (find == false)
      return "";
    keys.clear();
    values.clear();
  }

  return new_source;
}

//-----------------------------------------------------------------------------
// Lecture d'un tableau de chaines de caracteres
//-----------------------------------------------------------------------------
int XJson::ReadArrayAsString(const std::string& source, std::vector<std::string>& T)
{
  if (source.size() < 2)
    return 0;
  if (source[0] != '[')
    return 0;
  std::string token;
  for (size_t i = 1; i < source.size(); i++) {
    if ((source[i] != ',') && (source[i] != '"') && (source[i] != ']'))
      token += source[i];
    else {
      if (source[i] == '"')
        continue;
      if ((source[i] == ',') || (source[i] == ']')) { // Fin d'un token
        T.push_back(token);
        token.clear();
        if ((source[i] == ']'))
          return (int)T.size();
      }
    }
  }
  return 0;
}

//-----------------------------------------------------------------------------
// Lecture d'un tableau de reels
//-----------------------------------------------------------------------------
int XJson::ReadArrayAsDouble(const std::string& source, std::vector<double>& T)
{
  if (source.size() < 2)
    return 0;
  if (source[0] != '[')
    return 0;
  std::string token;
  for (size_t i = 1; i < source.size(); i++) {
    if ((source[i] != ',') && (source[i] != '"') && (source[i] != ']'))
      token += source[i];
    else {
      if (source[i] == '"')
        continue;
      if ((source[i] == ',') || (source[i] == ']')) { // Fin d'un token
        T.push_back(std::stod(token));
        token.clear();
        if ((source[i] == ']'))
          return (int)T.size();
      }
    }
  }
  return 0;
}