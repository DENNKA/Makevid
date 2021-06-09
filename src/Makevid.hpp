#ifndef MAKEVID_HPP
#define MAKEVID_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>

#include "Phrase.hpp"
#include "Render.hpp"

class Makevid{
    public:
        Makevid();
        void help();
        void parseArgs(std::vector<std::string> args);
        void saveToFile(std::string file, std::vector<std::string> lines);
        std::vector<std::string> readFromFile(std::string file);
        std::vector<Phrase> parseSubtitles(std::string file, int subtitlesOffset = 0 );
        std::vector<Phrase> processing(std::vector<Phrase> phrases);
        void render(std::vector<Phrase> phrases, std::string musicFile);
    private:

};

#endif // MAKEVID_HPP
