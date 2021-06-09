#include "Render.hpp"

namespace fs = std::filesystem;

/*std::size_t getNumberOfFiles(std::filesystem::path path)
{
    return (std::size_t)std::distance(std::filesystem::directory_iterator{path}, std::filesystem::directory_iterator{});
}*/

Render::Render() {
    //for(auto& p : fs::directory_iterator("glsl/animated"))
        //shadersPaths.push_back(p.path());

    std::string line;
    std::ifstream in("shaders_static.txt");
    if (in.is_open())
    {
        while (getline(in, line))
        {
            shadersPaths.push_back(line);
        }
    }
    in.close();

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
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Makevid");
    window.setFramerateLimit(70);

    int width = window.getSize().x;
    int height = window.getSize().y;
    sf::RectangleShape rect(sf::Vector2f(width, height));
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
    int currentShader = 850; // service
    int lastShader = currentShader;
    bool activePhrase = false;
    bool switchShader = true;
    int charSize = 70;
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(4.0f);

    bool running = false;
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

    //double time = 0.0d;

    std::wstring_convert<std::codecvt_utf8<wchar_t>> cv;

    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
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
                                out << shadersPaths[currentShader] << std::endl;
                                currentShader++;
                            }
                            break;
                        case sf::Keyboard::Up:
                            if (service) {
                                std::ofstream out;
                                out.open("shaders_static.txt", std::ios_base::app);
                                out << shadersPaths[currentShader] << std::endl;
                                currentShader++;
                            }
                            break;
                        case sf::Keyboard::Down:
                            if (service) {
                                std::ofstream out;
                                out.open("shaders_normal.txt", std::ios_base::app);
                                out << shadersPaths[currentShader] << std::endl;
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

        if (!running) {
            continue;
        }

        if (!musicRunning) {
            musicRunning = true;
            music.play();
        }

        auto time = clock.getElapsedTime().asMilliseconds();

        if (currentPhrase < (int)phrases.size()) {

            std::clog << "start " << phrases[currentPhrase].getStart() << " end " << phrases[currentPhrase].getEnd() << " time " << time << "active" << (activePhrase ? "TRUE" : "FALSE") << std::endl;

            if (phrases[currentPhrase].getStart() <= time && phrases[currentPhrase].getEnd() > time) {
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
                    std::clog << "charSize: " << charSize << std::endl;
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
                        case Mode::InZoom: {
                            ForF forF = {f(params[0]), charSize, f(params[1]), timeFromStartPhrase};
                            text.setCharacterSize(std::min(linear(forF), (float)charSize));
                            //ForF forF = {charSize, f(params[0]), f(params[1]), timeFromStartPhrase, f(params[1]) - timePhrase};
                            //text.setCharacterSize(std::max(linear(forF), (float)charSize));
                            /*renderFunction.clear();

                            sf::RectangleShape rectangle(sf::Vector2f(120.f, 50.f));
                            rectangle.setSize(sf::Vector2f(1, 1));
                            for (int i = 0; i < 1000; i++) {
                                ForF forF = {charSize, f(params[0]), f(params[1]), i, -500};
                                auto y = std::min(linear(forF), (float)charSize);
                                rectangle.setPosition(i, height/2 - y);
                                rectangle.setFillColor(sf::Color::Blue);
                                renderFunction.draw(rectangle);
                                y = linear(forF);
                                rectangle.setFillColor(sf::Color::Red);
                                rectangle.setPosition(i, height/2 - y);
                                renderFunction.draw(rectangle);
                            }
                            renderFunction.display();*/
                            needCenter = true;
                            break;
                        }
                        case Mode::OutZoom: {
                            ForF forF = {charSize, f(params[0]), f(params[1]), timeFromStartPhrase, f(params[1]) - timePhrase};
                            text.setCharacterSize(std::min(linear(forF), (float)charSize));
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
        //shaderText.setUniform("time", (float)time / 1000);

        /*(renderTexture.clear();
        renderTexture.draw(background, &shader);
        renderTexture.draw(text);
        renderTexture.display();

        auto img = renderTexture.getTexture().copyToImage();
        auto number = std::to_string(numberFrame++);
        std::string numberWithZeros = std::string(7 - number.length(), '0') + number;
        auto future1 = std::async(std::launch::async, [&]{ img.saveToFile("video/" + numberWithZeros + ".png"); });*/


        window.clear();

        window.draw(background, &shader);

        const sf::Texture& texture = renderFunction.getTexture();
        sf::Sprite sprite(texture);
        window.draw(sprite);

        window.draw(text);

        window.display();

        //std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 70));

        //time += 1000.0d / 60.0d;
    }
}