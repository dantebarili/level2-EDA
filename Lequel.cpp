/**
 * @brief Lequel? language identification based on trigrams
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2022-2023
 *
 * @cite https://towardsdatascience.com/understanding-cosine-similarity-and-its-application-fd42f585296a
 */

#include <cmath>
#include <codecvt>
#include <locale>


#include "Lequel.h"

using namespace std;

/**
 * @brief Builds a trigram profile from a given text.
 *
 * @param text Vector of lines (Text)
 * @return TrigramProfile The trigram profile
 */
TrigramProfile buildTrigramProfile(const Text &text)
{

    // Se cambió std::codecvt_utf8_utf16<wchar_t> por std::codecvt_utf8<wchar_t>
    // porque el programa se crasheaba si con simbolos unicode muy grandes (ej: un smiley)

    // De cppreference.com:
    // codecvt_utf8_utf16 converts between UTF-8 and UTF-16
    // codecvt_utf8 converts between UTF-16 and UCS-2/UCS-4
    wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    // Creamos el perfil de trigramas del texto ingresado
    TrigramProfile profile;

    // Iteramos sobre las lineas del texto eliminando los saltos de linea
    for (auto line : text){

        if ((line.length() > 0) &&
            (line[line.length() - 1] == '\r'))
            line = line.substr(0, line.length()-1);

        // Convierto la linea de texto a un string en formato Unicode
        wstring unicodeString = converter.from_bytes(line);

        if(unicodeString.length() < 3){
            continue;
        }

        auto i = 0;

        for(auto letter : unicodeString){
            wstring wtrigram = unicodeString.substr(i++, 3);
            // Convierto el Unicode string en un string comun para poder cargarlo en
            // el perfil de trigramas.
            string trigram = converter.to_bytes(wtrigram);
            auto it = profile.find(trigram);
            if (it != profile.end())
                it->second++;
            else
                profile.insert(std::pair<string,float>(trigram,1));
        }

    }

    return profile;
}

/**
 * @brief Normalizes a trigram profile.
 *
 * @param trigramProfile The trigram profile.
 */
void normalizeTrigramProfile(TrigramProfile &trigramProfile){

    float tot = 0;

    for(auto &el : trigramProfile)
        tot += el.second;

    // Termino de calcular la cantidad total de trigramas
    tot = sqrt(tot);

    // Calculo la frecuencia de cada trigrama
    for(auto &el : trigramProfile)
        el.second /= tot;

}

/**
 * @brief Calculates the cosine similarity between two trigram profiles
 *
 * @param textProfile The text trigram profile
 * @param languageProfile The language trigram profile
 * @return float The cosine similarity score
 */
float getCosineSimilarity(TrigramProfile &textProfile, TrigramProfile &languageProfile){

    float simCos = 0;

    // Calculo la similitud coseno entre el perfil del texto y de un lenguaje
    for (auto &p1 : textProfile)
        if (languageProfile.find(p1.first) != languageProfile.end())
            simCos += p1.second * languageProfile[p1.first];

    return simCos;
}

/**
 * @brief Identifies the language of a text.
 *
 * @param text A Text (vector of lines)
 * @param languages A list of Language objects
 * @return string The language code of the most likely language
 */
string identifyLanguage(const Text &text, LanguageProfiles &languages){

    // Creo el perfil de trigramas del texto ingresado
    TrigramProfile textProfile = buildTrigramProfile(text);

    float maxSim = 0;
    string langCode = "Err";

    // Analizo la similitud coseno del perfil del texto con los perfiles de todos los
    // lenguajes disponibles, almacenando el mayor resultado para, a partir de este,
    // mostrar el idioma con mayor similitud.
    for(auto &language : languages){
        float actSim = getCosineSimilarity(textProfile, language.trigramProfile);

        if(actSim > maxSim) {
            maxSim = actSim;
            langCode = language.languageCode;
        }

    }

    // Si ocurre un problema o no se haya similitud con ningun lenguaje se devuelve
    // un string que indica error.
    if(langCode == "Err")
        return "No se encontró lenguaje similar";

    return langCode;
}





