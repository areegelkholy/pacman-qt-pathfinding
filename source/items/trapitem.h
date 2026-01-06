#ifndef TRAPITEM_H
#define TRAPITEM_H

#include "item.h"

class TrapItem : public Item
{
public:
    TrapItem(Type type, const QPixmap& sprite);
    void applyEffect(Pacman* pacman) override;
    Type getType() const override;

private:
    Type trapType;
};

#endif
