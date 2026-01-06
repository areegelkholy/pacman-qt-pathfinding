#include "simpleitem.h"
#include "gamemanager.h"
#include "pacman.h"
#include "constants.h"
#include "level.h"


SimpleItem::SimpleItem(Item::Type type, const QPixmap& sprite, int score)
    : Item(sprite), itemType(type), level(nullptr)
{
    setScore(score);
}

SimpleItem::SimpleItem(Item::Type type, const QPixmap& sprite, int score, Level* level)
    : Item(sprite), itemType(type), level(level)
{
    setScore(score);
}


// defines what happens when Pacman touches an item, applies effect based on item
void SimpleItem::applyEffect(Pacman* pacman)
{
    if (!pacman) return;

    switch (itemType) {
        qDebug() << "Inside Switch Statement";
    case Item::PowerBall: {
        // Adds score and puts all ghosts into panic mode

        pacman->addScore(getScore());
        auto lvl = pacman->getLevel();
        for (int i = 0; i < Ghost::GhostNum; ++i) {
            Ghost* g = lvl->ghost[i];
            if (g->getStatus() != Ghost::Running)
                g->enterPanicMode(PANIC_DURATION);
        }
        if (level) {
            level->setBallCount(level->getBallCount() - 1);
        }
        break;
    }

    case Item::Ball:
        // Basic ball: add score and decrease ball count
        pacman->addScore(getScore());
        if (level) {
            level->setBallCount(level->getBallCount() - 1);
        }
        break;

    case Item::Strawberry:
        // Activates score-doubling power-up
        pacman->activateDoubleScore();
        break;

    case Item::Grape:
        // Grants Pacman 50 coins
        pacman->addCoins(50);
        break;

    default:
        break;

    }
}


// Returns the type of the item, used in collision
Item::Type SimpleItem::getType() const {
    return itemType;
}
