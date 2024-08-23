#include "core/QuadTree.h"
#include "core/BaseObj.h"
#include "core/lib/Gmath.h"
#include "core/lib/Constants.h"
#include "core/Constraint.h"
#include "core/GameMain.h"


//Example：

//由于存放普通引用的方格只需要遍历获得坐标和flags，无需进行频繁的分割
template<>
const unsigned int Grid<gObj>::threshold = 100;


// 约束求解时 速度约束 的迭代次数
const int mVelAndPosSolver::vIterations = 2;
// 约束求解时 位置约束 的迭代次数
const int mVelAndPosSolver::pIterations = 4;

const double PhysicsContactConstraint::posBiasFactor = 0.5;
const double PhysicsContactConstraint::maxPenetraintion = 0.1;


std::chrono::milliseconds clocksPerFrame(1000 / 60);




