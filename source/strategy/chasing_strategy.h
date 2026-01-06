#ifndef CHASING_STRATEGY_H
#define CHASING_STRATEGY_H

#include <QPair>
class Ghost;

QPair<int,int> bfsNextStep(Ghost* ghost, int target_x, int target_y);

QPair<int,int> strategy1(Ghost* ghost);

QPair<int,int> strategy2(Ghost* ghost);

QPair<int,int> strategy3(Ghost* ghost);

QPair<int,int> strategy4(Ghost* ghost);

#endif
