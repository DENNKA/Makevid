#ifndef RECORD_HPP
#define RECORD_HPP

#include <string>
#include <cstdio>

#include <SFML/OpenGL.hpp>

class Record {
    public:
        void run(int fps, int width, int height, std::string encoder="libx264");
        void write(GLubyte* buffer);
        ~Record();

    private:
        FILE* ffmpeg;
        int sizeScreen;
};

#endif // RECORD_HPP
