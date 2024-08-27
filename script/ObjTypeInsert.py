import os
import re

obj_dir="../include/GameObjects"
dest_file="../include/core/gObj.h"
class_names=''


def read_file_lines(file_path):
    with open(file_path, 'r', encoding='utf-8') as file:
        return file.readlines()

def read_file_content(file_path):
    with open(file_path, 'r', encoding='utf-8') as file:
        return file.read()

def read_gObj_content(file_path=dest_file):
    with open(file_path, 'r', encoding='utf-8') as file:
        return file.read()
    
def write_file_lines(lines,file_path):
    with open(file_path, 'w', encoding='utf-8') as file:
        file.writelines(lines)
def write_file_content(content,file_path=dest_file):
    with open(file_path, 'w', encoding='utf-8') as file:
        file.write(content)


def start(file_path):
    global class_names
    lines = read_file_lines(file_path)
    content=read_file_content(file_path)

    
    
    # 使用正则表达式匹配类定义，这里假设类定义的基本形式为"class 类名 {"
    #class_definitions = re.findall(r'class\s+(\w+)\s*:\s*', content)
    
    new_lines = []
    class_name = ''
    flag=False
    
    for line in lines:
        # 搜索类定义开始，考虑继承情况
        match = re.search(r'class\s+(\w+)\s*:\s*', line)
        if match:
            class_name = match.group(1)
            class_names+=',\n\t\t{}'.format(class_name)
            new_lines.append(line)  # 保留当前行
            flag=True
            continue
        else:
            new_lines.append(line)
        if flag==True and (f"DECLARE_INSTANCE_TYPE({class_name});" not in content):
            new_lines.append(f"\tDECLARE_INSTANCE_TYPE({class_name});\n")
            flag=False
    

    # 写回文件，注意这里会覆盖原文件
    write_file_lines(new_lines,file_path)
    
    print("声明已插入或文件未修改（如果所有类已包含声明）。")

    content=read_gObj_content()
    start_mark=\
    '''namespace mtype{
    enum gInstanceTypes{
        gInstance'''
    end_mark="};"
    start_index = content.find(start_mark) + len(start_mark)
    end_index = content.find(end_mark, start_index)
    
    # 提取要替换的字符串
    to_replace = content[start_index:end_index]
    
    # 进行替换
    new_content = to_replace.replace(to_replace, class_names)
    
    # 拼接结果，替换原内容
    result = content[:start_index] + new_content + '\n\t' + content[end_index:]

    write_file_content(result)
    print("gObj文件已经更新")
    

    




if __name__ == '__main__':
    for filename in os.listdir(obj_dir):
        if filename == "allObjects.h":
            continue
        start(obj_dir+'/'+filename)

