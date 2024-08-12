# coding=utf-8
import os
import json
import re

#一些全局常量
cpp_input_path='./test.cpp'
h_output_path='Maps.h'
cpp_output_path='Maps.cpp'

h_header= \
'''#include "GameMain.h"
#include "Gmath.h"
#include "BaseObj.h"
\n'''

cpp_header=\
'''
#include "Maps.h"
\n
'''


#建立一个有向简单图，计算（间接）基类是否含有ActObj和EntityObj
graph=[]
graph_map={}#建立一个映射关系，免得无法根据节点字符串名称快速找到节点
grid_bases={} #存储需要加入Grid的基类名称和个数

#图的节点类
class Node:
    def __init__(self, name):
        self.obj_list=[] #某次读取后的属于改类的待创建的对象的坐标
        self.num=0#某次读取后属于该类的对象的数目
        self.name = name#节点名称
        self.adjacent_vertices = []#节点的邻接点名称
        self.belonging_list=[]#列表对象，存储需要加入Grid的基类名字

    #重置节点中的一次性变量
    def reset(self):
        self.obj_list=[]
        self.num=0

#读取json文件
def read_json_file(file_path):
    with open(file_path,'r',encoding='utf-8') as file:
        return json.load(file)

#读取cpp文件
def read_cpp_file(file_path):
    with open(file_path, 'r', encoding='utf-8') as file:
        return file.read()
    
#预处理，生成类与类之间的继承关系并分析该类的（间接）父类是否包含GRID_NEEDED基类
def preprocessing():

    # 正则表达式匹配派生类和其直接基类
    pattern = r"class\s+(\w+)\s*:.*public\s+(\w+(?:,\s*\w+)*)"

    # 初始化一个字典来存储继承关系，键为派生类，值为一个列表，包含直接基类
    inheritance_relations = {}

    # 建立有向简单图
    def create_graph():
        for derived in inheritance_relations.keys():
            node=Node(derived)
            graph.append(node)
            graph_map.update({derived:node})
            
        for derived,bases in inheritance_relations.items():
            for base in bases:
                if base!='gObj':
                    graph_map[derived].adjacent_vertices.append(graph_map[base])
                    
    #根据类的定义或声明前面的宏定义确定该类是否属于GRID_NEEDED基类
    def find_grid_bases_and_priority_level():
        # 初始化类名列表
        classes_after_grid_needed = []

        # 遍历源代码的每一行
        for line_number, line in enumerate(cpp_code.split('\n')):
            # 检查当前行是否定义了GRID_NEEDED
            if "#define GRID_NEEDED" in line:
                match1=None
                match2=None
                # 寻找接下来的类定义
                for search_line_number in range(line_number + 1, len(cpp_code.split('\n'))):
                    next_line = cpp_code.split('\n')[search_line_number]
                    # 简单匹配类定义，这里假设类定义以"class"开始，且类名紧随其后
                    if match1 is None:
                        match1 = re.match(r'class\s+(\w+)', next_line)
                    #匹配PRIORITY_LEVEL
                    if match2 is None:
                        match2 = re.match(r'#define PRIORITY_LEVEL(\w+)', next_line)
                    if match1 and match2 :
                        # 找到类名，加入列表
                        classes_after_grid_needed.append({match1.group(1):match2.group(1)})
                        # 找到后停止搜索，假设每个GRID_NEEDED后只找一个类
                        break


        # 使用sorted函数，指定key为取字典的值，并通过int转换以进行数值比较，reverse=True表示降序排序
        sorted_dicts = sorted(classes_after_grid_needed, key=lambda x: int(next(iter(x.values()))), reverse=True)
        # 提取排序后的单词到新列表，这里假设您想基于排序后的字典获取键（单词）
        for d in sorted_dicts:
            grid_bases.update({list(d.keys())[0]:0})
        for grid_base in grid_bases.keys():
            if grid_base!='gObj':
                graph_map[grid_base].belonging_list.append(grid_base)
                
        

    # 对简单有向图进行深搜
    def dfs_of_all():

        def dfs(node,grid_base_name):
            flag=False
            for adj_now in node.adjacent_vertices:
                if grid_base_name in node.belonging_list:
                    flag=True
                    break
                if grid_base_name in adj_now.belonging_list:
                    flag=True
                    node.belonging_list.append(grid_base_name)
                else:
                    if dfs(adj_now,grid_base_name):
                        flag=True
                        node.belonging_list.append(grid_base_name)
            return flag
            
        #对于每个GridNeeded类进行每个节点的深搜
        for grid_base in grid_bases.keys():
            for node in graph:
                dfs(node,grid_base)

    def reset_file():
        try:
            # 尝试删除文件
            os.remove(h_output_path)
            print(f"文件 {h_output_path} 已成功删除。")
        except FileNotFoundError:
            print(f"提示：文件 {h_output_path} 未找到。")
        except PermissionError:
            print(f"错误：没有权限删除文件 {h_output_path}。")
        except Exception as e:
            print(f"删除文件时发生错误：{e}")
        try:
            # 尝试删除文件
            os.remove(cpp_output_path)
            print(f"文件 {cpp_output_path} 已成功删除。")
        except FileNotFoundError:
            print(f"提示：文件 {cpp_output_path} 未找到。")
        except PermissionError:
            print(f"错误：没有权限删除文件 {cpp_output_path}。")
        except Exception as e:
            print(f"删除文件时发生错误：{e}")

        with open(h_output_path,'a',encoding='utf-8') as file:
            file.write(h_header)
        with open(cpp_output_path,'a',encoding='utf-8') as file:
            file.write(cpp_header)
        
                    



    #获取文件字符串
    cpp_code=read_cpp_file(cpp_input_path)

    # 查找所有匹配的行
    for line in cpp_code.split("\n"):
        match = re.search(pattern, line)
        if match:
            derived_class = match.group(1).replace(' ','')
            base_classes = match.group(2).split(",")
            base_classes=[base.replace(' ','') for base in base_classes]
            inheritance_relations[derived_class] = base_classes


    # 打印派生类及其直接基类
    #for derived, bases in inheritance_relations.items():
    #    print(f"派生类: {derived}, 直接基类: {', '.join(bases)}")
    reset_file()
    #创建图
    create_graph()

    #查找需要纳入Grid管理的类
    find_grid_bases_and_priority_level()

    #深度优先搜索
    dfs_of_all()




#生成代码，脚本核心部分
def generate_cpp_code(json_data,outputstring):
    #创建Obj对象
    def create_Obj():
        temp_sentence=''
        for node in graph:
            if node.obj_list:
                for index,oneObj in enumerate(node.obj_list):
                    name=node.name.lower()
                    _=[name,str(index+1),node.name,str(oneObj['x']),str(oneObj['y'])]
                    temp_sentence+="\tauto& %s%s = newObj<%s>(gMath::Crdinate{%s,%s});\n"%tuple(_)
                    temp_sentence+=f"\tgObjRootGrid->insert(&{name}{str(index+1)});\n"
                    for grid_base_name in grid_bases.keys():
                        if grid_base_name in node.belonging_list:
                            temp_sentence+=f"\t{grid_base_name}RootGrid->insert(&{name}{str(index+1)});\n"
                temp_sentence+='\n'
        return temp_sentence
    
    #重置grid_bases以及graph中的一次性变量
    def reset():
        for node in graph:
            node.reset()
        global grid_bases
        grid_bases=dict.fromkeys(grid_bases.keys(),0)


    #获取地图边界dict
    boundary=json_data['boundary']

    #获取物品dict
    mGame_list1=''
    mGame_list2=''
    mGame_list3=''
    mGame_list4=''
    gObj=json_data['gObj']
    for node in graph:
        node.obj_list=gObj.get(node.name)
        node.num=len(node.obj_list) if node.obj_list is not None else 0
        grid_bases['gObj']+=node.num
        for grid_base in node.belonging_list:
            grid_bases[grid_base]+=node.num
        if node.num !=0:
            mGame_list1+=(','+node.name)
            mGame_list3+=(','+str(node.num))
    for grid_base_name,num in grid_bases.items():
            mGame_list2+=(','+str(grid_base_name)) 
            mGame_list4+=(", {%s,3* (%s/ Grid<%s>::threshold)+ 1 }"%tuple([str(num),str(num),grid_base_name]))


    mGame_list1=mGame_list1[1:]
    mGame_list3=mGame_list3[1:]
    mGame_list4=mGame_list4[1:]
    

    #生成创建mGame对象的语句
    '''auto& theGame = mGame<ObjectManager<MoveObj,EntityObj,ActObj>, gObj,ActObj,EntityObj>
             ({2, 1, 1} , { {5, 1}, {3, 1} ,{1, 1} }, gMath::mRectangele(-1000,1000,500,-300));
    '''
   
    sentence_mGame="\tauto& theGame = mGame<ObjectManager<"+mGame_list1+">"+mGame_list2+"> ({"+mGame_list3+"} , \n\t{ "+mGame_list4+"},\n\tgMath::mRectangele(%s,%s,%s,%s))\n"%tuple([str(boundary['l']),str(boundary['r']),str(boundary['t']),str(boundary['b'])])
    #print(sentence_mGame)

    #生成创建RootGrid的语句
    sentence_create_Grid=''
    for grid_base_name in grid_bases.keys():
        sentence_create_Grid+=f"\tauto {grid_base_name}RootGrid = std::get<Grid<{grid_base_name}>*>(theGame.rootGrids);\n"

    #print(sentence_create_Grid)

    #生成创建Obj对象的语句
    sentence_create_Obj=create_Obj()
    all_sentence=sentence_mGame+'\n'+sentence_create_Grid+'\n'+sentence_create_Obj+'\n'
   

    #头文件语句
    h_sentence=\
'''
\n
class Map%s:virtual public mGame
{
    Map%s();

    mGame& initializeGame();

};
'''%tuple([outputstring,outputstring])
    
    #cpp文件语句
    cpp_sentence=\
'''\n\n\nMap%s::Map%s(){ }\n

mGame& Map%s::initializeGame(){
%s}
'''%tuple([outputstring,outputstring,outputstring,all_sentence])


    reset()
    return h_sentence,cpp_sentence
    

    
def process_file(filename):

    print(f"找到文件: {filename}")
    output_string=filename
    output_string=output_string.replace('map','')
    output_string=output_string.replace('.json','')

    json_data=read_json_file(filename)
    h_code,cpp_code=generate_cpp_code(json_data,output_string)

    with open(h_output_path, 'a', encoding='utf-8') as file:
        file.write(h_code)
    with open(cpp_output_path, 'a', encoding='utf-8') as file:
        file.write(cpp_code)
    print(f"C++代码已生成至：{h_output_path},{cpp_output_path}")



if __name__=='__main__':
    preprocessing()
    current_dir = os.getcwd()

    for filename in os.listdir(current_dir):
    # 检查文件名是否包含'map'且文件扩展名为.json
        if 'map' in filename and filename.endswith('.json'):
            process_file(filename)
            
    
