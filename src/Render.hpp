#ifndef RENDER_HPP
#define RENDER_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <cmath>
#include <future>
#include <locale>
#include <errno.h>

#include "Phrase.hpp"
#include "Record.hpp"

class Render {
    public:
        Render();
        void render(std::vector<Phrase> phrases, std::string musicFile);
    private:
        std::vector<std::string> shadersPaths;
};

#endif // RENDER_HPP
