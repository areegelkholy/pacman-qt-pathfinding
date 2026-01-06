#ifndef ENTITY_H
#define ENTITY_H

#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QObject>

class Level;

class Entity : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    enum Dir { Up = 0, Down = 1, Left = 2, Right = 3, Stop = 4 };
    static const int Width = 20;

    Entity(QPixmap sprite);
    virtual ~Entity();

    int getX() const;
    int getY() const;
    void setX(int x);
    void setY(int y);

    int getScore() const { return score; }
    void setScore(int s) { score = s; }

    Dir getDir() const;
    void setDir(Dir d);
    Dir getNextDir() const;
    void setNextDir(Dir d);

    virtual void move() = 0;

protected:
    int _x, _y;
    int score = 0;
    Dir dir;
    Dir next_dir;
};

#endif
