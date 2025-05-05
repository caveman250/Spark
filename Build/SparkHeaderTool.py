import os
import sys
from dataclasses import dataclass

@dataclass
class ComponentFile:
    path: str
    name: str
    namespace: str

def MakeNamespace(namespace_stack):
    ret = ""
    for i in range(len(namespace_stack)):
        ret += namespace_stack[i]
        if i < len(namespace_stack) - 1:
            ret += "::"
    return ret

def ProcessWidget(line, widget_list, path, namespace_stack, components):
    start_index = len("DECLARE_SPARK_WIDGET_COMPONENT") + 1
    end_index = len(line)
    type = ""
    namespace = MakeNamespace(namespace_stack)
    for i in range(start_index, end_index):
        if line[i] == ")":
            widget_list.append(ComponentFile(os.path.abspath(path), type, namespace))
            break
        type += line[i]
    components.append(ComponentFile(os.path.abspath(path), type, namespace))

def IncludeWidgetFiles(widget_list):
    ret = ""
    for i in range(len(widget_list)):
        ret += "#include \"" + widget_list[i].path + "\"\n"
    return ret

def WriteWidgetTypesMacros(widget_list):
    ret = "\n#define SPARK_WIDGET_TYPES "
    for i in range(len(widget_list)):
        if len(widget_list[i].namespace) > 0:
            ret += widget_list[i].namespace + "::"
        ret += widget_list[i].name
        if i < len(widget_list) - 1:
            ret += ","

    ret += "\n#define SPARK_CONST_WIDGET_TYPES "
    for i in range(len(widget_list)):
        ret += "const "
        if len(widget_list[i].namespace) > 0:
            ret += widget_list[i].namespace + "::"
        ret += widget_list[i].name
        if i < len(widget_list) - 1:
            ret += ","

    ret += "\n#define SPARK_WIDGET_TYPES_WITH_NULLTYPE "
    for i in range(len(widget_list)):
        if len(widget_list[i].namespace) > 0:
            ret += widget_list[i].namespace + "::"
        ret += widget_list[i].name
        if i < len(widget_list) - 1:
            ret += ","
    ret += ",se::ecs::NullComponentType"

    ret += "\n#define SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE "
    for i in range(len(widget_list)):
        ret += "const "
        if len(widget_list[i].namespace) > 0:
            ret += widget_list[i].namespace + "::"
        ret += widget_list[i].name
        if i < len(widget_list) - 1:
            ret += ","
    ret += ",const se::ecs::NullComponentType"

    ret += "\n#define SPARK_WIDGET_POINTER_TYPES "
    for i in range(len(widget_list)):
        if len(widget_list[i].namespace) > 0:
            ret += widget_list[i].namespace + "::"
        ret += widget_list[i].name
        ret += "*"
        if i < len(widget_list) - 1:
            ret += ","

    ret += "\n#define SPARK_WIDGET_CONST_POINTER_TYPES "
    for i in range(len(widget_list)):
        ret += "const "
        if len(widget_list[i].namespace) > 0:
            ret += widget_list[i].namespace + "::"
        ret += widget_list[i].name
        ret += "*"
        if i < len(widget_list) - 1:
            ret += ","

    ret += "\n#define SPARK_WIDGET_POINTER_TYPES_WITH_NULLTYPE "
    for i in range(len(widget_list)):
        if len(widget_list[i].namespace) > 0:
            ret += widget_list[i].namespace + "::"
        ret += widget_list[i].name
        ret += "*"
        if i < len(widget_list) - 1:
            ret += ","
    ret += ",se::ecs::NullComponentType*"

    ret += "\n#define SPARK_WIDGET_CONST_POINTER_TYPES_WITH_NULLTYPE "
    for i in range(len(widget_list)):
        ret += "const "
        if len(widget_list[i].namespace) > 0:
            ret += widget_list[i].namespace + "::"
        ret += widget_list[i].name
        ret += "*"
        if i < len(widget_list) - 1:
            ret += ","
    ret += ",const se::ecs::NullComponentType*"

    return ret

def WriteWidgetVariant():
    ret = "\ntypedef std::variant<SPARK_WIDGET_POINTER_TYPES> WidgetVariant;"
    ret += "\ntypedef std::variant<SPARK_WIDGET_CONST_POINTER_TYPES> ConstWidgetVariant;"
    return ret

def WriteWidgetHeader(widget_list):
    contents = "#pragma once\n\n#include \"spark.h\"\n"

    contents += IncludeWidgetFiles(widget_list)
    contents += WriteWidgetTypesMacros(widget_list)
    contents += WriteWidgetVariant()

    output_dir = "../Engine/sht_generated/"
    output_path = "../Engine/sht_generated/Widgets.generated.h"
    if not os.path.exists(output_dir):
        os.mkdir(output_dir)

    existing_contents = ""
    if os.path.isfile(output_path):
        input_handle = open(output_path, "r")
        existing_contents = input_handle.read()
        input_handle.close()
    if existing_contents != contents:
        print("-- Widgets.generated.h generating...")
        output_handle = open(output_path, "w+")
        output_handle.write(contents)
        output_handle.close()
    else:
        print("-- Widgets.generated.h up to date. skipping.")

def ProcessNamespace(line, namespace_stack, namespace_scope_depth_stack, current_scope_depth):
    start_index = len("namespace") + 1
    end_index = len(line)
    namespace_scope_depth_stack.append(current_scope_depth + 1)
    namespace = ""
    for i in range(start_index, end_index):
        if line[i] == " " or line[i] == "\n" or line[i] == "{":
            break
        namespace += line[i]
    namespace_stack.append(namespace)

def ProcessComponent(line, components, path, namespace_stack):
    start_index = len("DECLARE_SPARK_COMPONENT") + 1
    end_index = len(line)
    type = ""
    for i in range(start_index, end_index):
        if line[i] == ")":
            break
        type += line[i]
    namespace = MakeNamespace(namespace_stack)
    components.append(ComponentFile(os.path.abspath(path), type, namespace))

def ProcessSingletonComponent(line, components, path, namespace_stack):
    start_index = len("DECLARE_SPARK_SINGLETON_COMPONENT") + 1
    end_index = len(line)
    type = ""
    for i in range(start_index, end_index):
        if line[i] == ")":
            break
        type += line[i]
    namespace = MakeNamespace(namespace_stack)
    components.append(ComponentFile(os.path.abspath(path), type, namespace))

def WriteComponentRegistrationFiles(components):
    output_dir = "../Engine/sht_generated/"

    header = "#pragma once\n\nnamespace se::ecs\n{\n\tclass World;\n\tvoid RegisterComponents(World* world);\n}"
    output_path = output_dir + "ComponentRegistration.generated.h"
    if not os.path.exists(output_dir):
        os.mkdir(output_dir)

    existing_contents = ""
    if os.path.isfile(output_path):
        input_handle = open(output_path, "r")
        existing_contents = input_handle.read()
        input_handle.close()
    if existing_contents != header:
        print("-- ComponentRegistration.generated.h generating...")
        output_handle = open(output_path, "w+")
        output_handle.write(header)
        output_handle.close()
    else:
        print("-- ComponentRegistration.generated.h up to date. skipping.")

    cpp = "#include \"ComponentRegistration.generated.h\"\n#include \"spark.h\"\n#include \"engine/reflect/Reflect.h\"\n"
    for i in range(len(components)):
        cpp += "#include \"" + components[i].path + "\"\n"
    cpp += "\nnamespace se::ecs\n{\n\t"
    cpp += "void RegisterComponents(World* world)\n\t{\n"
    for component in components:
        name = ""
        if len(component.namespace) > 0:
            name += component.namespace + "::"
        name += component.name
        cpp += f"\t\tworld->RegisterComponent<{name}>();\n"
    cpp += "\t}\n"
    cpp += "}\n"
    output_path = output_dir + "ComponentRegistration.generated.cpp"
    if not os.path.exists(output_dir):
        os.mkdir(output_dir)

    existing_contents = ""
    if os.path.isfile(output_path):
        input_handle = open(output_path, "r")
        existing_contents = input_handle.read()
        input_handle.close()
    if existing_contents != cpp:
        print("-- ComponentRegistration.generated.cpp generating...")
        output_handle = open(output_path, "w+")
        output_handle.write(cpp)
        output_handle.close()
    else:
        print("-- ComponentRegistration.generated.cpp up to date. skipping.")

def ProcessHeaders():
    components = []
    widgets = []
    source_dirs = sys.argv[1].split(':')

    for dir in source_dirs:
        for root, dirs, files in os.walk(dir):
            for file_path in files:
                if file_path.endswith(".h"):
                    current_scope_depth = 0
                    namespace_stack = []
                    namespace_scope_depth_stack = []
                    path = root + "/" + file_path
                    with open(path, 'r') as file:
                        for line in file:
                            line = line.strip()
                            if line.startswith("DECLARE_SPARK_WIDGET_COMPONENT"):
                                ProcessWidget(line, widgets, path, namespace_stack, components)
                            elif line.startswith("DECLARE_SPARK_COMPONENT"):
                                ProcessComponent(line, components, path, namespace_stack)
                            elif line.startswith("DECLARE_SPARK_SINGLETON_COMPONENT"):
                                ProcessSingletonComponent(line, components, path, namespace_stack)
                            elif line.startswith("namespace"):
                                ProcessNamespace(line, namespace_stack, namespace_scope_depth_stack, current_scope_depth)

                            for char in line:
                                if char == "{":
                                    current_scope_depth += 1
                                elif char == "}":
                                    if len(namespace_scope_depth_stack) > 0 and current_scope_depth == namespace_scope_depth_stack[-1]:
                                        namespace_scope_depth_stack.pop()
                                        namespace_stack.pop()
                                    current_scope_depth -= 1

    WriteWidgetHeader(widgets)
    WriteComponentRegistrationFiles(components)
if __name__ == '__main__':
    print("Running Spark Header Tool...")
    ProcessHeaders()
    print("-- done.\n")



