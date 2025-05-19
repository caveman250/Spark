from dataclasses import dataclass
import Namespace
import os

@dataclass
class Enum:
    name: str
    path: str
    namespace: str
    values: list
    project_src_dir:str

def ProcessEnum(line, enum_list, lines, line_index, namespace_stack, filepath, source_dir):
    start_index = len("SPARK_ENUM_BEGIN") + 1
    end_index = len(line)
    enum = ""
    for i in range(start_index, end_index):
        if line[i] == ",":
            break
        enum += line[i]

    namespace = Namespace.MakeNamespace(namespace_stack)
    values = []
    for i in range(line_index + 1, len(lines)):
        line = lines[i].strip()
        if line.startswith("SPARK_ENUM_END"):
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
    
def DefineEnumBegin(enum):
    full_enum_name = enum.namespace + "::" + enum.name
    return f"""DEFINE_SPARK_TYPE({full_enum_name}) 
std::string {full_enum_name}::ToString({full_enum_name}::Type val) 
{{ 
    se::reflect::Enum* enumReflection = se::reflect::EnumResolver<{full_enum_name}>::get(); 
    return enumReflection->ToString(val); 
}} 
{full_enum_name}::Type {full_enum_name}::FromString(const std::string& str) 
{{ 
    se::reflect::Enum* enumReflection = se::reflect::EnumResolver<{full_enum_name}>::get(); 
    return static_cast<{full_enum_name}::Type>(enumReflection->FromString(str)); 
}} 
namespace se::reflect 
{{ 
    template <> 
    Type* getPrimitiveDescriptor<{full_enum_name}::Type>() 
    {{ 
        return TypeResolver<{full_enum_name}>::get(); 
    }} 
}} 
size_t {full_enum_name}::ValuesCount() 
{{ 
    se::reflect::Enum* enumReflection = se::reflect::EnumResolver<{full_enum_name}>::get(); 
    return enumReflection->values.size(); 
}} 
se::reflect::Enum* {full_enum_name}::GetReflection() 
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
    return f"            {{\"{value}\", {enum.name}::{value}}},\n"

def DefineEnumEnd():
    return f"""        }};
    }}
    return s_Instance;
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