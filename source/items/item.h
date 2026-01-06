#ifndef ITEM_H
#define ITEM_H

#include <QGraphicsPixmapItem>
#include <QGraphicsPixmapItem>
#include "entity.h"

class Pacman;

class Item :public QObject, public QGraphicsPixmapItem
{
public:
    enum Type { Ball, PowerBall, Wall, Gate, Blank, Strawberry, Grape, SlowTrap, BombTrap };

    Item(QPixmap sprite);
    virtual ~Item() = default;

    virtual void applyEffect(Pacman* pacman) = 0;
    virtual Type getType() const = 0;

    int getScore() const;
    void setScore(int s);

protected:
    int score;
};

#endif
