import os
import subprocess

line_count = 0

dir_list = [
    "AST_classes",
    "Supporting_classes",
    "Visitor_classes"
]

file_list = ["compiler.cpp"]

for cur_dir in dir_list:
    for cur_file in os.listdir(cur_dir):
        result = subprocess.run(
            ["wc", "-l", cur_dir + "/" + cur_file], capture_output=True)
        line_count += int(result.stdout.split()[0])

for cur_file in file_list:
    result = subprocess.run(["wc", "-l", cur_file], capture_output=True)
    line_count += int(result.stdout.split()[0])

print("total number of lines in compiler project is " + str(line_count))
