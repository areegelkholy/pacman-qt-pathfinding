
#include "ghost.h"
#include "level.h"
#include "constants.h"
#include "items/simpleitem.h"
#include "chasing_strategy.h"
#include <QRandomGenerator>

#define W Entity::Width


// checks if a ghost is still inside the cage
inline bool Ghost::insideCage(int r) const
{
    return r > level->gateRow;
}


Ghost::Ghost(const QPixmap& pixmap)
    : Entity(pixmap)
{

}

//  ghost constructor

Ghost::Ghost(Color c, Level* l)
    : Entity(QPixmap()), color(c), level(l)
{
    status       = Normal;
    panicTime    = 0;
    releaseTime  = 0;
    released     = false;
    outside      = false;
    animIndex    = 0;
    score        = GHOST_SCORE;
    chaseStrategy = nullptr;
    dir          = Stop;

    //  ghost animations based on color
    const char* name[4] = {"red","yellow","pink","green"};
    QString p = name[c];
    anim[Right] << QPixmap(":/game_objects/ghosts/"+p+"right1.png")
                << QPixmap(":/game_objects/ghosts/"+p+"right2.png");
    anim[Left]  << QPixmap(":/game_objects/ghosts/"+p+"left1.png")
               << QPixmap(":/game_objects/ghosts/"+p+"left2.png");
    anim[Up]    << QPixmap(":/game_objects/ghosts/"+p+"up1.png")
             << QPixmap(":/game_objects/ghosts/"+p+"up2.png");
    anim[Down]  << QPixmap(":/game_objects/ghosts/"+p+"down1.png")
               << QPixmap(":/game_objects/ghosts/"+p+"down2.png");


    // Panic and running modes

    panicAnim   << QPixmap(":/game_objects/ghosts/bl1.png")
              << QPixmap(":/game_objects/ghosts/bl2.png");
    runAnim     << QPixmap(":/game_objects/ghosts/run.png");

    setPixmap(anim[Right][0]);
}


// checks if another ghost is occupying the tile to avoid overlaps

inline bool Ghost::occupiedByGhost(int r, int c) const {

    if (insideCage(r)) return false;

    for (int g = 0; g < Ghost::GhostNum; ++g) {
        const Ghost *gh = level->ghost[g];
        if (!gh || gh == this) continue;
        if (gh->status == Running) continue;
        if (gh->_y == r && gh->_x == c) return true;
    }
    return false;
}


// determines if a tile is walkable ( walls and gates)

inline bool Ghost::walkable(int r, int c) const
{
    if (occupiedByGhost(r, c)) return false;

    if (!level || r < 0 || c < 0 || r >= level->map_height || c >= level->map_width)
        return false;

    auto* s = dynamic_cast<SimpleItem*>(level->map[r][c]);
    if (!s) return true;

    switch (s->getType()) {
    case SimpleItem::Wall:  return false;
    case SimpleItem::Gate:
        return (status == Running) || status == Panic || !outside;
    default:
        return true;
    }
}


// allows passing through items even if not walkable, used for ghost return to cage logic

inline bool Ghost::overlapable(int r, int c) const
{
    if (!level || r < 0 || c < 0 || r >= level->map_height || c >= level->map_width)
        return false;

    auto* s = dynamic_cast<SimpleItem*>(level->map[r][c]);
    if (!s) return true;

    switch (s->getType()) {
    case SimpleItem::Wall:  return false;
    case SimpleItem::Gate:  return (status == Running);
    default:                return true;
    }
}


// handles ghost sprite animation and movement on screen
void Ghost::animate(int dx,int dy)
{
    QVector<QPixmap>* f =
        (status == Panic)   ? &panicAnim :
            (status == Running) ? &runAnim   :
            &anim[dir];

    animIndex = (animIndex + 1) % f->size();
    setPixmap((*f)[animIndex]);

    moveBy(dx, dy);
}

// activates panic mode, slows ghost and changes appearance when in panic mode

void Ghost::enterPanicMode(int duration) {
    if (status != Running) {
        status = Panic;
        panicTime = duration;
        setPixmap(QPixmap(":/game_objects/ghosts/bl1.png"));
        level->getGhostTimer(color)->setInterval(PANIC_INTERVAL);
    }
}


// main ghost behavior function

void Ghost::move()
{
    if (!level) return;


    // dellay ghost release from cage
    if (!released) {
        if (--releaseTime > 0) return;
        released = true;
        dir = Up;
    }


    // panic timeout check
    if (status == Panic && --panicTime == 0) {
        status = Normal;
        level->getGhostTimer(color)->setInterval(NORMAL_INTERVAL);
    }

    // to get current pixel and tile based positions

    int gx = int(x());
    int gy = int(y());
    int col = (gx - level->getGeoX()) / W;
    int row = (gy - level->getGeoY()) / W;
    int rx = (gx - level->getGeoX()) % W;
    int ry = (gy - level->getGeoY()) % W;

    if (rx == 0 && ry == 0) {
        _x = col;
        _y = row;

        if (!outside) {
            if (insideCage(row)) dir = Up;
            else outside = true;
        } else if (status == Running) {
            returnToCage(); // Returning to cage after being eaten
        } else if (status == Panic) {
            dodgePacman();// Randomly avoid Pacman
        } else {
            chasePacman(); //  chase Pacman
        }
    }

    // calculate direction
    int dx = (dir == Right) - (dir == Left);
    int dy = (dir == Down)  - (dir == Up);



    // avoid bumping into another ghost
    if (rx == 0 && ry == 0 && status != Running && !insideCage(_y)) {
        int nextX = _x + dx;
        int nextY = _y + dy;
        for (int i = 0; i < Ghost::GhostNum; ++i) {
            Ghost* other = level->ghost[i];
            if (!other || other == this || insideCage(other->_y)) continue;
            if (other->_x == nextX && other->_y == nextY) {
                // If the next tile is occupied, recalculate direction
                if (status == Panic)
                    dodgePacman();
                else
                    chasePacman();

                dx = (dir == Right) - (dir == Left);
                dy = (dir == Down)  - (dir == Up);
                nextX = _x + dx;
                nextY = _y + dy;

                // If still not valid, don't move this frame
                if (occupiedByGhost(nextY, nextX) || !walkable(nextY, nextX))
                    return;
                break;
            }
        }
    }


    // if the next tile is nort walkable find another route or path

    if (!walkable(_y + dy, _x + dx)) {
        if (status == Panic)        dodgePacman();
        else if (status != Running) chasePacman();

        dx = (dir == Right) - (dir == Left);
        dy = (dir == Down)  - (dir == Up);
        if (!walkable(_y + dy, _x + dx)) return;
    }

    animate(dx, dy);

    // Hit cooldown countdown
    if (level->hitCooldown > 0)
        --level->hitCooldown;

    // Check for collision with Pacman
    if (collidesWithItem(level->pacman, Qt::IntersectsItemShape)) {
        if (status == Normal && level->hitCooldown == 0) {
            // if Pacman has shield just trigger cooldown
            if (level->isShieldActive()) {
                level->hitCooldown = HIT_COOLDOWN_FRAMES;
            } else {
                // damage Pacman
                level->decrementLives();
                level->hitCooldown = HIT_COOLDOWN_FRAMES;
                if (level->getLives() <= 0) {
                    level->setStatus(Level::Lose);
                    emit level->gameLost();
                    level->stop();
                } else {
                    level->respawnEntities();
                }
            }
        } else if (status == Panic) {
            // Pacman  can eatt the gGhost
            setPixmap(QPixmap(":/game_objects/ghosts/run.png"));
            level->increaseScore(score);
            status = Running;
            releaseTime = 0;
            released = true;
            outside = true;
            level->getGhostTimer(color)->setInterval(RUNNING_INTERVAL);


            // updating tile coordinates
            int gx = static_cast<int>(x());
            int gy = static_cast<int>(y());
            _x = (gx - level->getGeoX()) / W;
            _y = (gy - level->getGeoY()) / W;

            qDebug() << "Ghost" << color << "was eaten → now Running from tile" << _x << _y;
        }

    }
}




// Breadth first chase strategy to move toward Pacman and chase

void Ghost::chasePacman()
{
    if (!level || !level->pacman) return;

    auto [dxTile, dyTile] = bfsNextStep(this, level->pacman->get_x(), level->pacman->get_y());
    if (dxTile == 0 && dyTile == 0) return;

    Entity::Dir proposedDir;
    if (std::abs(dxTile) > std::abs(dyTile))
        proposedDir = (dxTile > 0 ? Right : Left);
    else
        proposedDir = (dyTile > 0 ? Down : Up);

    int newX = _x + (proposedDir == Right) - (proposedDir == Left);
    int newY = _y + (proposedDir == Down) - (proposedDir == Up);

    // avoid overlap outside the cage
    if (!occupiedByGhost(newY, newX)) {
        dir = proposedDir;
    } else {
        // fallback: try any valid unoccupied direction
        QVector<Dir> options = {Up, Down, Left, Right};
        for (Dir d : options) {
            int tx = _x + (d == Right) - (d == Left);
            int ty = _y + (d == Down) - (d == Up);
            if (walkable(ty, tx) && !occupiedByGhost(ty, tx)) {
                dir = d;
                return;
            }
        }
        dir = Stop; // No good option found
    }
}

// in panic mode ghost chooses direction opposite of Pacman
void Ghost::dodgePacman()
{
    int dx = level->pacman->get_x() - _x;
    int dy = level->pacman->get_y() - _y;
    Dir proposedDir = (std::abs(dx) > std::abs(dy)) ? (dx > 0 ? Left : Right)
                                                    : (dy > 0 ? Up : Down);

    int newX = _x + (proposedDir == Right) - (proposedDir == Left);
    int newY = _y + (proposedDir == Down) - (proposedDir == Up);

    if (walkable(newY, newX) && !occupiedByGhost(newY, newX)) {
        dir = proposedDir;
    } else {
        QVector<Dir> options = {Up, Down, Left, Right};
        for (Dir d : options) {
            int tx = _x + (d == Right) - (d == Left);
            int ty = _y + (d == Down) - (d == Up);
            if (walkable(ty, tx) && !occupiedByGhost(ty, tx)) {
                dir = d;
                return;
            }
        }
        dir = Stop;
    }
}


// guiding a ghost back into the cage after being eaten
void Ghost::returnToCage()
{
    // ghost reached the center of the cage , reset the status
    if (_x == level->gateCol && _y == level->gateRow + 1) {
        status      = Normal;
        released    = false;
        outside     = false;
        releaseTime = 200;
        dir         = Stop;
        level->getGhostTimer(color)->setInterval(NORMAL_INTERVAL);
        return;
    }


    bool okdir[5] = {false, false, false, false, false};
    QVector<Dir> oklist;

    if (overlapable(_y, _x + 1)) {
        okdir[Right] = true;
        oklist.push_back(Right);
    }
    if (overlapable(_y, _x - 1)) {
        okdir[Left] = true;
        oklist.push_back(Left);
    }
    if (overlapable(_y + 1, _x)) {
        okdir[Down] = true;
        oklist.push_back(Down);
    }
    if (overlapable(_y - 1, _x)) {
        okdir[Up] = true;
        oklist.push_back(Up);
    }

    Dir backward_dir = Stop;
    switch (dir) {
    case Up:    backward_dir = Down; break;
    case Down:  backward_dir = Up;   break;
    case Left:  backward_dir = Right;break;
    case Right: backward_dir = Left; break;
    default:    break;
    }


    // choosee new directionn based on shortest path to cage
    if (oklist.size() > 2) {
        int dist_x = level->gateCol - _x;
        int dist_y = (level->gateRow + 1) - _y;

        if (dist_y > 0) {
            if (okdir[Down] && dir != Up) {
                dir = Down;
            } else if (dist_x >= 0 && okdir[Right]) {
                dir = Right;
            } else if (okdir[Left]) {
                dir = Left;
            }
        } else if (dist_y < 0) {
            if (okdir[Up] && dir != Down) {
                dir = Up;
            } else if (dist_x >= 0 && okdir[Right]) {
                dir = Right;
            } else if (okdir[Left]) {
                dir = Left;
            }
        } else if (dist_x > 0) {
            if (okdir[Right] && dir != Left) {
                dir = Right;
            } else if (okdir[Up]) {
                dir = Up;
            } else if (okdir[Down]) {
                dir = Down;
            }
        } else if (dist_x < 0) {
            if (okdir[Left] && dir != Right) {
                dir = Left;
            } else if (okdir[Up]) {
                dir = Up;
            } else if (okdir[Down]) {
                dir = Down;
            }
        }
    } else if (oklist.size() == 2) {
        if (!okdir[dir]) {
            dir = (oklist[0] == backward_dir) ? oklist[1] : oklist[0];
        }
    }

}

// moves the ghost out of the cage in the correct direction towrds the gate
void Ghost::goOutCage()
{
    int dx = level->gateCol - _x;
    int dy = level->gateRow - _y;
    if (dx > 0)         dir = Right;
    else if (dx < 0)    dir = Left;
    else if (dy > 0)    dir = Down;
    else                dir = Up;
}


// setters, getters, simple functions useed throughout
void Ghost::setStrategy(QPair<int,int>(*f)(Ghost*)) { chaseStrategy = f; }
void Ghost::setLevel(Level* l)                    { level = l; }
Ghost::Status Ghost::getStatus() const            { return status; }
void Ghost::setStatus(Status s)                   { status = s; }
int Ghost::getPanicTime() const                   { return panicTime; }
void Ghost::setPanicTime(int t)                   { panicTime = t; }
void Ghost::setReleaseTime(int t)                 { releaseTime = t; }
void Ghost::setReleased(bool r)                   { released = r; }
bool Ghost::isReleased() const                    { return released; }
Ghost::Color Ghost::getColor() const              { return color; }
void Ghost::setScore(int s)                       { score = s; }
int Ghost::getScore() const                       { return score; }
int Ghost::get_x() const                          { return _x; }
int Ghost::get_y() const                          { return _y; }
Entity::Dir Ghost::get_dir() const                { return dir; }
Level* Ghost::getLevel() const {
    return level;
}

