#ifndef GAME_IDHANDLER
#define GAME_IDHANDLER

#include <climits>
#include <stdexcept>
using mID = unsigned int;
//id管理器
class idHandler{
    private:
        mID idPT;
    public:
        idHandler(): idPT(0){}
        //获取一个有效的新id
        mID getNewID(){
            if (idPT == UINT_MAX){
                throw std::runtime_error("No More ID for use!"); 
            }
            return idPT++;
        }


};

#endif
