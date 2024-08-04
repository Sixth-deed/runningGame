#include "QuadTree.h"

template <typename T>
Grid<T>::Grid(gMath::axisV l_,gMath::axisV r_,gMath::axisV t_,gMath::axisV b_
            , const std::vector<T*>& v
            , short thrs ):
    l(l_), r(r_), t(t_), b(b_), threshold(thrs) 
{
   //将vector中的对象向引用池中绑定
   //...to be added
}
template <typename T>
idHandler Grid<T>::gIDhdr;

template <typename T>
T& Grid<T>::getObj(mID objID) const{
    return *( (T*) ( (*mManager).get(rfCode()) ) );
}

template <typename T>
void Grid<T>::insert(T* pObj){
    
}


