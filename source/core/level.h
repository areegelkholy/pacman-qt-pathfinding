#ifndef LEVEL_H
#define LEVEL_H

#include <QGraphicsScene>
#include <QTimer>
#include <QVector>
#include <QString>
#include <QPointer>
#include <QLabel>


#include "entities/blade.h"
#include "pacman.h"
#include "ghost.h"
#include "entity.h"
#include "items/item.h"
#include "items/trapitem.h"
#include "items/simpleitem.h"

#include "entities/thief.h"

class Level : public QGraphicsScene
{
    Q_OBJECT

public:
    enum LevelStatus { Playing, Win, Lose, Paused };

    Level(int x, int y, int mapW, int mapH, const QString& mapPath, const QString& levelName);
    ~Level();

    void start();
    void stop();
    void pause();
    void resume();
    void reset();
    void loadMapFromMemory();
    void stopAllTimers();
    void removePowerBall(Item* item);
    QString levelName;
    QLabel* livesLabel = nullptr;



    void setNextDirection(Entity::Dir d);
    int getScore() const;
    int getBallCount();
    void setBallCount(int count);
    int getCoins() const;
    int getLives() const;
    void setCoins(int c);
    void activateShield();
    LevelStatus getStatus() const;
    void updateLivesDisplay();



    Thief* thief = nullptr;
    bool thiefSpawned = false;

    void showFloatingMessage(const QString& text);





    Item*** map;
    QPointF ghostSpawn[Ghost::GhostNum];
    Pacman* pacman;
    Ghost* ghost[Ghost::GhostNum];
    Item* gate;
    // after:
    QVector<Item*> powerballs;
    QVector<Ghost*> ghosts;
    int gateRow = -1;
    int gateCol = -1;
    QVector<QByteArray> originalMap;
    int map_width, map_height;
    int hitCooldown = 0;



    void applySlowEffect(int durationMs);
    void handleBombHit();
    void activatePanicMode();
    int getGeoX() const { return geo_x; }
    int getGeoY() const { return geo_y; }
    QTimer* getGhostTimer(Ghost::Color c) { return ghostTimers[c]; }
    bool isShieldActive() const { return shieldActive; }
    void deactivateShield() { shieldActive = false; }
    void decrementLives();
    void increaseLives(int n);


    void setStatus(LevelStatus s) { status = s; }
    void respawnEntities() { pacman->resetPosition();}

    void increaseScore(int s) { score += s; }
    void setPacmanSpeed(int interval);

signals:
    void levelWon();
    void gameLost();
    void bombHitTriggered();


private slots:
    void updatePacman();
    void updateGhost(int id);
    void flashPowerballs();
    void spawnBlade();

private:
    void loadMap(const QString& mapPath);

    int geo_x, geo_y;
    int score;
    int coins;
    int lives;
    int lastScoreCoinCheckpoint;

     QTimer* bladeSpawner = nullptr;
    int ballCount;

    bool doubleScoreActive;
    bool shieldActive;
    bool speedBoostActive;
    bool flashTick;

    LevelStatus status;

    QTimer* pacmanTimer;
    QTimer* powerballFlashTimer;
    QTimer* ghostTimers[Ghost::GhostNum];

    friend class Pacman;
    friend class TrapItem;
};

#endif
