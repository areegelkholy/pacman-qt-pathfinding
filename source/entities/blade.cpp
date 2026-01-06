#include "entities/blade.h"
#include "core/level.h"
#include <QTimer>
#include <QGraphicsScene>


// Blade movement constants
static constexpr int STEP_PIXELS = 5;
static constexpr int STEP_MS     = 50;
static constexpr float BLADE_SCALE = 1.5f;



// Initializes blade visual, position, and falling behavior

Blade::Blade(Level* lvl, qreal spawnX)
    : Entity(QPixmap())
    , level(lvl)
{
    QPixmap raw(":/game_objects/map_objects/blade.png");

    int bladeSize = static_cast<int>(Entity::Width * BLADE_SCALE);


    // Resize blade image
    QPixmap tile = raw.scaled(
        bladeSize,
        bladeSize,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );
    setPixmap(tile);
    setZValue(5);

    // Calculate starting position
    qreal x = level->getGeoX() + spawnX;
    qreal y = level->getGeoY() - tile.height();
    setPos(x, y);


    // initialize blade drop timer
    dropTimer = new QTimer(this);
    connect(dropTimer, &QTimer::timeout, this, &Blade::dropStep);
    dropTimer->start(STEP_MS);
}

void Blade::move()
{
    dropStep();
}



// called every STEP_MS  milliseconds , moves the blade down and checks for collisions
void Blade::dropStep()
{
    moveBy(0, STEP_PIXELS);
    // If blade hits Pacman, lose 2 lives then delete image
    if (level->pacman && collidesWithItem(level->pacman)) {
        level->decrementLives();
        level->decrementLives();

        level->updateLivesDisplay();
        dropTimer->stop();
        if (scene()) scene()->removeItem(this);
        delete this;
        return;
    }

    // if blade goes off-screen it is removed
    if (y() > level->sceneRect().height()) {
        dropTimer->stop();
        if (scene()) scene()->removeItem(this);
        delete this;
    }
}
