#include "gamemanager.h"


// paths to level layout files
QStringList levelPaths = {
    ":/game_objects/map_objects/level1.txt",
    ":/game_objects/map_objects/level2.txt",
    ":/game_objects/map_objects/level3.txt",
    ":/game_objects/map_objects/level4.txt",
    ":/game_objects/map_objects/level5.txt"
};



//Initizialize game manager with the defaultt values
GameManager::GameManager(QObject *parent)
    : QObject(parent),
    currentLevel(nullptr),
    currentLevelIndex(0),
    lives(3),
    score(0),
    coins(0),
    status(Playing)
{
}

// start a game
void GameManager::startGame() {
    currentLevelIndex = 0;
    lives = 3;
    score = 0;
    coins = 0;
    status = Playing;

    loadLevel(currentLevelIndex);
}

// restart level (not used in the code)

void GameManager::restartLevel() {
    if (!currentLevel) return;
    currentLevel->stop();
    currentLevel->start();
    coins = currentLevel->getCoins();
    score = currentLevel->getScore();

}


// addvances to the next level while preserving score and coins

void GameManager::advanceLevel() {
    currentLevelIndex++;

    // checkks if there are more levels to play
    if (currentLevel && currentLevelIndex < levelPaths.size()) {

        // save current score and coins

        coins = currentLevel->getCoins();
        score = currentLevel->getScore();

        // cleans current level
        delete currentLevel;

        // loads the next level
        QString path = levelPaths[currentLevelIndex];
        currentLevel = new Level(0, 0, 29, 20, path, path);

        currentLevel->setCoins(coins);
        currentLevel->increaseScore(score);

        currentLevel->start();
        emit levelChanged(currentLevel);
        emit levelAdvanced();
    } else {
        qDebug() << "All levels complete!";
        emit gameWon(); //signal game completion
    }
}




void GameManager::pauseGame() {
    if (status != Playing) return;
    status = Paused;
    if (currentLevel) currentLevel->stop();
    emit gamePaused();
}



// getter for current level index
int GameManager::getCurrentLevelIndex() const {
    return currentLevelIndex;
}

void GameManager::resumeGame() {
    if (status != Paused) return;
    status = Playing;
    if (currentLevel) currentLevel->start();
    emit gameResumed();
}


// loads a level from the levelPaths
void GameManager::loadLevel(int levelIndex) {
    if (currentLevel) {
        delete currentLevel;
    }

    if (levelIndex < levelPaths.size()) {
        QString path = levelPaths[levelIndex];
        currentLevel = new Level(0, 0, 29, 20, path, path);

        currentLevel->start();
        emit levelChanged(currentLevel);
    }
}

// getters for remaining lives, score, coins, status, level
int GameManager::getLives() const { return lives; }
int GameManager::getScore() const { return score; }
int GameManager::getCoins() const { return coins; }
GameManager::GameStatus GameManager::getStatus() const { return status; }
Level* GameManager::getCurrentLevel() const { return currentLevel; }

// adds a specified number of lives (for powerup) and emits change signal
void GameManager::addLives(int n) {
    lives += n;
    emit livesChanged(lives);
}

