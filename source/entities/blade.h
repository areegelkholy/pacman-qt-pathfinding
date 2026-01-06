#pragma once
#include "entity.h"
class Level;
#include <QTimer>

class Blade : public Entity {
    Q_OBJECT

public:
    Blade(Level* level, qreal startX);
    void move() override;

private slots:
    void dropStep();

private:
    Level* level;
    QTimer* dropTimer;
    static constexpr int STEP_PIXELS = 5;
    static constexpr int STEP_MS     = 50;
};
