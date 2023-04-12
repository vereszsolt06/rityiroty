#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <iostream>
#include <list>
#include <fstream>
#include <sstream>
using namespace sf;

short i, j, k, currAblePlaces = 40, points = 0;

///Kartyak strukturaja
struct ts {
    RectangleShape tile;
    Texture t;
    char type[4];
    char con[4]; //Ora mutato jarasaval megegyezoen hany oldallal van arreb a csatlakozo oldal
    bool seen[4] = { false };
    bool emp = true;
}tiles[16];

std::vector<std::vector<std::vector<bool>>> able; ///Able vector
std::vector<std::vector<ts>> tileMap; ///Map

///Check selected tile
void ableToPlace(ts actT) {
    currAblePlaces = 0;
    for (i = 0; i < 8; i++)
        for (j = 0; j < 5; j++)
            for (k = 0; k < 4; k++)
            {
                if (tileMap[i][std::min(j + 1, 4)].emp && tileMap[i][std::max(j - 1, 0)].emp && tileMap[std::min(i + 1, 7)][j].emp && tileMap[std::max(i - 1, 0)][j].emp)
                    able[i][j][k] = false;
                else if (!tileMap[i][j].emp)
                    able[i][j][k] = false;
                else if ((tileMap[std::max(0, i - 1)][j].type[2] == actT.type[(0 + k) % 4] || tileMap[std::max(0, i - 1)][j].emp) &&
                    (tileMap[std::min(7, i + 1)][j].type[0] == actT.type[(2 + k) % 4] || tileMap[std::min(7, i + 1)][j].emp) &&
                    (tileMap[i][std::max(0, j - 1)].type[3] == actT.type[(1 + k) % 4] || tileMap[i][std::max(0, j - 1)].emp) &&
                    (tileMap[i][std::min(4, j + 1)].type[1] == actT.type[(3 + k) % 4] || tileMap[i][std::min(4, j + 1)].emp))
                    able[i][j][k] = true;
                else able[i][j][k] = false;
                currAblePlaces += able[i][j][k];
            }
}

std::list<std::pair<short, short>> l; ///Assistant List for conTiles
///Extra points for closed road/city
bool conTiles(short x, short y, short side) {
    if (tileMap[x][y].emp)
        return false;
    if (tileMap[x][y].seen[side])
        return true;
    l.push_back(std::make_pair(x, y));
    tileMap[x][y].seen[side] = true;
    switch (side) {
    case 0:
        if (x == 0) {
            if (tileMap[x][y].con[side] != 'v')
                conTiles(x, y, (tileMap[x][y].con[side] + side) % 4);
            return false;
        }
        else if (!tileMap[x - 1][y].seen[2])
            if (!conTiles(x - 1, y, 2)) {
                if (tileMap[x][y].con[side] != 'v')
                    conTiles(x, y, (tileMap[x][y].con[side] + side) % 4);
                return false;
            }
        break;
    case 1:
        if (y == 0) {
            if (tileMap[x][y].con[side] != 'v')
                conTiles(x, y, (tileMap[x][y].con[side] + side) % 4);
            return false;
        }
        else if (!tileMap[x][y - 1].seen[3])
            if (!conTiles(x, y - 1, 3)) {
                if (tileMap[x][y].con[side] != 'v')
                    conTiles(x, y, (tileMap[x][y].con[side] + side) % 4);
                return false;
            }
        break;
    case 2:
        if (x == 7) {
            if (tileMap[x][y].con[side] != 'v')
                conTiles(x, y, (tileMap[x][y].con[side] + side) % 4);
            return false;
        }
        else if (!tileMap[x + 1][y].seen[0])
            if (!conTiles(x + 1, y, 0)) {
                if (tileMap[x][y].con[side] != 'v')
                    conTiles(x, y, (tileMap[x][y].con[side] + side) % 4);
                return false;
            }
        break;
    case 3:
        if (y == 4) {
            if (tileMap[x][y].con[side] != 'v')
                conTiles(x, y, (tileMap[x][y].con[side] + side) % 4);
            return false;
        }
        else if (!tileMap[x][y + 1].seen[1])
            if (!conTiles(x, y + 1, 1)) {
                if (tileMap[x][y].con[side] != 'v')
                    conTiles(x, y, (tileMap[x][y].con[side] + side) % 4);
                return false;
            }
        break;
    }
    if (tileMap[x][y].con[side] != 'v') {
        if (!tileMap[x][y].seen[(tileMap[x][y].con[side] + side) % 4]) {
            tileMap[x][y].seen[(tileMap[x][y].con[side] + side) % 4] = true;
            switch ((tileMap[x][y].con[side] + side) % 4) {
            case 0:
                if (x == 0) return false;
                else if (!tileMap[x - 1][y].seen[2])
                    if (!conTiles(x - 1, y, 2))
                        return false;
                break;
            case 1:
                if (y == 0) return false;
                else if (!tileMap[x][y - 1].seen[3])
                    if (!conTiles(x, y - 1, 3))
                        return false;
                break;
            case 2:
                if (x == 7) return false;
                else if (!tileMap[x + 1][y].seen[0])
                    if (!conTiles(x + 1, y, 0))
                        return false;
                break;
            case 3:
                if (y == 4) return false;
                else if (!tileMap[x][y + 1].seen[1])
                    if (!conTiles(x, y + 1, 1))
                        return false;
                break;
            }
        }
    }
    return true;
}

Font font;
Text tCreate(std::string s, short charS, Color c, float posX, float posY, bool setO) {
    Text t;
    t.setFont(font);
    t.setString(s);
    t.setCharacterSize(charS);
    t.setFillColor(c);
    if(setO) t.setOrigin(t.getGlobalBounds().width / 2, t.getGlobalBounds().height / 2);
    t.setPosition(posX, posY);
    t.setOutlineThickness((float)charS / 18);
    if (c == Color::Black)
        t.setOutlineColor(Color::White);
    else t.setOutlineColor(Color::Black);
    return t;
}
int main()
{
    font.loadFromFile("Barbedor.ttf");
    srand((unsigned)time(NULL));
    int gridSize = 150;
    float gridSizeF = 150.f;
    float dt = 1.f;
    Clock dtClock;
    Vector2i mousePosScreen;
    Vector2i mousePosWindow;
    Vector2f mousePosView;
    Vector2i mousePosGrid;
    Vector2u screenSize;
    bool menuOpened = false;
    bool homeActive = true;
    bool resizeNeeded;
    short placedTiles = 0;
    float zoomF=1.f;
    Image icon;
    std::vector<std::pair<std::string, short>> playerRes;
    icon.loadFromFile("pic/icon2.png");

    ///Home variables
    std::string user;
    View home;
    Text title = tCreate("Carcassonne", 72, Color::White, 640, 108,true), nameT = tCreate("Adja meg a nevét", 42, Color::White, 640, 468,true), nameCont = tCreate("", 42, Color::White, 640,393,false), startText = tCreate("Start", 42, Color::Black, 640, 576,true);
    Texture backgroundT;
    backgroundT.loadFromFile("pic/Carcassonne.jpg");
    RectangleShape background,nameL;
    nameL.setFillColor(Color::Black);
    nameL.setSize(Vector2f(450, 3));
    nameL.setOrigin(225.f, 1.5f);
    nameL.setPosition(640, 440);
    background.setTexture(&backgroundT);
    background.setSize(Vector2f(1280.f, 720.f));
    background.setOrigin(640.f, 360.f);
    background.setPosition(640.f, 360.f);

    ///Menu variables
    std::pair<std::string, std::vector<std::pair<short, short>>> t[3]; ///Resolutions
    std::ifstream be("pic/res.txt");
    for (i = 0; i < 3; i++) {
        be >> t[i].first;
        t[i].second.resize(5);
        for (short j = 0; j < 5; j++)
            be >> t[i].second[j].first >> t[i].second[j].second;
    }
    View menu;
    unsigned short r = 1, f = 0;
    bool fullS = false;
    Text aspT=tCreate("Képarány", 42, Color::White,50,304,false), aspRat = tCreate("16:9", 42, Color::White, 125 + aspT.getGlobalBounds().width, 304, false), resT = tCreate("Felbontás", 42, Color::White, 50, 360, false),
        res = tCreate("1280x720", 42, Color::White, 125 + aspT.getGlobalBounds().width, 360, false), fullScr=tCreate("Teljes képernyös mód",42,Color::Red,50,416,false), 
        cont = tCreate("Folytatás", 42, Color::White, 50, 248, false), esc = tCreate("Kilépés", 42, Color::White, 50, 472, false);
    Texture arrowT;
    arrowT.loadFromFile("pic/arrow.png");
    RectangleShape arrows[4];
    for (i = 0; i < 4; i++) {
        arrows[i].setTexture(&arrowT);
        arrows[i].setSize(Vector2f(32.f, 50.f));
        arrows[i].setOrigin(16.f, 25.f);
        if (i % 2 == 1)
            arrows[i].rotate(180);
    }
    arrows[0].setPosition(100+aspT.getGlobalBounds().width, 330);
    arrows[1].setPosition(150 + aspT.getGlobalBounds().width + aspRat.getGlobalBounds().width, 330);
    arrows[2].setPosition(100+aspT.getGlobalBounds().width, 390);
    arrows[3].setPosition(150 + aspT.getGlobalBounds().width + res.getGlobalBounds().width, 390);

    ///Endscreen variables
    View endS,leaderbS;
    bool endActive = false,leaderbActive=false;
    RectangleShape resultBackgr;
    std::vector<Text> players;
    Text result = tCreate("", 42, Color::White, 640, 252, true), endEsc=tCreate("Kilépés",42,Color::Black,640,507,true), leaderboard = tCreate("Rangsor", 42, Color::Black, 640, 432, true),
         resultT=tCreate("A TOP 10 játékos",42,Color::White,640,180,true),back=tCreate("Vissza", 42, Color::White,640,540,true);
    resultBackgr.setFillColor(Color(255, 255, 255, 192));
    resultBackgr.setSize(Vector2f(640.f, 456.f));
    resultBackgr.setOrigin(320.f, 228.f);
    resultBackgr.setPosition(640.f, 360.f);

    ///Tiles
    std::string idgname;
    std::ifstream imginf("pic/img_inf.txt");
    short actTileNum = rand() % 16;
    short tileCount[16] = { 0 };
    tileCount[actTileNum]++;
    for (i = 0; i < 16; ++i)
    {
        tiles[i].t.loadFromFile("pic/p" + std::to_string(i) + ".png");
        tiles[i].tile.setTexture(&tiles[i].t);
        tiles[i].tile.setSize(Vector2f(gridSizeF, gridSizeF));
        tiles[i].tile.setOrigin(Vector2f(gridSizeF / 2, gridSizeF / 2));
        tiles[i].emp = false;
        tileCount[i] = 0;
        imginf >> tiles[i].type[0] >> tiles[i].con[0] >> tiles[i].type[1] >> tiles[i].con[1] >> tiles[i].type[2] >> tiles[i].con[2] >> tiles[i].type[3] >> tiles[i].con[3];
    }
    imginf.close();

    ///Actual Tile create
    ts actualTile;
    actualTile.tile.setTexture(&tiles[actTileNum].t);
    actualTile.tile.setSize(Vector2f(gridSizeF, gridSizeF));
    actualTile.tile.setOrigin(Vector2f(gridSizeF / 2, gridSizeF / 2));
    actualTile.tile.setOutlineThickness(2.f);
    actualTile.emp = false;
    for (i = 0; i < 4; i++) {
        actualTile.type[i] = tiles[actTileNum].type[i];
        actualTile.con[i] = tiles[actTileNum].con[0];
    }
    ///Able array --> tartalmazza hogy mely helyekre lehet tenni
    able.resize(8, std::vector<std::vector<bool>>(5, std::vector<bool>(4, true)));

    ///Tile Map
    tileMap.resize(8, std::vector<ts>(5));

    for (i = 0; i < 8; i++)
        for (j = 0; j < 5; j++)
        {
            tileMap[i][j].tile.setSize(Vector2f(gridSizeF, gridSizeF));
            tileMap[i][j].tile.setFillColor(Color(0, 0, 0, 176));
            tileMap[i][j].tile.setOutlineThickness(2.f);
            tileMap[i][j].tile.setOutlineColor(Color::White);
            tileMap[i][j].tile.setPosition(i * gridSizeF, j * gridSizeF);
        }

    ///Window create
    RenderWindow app(VideoMode(1280, 720), "Carcassonne", sf::Style::Titlebar | sf::Style::Close);
    View view;
    view.setSize(1280.f, 720.f);
    view.setCenter(app.getSize().x / 2.f, app.getSize().y / 2.f);
    menu = view;
    home = view;
    endS = view;
    leaderbS = view;
    screenSize = Vector2u(VideoMode::getDesktopMode().width, VideoMode::getDesktopMode().height);
    view.zoom(1.1f);
    app.setFramerateLimit(120);
    float viewSpeed = 200.f;
    app.setIcon(192, 192, icon.getPixelsPtr());
    
    while (app.isOpen())
    {
        ///Update dt
        dt = dtClock.restart().asSeconds();
        ///Update MousePos
        app.setView(view);
        mousePosWindow = Mouse::getPosition(app);
        mousePosView = app.mapPixelToCoords(mousePosWindow);
        if (mousePosView.x >= 0.f)
            mousePosGrid.x = int(mousePosView.x / gridSizeF);
        if (mousePosView.y >= 0.f)
            mousePosGrid.y = int(mousePosView.y / gridSizeF);
        app.setView(app.getDefaultView());
        mousePosGrid.x = std::max(std::min(mousePosGrid.x, 7), 0);
        mousePosGrid.y = std::max(std::min(mousePosGrid.y, 4), 0);
        ///Update game elements
        if(!endActive)
            actualTile.tile.setPosition(mousePosGrid.x * gridSizeF + gridSizeF / 2, mousePosGrid.y * gridSizeF + gridSizeF / 2); //x:gridPos.x + tileOrigin | y:gridPos.y + tileOrigin

        ///Update able
        if (able[mousePosGrid.x][mousePosGrid.y][0])
            actualTile.tile.setOutlineColor(Color::Green);
        else if (able[mousePosGrid.x][mousePosGrid.y][1] || able[mousePosGrid.x][mousePosGrid.y][2] || able[mousePosGrid.x][mousePosGrid.y][3])
            actualTile.tile.setOutlineColor(Color::Yellow);
        else actualTile.tile.setOutlineColor(Color::Red);

        Event event;
        while (app.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                app.close();
            if (event.type == Event::KeyReleased && event.key.code == Keyboard::Escape)
                menuOpened = !menuOpened;
            if (menuOpened) {
                if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left) {
                    if (cont.getGlobalBounds().contains(mousePosWindow.x * 1.f, mousePosWindow.y * 1.f))
                        menuOpened = !menuOpened;
                    else if (fullScr.getGlobalBounds().contains(mousePosWindow.x * 1.f, mousePosWindow.y * 1.f))
                    {
                        if (!fullS) {
                            app.create(sf::VideoMode(t[r].second[f].first, t[r].second[f].second), "Carcassonne", sf::Style::Titlebar | sf::Style::Close | Style::Fullscreen);
                            fullScr.setFillColor(Color::Green);
                            fullS = true;
                        }
                        else {
                            app.create(sf::VideoMode(t[r].second[f].first, t[r].second[f].second), "Carcassonne", sf::Style::Titlebar | sf::Style::Close);
                            fullScr.setFillColor(Color::Red);
                            fullS = false;
                        }
                    }
                    else if (esc.getGlobalBounds().contains(mousePosWindow.x * 1.f, mousePosWindow.y * 1.f))
                        return EXIT_SUCCESS;
                    else {
                        resizeNeeded = true;
                        for (i = 0; i < 4; i++)
                            if (arrows[i].getGlobalBounds().contains(mousePosWindow.x * 1.f, mousePosWindow.y * 1.f)) {
                                switch (i) {
                                case 0:
                                    if (r != 0 && unsigned(t[r - 1].second[0].first) <= screenSize.x && unsigned(t[r - 1].second[0].second) <= screenSize.y) {
                                        r--; f = 0;
                                    }
                                    else resizeNeeded = false;
                                    break;
                                case 1:
                                    if (r != 2 && unsigned(t[r + 1].second[0].first) <= screenSize.x && unsigned(t[r + 1].second[0].second) <= screenSize.y) {
                                        r++; f = 0;
                                    }
                                    else resizeNeeded = false;
                                    break;
                                case 2:
                                    if (f != 0 && unsigned(t[r].second[f - 1].first) <= screenSize.x && unsigned(t[r].second[f - 1].second) <= screenSize.y) f--;
                                    else resizeNeeded = false;
                                    break;
                                case 3:
                                    if (f != 4 && unsigned(t[r].second[f + 1].first) <= screenSize.x && unsigned(t[r].second[f + 1].second) <= screenSize.y) f++;
                                    else resizeNeeded = false;
                                    break;
                                }
                                if (resizeNeeded) {
                                    if(fullS)
                                        app.create(sf::VideoMode(t[r].second[f].first, t[r].second[f].second), "Carcassonne", sf::Style::Titlebar | sf::Style::Close | Style::Fullscreen);
                                    else app.create(sf::VideoMode(t[r].second[f].first, t[r].second[f].second), "Carcassonne", sf::Style::Titlebar | sf::Style::Close);
                                    app.setIcon(192, 192, icon.getPixelsPtr());
                                    aspRat.setString(t[r].first);
                                    res.setString(std::to_string(t[r].second[f].first) + "x" + std::to_string(t[r].second[f].second));

                                    cont.setPosition(50, float(t[r].second[f].second / 2 - 112));
                                    aspT.setPosition(50, float(t[r].second[f].second / 2 - 56));
                                    aspRat.setPosition(125 + aspT.getGlobalBounds().width, float(t[r].second[f].second / 2 - 56));
                                    resT.setPosition(50, float(t[r].second[f].second / 2));
                                    res.setPosition(125 + aspT.getGlobalBounds().width, float(t[r].second[f].second / 2));
                                    fullScr.setPosition(50, float(t[r].second[f].second / 2 + 56));
                                    esc.setPosition(50, float(t[r].second[f].second / 2 + 112));

                                    arrows[0].setPosition(100 + aspT.getGlobalBounds().width, float(t[r].second[f].second / 2 - 30));
                                    arrows[1].setPosition(150 + aspT.getGlobalBounds().width + aspRat.getGlobalBounds().width, float(t[r].second[f].second / 2 - 30));
                                    arrows[2].setPosition(100 + aspT.getGlobalBounds().width, float(t[r].second[f].second / 2 + 30));
                                    arrows[3].setPosition(150 + aspT.getGlobalBounds().width + res.getGlobalBounds().width, float(t[r].second[f].second / 2 + 30));
                                    
                                    endS.setSize(t[r].second[f].first, t[r].second[f].second);
                                    endS.setCenter(t[r].second[f].first / 2, t[r].second[f].second / 2);
                                    leaderbS = endS;
                                    for (i = 0; i < players.size();i++)
                                        players[i].setPosition(t[r].second[f].first / 2, t[r].second[f].second * 0.32 + i*30);

                                    resultBackgr.setSize(Vector2f(float(std::min(std::max(512, t[r].second[f].first / 2), 960)), float(std::max(456, int(t[r].second[f].second*0.6)))));
                                    resultBackgr.setOrigin(resultBackgr.getSize().x / 2, resultBackgr.getSize().y / 2);
                                    resultBackgr.setPosition(t[r].second[f].first / 2, t[r].second[f].second / 2);
                                    result.setPosition(t[r].second[f].first / 2, t[r].second[f].second * 0.35);
                                    resultT.setPosition(t[r].second[f].first / 2, t[r].second[f].second * 0.25);
                                    back.setPosition(t[r].second[f].first / 2, t[r].second[f].second * 0.75);
                                    leaderboard.setPosition(t[r].second[f].first / 2, t[r].second[f].second * 0.6);
                                    endEsc.setPosition(t[r].second[f].first / 2, t[r].second[f].second * 0.6 + 75);

                                    if (homeActive) {
                                        home.setSize(t[r].second[f].first, t[r].second[f].second);
                                        home.setCenter(t[r].second[f].first / 2, t[r].second[f].second / 2);

                                        title.setPosition(t[r].second[f].first / 2, t[r].second[f].second * 0.15f);
                                        nameT.setPosition(t[r].second[f].first / 2, t[r].second[f].second * 0.65f);
                                        nameCont.setPosition((t[r].second[f].first - nameCont.getGlobalBounds().width) / 2, t[r].second[f].second * 0.65f - 75);
                                        nameL.setPosition(t[r].second[f].first / 2, t[r].second[f].second * 0.65f - 25);
                                        startText.setPosition(t[r].second[f].first / 2, t[r].second[f].second * 0.8f);
                                    }
                                    else {
                                        view.setSize(t[r].second[f].first, t[r].second[f].second);
                                        view.setCenter(app.getSize().x / 2.f, app.getSize().y / 2.f);
                                    }
                                    background.setSize(Vector2f(std::max(t[r].second[f].first * 1.f, t[r].second[f].second / 0.5625f), std::max(t[r].second[f].first * 0.5625f, t[r].second[f].second * 1.f)));
                                    menu.setSize(t[r].second[f].first, t[r].second[f].second);
                                    menu.setCenter(app.getSize().x / 2.f, app.getSize().y / 2.f);
                                }
                            }
                    }
                }
            }
            else if (homeActive) {
                if (event.key.code == Keyboard::Backspace && event.type == event.KeyReleased) {
                    user = user.substr(0, user.length() - 1);
                    nameCont.setString(user);
                    nameCont.setPosition((t[r].second[f].first - nameCont.getGlobalBounds().width) / 2, t[r].second[f].second * 0.65f - 75);
                }
                else if (event.type == event.TextEntered && user.length() <= 16) {
                    if (((event.text.unicode <= 90 && event.text.unicode >= 65) || (event.text.unicode <= 122 && event.text.unicode >= 97)) ||
                        (event.text.unicode == 193 || event.text.unicode == 201 || event.text.unicode == 205 || event.text.unicode == 211 ||
                            event.text.unicode == 214 || event.text.unicode == 218 || event.text.unicode == 225 || event.text.unicode == 233 ||
                            event.text.unicode == 237 || event.text.unicode == 243 || event.text.unicode == 246 || event.text.unicode == 250 ||
                            event.text.unicode == 252)) {
                        user.push_back(event.text.unicode);
                        nameCont.setString(user);
                        nameCont.setPosition((t[r].second[f].first - nameCont.getGlobalBounds().width) / 2, t[r].second[f].second * 0.65f - 75);
                    }
                }
                else if (event.type == Event::MouseButtonReleased && event.mouseButton.button==Mouse::Left && startText.getGlobalBounds().contains(mousePosWindow.x * 1.f, mousePosWindow.y * 1.f) && user.length() != 0)
                    homeActive = false;
            }
            else if (endActive){
                
                if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left)
                {
                    if (!leaderbActive) {
                        if (endEsc.getGlobalBounds().contains(mousePosWindow.x * 1.f, mousePosWindow.y * 1.f))
                            return EXIT_SUCCESS;
                        else if (leaderboard.getGlobalBounds().contains(mousePosWindow.x * 1.f, mousePosWindow.y * 1.f))
                            leaderbActive = true;
                    }
                    else {
                        if (back.getGlobalBounds().contains(mousePosWindow.x * 1.f, mousePosWindow.y * 1.f))
                            leaderbActive = false;
                    }
                }
            }
            else {
                if (event.type == Event::MouseWheelScrolled) {
                    if (event.mouseWheelScroll.delta == 1) zoomF = 0.75;
                    else zoomF =1.3334f;
                    view.zoom(zoomF);
                }
                if (event.type == Event::MouseButtonReleased && !endActive) {
                    if (event.mouseButton.button == Mouse::Left && tileMap[mousePosGrid.x][mousePosGrid.y].emp == true && able[mousePosGrid.x][mousePosGrid.y][0] && tileMap[mousePosGrid.x][mousePosGrid.y].tile.getGlobalBounds().contains(mousePosView.x,mousePosView.y))
                    {
                        actualTile.tile.setOutlineColor(Color::Transparent);
                        tileMap[mousePosGrid.x][mousePosGrid.y] = actualTile;
                        tileCount[actTileNum]++;
                        do {
                            actTileNum = rand() % 16;
                        } while (tileCount[actTileNum] == 5);
                        actualTile = tiles[actTileNum];            
                        actualTile.tile.setPosition(mousePosGrid.x* gridSizeF + gridSizeF / 2, mousePosGrid.y* gridSizeF + gridSizeF / 2); //x:gridPos.x + tileOrigin | y:gridPos.y + tileOrigin
                        actualTile.tile.setOutlineThickness(2.f);
                        placedTiles++;

                        ///Uj kartya kerul a kezbe, ezert ujra generaljuk hogy hova teheto kartya
                        ableToPlace(actualTile);
                    }
                    if (event.mouseButton.button == Mouse::Right)
                    {
                        actualTile.tile.rotate(90);
                        std::swap(actualTile.type[0], actualTile.type[3]);
                        std::swap(actualTile.type[2], actualTile.type[3]);
                        std::swap(actualTile.type[2], actualTile.type[1]);
                        std::swap(actualTile.con[0], actualTile.con[3]);
                        std::swap(actualTile.con[2], actualTile.con[3]);
                        std::swap(actualTile.con[2], actualTile.con[1]);

                        ///Mas iranyba nez a kartya, ezert ujra generaljuk hogy hova teheto a kartya
                        if (placedTiles != 0 && currAblePlaces != -1)
                            ableToPlace(actualTile);
                    }
                }
                if (currAblePlaces == 0) {
                    for (i = 0; i < 8; i++)
                        for (j = 0; j < 5; j++)
                            for (k = 0; k < 4; k++)
                                switch (tileMap[i][j].type[k]) {
                                    ///Roads
                                    case '1':
                                        if (!tileMap[i][j].seen[k]) {
                                            l = {};
                                            if (conTiles(i, j, k))
                                                points += 2;
                                            l.sort();
                                            l.unique();
                                            points += short(l.size());
                                        }
                                    break;
                                    ///Cities
                                    case '2':
                                        if (!tileMap[i][j].seen[k]) {
                                            l = {};
                                            if (conTiles(i, j, k))
                                                points += 5;
                                            l.sort();
                                            l.unique();
                                            points += short(l.size() * 2);
                                        }
                                    break;
                                    ///Monasteries
                                    case '0':
                                        if (tileMap[i][j].con[k] == 'k')
                                            for (short x = -1; x < 2; x++)
                                                for (short y = -1; y < 2; y++)
                                                    if (0 <= (i + x) && (i + x) <= 7 && 0 <= (j + y) && (j + y) <= 4 && !(x==0 && y==0))
                                                        points += !tileMap[i + x][j + y].emp;
                                    break;
                                }
                    if (placedTiles == 40)
                        points += 10;
                    currAblePlaces = -1;
                    std::pair<std::string,short> idg;
                    std::ifstream pp("playerRes.txt");
                    while(pp>>idg.first>>idg.second)
                        playerRes.push_back(idg);
                    pp.close();
                    playerRes.push_back(std::make_pair(user, points));
                    for(j=playerRes.size()-1;j>0;j--)
                        for (i = 0; i < j; i++) 
                            if (playerRes[i].second < playerRes[i + 1].second)
                                swap(playerRes[i], playerRes[i + 1]);
                    std::ofstream outpp("playerRes.txt");
                    for (i = 0; i < playerRes.size(); i++) {
                        outpp << playerRes[i].first << " " << playerRes[i].second << '\n';
                        players.push_back(tCreate(std::to_string(i + 1) + ". " + playerRes[i].first + " - " + std::to_string(playerRes[i].second) + " pont", 28, Color::White, t[r].second[f].first / 2, t[r].second[f].second * 0.32 + i * 30, true));
                    }
                    outpp.close();
                    endActive = true;
                    endS.setSize(t[r].second[f].first, t[r].second[f].second);
                    endS.setCenter(t[r].second[f].first / 2, t[r].second[f].second/2);
                    actualTile.tile.setPosition(mousePosGrid.x* gridSizeF + gridSizeF / 2, mousePosGrid.y* gridSizeF + gridSizeF / 2); //x:gridPos.x + tileOrigin | y:gridPos.y + tileOrigin
                    result.setString(std::to_string(points) + " pontot ért el.");
                    result.setOrigin(result.getGlobalBounds().width/2,result.getGlobalBounds().height/2);
                    result.setPosition(t[r].second[f].first / 2, t[r].second[f].second * 0.35);
                }
            }
        }


        ///Clear screen
        app.clear();

        app.draw(background);
        if (!menuOpened) {
            if (homeActive) {
                ///Render Start page
                app.setView(home);
                app.draw(startText);
                app.draw(title);
                app.draw(nameCont);
                app.draw(nameT);
                app.draw(nameL);
            }
            else {
                ///Update View Pos
                if (Keyboard::isKeyPressed(Keyboard::A))
                    view.move(-viewSpeed * dt, 0.f);
                if (Keyboard::isKeyPressed(Keyboard::D))
                    view.move(viewSpeed * dt, 0.f);
                if (Keyboard::isKeyPressed(Keyboard::W))
                    view.move(0.f, -viewSpeed * dt);
                if (Keyboard::isKeyPressed(Keyboard::S))
                    view.move(0.f, viewSpeed * dt);

                ///Render game elements
                app.setView(view);
                for (i = 0; i < 8; i++)
                    for (j = 0; j < 5; j++)
                        app.draw(tileMap[i][j].tile);
                app.draw(actualTile.tile);
                ///Render End screen
                if (endActive) {
                    if (leaderbActive) {
                        app.setView(leaderbS);
                        app.draw(resultBackgr);
                        app.draw(resultT);
                        for (i=0;i<std::min(int(players.size()),10);i++)
                            app.draw(players[i]);
                        app.draw(back);
                    }
                    else {
                        app.setView(endS);
                        app.draw(resultBackgr);
                        app.draw(result);
                        app.draw(leaderboard);
                        app.draw(endEsc);
                    }
                }
            }
        }
        else {
            ///Render Menu
            app.setView(menu);
            for (i = 0; i < 4; i++)
                app.draw(arrows[i]);
            app.draw(cont);
            app.draw(aspT);
            app.draw(aspRat);
            app.draw(resT);
            app.draw(res);
            app.draw(fullScr);
            app.draw(esc);
        }
        ///Update the window
        app.display();
    }

    return EXIT_SUCCESS;
}
