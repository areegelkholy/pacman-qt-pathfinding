#ifndef PACMAN_H
#define PACMAN_H

#include "entity.h"
#include <QVector>
#include <QPixmap>
#include <QObject>

#include "constants.h"


class Level;

class Pacman : public Entity
{
    Q_OBJECT
public:
    Pacman(Level* level, const QString& baseSpritePath);

    void move() override;
    void resetPosition();
    void setNextDir(Dir d);
    void activateDoubleScore();
    void activateSpeedBoost();
    void activateShield();
    void deactivateSpeedBoost();

    Level* getLevel() const;

    void addScore(int value);
    void addCoins(int value);

    void applySlowEffect();
    void takeBombHit();

    int get_x() const;
    int get_y() const;
    Dir get_dir() const;

    int start_x, start_y;

private:
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void animateAndMove(Dir d, int dx, int dy);
    QTimer* speedTimer;



    void eatItem(int y, int x);
    bool isWalkable(int i, int j);

    Level* level;
    QVector<QPixmap> animations[4];
    int animationIndex;

    bool doubleScore;
    bool speedBoost;
    bool shield;

};

#endif
