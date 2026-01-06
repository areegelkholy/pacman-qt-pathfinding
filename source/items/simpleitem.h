#ifndef SIMPLEITEM_H
#define SIMPLEITEM_H

#include "items/item.h"

class Level;

class SimpleItem : public Item
{
public:
    SimpleItem(Item::Type type, const QPixmap& sprite, int score = 0);

    SimpleItem(Item::Type type, const QPixmap& sprite, int score, Level* level);

    void applyEffect(Pacman* pacman) override;
    Item::Type getType() const override;

private:
    Item::Type itemType;
    Level* level = nullptr;
};

#endif
