#ifndef THIEF_H
#define THIEF_H

#include "entity.h"
#include <QTimer>

class Level;

class Thief : public Entity {
    Q_OBJECT
public:
    explicit Thief(Level* level);
    void startChasing();
    void stopChasing();
public slots:
    void move() override;

private:
    bool isWalkable(int r, int c) const;
    std::pair<int,int> nextBfsStep(int col, int row, int targetCol, int targetRow) const;

    Level*      level;
    QTimer*     moveTimer;
    bool        outsideCage = false;
    const int   chaseDuration = 20000;
    const int   moveInterval   =   175;
};

#endif
