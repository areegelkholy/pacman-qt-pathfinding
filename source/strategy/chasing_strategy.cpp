#include "chasing_strategy.h"
#include "ghost.h"
#include "level.h"
#include "pacman.h"
#include "items/simpleitem.h"
#include <QQueue>
#include <QVector>
#include <algorithm>

// Checks if a tile is walkable
static bool isTileWalkable(Level* level, int r, int c) {
    if (!level || r < 0 || c < 0 || r >= level->map_height || c >= level->map_width)
        return false;

    auto* item = dynamic_cast<SimpleItem*>(level->map[r][c]);
    if (!item) return true;

    switch (item->getType()) {
    case SimpleItem::Wall: return false;
    case SimpleItem::Gate:
        return true;
    default: return true;
    }
}

// prevent ghosts from overlapping, except when returning to cage)
static bool isTileOccupiedByGhost(Ghost* self, int r, int c) {
    Level* level = self->getLevel();
    for (int i = 0; i < Ghost::GhostNum; ++i) {
        Ghost* g = level->ghost[i];
        if (!g || g == self) continue;
        if (g->get_x() == c && g->get_y() == r)
            return true;
    }
    return false;
}


// Main BFS Breadth First Strategy function
QPair<int, int> bfsNextStep(Ghost* ghost, int target_x, int target_y) {
    Level* level = ghost->getLevel();
    if (!level) return {0, 0};

    const int H = level->map_height;
    const int W = level->map_width;

    target_x = std::clamp(target_x, 0, W - 1);
    target_y = std::clamp(target_y, 0, H - 1);

    QVector<QVector<bool>> visited(H, QVector<bool>(W, false));
    QVector<QVector<QPair<int, int>>> parent(H, QVector<QPair<int, int>>(W, {-1, -1}));
    QQueue<QPair<int, int>> q;

    int sx = ghost->get_x();
    int sy = ghost->get_y();
    if (sx < 0 || sx >= W || sy < 0 || sy >= H)
        return {0, 0};

    visited[sy][sx] = true;
    q.enqueue({sy, sx});

    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = { 0, 0,-1, 1};
    QVector<int> dirs = {0, 1, 2, 3};
    int offset = (ghost->getColor() == Ghost::Green) ? 0 :
                     static_cast<int>(ghost->getColor()) % dirs.size();
    std::rotate(dirs.begin(), dirs.begin() + offset, dirs.end());
    while (!q.isEmpty()) {
        auto [r, c] = q.dequeue();
        if (r == target_y && c == target_x) break;
        for (int idx : dirs) {
            int nr = r + dr[idx];
            int nc = c + dc[idx];
            if (nr < 0 || nc < 0 || nr >= H || nc >= W) continue;
            if (visited[nr][nc]) continue;
            if (!isTileWalkable(level, nr, nc)) continue;
            if (ghost->getStatus() != Ghost::Running && isTileOccupiedByGhost(ghost, nr, nc)) continue;


            visited[nr][nc] = true;
            parent[nr][nc] = {r, c};
            q.enqueue({nr, nc});
        }
    }

    if (parent[target_y][target_x].first == -1)
        return {0, 0};

    int cr = target_y;
    int cc = target_x;
    while (parent[cr][cc] != QPair<int, int>(sy, sx)) {
        auto p = parent[cr][cc];
        cr = p.first;
        cc = p.second;
    }
    return {cc - sx, cr - sy};
}



// Strategy 1: red ghost , chase Pacman's current position directly
QPair<int, int> strategy1(Ghost* ghost) {
    auto* level = ghost->getLevel();
    if (!level || !level->pacman) return {0, 0};
    return bfsNextStep(ghost,
                       level->pacman->get_x(),
                       level->pacman->get_y());
}

// Strategy 2: yellow ghost , aims 4 tiles ahead of Pacman’s current direction
QPair<int, int> strategy2(Ghost* ghost) {
    auto* level = ghost->getLevel();
    if (!level || !level->pacman) return {0, 0};

    int tx = level->pacman->get_x();
    int ty = level->pacman->get_y();
    switch (level->pacman->get_dir()) {
    case Entity::Up:    ty -= 4; break;
    case Entity::Down:  ty += 4; break;
    case Entity::Left:  tx -= 4; break;
    case Entity::Right: tx += 4; break;
    default: break;
    }
    tx = std::clamp(tx, 0, level->map_width  - 1);
    ty = std::clamp(ty, 0, level->map_height - 1);

    return bfsNextStep(ghost, tx, ty);
}


// Strategy 3: pink ghost, uses both Pacman’s movement and red ghost’s position
QPair<int, int> strategy3(Ghost* ghost) {
    auto* level = ghost->getLevel();
    if (!level || !level->pacman || !level->ghost[Ghost::Red]) return {0, 0};

    auto* pac = level->pacman;
    auto* red = level->ghost[Ghost::Red];
    int tx = (pac->get_x() - red->get_x()) * 2 + red->get_x();
    int ty = (pac->get_y() - red->get_y()) * 2 + red->get_y();
    tx = std::clamp(tx, 0, level->map_width  - 1);
    ty = std::clamp(ty, 0, level->map_height - 1);

    return bfsNextStep(ghost, tx, ty);
}


// Strategy 4: greene ghost,  moves in the opposite direction of Pacman
QPair<int, int> strategy4(Ghost* ghost) {
    auto* level = ghost->getLevel();
    if (!level || !level->pacman) return {0, 0};

    int tx = level->pacman->get_x();
    int ty = level->pacman->get_y();
    switch (level->pacman->get_dir()) {
    case Entity::Up:    ty += 4; break;
    case Entity::Down:  ty -= 4; break;
    case Entity::Left:  tx += 4; break;
    case Entity::Right: tx -= 4; break;
    default: break;
    }
    tx = std::clamp(tx, 0, level->map_width  - 1);
    ty = std::clamp(ty, 0, level->map_height - 1);

    return bfsNextStep(ghost, tx, ty);
}
