import os
from dataclasses import dataclass
from enum import Enum

import Namespace


@dataclass
class Member:
    name: str
    serialized: bool
    full_type_name: str

ClassType = Enum('ClassType',
                 [('CLASS', 1),
                           ('COMPONENT', 2),
                           ('WIDGET_COMPONENT', 3),
                           ('SINGLETON_COMPONENT', 4),
                           ('POD_CLASS', 5),
                           ('SYSTEM', 6),])

@dataclass
class Class:
    type: ClassType
    name: str
    path: str
    namespace: str
    abstract: bool
    members: list

def GetFullClassName(class_name, namespace_stack):
    if "::" not in class_name:
        namespace = Namespace.MakeNamespace(namespace_stack)
        if len(namespace) > 0:
           return namespace + "::" + class_name
        else:
            return class_name

    local_namespace = ""
    end_index = 0
    if '<' in class_name:
        for i in range(0, len(class_name)):
            if class_name[i] == '<':
                break
            end_index += 1
    else:
        end_index = len(class_name)

    str_minus_templates = class_name[0:end_index]
    local_namespace = str_minus_templates.rsplit("::", 1)

    namespace = Namespace.MakeNamespace(namespace_stack)
    if len(local_namespace) > 1 and len(local_namespace[1]) > 0:
        if len(namespace) > 0:
            return namespace + "::" + local_namespace[0] + "::" + local_namespace[1]
        else:
            return local_namespace[0] + "::" + class_name
    else:
        return class_name

def ProcessNativeClass(line, namespace_stack, class_list):
    start_index = len("class") + 1
    end_index = len(line)
    class_name = ""
    num_open_template_params = 0

    for i in range(start_index, end_index):
        if line[i] == '<':
            num_open_template_params += 1
        elif line[i] == '>':
            num_open_template_params -= 1

        if line[i] == " " and num_open_template_params == 0:
            break
        class_name += line[i]

    if len(class_name) > 0:
        class_list.append(GetFullClassName(class_name, namespace_stack))

def RemoveNamespaceAtIndex(type, index):
    split = type.split("::")
    while "<" in split[index] and index > 0:
        index -= 1
    del split[index]
    return "::".join(split)

def RemoveTemplateParams(type):
    start_remove = type.find("<")
    end_remove = type.rfind(">")
    if start_remove != -1 and end_remove != -1:
        copy = type[0:start_remove]
        copy += type[end_remove + 1: len(type)]
        return copy
    return type
def FindRealClassType(type, class_list, end_of_file_namespace_index, using_namespace_stack):
    while class_list.count(type) == 0:
        type = RemoveNamespaceAtIndex(type, end_of_file_namespace_index)
        type = RemoveTemplateParams(type)
        end_of_file_namespace_index -= 1
        if end_of_file_namespace_index <= 0:
            break #no more to remove

    if class_list.count(type) == 0:
        # try adding in our using namespaces
        # Note: not supporting joining nested usings together at this stage. see hwo annoying it becomes.
        for namespace_list in using_namespace_stack:
            for namespace in namespace_list:
                if class_list.count(namespace + "::" + type) != 0:
                    type = namespace + "::" + type
                    break
    return type

def CountNamespaces(namespace_str):
    split = namespace_str.split("::")
    return len(split)

def ProcessNativeClassInheritance(line, class_list, class_heirachy_map, namespace_stack, using_namespace_stack):
    start_index = len("class") + 1
    end_index = len(line)
    num_open_template_params = 0
    has_started_super = False
    has_started_inheritance_type = False
    class_name = ""
    inheritance_type = ""
    super_name = ""
    for i in range(start_index, end_index):
        if not has_started_inheritance_type:
            if line[i] == '<':
                num_open_template_params += 1
            elif line[i] == '>':
                num_open_template_params -= 1

            if line[i] == " " and num_open_template_params == 0:
                has_started_inheritance_type = True
                continue
            class_name += line[i]
        elif not has_started_super and has_started_inheritance_type:
            if line[i] == ':':
                continue
            if line[i] == ' ':
                if len(inheritance_type) == 0:
                    continue
                else:
                    has_started_super = True
                    continue
            inheritance_type += line[i]
            if not "private".startswith(inheritance_type) and not "public".startswith(inheritance_type):
                has_started_super = True
                super_name = inheritance_type
        elif has_started_super:
            if line[i] == '\n':
                break
            if line[i] == '<':
                num_open_template_params += 1
            elif line[i] == '>':
                num_open_template_params -= 1
            if line[i] == ' ' and num_open_template_params == 0:
                if len(super_name) == 0:
                    continue
                else:
                    break
            super_name += line[i]

    if len(super_name) > 0:
        last_namespace_idx = CountNamespaces(Namespace.MakeNamespace(namespace_stack)) - 1
        full_name = GetFullClassName(super_name, namespace_stack)
        class_heirachy_map[GetFullClassName(class_name, namespace_stack)] = FindRealClassType(full_name, class_list, last_namespace_idx, using_namespace_stack)

def ProcessClass(tag, line, class_stack, class_depth_stack, namespace_stack, current_scope_depth, filepath):
    if tag == "DECLARE_SPARK_CLASS_TEMPLATED":
        return #templated classes auto gen not supported yet.

    start_index = len(tag) + 1
    end_index = len(line)
    type = ""
    started_processing_decorators = False
    current_decorator = ""
    decorators = []
    for i in range(start_index, end_index):
        if line[i] == " ":
            continue
        if line[i] == ',':
            if not started_processing_decorators:
                started_processing_decorators = True
                continue
            else:
                decorators.append(current_decorator)
                current_decorator = ""

        if line[i] == ")":
            if started_processing_decorators:
                decorators.append(current_decorator)
            break

        if started_processing_decorators:
            current_decorator += line[i]
        else:
            type += line[i]

    is_abstract = decorators.count("Abstract") > 0
    namespace = Namespace.MakeNamespace(namespace_stack)

    class_type = ClassType.CLASS
    if tag == "DECLARE_SPARK_CLASS":
        class_type = ClassType.CLASS
    elif tag == "DECLARE_SPARK_SYSTEM":
        class_type = ClassType.SYSTEM
    elif tag == "DECLARE_SPARK_POD_CLASS":
        class_type = ClassType.POD_CLASS
    elif tag == "DECLARE_SPARK_SINGLETON_COMPONENT":
        class_type = ClassType.SINGLETON_COMPONENT
    elif tag == "DECLARE_SPARK_COMPONENT":
        class_type = ClassType.COMPONENT
    elif tag == "DECLARE_SPARK_WIDGET_COMPONENT":
        class_type = ClassType.WIDGET_COMPONENT

    class_stack.append(Class(class_type, type, filepath, namespace, is_abstract, []))
    class_depth_stack.append(current_scope_depth)

def ProcessMember(line, next_line, class_stack, namespace_stack, class_list, using_namespace_stack):
    start_index = len("SPARK_MEMBER") + 1
    end_index = len(line)
    currentDecorator = ""
    decorators = []

    for i in range(start_index, end_index):
        if line[i] == ")":
            if len(currentDecorator) > 0:
                decorators.append(currentDecorator)
            break
        if line[i] == ",":
            decorators.append(currentDecorator)
            currentDecorator = ""
            continue
        currentDecorator += line[i]

    start_index = 0
    end_index = len(next_line)
    num_open_template_params = 0
    has_begun_type = False
    has_begun_name = False
    type = ""
    name = ""
    for i in range(start_index, end_index):
        if has_begun_name:
            if next_line[i] == ' ' or next_line[i] == ';':
                break
            name += next_line[i]
            continue
        elif not has_begun_type:
            if next_line[i] != ' ':
                has_begun_type = True

        if has_begun_type:
            if next_line[i] == ' ' and num_open_template_params == 0:
                has_begun_name = True
            else:
                type += next_line[i]

        if not has_begun_name:
            if next_line[i] == '<':
                num_open_template_params += 1
            elif next_line[i] == '>':
                num_open_template_params -= 1

    last_namespace_idx = CountNamespaces(Namespace.MakeNamespace(namespace_stack)) - 1
    member_full_type = GetFullClassName(type, namespace_stack)
    member_full_type = FindRealClassType(member_full_type, class_list, last_namespace_idx, using_namespace_stack)

    serialized = decorators.count("Serialized") > 0
    class_stack[-1].members.append(Member(name, serialized, member_full_type))

def GetFullnameFromClassObject(class_obj: Class):
    if len(class_obj.namespace) > 0:
        return class_obj.namespace + "::" + class_obj.name
    return class_obj.name

def DefineAbstractClassBegin(class_name):
    return f"""DEFINE_SPARK_TYPE({class_name})
    se::reflect::Class* {class_name}::GetReflection()
    {{
        static se::reflect::Class* s_Reflection = nullptr;
        if (!s_Reflection)
        {{
            s_Reflection = new se::reflect::Class();
            se::reflect::TypeLookup::GetTypeMap()["{class_name}"] = s_Reflection;
            s_Reflection->name = \"{class_name}\";
            s_Reflection->size = sizeof({class_name});
            s_Reflection->heap_constructor = nullptr;
            s_Reflection->inplace_constructor = nullptr;
            s_Reflection->heap_copy_constructor = nullptr;
            s_Reflection->inplace_copy_constructor = nullptr;
            s_Reflection->destructor = nullptr;
            s_Reflection->members = {{}};
        }}
        return s_Reflection;
    }}
    void {class_name}::InitMembers()
    {{
        GetReflection()->members =
        {{"""

def DefineClassBeginCommon(class_name):
    return f"""    se::reflect::Class* {class_name}::GetReflection() 
    {{
        static se::reflect::Class* s_Reflection = nullptr;
        if (!s_Reflection)
        {{
            s_Reflection = new se::reflect::Class();
            se::reflect::TypeLookup::GetTypeMap()["{class_name}"] = s_Reflection;
            s_Reflection->name = "{class_name}"; 
            s_Reflection->size = sizeof({class_name});                           
            s_Reflection->has_default_constructor = std::is_default_constructible<{class_name}>::value; 
            CreateDefaultConstructorMethods<{class_name}>(s_Reflection);
            s_Reflection->heap_copy_constructor = [](void* other){{ return new {class_name}(*reinterpret_cast<{class_name}*>(other)); }}; 
            s_Reflection->inplace_copy_constructor = [](void* mem, void* other){{ return new(mem) {class_name}(*reinterpret_cast<{class_name}*>(other)); }};
            s_Reflection->destructor = [](void* data){{ reinterpret_cast<{class_name}*>(data)->~{class_name}(); }};
            s_Reflection->members = {{}};
        }}
        return s_Reflection;
    }}
    void {class_name}::InitMembers()
    {{
        GetReflection()->members = 
        {{\n"""

def DefineClassBegin(class_name):
    return f"    DEFINE_SPARK_TYPE({class_name})\n" + DefineClassBeginCommon(class_name)

def DefineClassEnd(class_name):
    return f"""        }};
    }}
    
    static auto SPARK_CAT(SPARK_CAT(type, Reflection), __COUNTER__) = {class_name}::GetReflection(); // ensure reflection has been initialised.
}}
"""

def DefinePODClassBegin(class_name):
    return f"    size_t {class_name}::s_StaticId = typeid({class_name}).hash_code();\n" + DefineClassBeginCommon(class_name)

def DefineComponentBegin(class_name):
    return f"    DEFINE_SPARK_TYPE({class_name})\nse::ecs::Id {class_name}::s_ComponentId = {{}};" + DefineClassBeginCommon(class_name)

def DefineSystem(class_name):
    return DefineClassBegin(class_name) + DefineClassEnd(class_name)

def DefineMember(class_name, name, serialized):
    bool_val = "true" if serialized else "false"
    return f"            {{\"{name}\", se::reflect::TypeResolver<decltype({class_name}::{name})>::get(), [](const void* obj){{ return (void*)&(({class_name}*)obj)->{name}; }},{bool_val}}},\n"

def WriteClassFiles(classes, base_class_map):
    output_dir = "../../Engine/src/generated/"

    init_members_h_content = "namespace se\n{\nvoid InitClassReflection();\n}"
    output_path = output_dir + "Classes.generated.h"
    existing_contents = ""
    if os.path.isfile(output_path):
        input_handle = open(output_path, "r")
        existing_contents = input_handle.read()
        input_handle.close()
    if existing_contents != init_members_h_content:
        print(f"-- -- Classes.generated.h generating...")
        output_handle = open(output_path, "w+")
        output_handle.write(init_members_h_content)
        output_handle.close()

    init_members_cpp_content = "#include \"spark.h\"\n"
    for full_name, class_obj in classes.items():
        init_members_cpp_content += f"#include \"{class_obj.path}\"\n"
    init_members_cpp_content += "\nnamespace se\n{\nvoid InitClassReflection()\n{\n"
    for full_name, class_obj in classes.items():
        init_members_cpp_content += "    " + full_name + "::InitMembers();\n"
    init_members_cpp_content += "}\n}"

    output_path = output_dir + "Classes.generated.cpp"
    existing_contents = ""
    if os.path.isfile(output_path):
        input_handle = open(output_path, "r")
        existing_contents = input_handle.read()
        input_handle.close()
    if existing_contents != init_members_cpp_content:
        print(f"-- -- Classes.generated.cpp generating...")
        output_handle = open(output_path, "w+")
        output_handle.write(init_members_cpp_content)
        output_handle.close()

    for full_name, classobj in classes.items():
        contents = f"#include \"spark.h\"\n#include \"engine/reflect/Reflect.h\"\n#include \"{classobj.path}\"\n"

        includes = []
        current_type = full_name
        while current_type in classes:
            class_obj = classes[current_type]
            for member in class_obj.members:
                if member.full_type_name in classes:
                    if includes.count(classes[member.full_type_name].path) == 0:
                        includes.append(classes[member.full_type_name].path)
            if current_type in base_class_map:
                current_type = base_class_map[current_type]
            else:
                break

        includes.sort()
        for include in includes:
            contents += f"#include \"{include}\"\n"

        contents += f"\nnamespace {classobj.namespace}\n{{\n"

        if classobj.type == ClassType.CLASS:
            if classobj.abstract:
                contents += DefineAbstractClassBegin(classobj.name)
            else:
                contents += DefineClassBegin(classobj.name)
        elif classobj.type == ClassType.SYSTEM:
            contents += DefineSystem(classobj.name)
        elif classobj.type == ClassType.POD_CLASS:
            contents += DefinePODClassBegin(classobj.name)
        elif classobj.type ==  ClassType.COMPONENT:
            contents += DefineComponentBegin(classobj.name)
        elif classobj.type == ClassType.WIDGET_COMPONENT:
            contents += DefineComponentBegin(classobj.name)
        elif classobj.type == ClassType.SINGLETON_COMPONENT:
            contents += DefineComponentBegin(classobj.name)

        if classobj.type != ClassType.SYSTEM:
            current_type = full_name
            while current_type in classes:
                class_obj = classes[current_type]
                for member in class_obj.members:
                    contents += DefineMember(classobj.name, member.name, member.serialized)
                if current_type in base_class_map:
                    current_type = base_class_map[current_type]
                else:
                    break

            contents += DefineClassEnd(classobj.name)

        
        namespace_text = classobj.namespace.replace("::", "_")
        output_path = output_dir + f"{namespace_text}_{classobj.name}.generated.cpp"
        existing_contents = ""
        if os.path.isfile(output_path):
            input_handle = open(output_path, "r")
            existing_contents = input_handle.read()
            input_handle.close()
        if existing_contents != contents:
            print(f"-- -- {namespace_text}_{classobj.name}.generated.h generating...")
            output_handle = open(output_path, "w+")
            output_handle.write(contents)
            output_handle.close()