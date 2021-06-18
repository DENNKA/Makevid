#include "Makevid.hpp"

Makevid::Makevid() {
    std::srand(std::time(NULL));
    setlocale(LC_ALL,"RUS");

}

void Makevid::help() {
    std::cout << "No help" << std::endl;
}

void Makevid::parseArgs(std::vector<std::string> args) {
    std::vector<Phrase> phrases;
    std::string musicFile;

    int subtitlesOffset = 0;

    try {

        for (int i = 0; i < (int)args.size(); i++) {
            auto& arg = args[i];

            #define arg1 (args.at(i + 1))
            #define check(x,y) if (arg == "--" x || arg == "-" y)
            #define I(x) (std::stoi(x))

            check("help", "h") {
                help();
            }
            else check("input", "i") {
                phrases = parseSubtitles(arg1, subtitlesOffset);
            }
            else check("music", "m") {
                musicFile = arg1;
            }
            else check("offset", "o") {
                subtitlesOffset = I(arg1);
            }
            else check("process", "p") {
                phrases = processing(phrases);
            }
            else check("render", "r") {
                render(phrases, musicFile);
            }
            else check("save", "s") {
                saveToFile(arg1, std::vector<std::string>(phrases.begin(), phrases.end()));
            }
            else check("load", "l") {
                auto lines = readFromFile(arg1);
                phrases = std::vector<Phrase>(lines.begin(), lines.end());
            }

            #undef I
            #undef check
            #undef arg1

        }
    }
    catch (std::out_of_range e) {
        std::cerr << "Excpected value" << std::endl;
    }
}

void Makevid::saveToFile(std::string file, std::vector<std::string> lines) {
    std::ofstream out(file);
    for (auto& line : lines)
        out << line << std::endl;
}

std::vector<std::string> Makevid::readFromFile(std::string file) {
    std::vector<std::string> lines;
    std::ifstream in(file);

    if (in.is_open()) {
        std::string line;

        while (getline(in, line)) {
            lines.push_back(line);
        }
    }

    return lines;
}

auto eraseHtml(std::string str) {
    while (str.find("<") != std::string::npos)
    {
        auto startpos = str.find("<");
        auto endpos = str.find(">") + 1;

        if (endpos != std::string::npos)
        {
            str.erase(startpos, endpos - startpos);
        }
    }
    return str;
}

inline std::string trim(const std::string &s)
{
   auto  wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
   return std::string(wsfront,std::find_if_not(s.rbegin(),std::string::const_reverse_iterator(wsfront),[](int c){return std::isspace(c);}).base());
}

std::vector<Phrase> Makevid::parseSubtitles(std::string file, int subtitlesOffset/* = 0 */) {
    std::vector<Phrase> phrases;
    std::string line;
    std::ifstream in(file);

    if (in.is_open()) {
        long start = 0, end = 0;
        std::string text = "";
        int currentPhrase = 0;
        while (getline(in, line)) {
            if (line.empty())
                continue;
            if (line.find(" --> ") != std::string::npos) {
                int h1, m1, s1, ml1, h2, m2, s2, ml2;
                auto lineStr = std::string(line.begin(), line.end());
                if (sscanf(lineStr.c_str(), "%d:%d:%d,%d --> %d:%d:%d,%d", &h1, &m1, &s1, &ml1, &h2, &m2, &s2, &ml2) >= 8) {
                    start = h1 * 60 * 60 * 1000 + m1 * 60 * 1000 + s1 * 1000 + ml1 + subtitlesOffset;
                    end = h2 * 60 * 60 * 1000 + m2 * 60 * 1000 + s2 * 1000 + ml2 + subtitlesOffset;
                } else std::cerr << "ERROR PARSE TIME FROM SUBTITLES" << std::endl;
            }
            else if (line.find(std::to_string(currentPhrase + 1)) != std::string::npos) {
                currentPhrase++;
            }
            else {
                text = trim(eraseHtml(line));
                phrases.push_back({start, end, text});
            }
        }
    } else {
        std::cerr << "File not found" << std::endl;
    }
    in.close();

    for (size_t i = 0; i < phrases.size() - 1; i++) {
        if (phrases[i].getEnd() > phrases[i + 1].getStart()) {
            std::cerr << "ERROR SUBTITLES INTERSECTION, ignoring. Numer "<< i + 1 << " and " << i + 2 << std::endl;
        }
    }

    return phrases;
}

std::vector<Phrase> Makevid::processing(std::vector<Phrase> phrases) {
    std::vector<std::string> shadersPaths;
    std::string line;
    std::ifstream in("shaders_good.txt");
    if (in.is_open())
    {
        while (getline(in, line))
        {
            shadersPaths.push_back(line);
        }
    }
    in.close();

    std::sort(shadersPaths.begin(), shadersPaths.end());

    for (int i = 0; i < (int)phrases.size(); i++) {
        auto& phrase = phrases[i];
        auto text = phrases[i].getText();
        phrase.addMode(Mode::Shader, {shadersPaths[rand() % shadersPaths.size()]});

        if (std::all_of(text.begin(), text.end(), [](unsigned char c){ return !std::isalpha(c) || std::isupper(c); })) {
            phrase.addMode(Mode::Zoom, {"1", "10", "100"});
        }

    }

    return phrases;
}

void Makevid::render(std::vector<Phrase> phrases, std::string musicFile) {
    Render render;
    render.render(phrases, musicFile);
}
