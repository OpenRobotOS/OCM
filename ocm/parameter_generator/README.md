# 使用
```bash
python3 parameter_head_generator.py <input_path> <output_path>
```
# 注意
- 输入输出均为文件夹，会递归处理所有yaml文件，生成到一个hpp文件中
- ymal需符合规范，禁止无key的list！！！
错误：
```yaml
- 1
- 2
```
正确：
```yaml
a:
  - 1
  - 2
```