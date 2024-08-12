#include "QuadTree.h"
#include "BaseObj.h"
#include "Gmath.h"
//不同类型对象的遍历要做的事情不同，遍历的程度不同，因此设置不同的分割门限

//Example：

//由于存放普通引用的方格只需要遍历获得坐标和flags，无需进行频繁的分割
const unsigned int Grid<gObj>::threshold = 50;







