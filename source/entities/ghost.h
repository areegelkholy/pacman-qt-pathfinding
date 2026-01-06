#ifndef GHOST_H
#define GHOST_H

#include "entity.h"
#include <QVector>
#include <QObject>
#include <QPixmap>
#include <QPair>

class Level;

class Ghost : public Entity
{

    Q_OBJECT
public:
    enum Color { Red = 0, Yellow, Pink, Green };
    enum Status { Normal, Panic, Running };
    static const int GhostNum = 4;

    Ghost(Color color, Level* level);
    Ghost(const QPixmap& pixmap);


    void move() override;
    void setStrategy(QPair<int, int> (*strategyFunc)(Ghost*));
    void setLevel(Level* level);


    Status getStatus() const;
    void setStatus(Status newStatus);
    int getPanicTime() const;
    void setPanicTime(int time);
    bool occupiedByGhost(int r,int c) const;

    Color getColor() const;
    void setReleaseTime(int t);
    void setReleased(bool released);
    bool isReleased() const;
    void enterPanicMode(int duration);
    bool outside;


    void setScore(int s);
    int getScore() const;
    int get_x() const;
    int get_y() const;
    Dir get_dir() const;
    Level* getLevel() const;

private:

    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void setRandomDirection();
    void chasePacman();
    void dodgePacman();
    void goOutCage();
    bool walkable(int r,int c) const;
    bool insideCage(int row) const;
    void animate(int dx,int dy);

    void returnToCage();
    bool canMoveTo(int y, int x);
    void animateAndMove(Dir d, int dx, int dy);
    inline bool overlapable(int r, int c) const;

    Color color;
    Status status;
    int panicTime;
    int releaseTime;
    bool is_released = false;
    int  release_time = 0;

    bool released;
    int animIndex;
    int score = 0;

    Level* level;
    QPair<int, int> (*chaseStrategy)(Ghost*);

    QVector<QPixmap> anim[4];
    QVector<QPixmap> panicAnim;
    QVector<QPixmap> runAnim;
};

#endif
