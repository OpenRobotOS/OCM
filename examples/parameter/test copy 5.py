import yaml
import sys
import os

# 用于存储已生成的类，避免重复生成
generated_classes = {}
# 用于存储每个YAML文件对应的顶级类全限定名
generated_top_classes = []

def capitalize(name):
    """将字段名转换为类名，首字母大写"""
    return ''.join(word.capitalize() for word in name.replace('-', '_').split('_'))

def determine_type(name, value, namespaces):
    """确定YAML值对应的C++类型"""
    if isinstance(value, bool):
        return "bool"
    elif isinstance(value, int):
        return "int"
    elif isinstance(value, float):
        return "double"
    elif isinstance(value, str):
        return "std::string"
    elif isinstance(value, list):
        return get_vector_type(name, value, namespaces)
    elif isinstance(value, dict):
        class_name = capitalize(name)
        generate_class(class_name, value, namespaces)
        return class_name
    elif value is None:
        return "std::string"  # 默认处理Null为string
    else:
        return "std::string"  # 默认类型

def get_vector_type(name, lst, namespaces):
    """确定列表中元素的C++类型"""
    if not lst:
        return "std::vector<std::string>"  # 默认类型

    first_elem = lst[0]
    elem_type = determine_type(name, first_elem, namespaces)

    # 检查所有元素类型是否一致
    for item in lst:
        current_type = determine_type(name, item, namespaces)
        if current_type != elem_type:
            return "std::vector<std::string>"  # 默认类型

    return f"std::vector<{elem_type}>"

def sanitize_field_name(name):
    """将字段名转换为有效的C++变量名"""
    # 替换非字母数字字符为下划线
    return ''.join(['_' if not c.isalnum() else c for c in name])

def generate_class(class_name, data, namespaces):
    """生成C++类代码并写入文件"""
    # 构建全限定类名以避免命名冲突
    full_class_name = '::'.join(namespaces + [class_name])
    if full_class_name in generated_classes:
        return  # 已生成，跳过

    generated_classes[full_class_name] = True

    member_vars = []
    update_lines = []

    for key, value in data.items():
        field_name = sanitize_field_name(key)
        field_type = determine_type(key, value, namespaces)
        member_vars.append(f"        {field_type} {field_name};")

    # 开始生成类代码
    class_code = []
    class_code.append(f"class {class_name} {{")
    class_code.append("public:")
    # 成员变量
    class_code.extend(member_vars)
    class_code.append("")
    # update_from_yaml 方法
    class_code.append("        void update_from_yaml(const YAML::Node& node) {")

    for key, value in data.items():
        field_name = sanitize_field_name(key)
        field_type = determine_type(key, value, namespaces)

        if field_type.startswith("std::vector<"):
            elem_type = field_type[len("std::vector<"):-1]
            class_code.append(f"            if(node[\"{key}\"]) {{")
            class_code.append(f"                {field_name}.clear();")
            class_code.append(f"                for(auto& item : node[\"{key}\"]) {{")
            if elem_type in ["std::string", "bool", "int", "double"]:
                class_code.append(f"                    {field_name}.push_back(item.as<{elem_type}>());")
            else:
                # elem_type 是一个自定义类
                class_code.append(f"                    {elem_type} elem;")
                class_code.append(f"                    elem.update_from_yaml(item);")
                class_code.append(f"                    {field_name}.push_back(elem);")
            class_code.append(f"                }}")
            class_code.append(f"            }}")
        elif field_type in ["std::string", "bool", "int", "double"]:
            class_code.append(f"            if(node[\"{key}\"]) {field_name} = node[\"{key}\"].as<{field_type}>();")
        else:
            # field_type 是一个自定义类
            class_code.append(f"            if(node[\"{key}\"]) {field_name}.update_from_yaml(node[\"{key}\"]);")  # 修正了这里

    class_code.append("        }")
    class_code.append("};")
    class_code.append("")  # 空行

    # 先生成子类
    for key, value in data.items():
        if isinstance(value, dict):
            sub_class_name = capitalize(key)
            generate_class(sub_class_name, value, namespaces)
        elif isinstance(value, list) and len(value) > 0 and isinstance(value[0], dict):
            # 假设列表名为复数形式，类名为单数
            sub_class_name = capitalize(key.rstrip('s'))
            generate_class(sub_class_name, value[0], namespaces)

    # 将生成的类写入文件，带有命名空间
    with open("generated_classes.hpp", "a") as f:
        # 写入命名空间开始
        for ns in namespaces:
            f.write(f"namespace {ns} {{\n")
        # 写入类代码
        for line in class_code:
            f.write(line + "\n")
        # 写入命名空间结束
        for ns in reversed(namespaces):
            f.write(f"}} // namespace {ns}\n")
        f.write("\n")  # 空行

def print_generated_classes():
    """打印每个YAML文件对应的同名顶级类，包含所有命名空间和类名，使用 :: 分隔"""
    print("生成的C++顶级类及其命名空间结构:")
    for class_name in generated_top_classes:
        print(class_name)

def generate_config_collect_class():
    """在 generated_classes.hpp 中生成 ConfigCollect 类，包含所有顶级类的成员变量和 update_from_yaml 方法"""
    if not generated_top_classes:
        print("没有生成任何顶级类，跳过 ConfigCollect 类的生成。")
        return

    with open("generated_classes.hpp", "a") as f:
        # 开始生成 ConfigCollect 类
        f.write("class ConfigCollect {\npublic:\n")
        # 添加成员变量
        for full_class_name in generated_top_classes:
            var_name = full_class_name.replace("::", "_")
            f.write(f"    {full_class_name} {var_name};\n")
        f.write("\n")

        # 添加 update_from_yaml 方法
        f.write("    void update_from_yaml(const std::string& base_path) {\n")
        f.write("        YAML::Node node;\n\n")
        for full_class_name in generated_top_classes:
            # 构建 YAML 文件路径
            parts = full_class_name.split("::")
            if len(parts) < 2:
                # 如果只有一个部分，直接使用类名作为文件名
                yaml_path = f"base_path + \"/{parts[0]}.yaml\""
            else:
                # YAML 文件路径由命名空间部分构成的文件夹加上前一个类名作为文件名
                # 例如，ActionManager::Test2::B::B 对应的 YAML 文件路径为 ActionManager/Test2/B.yaml
                dir_parts = parts[:-2] if len(parts) > 2 else []
                file_name = parts[-2]  # 前一个部分作为文件名
                if dir_parts:
                    dir_path = "/".join(dir_parts)
                    yaml_path = f"base_path + \"/{dir_path}/{file_name}.yaml\""
                else:
                    yaml_path = f"base_path + \"/{file_name}.yaml\""

            var_name = full_class_name.replace("::", "_")
            f.write(f"        // 更新 {full_class_name}\n")
            f.write(f"        node = YAML::LoadFile({yaml_path});\n")
            f.write(f"        {var_name}.update_from_yaml(node);\n\n")
        f.write("    }\n")
        f.write("};\n\n")
    print("已生成 ConfigCollect 类。")

def main(input_path):
    # 清空输出文件并写入头文件内容
    with open("generated_classes.hpp", "w") as f:
        f.write("#pragma once\n#include <string>\n#include <vector>\n#include <yaml-cpp/yaml.h>\n\n")

    # 遍历输入路径
    for root, dirs, files in os.walk(input_path):
        for file in files:
            if file.endswith(('.yaml', '.yml')):
                yaml_file = os.path.join(root, file)
                # 计算相对于输入路径的相对路径
                rel_path = os.path.relpath(root, input_path)
                # 生成命名空间列表
                if rel_path == '.' or rel_path == '':
                    namespaces = []
                else:
                    # 将相对路径分割为各个部分，并转换为命名空间
                    namespaces = [capitalize(part) for part in rel_path.replace('-', '_').split(os.sep)]
                # 使用文件名（不含扩展名）作为额外的命名空间
                base_name = os.path.splitext(file)[0]
                file_namespace = capitalize(base_name)
                all_namespaces = namespaces + [file_namespace]
                # 类名与文件名相同
                class_name = file_namespace
                # 读取YAML文件
                try:
                    with open(yaml_file, 'r') as f_yaml:
                        data = yaml.safe_load(f_yaml)
                        if data is None:
                            print(f"YAML 文件 {yaml_file} 是空的，跳过。")
                            continue
                except yaml.YAMLError as e:
                    print(f"解析 YAML 文件 {yaml_file} 时出错: {e}")
                    continue
                except Exception as e:
                    print(f"读取文件 {yaml_file} 时出错: {e}")
                    continue
                # 生成类
                generate_class(class_name, data, all_namespaces)
                # 构建全限定类名，包括类名本身
                full_class_name = '::'.join(all_namespaces + [class_name])
                generated_top_classes.append(full_class_name)

    # 打印生成的类
    print_generated_classes()
    # 生成 ConfigCollect 类
    generate_config_collect_class()
    print("C++ 类已生成到 'generated_classes.hpp' 文件中。")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("用法: python generate_cpp_classes.py <input_directory>")
        sys.exit(1)
    input_path = sys.argv[1]
    if not os.path.isdir(input_path):
        print(f"输入路径 '{input_path}' 不是一个有效的目录。")
        sys.exit(1)
    main(input_path)