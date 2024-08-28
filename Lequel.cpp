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
#include <iostream>


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
    wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;


    TrigramProfile profile;

    for (auto line : text)
    {
        if ((line.length() > 0) &&
            (line[line.length() - 1] == '\r'))
            line = line.substr(0, line.length()-1);

        wstring unicodeString = converter.from_bytes(line);

        if(unicodeString.length() < 3){
            continue;
        }
        auto i = 0;
        for(auto letter : unicodeString){
            wstring wtrigram = unicodeString.substr(i++, 3);
            string trigram = converter.to_bytes(wtrigram);
            auto it = profile.find(trigram);
            if (it != profile.end())
                it->second++;
            else
                profile.insert(std::pair<string,float>(trigram,1));
        }


    }
    return profile; // Fill-in result here
}

/**
 * @brief Normalizes a trigram profile.
 *
 * @param trigramProfile The trigram profile.
 */
void normalizeTrigramProfile(TrigramProfile &trigramProfile)
{
    float tot = 0;

    for(auto &el : trigramProfile)
        tot += el.second;

    tot = sqrt(tot);

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
float getCosineSimilarity(TrigramProfile &textProfile, TrigramProfile &languageProfile)
{
    float simCos = 0;
    for (auto &p1 : textProfile)
        if (languageProfile.find(p1.first) != languageProfile.end())
            simCos += p1.second * languageProfile[p1.first];
    return simCos; // Fill-in result here
}

/**
 * @brief Identifies the language of a text.
 *
 * @param text A Text (vector of lines)
 * @param languages A list of Language objects
 * @return string The language code of the most likely language
 */
string identifyLanguage(const Text &text, LanguageProfiles &languages)
{

    TrigramProfile textProfile = buildTrigramProfile(text);

    float maxSim = 0;
    string langCode = "Err";

    for(auto &language : languages){
        float actSim = getCosineSimilarity(textProfile, language.trigramProfile);

        if(actSim > maxSim) {
            maxSim = actSim;
            langCode = language.languageCode;
        }

    }

    if(langCode == "Err")
        return "No se encontró lenguaje similar";

    return langCode; // Fill-in result here
}
