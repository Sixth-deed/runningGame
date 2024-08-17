#include "lib/QuadTree.h"
#include "BaseObj.h"
#include "lib/Gmath.h"
#include "lib/Constants.h"
#include "Constraint.h"


//不同类型对象的遍历要做的事情不同，遍历的程度不同，因此设置不同的分割门限
template <typename T>
const unsigned int Grid<T>::threshold = 50;
//Example：

//由于存放普通引用的方格只需要遍历获得坐标和flags，无需进行频繁的分割
template<>
const unsigned int Grid<gObj>::threshold = 100;


// 约束求解时 速度约束 的迭代次数
const int VelAndPosSolver::vIterations = 2;
// 约束求解时 位置约束 的迭代次数
const int VelAndPosSolver::pIterations = 4;

const double PhysicsContactConstraint::posBiasFactor = 0.5;
const double PhysicsContactConstraint::maxPenetraintion = 0.1;






