import yaml

# 输入 YAML 文件名
input_yaml = "/home/lizhen/works/code/OpenRobot/OCM/examples/executer/yaml_template/task/task_config.yaml"

# 输出 C++ 文件名
output_cpp = "output.cpp"

# 读取 YAML 文件
def load_yaml(yaml_file):
    with open(yaml_file, 'r') as f:
        return yaml.safe_load(f)

# 生成 C++ 类的代码
def generate_cpp_class(class_name, data):
    class_code = f"class {class_name} {{\n"
    class_code += "public:\n"

    for key, value in data.items():
        cpp_type = get_cpp_type(value)
        class_code += f"    {cpp_type} {key};\n"

    class_code += "\n    // Constructor\n"
    class_code += f"    {class_name}() {{ }}\n"

    class_code += "\n    // Methods\n"
    class_code += "    void print() const {\n"
    class_code += "        // Print class data here\n"
    class_code += "    }\n"

    class_code += "};\n\n"
    return class_code

# 推断 C++ 类型
def get_cpp_type(value):
    if isinstance(value, int):
        return "int"
    elif isinstance(value, float):
        return "double"
    elif isinstance(value, str):
        return "std::string"
    elif isinstance(value, bool):
        return "bool"
    elif isinstance(value, list):
        return "std::vector<auto>"
    elif isinstance(value, dict):
        return "auto"
    else:
        return "auto"

# 遍历 YAML 生成 C++ 代码
def generate_cpp_code_from_yaml(data, base_class_name="Root"):
    cpp_code = ""
    classes_to_generate = [(base_class_name, data)]

    while classes_to_generate:
        class_name, class_data = classes_to_generate.pop()
        cpp_code += generate_cpp_class(class_name, class_data)

        # 如果是字典嵌套，生成子类
        for key, value in class_data.items():
            if isinstance(value, dict):
                classes_to_generate.append((key.capitalize(), value))

    return cpp_code

# 主程序
def main():
    data = load_yaml(input_yaml)
    cpp_code = generate_cpp_code_from_yaml(data)
    
    # 将生成的 C++ 代码写入文件
    with open(output_cpp, 'w') as f:
        f.write(cpp_code)

if __name__ == "__main__":
    main()
