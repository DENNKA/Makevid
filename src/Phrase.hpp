#ifndef PHRASE_HPP
#define PHRASE_HPP

#include <string>
#include <vector>
#include <iterator>
#include <sstream>

enum class Mode {
    Shader,
    InZoom,
    OutZoom
};

struct Operations {
    std::string join(std::vector<std::string> strings, std::string delim = " ") {
        std::string out;
        for (auto& str : strings) {
            out += str + delim;
        }
        if (!delim.empty()) {
            out = out.substr(0, out.size() - delim.size());
        }
        return out;
    }
    std::vector<std::string> split(std::string s, std::string delim) {
        std::vector<std::string> elems;
        size_t pos = 0;
        std::string token;
        while ((pos = s.find(delim)) != std::string::npos) {
            token = s.substr(0, pos);
            elems.push_back(token);
            s.erase(0, pos + delim.length());
        }
        elems.push_back(s);
        return elems;
    }
};

struct PhraseMode {
    PhraseMode (Mode mode, std::vector<std::string> params) : mode(mode), params(params) {;}
    PhraseMode (std::string text) {
        auto parts = operations.split(text, delimiter);
        mode = (Mode)std::stoi(parts[0]);
        for (int i = 1; (unsigned)i < parts.size(); i++) {
            params.push_back(parts[i]);
        }
    }
    operator std::string() {
        return std::to_string((int)mode) + " " + operations.join(params);
    }

    Mode mode;
    std::vector<std::string> params;
    const char* const delimiter = " ";
    Operations operations;
};

class Phrase {
    public:
        Phrase (long start, long end, std::string text);
        Phrase (std::string text);
        auto getStart() {return start;}
        void setStart(long start) {this->start = start;}
        auto getEnd() {return end;}
        void setEnd(long end) {this->end = end;}
        auto getText() {return text;}
        void setText(std::string text) {this->text = text;}
        void addMode(Mode mode, std::vector<std::string> params) {modes.push_back({mode, params});}
        auto getModes() {return modes;}

        operator std::string() {
            return std::to_string(getStart()) + delimiter + std::to_string(getEnd()) + delimiter + getText() + delimiter +
            operations.join(std::vector<std::string>(modes.begin(), modes.end()), delimiter);
        }
    private:
        long start, end;
        std::string text;
        std::vector<PhraseMode> modes;
        std::string delimiter = "|^-^|";
        Operations operations;
};

#endif // PHRASE_HPP
