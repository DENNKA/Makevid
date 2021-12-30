#include "Render.hpp"
#include "format.cpp"

namespace fs = std::filesystem;

/*std::size_t getNumberOfFiles(std::filesystem::path path)
{
    return (std::size_t)std::distance(std::filesystem::directory_iterator{path}, std::filesystem::directory_iterator{});
}*/

Render::Render() {
    for(auto& p : fs::directory_iterator("glsl/glsl"))
        shadersPaths.push_back(p.path());

    /*std::string line;
    std::ifstream in("shaders_static.txt");
    if (in.is_open())
    {
        while (getline(in, line))
        {
            shadersPaths.push_back(line);
        }
    }
    in.close();*/

    std::sort(shadersPaths.begin(), shadersPaths.end());
}

struct ForF {
    float min, max, threshold, time, offset;
};

/*struct Flo {
    Flo(float number) : number(number) {;};
    float number;
    operator float() {
        return number;
    }
    const float operator < ( const Flo &r ) const{
        return number < r.number ? number : r.number;
    }
    const float operator > ( const Flo &r ) const{
        return number > r.number ? number : r.number;
    }
};*/

float linear(ForF in) {
    return in.time * ((in.max - in.min) / in.threshold) + in.min - in.offset;
}

/*float max(ForF in) {
    return in.threshold > in.time ? in.max : 0;
}

float min(ForF in) {
    return in.threshold < in.time ? in.max : 0;
}*/

void Render::render(std::vector<Phrase> phrases, std::string musicFile) {
    //std::cout << (Flo(10.f) > Flo(5.0f)) << std::endl;
    /*for (auto phrase : phrases) {
        std::cerr<<phrase.getStart()<<L" : "<<phrase.getEnd()<<L" "<<phrase.getText()<<std::endl;
    }*/
    //sf::RenderWindow window(sf::VideoMode::getFullscreenModes()[0], "Makevid");
    #ifdef REALTIME
        sf::RenderWindow window(sf::VideoMode(1600, 848), "Makevid");
    #else
        sf::RenderTexture window;
        window.create(1600, 900);
    #endif
    const int fps = 64;
    //window.setFramerateLimit(60);

    int width = window.getSize().x;
    int height = window.getSize().y;
    sf::RectangleShape rect(sf::Vector2f(width, height));
    sf::RectangleShape rectangleBackground(sf::Vector2f(width, height));
    rect.setFillColor(sf::Color::Black);

    sf::RenderTexture rectangle;
    if (!rectangle.create(width, height)){ //create a render texture
        return;
    }
    rectangle.draw(rect);
    rectangle.display();

    sf::Sprite background(rectangle.getTexture());

    sf::Shader shader;
    sf::Shader shaderService;
    sf::Shader shaderText;
    sf::Clock clock;
    //sf::Clock clockStartPhrase;

    std::string fontName = "fonts/Play/Play-Regular.ttf";
    //std::string fontName = "fonts/Spinnaker/Spinnaker-Regular.ttf";
    sf::Font font;
    if (!font.loadFromFile(fontName))
    {
        std::cerr << "FONT ERROR" << std::endl;
    }
    sf::Text text;
    text.setFont(font);
    //text.setString(L"Привет мир");
    text.setCharacterSize(50);
    text.setFillColor(sf::Color::White);

    int currentPhrase = 0;
    int currentShader = 1202; // service
    int lastShader = currentShader;
    bool activePhrase = false;
    bool switchShader = true;
    int charSize = 70;
    float thickness = 4.0f;

    bool running = true;
    bool service = false;
    bool musicRunning = false;

    //int numberFrame = 0;

    sf::Music music;
    if (!musicFile.empty())
        if (!music.openFromFile(musicFile))
            std::cerr << L"MUSIC ERROR" << std::endl;

    sf::RenderTexture renderFunction;
    if (!renderFunction.create(width, height))
    {
        // error...
    }

    double time = 0.0; // ms

    sf::RenderTexture renderTexture;
    renderTexture.create(width, height);

    std::wstring_convert<std::codecvt_utf8<wchar_t>> cv;

    GLubyte* screenBuffer = new GLubyte[width * height * 4];
    Record record;
    record.run(fps, width, height);

    while (running) {

        #ifdef REALTIME
            sf::Event event;
            while (window.pollEvent(event)) {
                switch (event.type) {
                    case sf::Event::Closed:
                        window.close();
                        running = false;
                        return;
                    case sf::Event::KeyPressed:
                        switch (event.key.code) {
                            case sf::Keyboard::Space:
                                running = !running;
                                clock.restart();
                                break;
                            case sf::Keyboard::Escape:
                                window.close();
                                return;
                            case sf::Keyboard::C:
                                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                                    window.close();
                                }
                                return;
                            case sf::Keyboard::S:
                                service = true;
                                break;
                            case sf::Keyboard::Right:
                                if (service) {
                                    std::ofstream out;
                                    out.open("shaders_good.txt", std::ios_base::app);
                                    std::size_t found = shadersPaths[currentShader].find_last_of("/\\");
                                    std::string filename = "glsl/glsl/" + shadersPaths[currentShader].substr(found + 1);
                                    out << filename << std::endl;
                                    currentShader++;
                                }
                                break;
                            case sf::Keyboard::Up:
                                if (service) {
                                    std::ofstream out;
                                    out.open("shaders_static.txt", std::ios_base::app);
                                    std::size_t found = shadersPaths[currentShader].find_last_of("/\\");
                                    std::string filename = "glsl/glsl/" + shadersPaths[currentShader].substr(found + 1);
                                    out << filename << std::endl;
                                    currentShader++;
                                }
                                break;
                            case sf::Keyboard::Down:
                                if (service) {
                                    std::ofstream out;
                                    out.open("shaders_normal.txt", std::ios_base::app);
                                    std::size_t found = shadersPaths[currentShader].find_last_of("/\\");
                                    std::string filename = "glsl/glsl/" + shadersPaths[currentShader].substr(found + 1);
                                    out << filename << std::endl;
                                    currentShader++;
                                }
                                break;
                            case sf::Keyboard::Left:
                                currentShader++;
                                break;
                            default: break;
                        }
                    default:
                        break;
                }
            }
        #endif

        bool textUpdated = false;

        if (service) {

            sf::RectangleShape rect(sf::Vector2f(width, height));
            if (currentShader != lastShader) {
                lastShader = currentShader;
                std::clog << currentShader << " " << shadersPaths[currentShader] << std::endl;
                if (!shaderService.loadFromFile(shadersPaths.at(currentShader), sf::Shader::Fragment)) {
                    std::cerr << "SHADER ERROR" << std::endl;
                    currentShader++;
                }
                shaderService.setUniform("resolution", sf::Vector2f(width, height));
            }

            shaderService.setUniform("time", (float)clock.getElapsedTime().asMilliseconds() / 1000.0f);

            rect.setFillColor(sf::Color::Red);

            window.clear();

            window.draw(rect, &shaderService);

            window.display();

            //lastShader = currentShader;
        }

        if (!musicRunning) {
            musicRunning = true;
            music.play();
        }

        #ifdef REALTIME
            time = (double)clock.getElapsedTime().asMilliseconds();
        #endif

        if (currentPhrase < (int)phrases.size()) {
            //std::clog << "start " << phrases[currentPhrase].getStart() << " end " << phrases[currentPhrase].getEnd() << " time " << time << "active" << (activePhrase ? "TRUE" : "FALSE") << std::endl;

            if (phrases[currentPhrase].getStart() <= time && phrases[currentPhrase].getEnd() >= time) {

                // NEXT PHRASE
                if (!activePhrase) {
                    text.setString(cv.from_bytes(phrases[currentPhrase].getText()));

                    // try find optimal charSize
                    text.setCharacterSize(charSize);
                    while (width - 400 > text.getLocalBounds().width && charSize < 140) {
                        charSize += 5;
                        text.setCharacterSize(charSize);
                    }
                    while (width - 300 < text.getLocalBounds().width) {
                        charSize -= 5;
                        text.setCharacterSize(charSize);
                    }

                    text.setOrigin(text.getLocalBounds().left + text.getLocalBounds().width / 2.0f, text.getLocalBounds().top + text.getLocalBounds().height / 2.0f);
                    text.setPosition(width / 2.0f, height / 2.0f);

                    std::clog << "Phrase: " << currentPhrase << " / " << phrases.size() << std::endl;
                    //std::clog << "charSize: " << charSize << std::endl;

                    rectangleBackground.setFillColor(sf::Color::Transparent);

                    text.setOutlineColor(sf::Color::Black);
                    text.setOutlineThickness(4.0f);

                    textUpdated = true;
                }

                activePhrase = true;
            } else {
                if (activePhrase) {
                    currentPhrase++;
                    switchShader = true;
                    text.setString(L"");
                }
                activePhrase = false;
            }

            // yes it's second check cuz above currentPhrase++
            if (currentPhrase < (int)phrases.size()) {
                for (auto& phraseMode : phrases[currentPhrase].getModes()) {
                    auto& params = phraseMode.params;
                    auto timeFromStartPhrase = time - phrases[currentPhrase].getStart();
                    auto timePhrase = phrases[currentPhrase].getEnd() - phrases[currentPhrase].getStart();

                    bool needCenter = false;

                    #define f(x) std::stof(x)
                    #define i(x) std::stoi(x)

                    // skip if time not match (from start)
                    #define checkTime(x) if (timeFromStartPhrase > x) {break;}
                    // reverse (from end)
                    #define checkTimeReverse(x) if (timeFromStartPhrase < x) {break;}

                    if (textUpdated) {
                        switch (phraseMode.mode) {
                            case Mode::BackgroundColor:
                                rectangleBackground.setFillColor(sf::Color(i(params[0]), i(params[1]), i(params[2]), i(params[3])));
                                break;
                            default:
                                break;
                        }
                    }

                    ForF forF;
                    float y;

                    switch (phraseMode.mode) {
                        case Mode::Shader:
                            if (switchShader) {
                                switchShader = false;
                                std::clog << "Shader: " << phraseMode.params[0] << std::endl;
                                if (!shader.loadFromFile(phraseMode.params[0], sf::Shader::Fragment)) {
                                    std::cerr << "SHADER ERROR" << std::endl;
                                }
                                shader.setUniform("resolution", sf::Vector2f(width, height));
                                shader.setUniform("mouse", sf::Vector2f(0.5f, 0.5f));
                            }
                            break;
                        case Mode::Zoom: {
                            if (i(params[0]) == 0) {
                                checkTime(f(params[2]));
                                forF = {f(params[1]), charSize, f(params[2]), timeFromStartPhrase};
                                y = std::min(linear(forF), (float)charSize);
                            } else {
                                checkTimeReverse(f(params[2]));
                                forF = {charSize, f(params[1]), f(params[2]), timeFromStartPhrase, f(params[2]) - timePhrase};
                                y = std::max(std::min(linear(forF), (float)charSize), 0.0f);
                            }

                            text.setCharacterSize(y);
                            text.setOutlineThickness(y / charSize * thickness);

                            needCenter = true;
                            break;
                        }
                        default:
                            break;
                    }

                    if (needCenter) {
                        text.setOrigin(text.getLocalBounds().left + text.getLocalBounds().width / 2.0f, text.getLocalBounds().top + text.getLocalBounds().height / 2.0f);
                        text.setPosition(width / 2.0f, height / 2.0f);
                    }

                    #undef i
                    #undef f

                }
            }
        } else {
            text.setString(L"");
        }

        #ifdef CHECK_FUNCTION
            renderFunction.clear();
            sf::RectangleShape rectangle(sf::Vector2f(120.f, 50.f));
            rectangle.setSize(sf::Vector2f(1, 1));
            for (int i = 0; i < 1000; i++) {

                ForF forF = {0, 100, 1000, i};
                auto y = linear(forF);

                rectangle.setPosition(i, height / 2 - y);
                rectangle.setFillColor(sf::Color::Blue);
                renderFunction.draw(rectangle);

                y = linear(forF);

                rectangle.setFillColor(sf::Color::Red);
                rectangle.setPosition(i, height / 2 - y);
                renderFunction.draw(rectangle);
            }
            renderFunction.display();
        #endif

        /*if (switchShader) {
            switchShader = false;*/
            /*bool error = true;
            while (error) {
                auto random = rand() % shadersPaths.size();
                std::clog << "Shader: " << shadersPaths[random] << std::endl;
                if (!shader.loadFromFile(shadersPaths[random], sf::Shader::Fragment)) {
                    std::cerr << L"SHADER ERROR" << std::endl;
                } else error = false;
            }*/
            /*error = true;
            while (error) {
                auto random = rand() % shadersPaths.size();
                std::clog << "Text shader: "  << shadersPaths[random] << std::endl;
                std::ifstream ifs(shadersPaths[random]);
                std::string content((std::istreambuf_iterator<char>(ifs)),
                                    (std::istreambuf_iterator<char>()));
                content.insert(0, "uniform sampler2D uTexture;\n");
                std::string toFind = "gl_FragColor";
                std::size_t found = content.find(toFind);
                if (found != std::string::npos) {
                    std::size_t found2 = content.find("=", found);
                    if (found2 != std::string::npos) {
                        content.replace(found2, 1, "= texture2D(uTexture, gl_TexCoord[0].st) *");
                    }
                }
                if (!shaderText.loadFromMemory(content, sf::Shader::Fragment)) {
                    std::cerr << "SHADER ERROR" << std::endl;
                } else error = false;
            }*/

            //shaderNumber++;

            /*shader.setUniform("resolution", sf::Vector2f(width, height));
            //shaderText.setUniform("resolution", sf::Vector2f(text.getLocalBounds().width, text.getLocalBounds().height));

            shader.setUniform("mouse", sf::Vector2f(0.5f, 0.5f));
            //shaderText.setUniform("mouse", sf::Vector2f(0.5f, 0.5f));
        }*/

        shader.setUniform("time", (float)time / 1000);

        window.clear();

        window.draw(background, &shader);

        #ifdef CHECK_FUNCTION
            const sf::Texture& texture = renderFunction.getTexture();
            sf::Sprite sprite(texture);
            window.draw(sprite);
        #endif

        window.draw(rectangleBackground);

        text.getCharacterSize() > 0 ? window.draw(text) : void();

        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer);
        record.write(screenBuffer);

        #ifdef REALTIME
            window.display();
        #else
            time += 1000.0d / (double)fps;
        #endif
    }
    delete[] screenBuffer;
}

/*Render::~Render() {

}*/
