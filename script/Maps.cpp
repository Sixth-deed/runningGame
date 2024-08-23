
#include "Maps.h"





MapA::MapA(){ }


mGame& MapA::initializeGame(){
	auto& theGame = mGame<ObjectManager<EntityObj,ActObj,HelloObj,MoveObj,MyObj,WorldObj>,EntityObj,ActObj,HelloObj,gObj> ({1,1,1,2,2,1} , 
	{  {4,3* (4/ Grid<EntityObj>::threshold)+ 1 }, {6,3* (6/ Grid<ActObj>::threshold)+ 1 }, {2,3* (2/ Grid<HelloObj>::threshold)+ 1 }, {8,3* (8/ Grid<gObj>::threshold)+ 1 }},
	gMath::mRectangele(-1000,1000,500,-300))

	auto EntityObjRootGrid = std::get<Grid<EntityObj>*>(theGame.rootGrids);
	auto ActObjRootGrid = std::get<Grid<ActObj>*>(theGame.rootGrids);
	auto HelloObjRootGrid = std::get<Grid<HelloObj>*>(theGame.rootGrids);
	auto gObjRootGrid = std::get<Grid<gObj>*>(theGame.rootGrids);

	auto& entityobj1 = newObj<EntityObj>(gMath::Crdinate{0,0});
	gObjRootGrid->insert(&entityobj1);
	EntityObjRootGrid->insert(&entityobj1);

	auto& actobj1 = newObj<ActObj>(gMath::Crdinate{500,200});
	gObjRootGrid->insert(&actobj1);
	ActObjRootGrid->insert(&actobj1);

	auto& helloobj1 = newObj<HelloObj>(gMath::Crdinate{20,30});
	gObjRootGrid->insert(&helloobj1);
	HelloObjRootGrid->insert(&helloobj1);

	auto& moveobj1 = newObj<MoveObj>(gMath::Crdinate{200,500});
	gObjRootGrid->insert(&moveobj1);
	ActObjRootGrid->insert(&moveobj1);
	auto& moveobj2 = newObj<MoveObj>(gMath::Crdinate{-500,400});
	gObjRootGrid->insert(&moveobj2);
	ActObjRootGrid->insert(&moveobj2);

	auto& myobj1 = newObj<MyObj>(gMath::Crdinate{100,-200});
	gObjRootGrid->insert(&myobj1);
	EntityObjRootGrid->insert(&myobj1);
	ActObjRootGrid->insert(&myobj1);
	auto& myobj2 = newObj<MyObj>(gMath::Crdinate{300,-300});
	gObjRootGrid->insert(&myobj2);
	EntityObjRootGrid->insert(&myobj2);
	ActObjRootGrid->insert(&myobj2);

	auto& worldobj1 = newObj<WorldObj>(gMath::Crdinate{-20,-30});
	gObjRootGrid->insert(&worldobj1);
	EntityObjRootGrid->insert(&worldobj1);
	ActObjRootGrid->insert(&worldobj1);
	HelloObjRootGrid->insert(&worldobj1);


}



MapC::MapC(){ }


mGame& MapC::initializeGame(){
	auto& theGame = mGame<ObjectManager<EntityObj>,EntityObj,ActObj,HelloObj,gObj> ({2} , 
	{  {2,3* (2/ Grid<EntityObj>::threshold)+ 1 }, {0,3* (0/ Grid<ActObj>::threshold)+ 1 }, {0,3* (0/ Grid<HelloObj>::threshold)+ 1 }, {2,3* (2/ Grid<gObj>::threshold)+ 1 }},
	gMath::mRectangele(-1000,1000,500,-300))

	auto EntityObjRootGrid = std::get<Grid<EntityObj>*>(theGame.rootGrids);
	auto ActObjRootGrid = std::get<Grid<ActObj>*>(theGame.rootGrids);
	auto HelloObjRootGrid = std::get<Grid<HelloObj>*>(theGame.rootGrids);
	auto gObjRootGrid = std::get<Grid<gObj>*>(theGame.rootGrids);

	auto& entityobj1 = newObj<EntityObj>(gMath::Crdinate{0,0});
	gObjRootGrid->insert(&entityobj1);
	EntityObjRootGrid->insert(&entityobj1);
	auto& entityobj2 = newObj<EntityObj>(gMath::Crdinate{300,500});
	gObjRootGrid->insert(&entityobj2);
	EntityObjRootGrid->insert(&entityobj2);


}



MapB::MapB(){ }


mGame& MapB::initializeGame(){
	auto& theGame = mGame<ObjectManager<ActObj,MoveObj>,EntityObj,ActObj,HelloObj,gObj> ({1,2} , 
	{  {0,3* (0/ Grid<EntityObj>::threshold)+ 1 }, {3,3* (3/ Grid<ActObj>::threshold)+ 1 }, {0,3* (0/ Grid<HelloObj>::threshold)+ 1 }, {3,3* (3/ Grid<gObj>::threshold)+ 1 }},
	gMath::mRectangele(-1000,1000,500,-300))

	auto EntityObjRootGrid = std::get<Grid<EntityObj>*>(theGame.rootGrids);
	auto ActObjRootGrid = std::get<Grid<ActObj>*>(theGame.rootGrids);
	auto HelloObjRootGrid = std::get<Grid<HelloObj>*>(theGame.rootGrids);
	auto gObjRootGrid = std::get<Grid<gObj>*>(theGame.rootGrids);

	auto& actobj1 = newObj<ActObj>(gMath::Crdinate{500,200});
	gObjRootGrid->insert(&actobj1);
	ActObjRootGrid->insert(&actobj1);

	auto& moveobj1 = newObj<MoveObj>(gMath::Crdinate{100,500});
	gObjRootGrid->insert(&moveobj1);
	ActObjRootGrid->insert(&moveobj1);
	auto& moveobj2 = newObj<MoveObj>(gMath::Crdinate{-500,400});
	gObjRootGrid->insert(&moveobj2);
	ActObjRootGrid->insert(&moveobj2);


}
