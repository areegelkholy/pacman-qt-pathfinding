#include "level.h"
#include "items/simpleitem.h"
#include "items/trapitem.h"
#include "chasing_strategy.h"

#include "constants.h"
#include "entities/thief.h"
#include "entities/blade.h"



#include <QFile>
extern QString chosenPacmanSpritePath;

#include <QRandomGenerator>



#define W 20 // our tile size in pixels


// ghost release times in millisecondsss
int GHOST_RELEASE_TIME[] = {0, 20, 40, 60};



// initizalizing the level, including map, entities, and timers
Level::Level(int x, int y, int mapW, int mapH, const QString& mapPath, const QString& name)
    : QGraphicsScene(x, y, W * mapW, W * mapH),
    geo_x(x), geo_y(y), map_width(mapW), map_height(mapH), levelName(name)
{
    score = 0;
    coins = 0;
    lives = 50;
    ballCount = 0;
    flashTick = false;
    lastScoreCoinCheckpoint = 0;
    doubleScoreActive = false;
    shieldActive = false;
    speedBoostActive = false;
    status = Playing;

    // map grid allocation
    map = new Item**[map_height];
    for (int i = 0; i < map_height; ++i) {
        map[i] = new Item*[map_width];
        for (int j = 0; j < map_width; ++j)
            map[i][j] = nullptr;
    }

    loadMap(mapPath);

    // initialize and connect timers for pacman, powerballs, and ghosts
    pacmanTimer = new QTimer(this);
    connect(pacmanTimer, &QTimer::timeout, this, &Level::updatePacman);


    powerballFlashTimer = new QTimer(this);
    connect(powerballFlashTimer, &QTimer::timeout, this, &Level::flashPowerballs);

    for (int i = 0; i < Ghost::GhostNum; ++i) {
        ghostTimers[i] = new QTimer(this);
    }

    // special case for level 5,  spawning blades every 10 seconds
    if (levelName.contains("level5")) {
        bladeSpawner = new QTimer(this);
        connect(bladeSpawner, &QTimer::timeout, this, &Level::spawnBlade);
        bladeSpawner->start(10 * 1000);

    }

}


// remvoe powerball (after pacman eats)
void Level::removePowerBall(Item* item)
{
    int idx = powerballs.indexOf(item);
    if (idx != -1) {
        powerballs.remove(idx);}
}


// loads map from a .txt file into the scene
void Level::loadMap(const QString& path)
{

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open map file!";
        return;
    }


    // loading icons for all types of items used in the game

    QPixmap wallPix(":/game_objects/map_objects/wall.png");

    wallPix = wallPix.scaled(W, W, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QPixmap ballPix(":/game_objects/map_objects/dot.png");
    QPixmap powerBallPix(":/game_objects/map_objects/power_ball.png");
    QPixmap gatePix(":/game_objects/map_objects/gate.png");
    QPixmap blankPix;


    QPixmap slowPix(":/game_objects/traps/slow_trap.png");
    slowPix = slowPix.scaled(Entity::Width, Entity::Width);

    QPixmap bombPix(":/game_objects/traps/bomb_trap.png");
    bombPix = bombPix.scaled(Entity::Width, Entity::Width);

    QPixmap strawberryPix(":/game_objects/traps/strawberry.png");
    strawberryPix = strawberryPix.scaled(Entity::Width, Entity::Width);

    QPixmap grapePix(":/game_objects/traps/grape.png");
    grapePix = grapePix.scaled(Entity::Width, Entity::Width);


    int ghostIndex = 0;

    // parsing the map line by line
    for (int i = 0; i < map_height; ++i) {
        QByteArray line = file.readLine();
        originalMap.append(line);
        for (int j = 0; j < map_width; ++j) {
            int px = geo_x + j * W;
            int py = geo_y + i * W;

            Item* item = nullptr;

            switch (line[j]) {
            case '1':
                item = new SimpleItem(SimpleItem::Wall, wallPix);
                break;
            case '0':
                item = new SimpleItem(SimpleItem::Ball, ballPix, BALL_SCORE, this);

                ballCount++;
                break;
            case '4':
                item = new SimpleItem(SimpleItem::PowerBall, powerBallPix, POWERBALL_SCORE, this);
                powerballs.push_back(item);
                ballCount++;
                break;
            case '2':
                item = new SimpleItem(SimpleItem::Gate, gatePix);
                gateRow = i;
                gateCol = j;
                break;

            case 'T':
                item = new TrapItem(TrapItem::SlowTrap, slowPix);
                break;
            case 'B':
                item = new TrapItem(TrapItem::BombTrap, bombPix);
                break;
            case 'p': {
                qDebug() << "🛠️ Level creating Pacman with sprite path:" << chosenPacmanSpritePath;

                pacman = new Pacman(this, chosenPacmanSpritePath); // NEW


                pacman->setX(j);
                pacman->setY(i);

                pacman->setPos(px, py);
                pacman->start_x = px;
                pacman->start_y = py;
                pacman->setZValue(2);
                addItem(pacman);

                item = new SimpleItem(SimpleItem::Blank, blankPix);
                break;
            }

                case 'g': {

                    ghost[ghostIndex] =
                        new Ghost(static_cast<Ghost::Color>(ghostIndex), this);
                    ghosts.append(ghost[ghostIndex]);



                    ghostTimers[ghostIndex] = new QTimer(this);


                    connect(ghostTimers[ghostIndex], &QTimer::timeout, this, [=]() {
                        ghost[ghostIndex]->move();
                    });



                    static const int REL[] = { 0, 200, 400, 600 };
                    ghost[ghostIndex]->setReleaseTime(REL[ghostIndex]);
                    if (REL[ghostIndex] == 0)
                        ghost[ghostIndex]->setReleased(true);

                    ghost[ghostIndex]->setX(j);
                    ghost[ghostIndex]->setY(i);
                    ghost[ghostIndex]->setScore(GHOST_SCORE);
                    ghost[ghostIndex]->setPos(px, py);
                    ghost[ghostIndex]->setZValue(2);
                    addItem(ghost[ghostIndex]);


                    ghostTimers[ghostIndex]->start(NORMAL_INTERVAL);
                    ++ghostIndex;
                    break;
                }


            case 'S': {
                item = new SimpleItem(SimpleItem::Strawberry, strawberryPix);
                break;
            }
            case 'G': {
                item = new SimpleItem(SimpleItem::Grape, grapePix);
                break;
            }

            default:
                item = new SimpleItem(SimpleItem::Blank, blankPix);
                break;
            }

            // adding item to map and scene
            if (item) {
                item->setX(j);
                item->setY(i);
                item->setPos(px, py);
                map[i][j] = item;
                addItem(item);
            }
        }
    }

    // Assigning chasing strategies to ghosts
    ghost[Ghost::Red]   ->setStrategy(strategy1);
    ghost[Ghost::Yellow]->setStrategy(strategy2);
    ghost[Ghost::Pink]  ->setStrategy(strategy3);
    ghost[Ghost::Green] ->setStrategy(strategy4);

    qDebug() << "BallCount" << ballCount ;
}



// starts all game timers
void Level::start()
{
    pacman->resetPosition();
    pacmanTimer->start(INTERVAL);

    powerballFlashTimer->start(FLASH_INTERVAL);
    for (int i = 0; i < Ghost::GhostNum; ++i)
        ghostTimers[i]->start(NORMAL_INTERVAL);
    status = Playing;
}

// activates panic mode on all ghosts, hapens when Pacman eats a powerball
void Level::activatePanicMode() {
    for (Ghost* ghost : ghosts) {
        if (ghost) {
            ghost->enterPanicMode(PANIC_DURATION);
        }
    }
}



// decreases player lives and ends the game if no lives are left
void Level::decrementLives() {
    if (lives > 0) lives--;
    if (lives <= 0) {
        lives = 0;
        status = Lose;
        stop();
    }
}

void Level::pause()
{
    pacmanTimer->stop();
    powerballFlashTimer->stop();
    for (int i = 0; i < Ghost::GhostNum; ++i)
        ghostTimers[i]->stop();
    status = Paused;
}


// resumes all game timers and sets status to Playing
void Level::resume()
{
    pacmanTimer->start(INTERVAL);
    powerballFlashTimer->start(FLASH_INTERVAL);
    for (int i = 0; i < Ghost::GhostNum; ++i)
        ghostTimers[i]->start(NORMAL_INTERVAL);
    status = Playing;
}


// stops all game activity and signals game lost if player is out of lives
void Level::stop()
{
    pause();
    if (status == Lose)
        emit gameLost();
}



// moves Pacman and triggers thief spawning logic based on coins and level
void Level::updatePacman()
{
    pacman->move();


    // Spawn thief at specific coin thresholds and levels
    if (!thiefSpawned && coins >= 750 && levelName.contains("level3") ) {
        thief = new Thief(this);


        thief->setX(gateCol);
        thief->setY(gateRow);
        thief->setPos(geo_x + gateCol * Entity::Width, geo_y + gateRow * Entity::Width);

        addItem(thief);
        thief->startChasing();

        thiefSpawned = true;

        QTimer::singleShot(20000, this, [this]() {
            if (thief) {
                removeItem(thief);
                delete thief;
                thief = nullptr;
            }
        });
    }


    if (!thiefSpawned && coins >= 1000 && levelName.contains("level4") ) {
        thief = new Thief(this);


        thief->setX(gateCol);
        thief->setY(gateRow);
        thief->setPos(geo_x + gateCol * Entity::Width, geo_y + gateRow * Entity::Width);

        addItem(thief);
        thief->startChasing();

        thiefSpawned = true;

        // Remove thief after 20 seconds
        QTimer::singleShot(20000, this, [this]() {
            if (thief) {
                removeItem(thief);
                delete thief;
                thief = nullptr;
            }
        });
    }

    if (!thiefSpawned && coins >= 1300 && levelName.contains("level5") ) {
        thief = new Thief(this);


        thief->setX(gateCol);
        thief->setY(gateRow);
        thief->setPos(geo_x + gateCol * Entity::Width, geo_y + gateRow * Entity::Width);

        addItem(thief);
        thief->startChasing();

        thiefSpawned = true;

        QTimer::singleShot(20000, this, [this]() {
            if (thief) {
                removeItem(thief);
                delete thief;
                thief = nullptr;
            }
        });
    }
}


// updating a specific ghost's movement
void Level::updateGhost(int id)
{
    ghost[id]->move();
}


// Stops all timers forPacman, ghosts, powerballs
void Level::stopAllTimers()
{
    pacmanTimer->stop();
    powerballFlashTimer->stop();
    for (int i = 0; i < Ghost::GhostNum; ++i)
        ghostTimers[i]->stop();
}



//  show message  when you pass the lvel
void Level::showFloatingMessage(const QString& text) {
    QGraphicsTextItem* msg = addText(text, QFont("Arial", 20, QFont::Bold));
    msg->setDefaultTextColor(Qt::yellow);
    msg->setZValue(10);
    msg->setPos(geo_x + 200, geo_y + 50);

    QTimer::singleShot(2000, [=]() {
        removeItem(msg);
        delete msg;
    });
}



// Makes powerballs flash

void Level::flashPowerballs()
{
    flashTick = !flashTick;


    for (int i = 0; i < powerballs.size(); ) {
        Item* ptr = powerballs[i];
        if (!ptr || ptr->scene() == nullptr) {
            powerballs.remove(i);
        } else {
            ptr->setVisible(!flashTick);
            ++i;
        }
    }
}



// Temporarily slows Pacman for a duration in milliseconds
void Level::applySlowEffect(int durationMs)
{
    pacmanTimer->setInterval(20);
    QTimer::singleShot(durationMs, [this]() {
        pacmanTimer->setInterval(INTERVAL);
    });
}


//  logic when Pacman hits a bomb
void Level::handleBombHit()
{
    if (!shieldActive) {
        lives--;
        if (lives <= 0) {
            setStatus(Level::Lose);
            emit gameLost();
        } else {
            pacman->resetPosition();
        }
    }

    emit bombHitTriggered(); //  Trigger signal for tje shake effect

}


// passes the next direction to Pacman from key press
void Level::setNextDirection(Entity::Dir d) {
    if (pacman) {
        pacman->setNextDir(d);
    }
}

// getters
int Level::getScore() const { return score; }
int Level::getCoins() const { return coins; }
int Level::getLives() const { return lives; }
void Level::setCoins(int c) { coins = c; }
int Level::getBallCount()  { return ballCount; }
void Level::setBallCount(int count) {
    this->ballCount = count;
}



// activates shield mode, Pacman is shilded from everyhtign except blade for 10 seconds
void Level::activateShield() {
    shieldActive = true;

    QTimer::singleShot(10000, this, [this]() {
        shieldActive = false;
        qDebug() << "Shield deactivated";
    });

    qDebug() << "Shield activated";
}



// gets current game status Playing, Lose etc
Level::LevelStatus Level::getStatus() const { return status; }

//  override Pacman’s speed interval for speed boost powerup
void Level::setPacmanSpeed(int interval) {
    pacmanTimer->setInterval(interval);
}


// increase number of lives for fruit icon
void Level::increaseLives(int n) {
    if (n <= 0) return;
    lives += n;
    qDebug() << "Lives increased to:" << lives;
}



// update onscreen text showing current lives
void Level::updateLivesDisplay() {
    if (livesLabel) {
        livesLabel->setText(QString("Lives: %1").arg(lives));
    }
}

// destructor to cclean up memory and timers

Level::~Level()
{
    for (int i = 0; i < map_height; ++i) {
        for (int j = 0; j < map_width; ++j)
            delete map[i][j];
        delete[] map[i];
    }
    delete[] map;

    delete pacmanTimer;
    delete powerballFlashTimer;
    for (int i = 0; i < Ghost::GhostNum; ++i)
        delete ghostTimers[i];
}




// blade trap spawn logic that s used in Level 5
void Level::spawnBlade()
{
    const int bladeW = Entity::Width;
    qreal maxX = sceneRect().width() - bladeW;
    qreal startX = QRandomGenerator::global()->bounded(maxX);

    Blade* blade = new Blade(this, startX);
    addItem(blade);
}
