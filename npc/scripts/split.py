import re
import os
import shutil
build_path = "./build/"
files = os.listdir(build_path)
txt_filepath = './build/top/top.sv'
splited_path = "build/splited_file/"
start_key='module'

for item in files:
    item_list = item.split(".")
    if not item_list.endswith("sv"):
        continue
    item_list.pop(-1)
    pure_name = "".join(item_list)
    path_name = build_path+ pure_name + "_splited/"
    lines_copy = []
    flag = 0
    if os.path.exists(path_name):
	    shutil.rmtree(path_name)

    os.mkdir(path_name)
    
    temp_f = open(path_name+"~generated", "w")
    temp_f.write("This is a file marking this script is used, do not delete it")
    temp_f.close()
    pattern=re.compile(' .+\(')   #匹配从 开始，到 结束的内容
    with open(build_path+item) as fp:
        lines = fp.readlines()
        for line in lines:
            # line = line.strip('\n')#去掉回车    
            if "endmodule" in line:
                fp_module.writelines(line)
                flag = 0    
                fp_module.close()             
            elif start_key in line:
                str_line = "".join(line)
                result=pattern.findall(str_line)
                module_name = "".join(result)
                module_name = module_name.rstrip("\(")        
                new_filepath = path_name+module_name.replace(" ", "")+'.sv'
                fp_module = open(new_filepath,'w')
                flag = 1                               
            if flag == 1:
                lines_copy.append(line)
                fp_module.writelines(line)
