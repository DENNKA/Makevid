#include "Phrase.hpp"

Phrase::Phrase (long start, long end, std::string text) : start(start), end(end), text(text) {;}

Phrase::Phrase (std::string text) {
    auto parts = operations.split(text, delimiter);
    setStart(std::stol(parts[0]));
    setEnd(std::stol(parts[1]));
    setText(parts[2]);
    for (int i = 3; (unsigned)i < parts.size(); i++) {
        modes.push_back(PhraseMode(parts[i]));
    }
}
