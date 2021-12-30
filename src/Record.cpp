#include "Record.hpp"
#include "format.cpp"

void Record::run(int fps, int width, int height, std::string encoder) {
    std::string exe = "ffmpeg";
    std::string input = format("-r %d -f rawvideo -pix_fmt rgba -s %dx%d -i -", fps, width, height);
    std::string other = "-threads 0";
    std::string output = format("-c:v %s -preset fast -crf 23 -vf vflip -y output.mp4", encoder.c_str());
    std::string log = "2> ffmpeg.log";
    std::string cmd = exe + " " + input + " " + other + " " + output + " " + log;

    sizeScreen = 4 * sizeof(GLubyte) * width * height;

    ffmpeg = popen(cmd.c_str(), "w");
    if (!ffmpeg) {
        //std::cerr << "FFMPEG ERROR: " << strerror(errno) << std::endl;
        perror("FFMPEG ERROR");
    }
}

void Record::write(GLubyte* buffer) {
    fwrite(buffer, sizeScreen, 1, ffmpeg);
}

Record::~Record() {
    pclose(ffmpeg);
}
