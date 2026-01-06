#include "entity.h"



// initializes the sprite and default direction states
Entity::Entity(QPixmap sprite) {
    setPixmap(sprite);
    dir = Stop;
    next_dir = Stop;
    _x = _y = 0;
}

Entity::~Entity() {
}


//getters and setters for directions and coordinates
int Entity::getX() const { return _x; }
int Entity::getY() const { return _y; }

void Entity::setX(int x) { _x = x; }
void Entity::setY(int y) { _y = y; }

Entity::Dir Entity::getDir() const { return dir; }
void Entity::setDir(Dir d) { dir = d; }

Entity::Dir Entity::getNextDir() const { return next_dir; }
void Entity::setNextDir(Dir d) { next_dir = d; }


