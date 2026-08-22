import os;
import SparkHeaderTool

def GetModuleName(path, module_map):
    return module_map[path]

def IncludeClass(path, module_map):
    if path.endswith(".ixx"):
        return f"import {GetModuleName(path, module_map)};\n"
    else:
        relpath = os.path.relpath(path, SparkHeaderTool.APP_SRC_DIR)
        return f"#include \"{relpath}\"\n"

def ProcessModule(line, path, module_map):
    module_name = ""
    start_index = len("export module ")
    end_index = len(line)
    for i in range(start_index, end_index):
        if line[i] == ";":
            break
        module_name += line[i]

    module_names = module_name.split(":")

    module_map[path] = module_names[0]
