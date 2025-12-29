from dataclasses import dataclass
from src import Namespace
import os

@dataclass
class Enum:
    name: str
    path: str
    namespace: str
    values: list
    project_src_dir:str

def ProcessEnum(next_line, enum_list, lines, line_index, namespace_stack, filepath, source_dir):
    start_index = len("enum class ")
    end_index = len(next_line)
    enum = ""
    for i in range(start_index, end_index):
        if next_line[i] == " ":
            break
        enum += next_line[i]

    namespace = Namespace.MakeNamespace(namespace_stack)
    values = []
    for i in range(line_index + 3, len(lines)):
        line = lines[i].strip()
        if line.startswith("};"):
            break
        val = line
        if "=" in line:
            val = line.split("=")[0]
        if val.endswith("\n"):
            val = val[:-1]
        if val.endswith(","):
            val = val[:-1]
        values.append(val)

    enum_list.append(Enum(enum, filepath, namespace, values, source_dir))

"""namespace se::reflect 
{ 
    template <> 
    Type* getPrimitiveDescriptor<editor::ui::properties::PropertyTitleMode>()
    {
        static se::reflect::Enum* s_Instance = nullptr;
        if (!s_Instance)
        {
            s_Instance = new se::reflect::Enum();
            se::reflect::TypeLookup::GetTypeMap()["se::editor::ui::properties::PropertyTitleMode"] = s_Instance;
            s_Instance->name = "se::editor::ui::properties::PropertyTitleMode";
            s_Instance->size = sizeof(se::editor::ui::properties::PropertyTitleMode);
            s_Instance->values =
            {
                EnumValue{"Inline", static_cast<int>(se::editor::ui::properties::PropertyTitleMode::Inline)},
                EnumValue{"NextLine", static_cast<int>(se::editor::ui::properties::PropertyTitleMode::NextLine)},
            };
        }
        return s_Instance;
    } 
}"""
    
def DefineEnumBegin(enum):
    full_enum_name = enum.namespace + "::" + enum.name
    return f"""
namespace se::reflect 
{{ 
    template <> 
    Type* getPrimitiveDescriptor<{full_enum_name}>() 
    {{ 
        static se::reflect::Enum* s_Instance = nullptr; 
        if (!s_Instance) 
        {{
            s_Instance = new se::reflect::Enum(); 
            se::reflect::TypeLookup::GetTypeMap()["{full_enum_name}"] = s_Instance; 
            s_Instance->name = "{full_enum_name}"; 
            s_Instance->size = sizeof({full_enum_name}); 
            s_Instance->values = 
            {{\n"""

def DefineEnumValue(enum, value):
    full_enum_name = enum.namespace + "::" + enum.name
    return f"            {{\"{value}\", static_cast<int>({full_enum_name}::{value})}},\n"

def DefineEnumEnd():
    return f"""            }};
        }}
        return s_Instance;
    }}
}}"""

def WriteEnumFiles(enum_list):
    for enum in enum_list:
        contents = f"#include \"spark.h\"\n#include \"engine/reflect/Reflect.h\"\n#include \"{enum.path}\"\n\n"
        contents += DefineEnumBegin(enum)
        for value in enum.values:
            contents += DefineEnumValue(enum, value)
        contents += DefineEnumEnd()

        namespace_text = enum.namespace.replace("::", "_")
        output_path = enum.project_src_dir + f"{namespace_text}_{enum.name}.generated.cpp"
        existing_contents = ""
        if os.path.isfile(output_path):
            input_handle = open(output_path, "r")
            existing_contents = input_handle.read()
            input_handle.close()
        if existing_contents != contents:
            print(f"-- -- {namespace_text}_{enum.name}.generated.cpp generating...")
            output_handle = open(output_path, "w+")
            output_handle.write(contents)
            output_handle.close()