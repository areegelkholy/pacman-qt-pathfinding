#include "item.h"

Item::Item(QPixmap sprite) : QGraphicsPixmapItem(sprite), score(0) {}


//return and set score
int Item::getScore() const {
    return score;
}
void Item::setScore(int s) {
    score = s;
}
