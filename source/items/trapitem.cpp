#include "trapitem.h"
#include "pacman.h"

TrapItem::TrapItem(Type type, const QPixmap& sprite)
    : Item(sprite), trapType(type)
{
}


// applies the trap's effect on Pacman when touched based on the iterm
void TrapItem::applyEffect(Pacman* pacman)
{
    if (!pacman) return;

    switch (trapType) {
    case SlowTrap:
        // Temporarily slows down Pacman's movement
        pacman->applySlowEffect();
        break;
    case BombTrap:
        // triggers damage effec to reduce lives
        pacman->takeBombHit();
        break;
    default:
        break;
    }
}

Item::Type TrapItem::getType() const {
    return trapType;
}
