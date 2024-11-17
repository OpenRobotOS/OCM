import yaml
import sys
import os

# 用于存储已生成的类，避免重复生成
generated_classes = {}

def capitalize(name):
    """将字段名转换为类名，首字母大写"""
    return ''.join(word.capitalize() for word in name.split('_'))

def determine_type(name, value):
    """确定YAML值对应的C++类型"""
    if isinstance(value, bool):
        return "bool"
    elif isinstance(value, int) or isinstance(value, float):
        return "double"
    elif isinstance(value, str):
        return "std::string"
    elif isinstance(value, list):
        return get_vector_type(name, value)
    elif isinstance(value, dict):
        class_name = capitalize(name)
        generate_class(class_name, value)
        return class_name
    elif value is None:
        return "std::string"  # 默认处理Null为string
    else:
        return "std::string"  # 默认类型

def get_vector_type(name, lst):
    """确定列表中元素的C++类型"""
    if not lst:
        return "std::vector<std::string>"  # 默认类型

    first_elem = lst[0]
    elem_type = determine_type(name, first_elem)

    # 检查所有元素类型是否一致
    for item in lst:
        current_type = determine_type(name, item)
        if current_type != elem_type:
            return "std::vector<std::string>"  # 默认类型

    return f"std::vector<{elem_type}>"

def sanitize_field_name(name):
    """将字段名转换为有效的C++变量名"""
    # 替换非字母数字字符为下划线
    return ''.join(['_' if not c.isalnum() else c for c in name])

def generate_class(class_name, data):
    """生成C++类代码并写入文件"""
    if class_name in generated_classes:
        return  # 已生成，跳过

    generated_classes[class_name] = True

    member_vars = []
    update_lines = []

    for key, value in data.items():
        field_name = sanitize_field_name(key)
        field_type = determine_type(key, value)
        member_vars.append(f"    {field_type} {field_name};")

    # 开始生成类代码
    class_code = f"class {class_name} {{\npublic:\n"
    # 成员变量
    for var in member_vars:
        class_code += var + "\n"
    class_code += "\n    void update_from_yaml(const YAML::Node& node) {\n"

    for key, value in data.items():
        field_name = sanitize_field_name(key)
        field_type = determine_type(key, value)

        if field_type.startswith("std::vector<"):
            elem_type = field_type[len("std::vector<"):-1]
            class_code += f"        if(node[\"{key}\"]) {{\n"
            class_code += f"            {field_name}.clear();\n"
            class_code += f"            for(auto& item : node[\"{key}\"]) {{\n"
            if elem_type in ["std::string", "bool", "double"]:
                class_code += f"                {field_name}.push_back(item.as<{elem_type}>());\n"
            else:
                # elem_type 是一个自定义类
                class_code += f"                {elem_type} elem;\n"
                class_code += f"                elem.update_from_yaml(item);\n"
                class_code += f"                {field_name}.push_back(elem);\n"
            class_code += f"            }}\n"
            class_code += f"        }}\n"
        elif field_type in ["std::string", "bool", "double"]:
            class_code += f"        if(node[\"{key}\"]) {field_name} = node[\"{key}\"].as<{field_type}>();\n"
        else:
            # field_type 是一个自定义类
            class_code += f"        if(node[\"{key}\"]) {field_name}.update_from_yaml(node[\"{key}\"]);\n"

    class_code += "    }\n};\n\n"

    # 先生成子类
    for key, value in data.items():
        if isinstance(value, dict):
            sub_class_name = capitalize(key)
            generate_class(sub_class_name, value)
        elif isinstance(value, list) and len(value) > 0 and isinstance(value[0], dict):
            # 假设列表名为复数形式，类名为单数
            sub_class_name = capitalize(key.rstrip('s'))
            generate_class(sub_class_name, value[0])

    # 将生成的类写入文件
    with open("generated_classes.hpp", "a") as f:
        f.write(class_code)

def main(yaml_file, root_class_name="Config"):
    # 清空输出文件
    with open("generated_classes.hpp", "w") as f:
        f.write("#pragma once\n#include <string>\n#include <vector>\n#include <yaml-cpp/yaml.h>\n\n")

    # 读取YAML文件
    with open(yaml_file, 'r') as f:
        data = yaml.safe_load(f)

    # 生成根类
    generate_class(root_class_name, data)
    print("C++ 类已生成到 'generated_classes.hpp' 文件中。")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("用法: python generate_cpp_classes.py <input.yaml>")
        sys.exit(1)
    yaml_file = sys.argv[1]
    main(yaml_file)
