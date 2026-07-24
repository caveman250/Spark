from dataclasses import dataclass
from src import Namespace
import os
from src import Log

@dataclass
class ComponentFile:
    path: str
    name: str
    namespace: str
    dev_only: bool
    editor_only: bool
    project_src_dir: str

def IncludeComponent(component):
    if component.path.endswith(".ixx"):
        return f"import {component.name};\n"
    else:
        return f"#include \"{component.path}\"\n"

def ProcessComponent(components, path, class_stack, source_dir):
    type = class_stack[-1].name
    namespace = class_stack[-1].namespace
    dev_only = class_stack[-1].dev_only
    editor_only = class_stack[-1].editor_only
    components.append(ComponentFile(os.path.abspath(path), type, namespace, dev_only, editor_only, source_dir))

def ProcessSingletonComponent(components, path, class_stack, source_dir):
    type = class_stack[-1].name
    namespace = class_stack[-1].namespace
    dev_only = class_stack[-1].dev_only
    editor_only = class_stack[-1].editor_only
    components.append(ComponentFile(os.path.abspath(path), type, namespace, dev_only, editor_only, source_dir))

def WriteComponentsFile(components, files_accounted_for):
    src_output_dir = "../../../app/src/generated/"
    header_output_dir = "../../engine/src/generated/"

    header = f"#pragma once\n\nnamespace se::ecs\n{{\n\tstatic constexpr size_t NumComponents = {len(components)};\n\tclass World;\n\tvoid RegisterComponents(World* world);\n}}"
    output_path = header_output_dir + "Components.generated.h"
    files_accounted_for.add(os.path.abspath(output_path))
    if not os.path.exists(header_output_dir):
        os.mkdir(header_output_dir)

    existing_contents = ""
    if os.path.isfile(output_path):
        input_handle = open(output_path, "r")
        existing_contents = input_handle.read()
        input_handle.close()
    if existing_contents != header:
        Log.Msg("Components.generated.h generating...")
        output_handle = open(output_path, "w+")
        output_handle.write(header)
        output_handle.close()

    cpp = "#include \"Components.generated.h\"\n#include \"spark.h\"\n#include \"engine/reflect/Reflect.h\"\n"
    for i in range(len(components)):
        cpp += IncludeComponent(components[i])
    cpp += "\nnamespace se::ecs\n{\n\t"
    cpp += "void RegisterComponents(World* world)\n\t{\n"
    for component in components:
        name = ""
        if len(component.namespace) > 0:
            name += component.namespace + "::"
        name += component.name
        if component.dev_only:
            cpp += "#if WITH_DEV_ONLY_CLASSES\n"
        if component.editor_only:
            cpp += "#if WITH_EDITOR_ONLY_CLASSES\n"
        cpp += f"\t\tworld->RegisterComponent<{name}>();\n"
        if component.editor_only:
            cpp += "#endif\n"
        if component.dev_only:
            cpp += "#endif\n"
    cpp += "\t}\n"
    cpp += "}\n"
    output_path = src_output_dir + "Components.generated.cpp"
    files_accounted_for.add(os.path.abspath(output_path))
    if not os.path.exists(src_output_dir):
        os.mkdir(src_output_dir)

    existing_contents = ""
    if os.path.isfile(output_path):
        input_handle = open(output_path, "r")
        existing_contents = input_handle.read()
        input_handle.close()
    if existing_contents != cpp:
        Log.Msg("Components.generated.cpp generating...")
        output_handle = open(output_path, "w+")
        output_handle.write(cpp)
        output_handle.close()