#ifndef GAME_IDHANDLER
#define GMAE_IDHANDLER

#include <climits>
#include <stdexcept>
using mID = unsigned int;
//id管理器
class idHandler{
    private:
        mID idPT;
        static idHandler* mainIdHandler;
    public:
        idHandler(): idPT(0){}
        //获取一个有效的新id
        mID getNewID(){
            if (idPT == UINT_MAX){
                throw std::runtime_error("No More ID for use!"); 
            }
            return idPT++;
        }


        //给其他模块访问mainIDHandler的引用的方式
        static idHandler& mainHandler(){
            return *mainIdHandler;
        }
};

#endif