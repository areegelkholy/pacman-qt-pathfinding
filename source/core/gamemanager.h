#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include "level.h"

class GameManager : public QObject
{
    Q_OBJECT

public:
    enum GameStatus { Playing, Paused, Win, Lose };

    explicit GameManager(QObject *parent = nullptr);

    void startGame();
    void restartLevel();
    void advanceLevel();
    void pauseGame();
    void resumeGame();
    void addLives(int n);




    int getCurrentLevelIndex() const;

    int getLives() const;
    int getScore() const;
    int getCoins() const;
    GameStatus getStatus() const;

    Level* getCurrentLevel() const;


signals:
    void gameOver();
    void levelAdvanced();
    void gamePaused();
    void gameResumed();
    void levelChanged(Level* newLevel);
    void gameWon();
    void livesChanged(int lives);


private:
    Level* currentLevel;
    int currentLevelIndex;
    int lives;
    int score;
    int coins;
    GameStatus status;

    void loadLevel(int levelIndex);
};

#endif
