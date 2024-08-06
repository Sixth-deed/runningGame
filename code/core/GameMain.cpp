#include "GameMain.h"
#include "Gmath.h"
template <typename ManagerT, typename... GridManagerTypes>
mGame<ManagerT,GridManagerTypes...>::mGame(std::initializer_list<std::size_t> initialSizes,std::initializer_list<std::tuple<int,int>> gridsInitialize, const gMath::mRectangele& rect):
     mainObjManager(initialSizes),
     GridManagers(createGirdManagers(std::make_index_sequence<sizeof...(GridManagerTypes)>(),gridsInitialize.begin())),
     rootGrids(createRootGrids(std::make_index_sequence<sizeof...(GridManagerTypes)>(),rect))
{

}
template<std::size_t... Is, typename T>
void clearHelper(std::index_sequence<Is...>,T &tuple){
    (std::get<Is>(tuple)->clear(),...);
}
template <typename ManagerT, typename... GridManagerTypes>
mGame<ManagerT,GridManagerTypes...>::~mGame(){
    mainObjManager.clear();
    clearHelper(std::make_index_sequence<sizeof...(GridManagerTypes)>(), rootGrids);
}

