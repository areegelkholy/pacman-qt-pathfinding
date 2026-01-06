#include "pacman.h"
#include "level.h"
#include "items/item.h"
#include "items/simpleitem.h"
#include "items/trapitem.h"
#include <QTimer>
#include "constants.h"

#define W Entity::Width


//  initializes Pacman

Pacman::Pacman(Level* lvl, const QString& baseSpritePath)
    : Entity(QPixmap(baseSpritePath + "a3.png").scaled(Entity::Width, Entity::Width)),
    level(lvl),
    animationIndex(0),
    doubleScore(false),
    speedBoost(false),
    shield(false)

{
    // timer to disable speed boost after a set duration
    speedTimer = new QTimer(this);
    connect(speedTimer, &QTimer::timeout, this, &Pacman::deactivateSpeedBoost);

    dir = Stop;
    next_dir = Stop;


    // load directional animations for each ghost

    animations[Right] = {
        QPixmap(baseSpritePath + "a1.png"), QPixmap(baseSpritePath + "a2.png"),
        QPixmap(baseSpritePath + "a3.png"), QPixmap(baseSpritePath + "a4.png"),
        QPixmap(baseSpritePath + "a5.png"), QPixmap(baseSpritePath + "a6.png")
    };

    animations[Left] = {
        QPixmap(baseSpritePath + "c1.png"), QPixmap(baseSpritePath + "c2.png"),
        QPixmap(baseSpritePath + "c3.png"), QPixmap(baseSpritePath + "c4.png"),
        QPixmap(baseSpritePath + "c5.png"), QPixmap(baseSpritePath + "c6.png")
    };

    animations[Up] = {
     QPixmap(baseSpritePath + "b1.png"), QPixmap(baseSpritePath + "b2.png"),
     QPixmap(baseSpritePath + "b3.png"), QPixmap(baseSpritePath + "b4.png"),
     QPixmap(baseSpritePath + "b5.png"), QPixmap(baseSpritePath + "b6.png")
    };

    animations[Down] = {



        QPixmap(baseSpritePath + "d1.png"), QPixmap(baseSpritePath + "d2.png"),
        QPixmap(baseSpritePath + "d3.png"), QPixmap(baseSpritePath + "d4.png"),
        QPixmap(baseSpritePath + "d5.png"), QPixmap(baseSpritePath + "d6.png")
    };

    setPixmap(animations[Right][2]);
}

// movemnet functions
void Pacman::moveUp()    { animateAndMove(Up, 0, -1);   }
void Pacman::moveDown()  { animateAndMove(Down, 0, 1);  }
void Pacman::moveLeft()  { animateAndMove(Left, -1, 0); }
void Pacman::moveRight() { animateAndMove(Right, 1, 0); }

// getter fro current lvele
Level* Pacman::getLevel() const {
    return level;
}


void Pacman::setNextDir(Dir d) {
    next_dir = d;
}


void Pacman::animateAndMove(Dir d, int dx, int dy)
{
    animationIndex = (animationIndex + 1) % animations[d].size();
    setPixmap(animations[d][animationIndex]);

    moveBy(dx , dy);
}

// determines if tile at i,j  walkable
bool Pacman::isWalkable(int i, int j)
{
    if (i < 0 || j < 0 || i >= level->map_height || j >= level->map_width) {
        qDebug() << "Out of bounds at" << i << "," << j;
        return false;
    }

    auto* tile = level->map[i][j];
    if (!tile) {
        qDebug() << "Null tile at" << i << "," << j;
        return false;
    }

    Item::Type t = tile->getType();

    bool walkable = (t != SimpleItem::Wall && t != SimpleItem::Gate);
    return walkable;
}



// handles eating items, calles applyeffect(), and replaces with blank tile after being eatne

void Pacman::eatItem(int y, int x)
{
    Item *item = level->map[y][x];
    if (!item) return;


    if (item->getType() == Item::PowerBall) {
        level->removePowerBall(item);
    }

    item->applyEffect(this);
    level->removeItem(item);


    QPixmap blankPix;
    Item *blank = new SimpleItem(SimpleItem::Blank, blankPix);
    blank->setX(x);
    blank->setY(y);
    blank->setPos(level->geo_x + x * W, level->geo_y + y * W);
    level->addItem(blank);
    level->map[y][x] = blank;

    item->deleteLater();
}




// pacman movement
void Pacman::move()
{
    // gets pixel-based and tile-based positions
    const int px   = static_cast<int>(x());
    const int py   = static_cast<int>(y());
    const int col  = (px - level->geo_x) / W;
    const int row  = (py - level->geo_y) / W;
    const int xRem = (px - level->geo_x) % W;
    const int yRem = (py - level->geo_y) % W;
    const bool aligned = (xRem == 0 && yRem == 0);


    if (aligned) {
        eatItem(row, col);
        _x = col;
        _y = row;

        // win condition: no balls left on the map
        if (level->getBallCount() == 0) {
            level->status = Level::Win;
            emit level->levelWon();
            return;
        }

    }

    // update direction
    if (aligned && next_dir != Stop) {
        if (dir == Stop && isWalkable(_y, _x)) {
            dir = next_dir;
        }
        switch (next_dir) {
        case Up:    if (isWalkable(_y - 1, _x)) dir = Up;    break;
        case Down:  if (isWalkable(_y + 1, _x)) dir = Down;  break;
        case Left:  if (isWalkable(_y, _x - 1)) dir = Left;  break;
        case Right: if (isWalkable(_y, _x + 1)) dir = Right; break;
        default: break;
        }
        next_dir = Stop;
    }

    // perform actual movement in the selected direction
    switch (dir) {
    case Up:
        if (yRem == 0 && !isWalkable(_y - 1, _x))
            dir = Stop;
        else
            moveUp();
        break;

    case Down:
        if (yRem == 0 && !isWalkable(_y + 1, _x))
            dir = Stop;
        else
            moveDown();
        break;

    case Left:
        if (xRem == 0 && !isWalkable(_y, _x - 1))
            dir = Stop;
        else
            moveLeft();
        break;

    case Right:
        if (xRem == 0 && !isWalkable(_y, _x + 1))
            dir = Stop;
        else
            moveRight();
        break;

    case Stop:

        break;
    }


    _x = (static_cast<int>(x()) - level->geo_x) / W;
    _y = (static_cast<int>(y()) - level->geo_y) / W;

}


// reset Pacman to starting position when pacman respawns or wins the levle
void Pacman::resetPosition()
{
    setPos(start_x, start_y);
    dir = Stop;
    next_dir = Stop;
}

// used for power-up: doubles score
void Pacman::activateDoubleScore() { doubleScore = true; }
//increases speed for 10 seconds
void Pacman::activateSpeedBoost() {
    if (speedBoost) return;
    speedBoost = true;

    level->setPacmanSpeed(60);

    speedTimer->start(10000);
}
// end speedboost
void Pacman::deactivateSpeedBoost() {
    speedBoost = false;
    level->setPacmanSpeed(INTERVAL);
}

// makes Pacman immune to damage, sets sheield equal to ttue
void Pacman::activateShield()      { shield = true; }


// Adds score (doubled if power-up is active) and converts  into coins
void Pacman::addScore(int value)
{

    if (doubleScore) value *= 2;


    level->score += value;


    constexpr int COIN_STEP = 30;

    while (level->score - level->lastScoreCoinCheckpoint >= COIN_STEP) {
        level->coins++;
        level->lastScoreCoinCheckpoint += COIN_STEP;
    }
}


// Adds coins directly used by fruit pickups on the map
void Pacman::addCoins(int value)   { level->coins += value; }


//applies a temporary slow effect
void Pacman::applySlowEffect()
{
    level->applySlowEffect(2000);
}


// called when hit by a bomb
void Pacman::takeBombHit()
{
    level->handleBombHit();
}


// getters for tile coordinates and direction
int Pacman::get_x() const { return _x; }
int Pacman::get_y() const { return _y; }
Entity::Dir Pacman::get_dir() const { return dir; }
