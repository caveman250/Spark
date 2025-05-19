from dataclasses import dataclass
import Namespace
import os
import Log

@dataclass
class ComponentFile:
    path: str
    name: str
    namespace: str

def ProcessComponent(components, path, class_stack):
    type = class_stack[-1].name
    namespace = class_stack[-1].namespace
    components.append(ComponentFile(os.path.abspath(path), type, namespace))

def ProcessSingletonComponent(components, path, class_stack):
    type = class_stack[-1].name
    namespace = class_stack[-1].namespace
    components.append(ComponentFile(os.path.abspath(path), type, namespace))

def WriteComponentRegistrationFiles(components):
    output_dir = "../../Engine/src/engine/generated/"

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
        Log.Msg("ComponentRegistration.generated.h generating...")
        output_handle = open(output_path, "w+")
        output_handle.write(header)
        output_handle.close()
    else:
        Log.Msg("ComponentRegistration.generated.h up to date. skipping.")

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
        Log.Msg("ComponentRegistration.generated.cpp generating...")
        output_handle = open(output_path, "w+")
        output_handle.write(cpp)
        output_handle.close()
    else:
        Log.Msg("ComponentRegistration.generated.cpp up to date. skipping.")