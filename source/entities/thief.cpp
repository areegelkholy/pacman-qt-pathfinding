#include "thief.h"
#include "level.h"
#include "pacman.h"
#include "items/simpleitem.h"

#include <QGraphicsScene>
#include <QDebug>
#include <queue>
#include <vector>
#include <algorithm>

#define W Entity::Width

//Initializes Thief and its timeer
Thief::Thief(Level* level)
    : Entity(
          QPixmap(":/game_objects/ghosts/thief.png")
              .scaled(W, W, Qt::KeepAspectRatio, Qt::SmoothTransformation)
          ),
    level(level)
{
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &Thief::move);
}


// Begins chasing Pacman from inside the cage
void Thief::startChasing() {
    outsideCage = false;
    _x = level->gateCol;
    _y = level->gateRow + 1;
    setPos(level->getGeoX() + _x * W,
           level->getGeoY() + _y * W);

    moveTimer->start(moveInterval);
    QTimer::singleShot(chaseDuration, this, &Thief::stopChasing);
}


// Stops chasing Pacman and hides the Thief
void Thief::stopChasing() {
    moveTimer->stop();
    hide();
}


// checks if tile is walkable
bool Thief::isWalkable(int r, int c) const {
    if (!level || r < 0 || c < 0
        || r >= level->map_height || c >= level->map_width)
        return false;

    Item* it = level->map[r][c];
    if (!it) return false;

    auto* s = dynamic_cast<SimpleItem*>(it);
    if (!s) return true;

    auto t = s->getType();
    if (!outsideCage) {
        return t != SimpleItem::Wall;
    } else {
        return t != SimpleItem::Wall
               && t != SimpleItem::Gate;
    }
}


// uses breadth first strategy  (BFS) to find the next step toward pacman
std::pair<int,int> Thief::nextBfsStep(
    int col, int row,
    int targetCol, int targetRow
    ) const {
    const int H = level->map_height;
    const int Wd= level->map_width;
    std::vector<std::vector<bool>> vis(H, std::vector<bool>(Wd,false));
    std::vector<std::vector<std::pair<int,int>>> parent(
        H, std::vector<std::pair<int,int>>(Wd,{-1,-1}));
    std::queue<std::pair<int,int>> q;

    vis[row][col] = true;
    q.push({row,col});

    int dr[4] = {-1,1,0,0};
    int dc[4] = { 0,0,-1,1};

    while (!q.empty()) {
        auto [r,c] = q.front(); q.pop();
        if (r == targetRow && c == targetCol) break;
        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i], nc = c + dc[i];
            if (nr<0||nc<0||nr>=H||nc>=Wd) continue;
            if (vis[nr][nc]) continue;
            if (!isWalkable(nr,nc)) continue;
            vis[nr][nc] = true;
            parent[nr][nc] = {r,c};
            q.push({nr,nc});
        }
    }
    if (parent[targetRow][targetCol].first == -1)
        return {0,0};

    int cr = targetRow, cc = targetCol;
    while (parent[cr][cc] != std::make_pair(row,col)) {
        auto p = parent[cr][cc];
        cr = p.first; cc = p.second;
    }
    return { cc - col, cr - row };
}


// moves Thief and handles interactions
void Thief::move() {
    if (!level || !level->pacman) return;

    int px = x(), py = y();
    int col = (px - level->getGeoX()) / W;
    int row = (py - level->getGeoY()) / W;

    qDebug() << "Thief at tile" << col << "," << row
             << "; outsideCage=" << outsideCage;

    // exit cage
    if (!outsideCage) {
        int r2 = row - 1, c2 = col;
        if (isWalkable(r2,c2)) {
            row = r2;
            _x = col; _y = row;
            setPos(level->getGeoX()+col*W,
                   level->getGeoY()+row*W);
            if (row < level->gateRow)
                outsideCage = true;
        }
        return;
    }

// chase using BFS
    auto* pac = level->pacman;
    int targetCol = pac->get_x();
    int targetRow = pac->get_y();
    auto [dx,dy] = nextBfsStep(col,row,targetCol,targetRow);
    if (dx||dy) {
        int nc = col + dx, nr = row + dy;
        if (isWalkable(nr,nc)) {
            _x = nc; _y = nr;
            setPos(level->getGeoX()+nc*W,
                   level->getGeoY()+nr*W);
        }
    }

    // collision with pacman, steal coins, dissapear
    if (collidesWithItem(pac)) {
        level->setCoins(std::max(0, level->getCoins() - 100));
        qDebug() << "Thief stole 100 coins!";
        moveTimer->stop();
        hide();
    }
}
