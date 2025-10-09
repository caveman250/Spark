import os
from dataclasses import dataclass
from enum import Enum
import Log
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
                           ('SYSTEM', 6),
                           ('TEMPLATED_CLASS', 6),])

@dataclass
class Class:
    type: ClassType
    name: str
    path: str
    namespace: str
    abstract: bool
    members: list
    is_reflected: bool
    is_template: bool
    template_params: list
    project_src_dir: str

@dataclass
class TemplateInstantiation:
    namespace: str
    class_name: str
    template_types: list
    filepath: str
    project_src_dir: str

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

def ProcessNativeClassFirstPass(tag, line, namespace_stack, class_list):
    # ignore forward decs
    if line.endswith(";"):
        return

    start_index = len(tag) + 1
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

    if class_list is not None and len(class_name) > 0:
        if class_list is not None:
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

def ProcessNativeClassSecondPass(tag, line, line_before, class_list, class_heirachy_map, namespace_stack, using_namespace_stack, class_stack, class_depth_stack, current_scope_depth, filepath, source_dir):
    # ignore forward decs
    if line.endswith(";"):
        return

    start_index = len(tag) + 1
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

    template_params = []
    if line_before.startswith("template <") or line_before.startswith("template<"):
        start_index = len("template <") if line_before.startswith("template <") else len("template<")
        end_index = len(line_before)
        num_open_template_params = 0
        current_template_param = ""
        for i in range(start_index, end_index):
            if line_before[i] == '<':
                num_open_template_params += 1
            elif line_before[i] == '>':
                num_open_template_params -= 1
                if num_open_template_params == -1 and len(current_template_param) > 0:
                    template_params.append(current_template_param)
                    current_template_param = ""

            if line_before[i] == "," and num_open_template_params == 0:
                template_params.append(current_template_param)
                current_template_param = ""
            else:
                current_template_param += line_before[i]

    if len(super_name) > 0:
        last_namespace_idx = CountNamespaces(Namespace.MakeNamespace(namespace_stack)) - 1
        full_name = GetFullClassName(super_name, namespace_stack)
        class_heirachy_map[GetFullClassName(class_name, namespace_stack)] = FindRealClassType(full_name, class_list, last_namespace_idx, using_namespace_stack)

    if len(class_name) > 0:
        class_stack.append(Class(ClassType.CLASS, class_name, filepath, Namespace.MakeNamespace(namespace_stack), False, [], False, len(template_params) > 0, template_params, source_dir))
        class_depth_stack.append(current_scope_depth + 1)

def IsBuiltinPrimitive(type):
    return (type == "bool" or
        type == "char" or
        type == "int" or
        type == "int8_t" or
        type == "uint8_t" or
        type == "int16_t" or
        type == "uint16_t" or
        type == "int32_t" or
        type == "uint32_t" or
        type == "int64_t" or
        type == "uint64_t" or
        type == "size_t" or
        type == "double" or
        type == "float" or
        type == "std::string")

def ProcessInstantiateTemplate(line, template_instantiations, namespace_stack, filepath, class_list, using_namespace_stack, source_dir):
    start_index = len("SPARK_INSTANTIATE_TEMPLATE") + 1
    end_index = len(line)
    num_open_template_params = 0
    has_started_template_types = False
    class_name = ""
    template_types = []
    current_template_type = ""
    for i in range(start_index, end_index):
        if not has_started_template_types:
            if line[i] == ",":
                has_started_template_types = True
            else:
                class_name += line[i]
        else:
            if line[i] == ' ':
                continue
            if line[i] == '<':
                num_open_template_params += 1
            elif line[i] == '>':
                num_open_template_params -= 1
                if num_open_template_params == 0:
                    template_types.append(current_template_type)
                    current_template_type = ""

            if line[i] == ',' and num_open_template_params == 0:
                template_types.append(current_template_type)
                current_template_type = ""
            elif line[i] == ')':
                template_types.append(current_template_type)
                current_template_type = ""
                break
            else:
                current_template_type += line[i]

    for i in range(0, len(template_types)):
        template_type = template_types[i]
        if not IsBuiltinPrimitive(template_type):
            last_namespace_idx = CountNamespaces(Namespace.MakeNamespace(namespace_stack)) - 1
            template_type = GetFullClassName(template_type, namespace_stack)
            template_type = FindRealClassType(template_type, class_list, last_namespace_idx, using_namespace_stack)
            template_types[i] = template_type

    template_instantiations.append(TemplateInstantiation(Namespace.MakeNamespace(namespace_stack), class_name, template_types, filepath, source_dir))

def ProcessClass(tag, line, class_stack):
    if tag == "SPARK_CLASS_TEMPLATED":
        return #templated classes auto gen not supported yet.

    start_index = len(tag) + 1
    end_index = len(line)
    current_decorator = ""
    decorators = []
    for i in range(start_index, end_index):
        if line[i] == " ":
            continue
        if line[i] == ',':
            decorators.append(current_decorator)
            current_decorator = ""

        if line[i] == ")":
            decorators.append(current_decorator)
            break

        current_decorator += line[i]

    is_abstract = decorators.count("Abstract") > 0

    class_type = ClassType.CLASS
    if tag == "SPARK_CLASS":
        class_type = ClassType.CLASS
    elif tag == "SPARK_SYSTEM":
        class_type = ClassType.SYSTEM
    elif tag == "SPARK_POD_CLASS":
        class_type = ClassType.POD_CLASS
    elif tag == "SPARK_SINGLETON_COMPONENT":
        class_type = ClassType.SINGLETON_COMPONENT
    elif tag == "SPARK_COMPONENT":
        class_type = ClassType.COMPONENT
    elif tag == "SPARK_WIDGET_COMPONENT":
        class_type = ClassType.WIDGET_COMPONENT

    class_stack[-1].type = class_type
    class_stack[-1].abstract = is_abstract
    class_stack[-1].is_reflected = True


def ProcessTemplateClass(line, lines, class_stack, file_path):
    start_index = len("SPARK_CLASS_TEMPLATED") + 1
    end_index = len(line)
    current_decorator = ""
    decorators = []
    for i in range(start_index, end_index):
        if line[i] == " ":
            continue
        if line[i] == ',':
            decorators.append(current_decorator)
            current_decorator = ""

        if line[i] == ")":
            decorators.append(current_decorator)
            break

        current_decorator += line[i]

    is_abstract = decorators.count("Abstract") > 0
    class_type = ClassType.TEMPLATED_CLASS

    class_stack[-1].type = class_type
    class_stack[-1].abstract = is_abstract
    class_stack[-1].is_reflected = True

    # make sure file ends with generated include
    last_line = lines[-1]
    namespace_text = class_stack[-1].namespace.replace("::", "_")
    class_name = class_stack[-1].name
    expected_text = f"#include \"{namespace_text}_{class_name}.generated.h\""
    if last_line.strip() != expected_text:
        Log.Error(f"File:{file_path}")
        Log.Error(f"Reflected templated class {class_name}: Header must end with: \"{expected_text}\"")
        return False

    return True


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

def DefineNonPODType(class_name):
    return f"""static_assert(std::is_convertible<{class_name}*, se::reflect::ObjectBase*>::value, "Reflectable types must inherit from reflect::ObjectBase");
size_t {class_name}::s_StaticId = typeid({class_name}).hash_code();\n"""

def DefineAbstractClassBegin(class_name):
    return DefineNonPODType(class_name) + f"""reflect::Type* {class_name}::GetReflectType() const
    {{
        return reflect::TypeResolver<{class_name}>::get();
    }}
    void {class_name}::Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName)
    {{
        reflect::TypeResolver<{class_name}>::get()->Serialize(obj, parentObj, fieldName);
    }}
    void {class_name}::Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName)
    {{
        reflect::TypeResolver<{class_name}>::get()->Deserialize(obj, parentObj, fieldName);
    }}
    asset::binary::StructLayout {class_name}::GetStructLayout(const void*) const
    {{
        return reflect::TypeResolver<{class_name}>::get()->GetStructLayout(nullptr);
    }}
    std::string {class_name}::GetTypeName() const
    {{
        return reflect::TypeResolver<{class_name}>::get()->GetTypeName(nullptr);
    }}
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

def DefineClassBeginCommon(class_name, is_pod):
    ret = ""
    if not is_pod:
        ret += f"""    reflect::Type* {class_name}::GetReflectType() const
    {{
        return reflect::TypeResolver<{class_name}>::get();
    }}
    void {class_name}::Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName)
    {{
        reflect::TypeResolver<{class_name}>::get()->Serialize(obj, parentObj, fieldName);
    }}
    void {class_name}::Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName)
    {{
        reflect::TypeResolver<{class_name}>::get()->Deserialize(obj, parentObj, fieldName);
    }}
    asset::binary::StructLayout {class_name}::GetStructLayout(const void*) const
    {{
        return reflect::TypeResolver<{class_name}>::get()->GetStructLayout(nullptr);
    }}
    std::string {class_name}::GetTypeName() const
    {{
        return reflect::TypeResolver<{class_name}>::get()->GetTypeName(nullptr);
    }}\n"""
    ret += f"""    se::reflect::Class* {class_name}::GetReflection() 
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
    return ret

def DefineClassBegin(class_name):
    return f"    " + DefineNonPODType(class_name) + DefineClassBeginCommon(class_name, False)

def DefineTemplateClassBegin(class_name, template_types, template_params):
    return f"""    template <{template_params}>
    size_t {class_name}<{template_types}>::s_StaticId = typeid({class_name}<{template_types}>).hash_code();
    template <{template_params}>
    reflect::Type* {class_name}<{template_types}>::GetReflectType() const
    {{
        return reflect::TypeResolver<{class_name}<{template_types}>>::get();
    }}
    template <{template_params}>
    void {class_name}<{template_types}>::Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) 
    {{
        reflect::TypeResolver<{class_name}<{template_types}>>::get()->Serialize(obj, parentObj, fieldName); 
    }}
    template <{template_params}>
    void {class_name}<{template_types}>::Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) 
    {{
        reflect::TypeResolver<{class_name}<{template_types}>>::get()->Deserialize(obj, parentObj, fieldName); 
    }}
    template <{template_params}>
    asset::binary::StructLayout {class_name}<{template_types}>::GetStructLayout(const void*) const 
    {{
        return reflect::TypeResolver<{class_name}<{template_types}>>::get()->GetStructLayout(nullptr); 
    }}
    template <{template_params}>
    std::string {class_name}<{template_types}>::GetTypeName() const 
    {{
        return reflect::TypeResolver<{class_name}<{template_types}>>::get()->GetTypeName(nullptr); 
    }}
    template <{template_params}>
    se::reflect::Class* {class_name}<{template_types}>::GetReflection()
    {{
        static se::reflect::TemplatedClass<{template_types}>* s_Reflection = nullptr;
        if (!s_Reflection)
        {{
            s_Reflection = new se::reflect::TemplatedClass<{template_types}>();
            static_assert(std::is_convertible<{class_name}<{template_types}>*, se::reflect::ObjectBase*>::value, "Reflectable types must inherit from reflect::ObjectBase");
            s_Reflection->name = \"{class_name}\";
            se::reflect::TypeLookup::GetTypeMap()[s_Reflection->GetTypeName(nullptr)] = s_Reflection;
            s_Reflection->size = sizeof({class_name}<{template_types}>); 
            s_Reflection->heap_constructor = []{{ return new {class_name}<{template_types}>(); }}; 
            s_Reflection->inplace_constructor = [](void* mem){{ return new(mem) {class_name}<{template_types}>(); }}; 
            s_Reflection->heap_copy_constructor = [](void* other){{ return new {class_name}<{template_types}>(*reinterpret_cast<{class_name}<{template_types}>*>(other)); }}; 
            s_Reflection->inplace_copy_constructor = [](void* mem, void* other){{ return new(mem) {class_name}<{template_types}>(*reinterpret_cast<{class_name}<{template_types}>*>(other)); }}; 
            s_Reflection->destructor = [](void* data){{ reinterpret_cast<{class_name}<{template_types}>*>(data)->~{class_name}<{template_types}>(); }}; 
            s_Reflection->members = {{}};
        }} 
        return s_Reflection; 
    }}
    template <{template_params}> 
    void {class_name}<{template_types}>::InitMembers() 
    {{
        GetReflection()->members = 
        {{\n"""

def DefineTemplateMember(type, name, template_types, serialized):
    serialized_str = "true" if serialized else "false"
    return f"            {{\"{name}\", se::reflect::TypeResolver<decltype({name})>::get(), [](const void* obj){{ return (void*)&(({type}<{template_types}>*)obj)->{name}; }}, {serialized_str}}},\n"

def DefineTemplateClassEnd():
    return """        };
    }
}"""
def DefineClassEnd(class_name):
    return f"""        }};
    }}
    
    static auto SPARK_CAT({class_name}Reflection, __COUNTER__) = {class_name}::GetReflection();
}}
"""

def DefinePODClassBegin(class_name):
    return f"    size_t {class_name}::s_StaticId = typeid({class_name}).hash_code();\n" + DefineClassBeginCommon(class_name, True)

def DefineComponentBegin(class_name):
    return f"    " + DefineNonPODType(class_name) + f"    se::ecs::Id {class_name}::s_ComponentId = {{}};\n" + DefineClassBeginCommon(class_name, False)

def DefineSystem(class_name):
    return f"    " + DefineNonPODType(class_name) + f"    se::ecs::Id {class_name}::s_SystemId = {{}};\n" + DefineClassBeginCommon(class_name, False) + DefineClassEnd(class_name)

def DefineMember(class_name, name, serialized):
    bool_val = "true" if serialized else "false"
    return f"            {{\"{name}\", se::reflect::TypeResolver<decltype({class_name}::{name})>::get(), [](const void* obj){{ return (void*)&(({class_name}*)obj)->{name}; }},{bool_val}}},\n"

def CreateClassInstantiationFiles(source_dirs, classes, template_instantiations):
    for src_dir in source_dirs:
        if not os.path.exists(src_dir):
            os.mkdir(src_dir)
        project_name = src_dir.split('/')[-4]
        init_members_h_content = f"#pragma once\nnamespace se\n{{\nvoid {project_name}_InitClassReflection();\n}}"
        output_path = src_dir + "Classes.generated.h"
        existing_contents = ""
        if os.path.isfile(output_path):
            input_handle = open(output_path, "r")
            existing_contents = input_handle.read()
            input_handle.close()
        if existing_contents != init_members_h_content:
            print(f"-- -- {src_dir}Classes.generated.h generating...")
            output_handle = open(output_path, "w+")
            output_handle.write(init_members_h_content)
            output_handle.close()

        init_members_cpp_content = "#include \"spark.h\"\n"
        for full_name, class_obj in classes.items():
            if class_obj.is_reflected and class_obj.project_src_dir == src_dir:
                init_members_cpp_content += f"#include \"{class_obj.path}\"\n"
        init_members_cpp_content += f"\nnamespace se\n{{\nvoid {project_name}_InitClassReflection()\n{{\n"
        for full_name, class_obj in classes.items():
            if class_obj.is_reflected and not class_obj.is_template and class_obj.project_src_dir == src_dir:
                init_members_cpp_content += "    " + full_name + "::InitMembers();\n"
        for template_instantiation in template_instantiations:
            if template_instantiation.project_src_dir == src_dir:
                template_params_string = ", ".join(template_instantiation.template_types)
                init_members_cpp_content += f"    {template_instantiation.namespace}::{template_instantiation.class_name}<{template_params_string}>::InitMembers();\n"
        init_members_cpp_content += "}\n}"

        output_path = src_dir + "Classes.generated.cpp"
        existing_contents = ""
        if os.path.isfile(output_path):
            input_handle = open(output_path, "r")
            existing_contents = input_handle.read()
            input_handle.close()
        if existing_contents != init_members_cpp_content:
            print(f"-- -- {src_dir}Classes.generated.cpp generating...")
            output_handle = open(output_path, "w+")
            output_handle.write(init_members_cpp_content)
            output_handle.close()

def CreateSystemInstantiationFiles(source_dirs, classes):
    for src_dir in source_dirs:
        if not os.path.exists(src_dir):
            os.mkdir(src_dir)
        project_name = src_dir.split('/')[-4]
        init_members_h_content = f"#pragma once\nnamespace se\n{{\nvoid {project_name}_InitSystems(ecs::World* world);\n}}"
        output_path = src_dir + "Systems.generated.h"
        existing_contents = ""
        if os.path.isfile(output_path):
            input_handle = open(output_path, "r")
            existing_contents = input_handle.read()
            input_handle.close()
        if existing_contents != init_members_h_content:
            print(f"-- -- {src_dir}Systems.generated.h generating...")
            output_handle = open(output_path, "w+")
            output_handle.write(init_members_h_content)
            output_handle.close()

        init_systems_cpp_content = "#include \"spark.h\"\n"
        for full_name, class_obj in classes.items():
            if class_obj.is_reflected and class_obj.project_src_dir == src_dir and class_obj.type == ClassType.SYSTEM:
                init_systems_cpp_content += f"#include \"{class_obj.path}\"\n"
        init_systems_cpp_content += f"\nnamespace se\n{{\nvoid {project_name}_InitSystems([[maybe_unused]]ecs::World* world)\n{{\n"
        for full_name, class_obj in classes.items():
            if class_obj.is_reflected and not class_obj.is_template and class_obj.project_src_dir == src_dir and class_obj.type == ClassType.SYSTEM:
                init_systems_cpp_content += f"    world->RegisterSystem<{full_name}>();\n"
        for full_name, class_obj in classes.items():
            if class_obj.is_reflected and not class_obj.is_template and class_obj.project_src_dir == src_dir and class_obj.type == ClassType.SYSTEM:
                if src_dir.endswith("engine/src/generated/"):
                    create_system_method = "CreateEngineSystem"
                else:
                    create_system_method = "CreateAppSystem"
                init_systems_cpp_content += f"    world->{create_system_method}<{full_name}>();\n"
        init_systems_cpp_content += "}\n}"

        output_path = src_dir + "Systems.generated.cpp"
        existing_contents = ""
        if os.path.isfile(output_path):
            input_handle = open(output_path, "r")
            existing_contents = input_handle.read()
            input_handle.close()
        if existing_contents != init_systems_cpp_content:
            print(f"-- -- {src_dir}Systems.generated.cpp generating...")
            output_handle = open(output_path, "w+")
            output_handle.write(init_systems_cpp_content)
            output_handle.close()

def WriteClassFiles(classes, base_class_map, template_instantiations):
    source_dirs = set()
    for full_name, class_obj in classes.items():
        source_dirs.add(class_obj.project_src_dir)

    CreateClassInstantiationFiles(source_dirs, classes, template_instantiations)
    CreateSystemInstantiationFiles(source_dirs, classes)

    for full_name, classobj in classes.items():
        if classobj.is_reflected and not classobj.is_template:
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
            output_path = classobj.project_src_dir + f"{namespace_text}_{classobj.name}.generated.cpp"
            existing_contents = ""
            if os.path.isfile(output_path):
                input_handle = open(output_path, "r")
                existing_contents = input_handle.read()
                input_handle.close()
            if existing_contents != contents:
                print(f"-- -- {namespace_text}_{classobj.name}.generated.cpp generating...")
                output_handle = open(output_path, "w+")
                output_handle.write(contents)
                output_handle.close()

        for full_name, classobj in classes.items():
            if classobj.is_reflected and classobj.is_template:
                contents = f"#pragma once\n\nnamespace {classobj.namespace}\n{{\n"

                template_params = ",".join(classobj.template_params)
                template_types = ""
                for i in range(0, len(classobj.template_params)):
                    template_types += classobj.template_params[i].split(" ", 1)[1]
                    if i < len(classobj.template_params) - 1:
                        template_types += ","

                contents += DefineTemplateClassBegin(classobj.name, template_types, template_params)

                current_type = full_name
                while current_type in classes:
                    class_obj = classes[current_type]
                    for member in class_obj.members:
                        contents += DefineTemplateMember(classobj.name, member.name, template_types, member.serialized)
                    if current_type in base_class_map:
                        current_type = base_class_map[current_type]
                    else:
                        break

                contents += DefineTemplateClassEnd()

                namespace_text = classobj.namespace.replace("::", "_")
                output_path = classobj.project_src_dir + f"{namespace_text}_{classobj.name}.generated.h"
                if not os.path.exists(classobj.project_src_dir):
                    os.mkdir(classobj.project_src_dir)
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

        instantiation_files = dict()
        for template_instantiation in template_instantiations:
            namespace_text = template_instantiation.namespace.replace("::", "_")
            output_file = f"{namespace_text}_{template_instantiation.class_name}.generated.cpp"
            full_output_path = template_instantiation.project_src_dir + output_file
            contents = ""
            if full_output_path in instantiation_files:
                contents += instantiation_files[full_output_path]
            else:
                contents += f"#include \"spark.h\"\n#include \"engine/reflect/Reflect.h\"\n#include \"{template_instantiation.filepath}\"\n"

            includes = list()
            for template_type in template_instantiation.template_types:
                if template_type in classes:
                    if includes.count(classes[template_type].path) == 0:
                        includes.append(classes[template_type].path)

            for include in includes:
                contents += f"#include \"{include}\"\n"

            template_types_str = ",".join(template_instantiation.template_types)
            contents += f"static auto SPARK_CAT({template_instantiation.class_name}Reflection, __COUNTER__) = {template_instantiation.namespace}::{template_instantiation.class_name}<{template_types_str}>::GetReflection();\n"
            instantiation_files[full_output_path] = contents

        for path, contents in instantiation_files.items():
            output_path = path
            existing_contents = ""
            if os.path.isfile(output_path):
                input_handle = open(output_path, "r")
                existing_contents = input_handle.read()
                input_handle.close()
            if existing_contents != contents:
                filename = path.rsplit("/", 1)[1]
                Log.Msg(f"{filename} generating...")
                output_handle = open(output_path, "w+")
                output_handle.write(contents)
                output_handle.close()
