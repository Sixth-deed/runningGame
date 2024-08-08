# coding=utf-8
import os
import json

#读取json文件
def read_json_file(file_path):
    with open(file_path,'r',encoding='utf-8') as file:
        return json.load(file)


#生成代码，脚本核心部分
def generate_cpp_code(json_data,outputstring):

    #创建MoveObj对象
    def create_MoveObj():
        temp_sentence=''
        for moveObj in MoveObj:
            _=[str(moveObj['x']),str(moveObj['y'])]
            temp_sentence+="\t\tauto& obj = newObj<MoveObj>(gMath::Crdinate{%s,%s});\n"%tuple(_)
            temp_sentence+="\t\tgObjRootGrid->insert(& obj);\n\t\tActRootGrid->insert(&obj);\n"
        return temp_sentence
    
    #创建EntityObj对象
    def create_EntityObj():
        temp_sentence=''
        for entityObj in EntityObj:
            _=[str(entityObj['x']),str(entityObj['y'])]
            temp_sentence+="\t\tauto& obj = newObj<EntityObj>(gMath::Crdinate{%s,%s});\n"%tuple(_)
            temp_sentence+="\t\tgObjRootGrid->insert(& obj);\n\t\tEntityRootGrid->insert(&obj);\n"
        return temp_sentence
    
    #创建ActObj对象
    def create_ActObj():
        temp_sentence=''
        for actObj in ActObj:
            _=[str(actObj['x']),str(actObj['y'])]
            temp_sentence+="\t\tauto& obj = newObj<ActObj>(gMath::Crdinate{%s,%s});\n"%tuple(_)
            temp_sentence+="\t\tgObjRootGrid->insert(& obj);\n\t\tActRootGrid->insert(&obj);\n"
        return temp_sentence

    '''auto& theGame = mGame<ObjectManager<MoveObj,EntityObj,ActObj>, gObj,ActObj,EntityObj>
             ({2, 1, 1} , { {5, 1}, {3, 1} ,{1, 1} }, gMath::mRectangele(-1000,1000,500,-300));
    '''

    #获取地图边界dict
    boundary=json_data['boundary']
    #获取物品dict
    
    gObj=json_data['gObj']
    MoveObj=gObj.get('MoveObj')
    EntityObj=gObj.get('EntityObj')
    ActObj=gObj.get('ActObj')
    #计算各个Obj数目
    num_of_MoveObj=len(MoveObj) if MoveObj is not None else 0
    num_of_EntityObj=len(EntityObj) if EntityObj is not None else 0
    num_of_ActObj=len(ActObj) if ActObj is not None else 0
    num_of_all_ActObj=num_of_ActObj+num_of_MoveObj
    num_of_gObj=num_of_all_ActObj+num_of_EntityObj
    #生成创建mGame对象的语句
    mGame_list1=''
    mGame_list2=''
    mGame_list3=[]
    mGame_list1+=(',MoveObj ' if num_of_MoveObj != 0 else '')
    mGame_list1+=(',EntityObj ' if num_of_EntityObj !=0  else '') 
    mGame_list1+=(',ActObj ' if num_of_ActObj != 0 else '') 
    mGame_list1=mGame_list1[1:]
    mGame_list2+=(',ActObj ' if num_of_all_ActObj != 0 else '') 
    mGame_list2+=(',EntityObj ' if  num_of_MoveObj != 0 else '')
    mGame_list3=[str(num_of_MoveObj),str(num_of_EntityObj),str(num_of_ActObj),str(num_of_gObj),str(num_of_gObj),str(num_of_all_ActObj),str(num_of_all_ActObj),str(num_of_EntityObj),str(num_of_EntityObj),str(boundary['l']),str(boundary['r']),str(boundary['t']),str(boundary['b'])]
    sentence_mGame="\t\tauto& theGame = mGame<ObjectManager<"+mGame_list1+">, gObj"+mGame_list2+"> ({%s, %s, %s} , \n\t\t{ {%s,3* (%s/ Grid<gObj>::threshold)+ 1 }, {%s,3* (%s/ Grid<ActObj>::threshold)+ 1} ,{%s,3* (%s/ Grid<EntityObj>::threshold)+ 1} },\n\t\tgMath::mRectangele(%s,%s,%s,%s))\n"%tuple(mGame_list3)
    #生成创建RootGrid的语句
    create_Grid="\t\tauto gObjRootGrid = std::get<Grid<gObj>*>(theGame.rootGrids);\n"
    create_Grid+="\t\tauto ActRootGrid = std::get<Grid<ActObj>*>(theGame.rootGrids);\n"if num_of_all_ActObj != 0 else ''
    create_Grid+="\t\tauto EntityRootGrid = std::get<Grid<EntityObj>*>(theGame.rootGrids);\n"if num_of_EntityObj != 0 else ''
    
    #生成创建Obj对象的语句
    create_Obj=''
    create_Obj=(create_MoveObj() if num_of_MoveObj!=0 else '')+'\n'+(create_EntityObj() if num_of_EntityObj!=0 else '')+'\n'+(create_ActObj() if num_of_ActObj!=0 else '')
    

    #函数前面的语句
    front='''#include "GameMain.h"
#include "Gmath.h"
#include "BaseObj.h"

class Map%s:virtual public mGame
{
    Map%s()
    {

    }
    mGame& initializeGame()
    {'''%tuple([outputstring,outputstring])

    #函数后面的语句
    end='''    }

};

'''
    return front+'\n'+sentence_mGame+'\n'+create_Grid+'\n'+create_Obj+'\n'+end
    

    

def process_file(filename):

    print(f"找到文件: {filename}")
    output_string=filename
    output_string=output_string.replace('map','')
    output_string=output_string.replace('.json','')
    cpp_output_path='./GameMap'+output_string+'.h'
    json_data=read_json_file(filename)
    generated_code=generate_cpp_code(json_data,output_string)
    with open(cpp_output_path, 'w', encoding='utf-8') as file:
        file.write(generated_code)
    print(f"C++代码已生成至：{cpp_output_path}")



if __name__=='__main__':
    current_dir = os.getcwd()

    for filename in os.listdir(current_dir):
    # 检查文件名是否包含'map'且文件扩展名为.json
        if 'map' in filename and filename.endswith('.json'):
            process_file(filename)
            
    
