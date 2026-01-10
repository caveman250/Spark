import os
from dataclasses import dataclass
from enum import Enum
from src import Log
from src import Namespace


@dataclass
class Member:
    name: str
    serialized: bool
    full_type_name: str

@dataclass
class Function:
    name: str
    full_type_name: str
    arg_types: list

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
    dev_only: bool
    editor_only: bool
    members: list
    functions: list
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
    str_minus_templates = RemoveTemplateParams(class_name)

    if str_minus_templates.startswith("std::"):
        return str_minus_templates

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

def GetTemplateTypes(type):
    num_open_template_params = 0
    template_types = []
    current_template_type = ""
    for i in range(0, len(type)):
        if type[i] == ' ':
            continue
        if type[i] == '<':
            num_open_template_params += 1
        elif type[i] == '>':
            num_open_template_params -= 1
            if num_open_template_params == 0:
                template_types.append(current_template_type)
                current_template_type = ""
            continue
        elif num_open_template_params > 0:
            current_template_type += type[i]

    return template_types

def FindRealClassType(type, class_list, end_of_file_namespace_index, using_namespace_stack):
    if type.startswith("std::"):
        return type

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
        class_stack.append(Class(ClassType.CLASS, class_name, filepath, Namespace.MakeNamespace(namespace_stack), False, False, False, [], [], False, len(template_params) > 0, template_params, source_dir))
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
                current_template_type += line[i]
                num_open_template_params -= 1
                if num_open_template_params == 0:
                    template_types.append(current_template_type)
                    current_template_type = ""
                continue

            if line[i] == ',' and num_open_template_params == 0:
                template_types.append(current_template_type)
                current_template_type = ""
            elif line[i] == ')':
                if len(current_template_type) > 0:
                    template_types.append(current_template_type)
                break
            else:
                current_template_type += line[i]

    for i in range(0, len(template_types)):
        template_type = template_types[i]
        if not IsBuiltinPrimitive(template_type):
            last_namespace_idx = CountNamespaces(Namespace.MakeNamespace(namespace_stack)) - 1
            template_suffix = ""
            if '<' in template_type:
                for j in range(0, len(template_type)):
                    if template_type[j] == '<':
                        template_suffix = template_type[j + 1:-1]
                        break
            if (len(template_suffix) > 0):
                inner_template_types = template_suffix.split(',')
                template_suffix = "<"
                for k in range(0, len(inner_template_types)):
                    inner_template_type = GetFullClassName(inner_template_types[k], namespace_stack)
                    inner_template_type = FindRealClassType(inner_template_type, class_list, last_namespace_idx, using_namespace_stack)
                    template_suffix += inner_template_type
                    if k < len(inner_template_types) - 1:
                        template_suffix += ","
                template_suffix += ">"
            template_type = GetFullClassName(template_type, namespace_stack)
            template_type = FindRealClassType(template_type, class_list, last_namespace_idx, using_namespace_stack)
            template_types[i] = template_type + template_suffix

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
            continue

        if line[i] == ")":
            decorators.append(current_decorator)
            break

        current_decorator += line[i]

    is_abstract = decorators.count("Abstract") > 0
    is_dev_only = decorators.count("DevOnly") > 0
    is_editor_only = decorators.count("EditorOnly") > 0

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
    class_stack[-1].dev_only = is_dev_only
    class_stack[-1].editor_only = is_editor_only
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

def GetMethodOrVariableType(line, namespace_stack, class_list, using_namespace_stack):
    line = line.strip()

    start_index = 0
    if line.startswith("mutable"):
        start_index += len("mutable")
    if line.startswith("const"):
        start_index += len("const")

    end_index = len(line)
    num_open_template_params = 0
    has_begun_type = False
    has_begun_name = False
    type = ""
    name = ""
    for i in range(start_index, end_index):
        if has_begun_name:
            if line[i] == ' ' or line[i] == ';' or line[i] == '(':
                break
            name += line[i]
            continue
        elif not has_begun_type:
            if line[i] != ' ':
                has_begun_type = True

        if has_begun_type:
            if line[i] == ' ' and num_open_template_params == 0:
                has_begun_name = True
            else:
                type += line[i]

        if not has_begun_name:
            if line[i] == '<':
                num_open_template_params += 1
            elif line[i] == '>':
                num_open_template_params -= 1

    last_namespace_idx = CountNamespaces(Namespace.MakeNamespace(namespace_stack)) - 1
    member_full_type = GetFullClassName(type, namespace_stack)
    member_full_type = FindRealClassType(member_full_type, class_list, last_namespace_idx, using_namespace_stack)
    return name, member_full_type

def GetFunctionArgTypes(line, namespace_stack, class_list, using_namespace_stack):
    ret = list()
    last_namespace_idx = CountNamespaces(Namespace.MakeNamespace(namespace_stack)) - 1
    line = line.strip()

    start_index = 0
    end_index = 0
    for i in range(0, len(line)):
        if line[i] == "(":
            start_index = i + 1
        elif line[i] == ")":
            end_index = i - 1
            break

    # so fragile, TODO multiline
    args = line[start_index:end_index]
    if len(args) > 0:
        arg_list = args.split(', ')
        for arg in arg_list:
            type = arg.split(' ')[0]
            full_type = GetFullClassName(type, namespace_stack)
            full_type = FindRealClassType(full_type, class_list, last_namespace_idx, using_namespace_stack)
            ret.append(full_type)


    return ret

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

    name, member_type = GetMethodOrVariableType(next_line, namespace_stack, class_list, using_namespace_stack)

    serialized = decorators.count("Serialized") > 0
    class_stack[-1].members.append(Member(name, serialized, member_type))

def ProcessFunction(line, next_line, class_stack, namespace_stack, class_list, using_namespace_stack):
    start_index = len("SPARK_FUNCTION") + 1
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

    name, function_type = GetMethodOrVariableType(next_line, namespace_stack, class_list, using_namespace_stack)
    arg_types = GetFunctionArgTypes(next_line, namespace_stack, class_list, using_namespace_stack)

    class_stack[-1].functions.append(Function(name, function_type, arg_types))

def GetFullnameFromClassObject(class_obj: Class):
    if len(class_obj.namespace) > 0:
        return class_obj.namespace + "::" + class_obj.name
    return class_obj.name

def DefineNonPODType(class_name):
    return f"""static_assert(std::is_convertible<{class_name}*, se::reflect::ObjectBase*>::value, "Reflectable types must inherit from reflect::ObjectBase");
    size_t {class_name}::s_StaticId = typeid({class_name}).hash_code();\n"""

def DefineAbstractClassBegin(class_name):
    return DefineNonPODType(class_name) + f"""\n    reflect::Type* {class_name}::GetReflectType() const
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
    }}\n"""

def DefineClassBeginCommon(class_name, is_pod, is_copyable):
    copy_constructors = ""
    if is_copyable:
        copy_constructors = f"""            s_Reflection->heap_copy_constructor = [](void* other){{ return new {class_name}(*reinterpret_cast<{class_name}*>(other)); }}; 
            s_Reflection->inplace_copy_constructor = [](void* mem, void* other){{ return new(mem) {class_name}(*reinterpret_cast<{class_name}*>(other)); }};"""
    else:
        copy_constructors = f"""            s_Reflection->heap_copy_constructor = nullptr; 
            s_Reflection->inplace_copy_constructor = nullptr;"""
    ret = ""
    if not is_pod:
        ret += f"""\n    reflect::Type* {class_name}::GetReflectType() const
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
    }}\n\n"""
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
            {copy_constructors}
            s_Reflection->destructor = [](void* data){{ reinterpret_cast<{class_name}*>(data)->~{class_name}(); }};
            s_Reflection->members = {{}};
        }}
        return s_Reflection;
    }}\n"""
    return ret

def BeginMembers(class_name):
    return f"""\n    void {class_name}::InitMembers()
    {{
        GetReflection()->members =
        {{\n"""

def EndMembers():
    return "        };\n"

def BeginFunctions():
    return """        GetReflection()->functions =
        {\n"""

def EndFunctions():
    return "        };\n    }\n\n"


def DefineClassBegin(class_name):
    return f"    " + DefineNonPODType(class_name) + DefineClassBeginCommon(class_name, False, True)

def DefineTemplateClassBegin(class_name, template_types, template_params):
    is_variadic = False
    reflect_type = ""
    if template_types.endswith("..."):
        reflect_type = "se::reflect::VariadicTemplatedClass"
        is_variadic = True
    else:
        reflect_type = "se::reflect::TemplatedClass"

    create_name = f"s_Reflection->name = \"{class_name}<\";\n"
    template_types_list = template_types.split(',')
    if is_variadic:
        template_type = template_types[0:-3]
        create_name += f"            (CollectName<{template_type}>(s_Reflection->name), ...);\n"
    else:
        for i in range(0, len(template_types_list)):
            create_name += f"            s_Reflection->name += se::reflect::TypeResolver<{template_types_list[i]}>::get()->name;\n"
            if i < len(template_types_list) - 1:
                create_name += "             s_Reflection->name += \", \";\n"
    create_name += "            s_Reflection->name += \">\";\n"

    ret = ""
    if is_variadic:
        ret += """    template<typename Cs>
    void CollectName(std::string& name)
    {
        name += se::reflect::TypeResolver<Cs>::get()->name;
        name += ", ";
    }\n\n"""

    ret += f"""    template <{template_params}>
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
        static {reflect_type}<{template_types}>* s_Reflection = nullptr;
        if (!s_Reflection)
        {{
            s_Reflection = new {reflect_type}<{template_types}>();
            static_assert(std::is_convertible<{class_name}<{template_types}>*, se::reflect::ObjectBase*>::value, "Reflectable types must inherit from reflect::ObjectBase");
            {create_name}
            se::reflect::TypeLookup::GetTypeMap()[s_Reflection->name] = s_Reflection;
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
    
    template <{template_params}>\n"""
    return ret

def DefineTemplateClassEnd():
    return "\n}"
def DefineClassEnd(class_name):
    return f"""
    static auto SPARK_CAT({class_name}Reflection, __COUNTER__) = {class_name}::GetReflection();
}}
"""

def DefinePODClassBegin(class_name):
    return f"    size_t {class_name}::s_StaticId = typeid({class_name}).hash_code();\n" + DefineClassBeginCommon(class_name, True, True)

def DefineComponentBegin(class_name, is_copyable):
    return f"    " + DefineNonPODType(class_name) + f"    se::ecs::Id {class_name}::s_ComponentId = {{}};\n" + DefineClassBeginCommon(class_name, False, is_copyable)

def DefineSystemBegin(class_name):
    ret = f"    " + DefineNonPODType(class_name) + f"    se::ecs::Id {class_name}::s_SystemId = {{}};\n"
    ret += f"""\n    reflect::Type* {class_name}::GetReflectType() const
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
    
    bool {class_name}::ImplementsUpdateMethod() const
    {{
        return !std::is_same_v<decltype(&System::OnUpdate), decltype(&{class_name}::OnUpdate)>;
    }}
    
    bool {class_name}::ImplementsRenderMethod() const
    {{
         return !std::is_same_v<decltype(&System::OnRender), decltype(&{class_name}::OnRender)>;
    }}
    
    se::reflect::Class* {class_name}::GetReflection() 
    {{
        static se::reflect::System* s_Reflection = nullptr;
        if (!s_Reflection)
        {{
            s_Reflection = new se::reflect::System();
            se::reflect::TypeLookup::GetTypeMap()["{class_name}"] = s_Reflection;
            s_Reflection->name = "{class_name}"; 
            s_Reflection->size = sizeof({class_name});                           
            s_Reflection->has_default_constructor = std::is_default_constructible<{class_name}>::value; 
            CreateDefaultConstructorMethods<{class_name}>(s_Reflection);
            s_Reflection->heap_copy_constructor = [](void* other){{ return new {class_name}(*reinterpret_cast<{class_name}*>(other)); }}; 
            s_Reflection->inplace_copy_constructor = [](void* mem, void* other){{ return new(mem) {class_name}(*reinterpret_cast<{class_name}*>(other)); }};
            s_Reflection->destructor = [](void* data){{ reinterpret_cast<{class_name}*>(data)->~{class_name}(); }};
            s_Reflection->GetStaticId = [](){{ return {class_name}::GetSystemId(); }};
            s_Reflection->members = {{}};
        }}
        return s_Reflection;
    }}\n"""
    return ret

def DefineMember(class_name, name, serialized):
    bool_val = "true" if serialized else "false"
    return f"            {{\"{name}\", se::reflect::TypeResolver<decltype({class_name}::{name})>::get(), [](const void* obj){{ return (void*)&(({class_name}*)obj)->{name}; }},{bool_val}}},\n"

def DefineFunction(class_name, name):
    return f"""            {{ 
                \"{name}\", 
                se::reflect::TypeResolver<se::reflect::function_traits<decltype(&{class_name}::{name})>::return_type>::get(), 
                se::reflect::MakeFunctionArgTypesArray(se::reflect::function_traits<decltype(&{class_name}::{name})>::args_type()),
                &{class_name}::{name}
            }},\n"""

def WriteInvokeMethod(class_name, functions):
    ret =  f"""    void {class_name}::Invoke([[maybe_unused]] const std::string& name, [[maybe_unused]] const std::vector<std::any>& args)
    {{\n"""

    for function in functions:
        ret += f"""        if (name == "{function.name}")
        {{
            {function.name}("""
        for i in range(0, len(function.arg_types)):
            ret += f"std::any_cast<{function.arg_types[i]}>(args[{i}])"
            if i < len(function.arg_types) - 1:
                ret += ", "
        ret += ");\n        }\n"

    ret += "    }\n"
    return ret


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
            if class_obj.dev_only:
                init_members_cpp_content += "#if WITH_DEV_ONLY_CLASSES\n"
            if class_obj.editor_only:
                init_members_cpp_content += "#if WITH_EDITOR_ONLY_CLASSES\n"
            if class_obj.is_reflected and not class_obj.is_template and class_obj.project_src_dir == src_dir:
                init_members_cpp_content += "    " + full_name + "::InitMembers();\n"
            if class_obj.editor_only:
                init_members_cpp_content += "#endif\n"
            if class_obj.dev_only:
                init_members_cpp_content += "#endif\n"
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
                if class_obj.dev_only:
                    init_systems_cpp_content += "#if WITH_DEV_ONLY_CLASSES\n"
                if class_obj.editor_only:
                    init_systems_cpp_content += "#if WITH_EDITOR_ONLY_CLASSES\n"
                init_systems_cpp_content += f"    world->RegisterSystem<{full_name}>();\n"
                if class_obj.editor_only:
                    init_systems_cpp_content += "#endif\n"
                if class_obj.dev_only:
                    init_systems_cpp_content += "#endif\n"
        for full_name, class_obj in classes.items():
            if class_obj.is_reflected and not class_obj.is_template and class_obj.project_src_dir == src_dir and class_obj.type == ClassType.SYSTEM:
                if src_dir.endswith("engine/src/generated/"):
                    create_system_method = "CreateEngineSystem"
                else:
                    create_system_method = "CreateAppSystem"
                if class_obj.dev_only:
                    init_systems_cpp_content += "#if WITH_DEV_ONLY_CLASSES\n"
                if class_obj.editor_only:
                    init_systems_cpp_content += "#if WITH_EDITOR_ONLY_CLASSES\n"
                init_systems_cpp_content += f"    world->{create_system_method}<{full_name}>();\n"
                if class_obj.editor_only:
                    init_systems_cpp_content += "#endif\n"
                if class_obj.dev_only:
                    init_systems_cpp_content += "#endif\n"
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

def DefineClass(classobj, full_name, classes, base_class_map):
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

    if classobj.dev_only:
        contents += "#if WITH_DEV_ONLY_CLASSES\n"

    if classobj.editor_only:
        contents += "#if WITH_EDITOR_ONLY_CLASSES\n"

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
        contents += DefineSystemBegin(classobj.name)
    elif classobj.type == ClassType.POD_CLASS:
        contents += DefinePODClassBegin(classobj.name)
    elif classobj.type ==  ClassType.COMPONENT:
        contents += DefineComponentBegin(classobj.name, True)
    elif classobj.type == ClassType.WIDGET_COMPONENT:
        contents += DefineComponentBegin(classobj.name, True)
    elif classobj.type == ClassType.SINGLETON_COMPONENT:
        contents += DefineComponentBegin(classobj.name, False)

    contents += BeginMembers(classobj.name)
    current_type = full_name
    while current_type in classes:
        class_obj = classes[current_type]
        for member in class_obj.members:
            contents += DefineMember(classobj.name, member.name, member.serialized)
        if current_type in base_class_map:
            current_type = base_class_map[current_type]
        else:
            break
    contents += EndMembers()

    contents += BeginFunctions()
    current_type = full_name
    while current_type in classes:
        class_obj = classes[current_type]
        for func in class_obj.functions:
            contents += DefineFunction(classobj.name, func.name)
        if current_type in base_class_map:
            current_type = base_class_map[current_type]
        else:
            break
    contents += EndFunctions()

    if classobj.type != ClassType.POD_CLASS:
        contents += WriteInvokeMethod(classobj.name, classobj.functions)

    contents += DefineClassEnd(classobj.name)

    if classobj.editor_only:
        contents += "#endif\n"

    if classobj.dev_only:
        contents += "#endif\n"

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

def DefineTemplateClass(classobj, full_name, classes, base_class_map):
    contents = f"#pragma once\n\nnamespace {classobj.namespace}\n{{\n"

    template_params = ",".join(classobj.template_params)
    template_types = ""
    for i in range(0, len(classobj.template_params)):
        split = classobj.template_params[i].split(" ", 1)
        param = split[0]
        template_types += split[1]
        if param.endswith("..."):
            template_types+= "..."
        if i < len(classobj.template_params) - 1:
            template_types += ","

    contents += DefineTemplateClassBegin(classobj.name, template_types, template_params)

    contents += BeginMembers(f"{classobj.name}<{template_types}>")
    current_type = full_name
    while current_type in classes:
        class_obj = classes[current_type]
        for member in class_obj.members:
            contents += DefineMember(f"{classobj.name}<{template_types}>", member.name, member.serialized)
        if current_type in base_class_map:
            current_type = base_class_map[current_type]
        else:
            break
    contents += EndMembers()

    contents += BeginFunctions()
    current_type = full_name
    while current_type in classes:
        class_obj = classes[current_type]
        for func in class_obj.functions:
            contents += DefineFunction(f"{classobj.name}<{template_types}>", func.name)
        if current_type in base_class_map:
            current_type = base_class_map[current_type]
        else:
            break
    contents += EndFunctions()

    contents += f"    template<{template_params}>\n"
    contents += WriteInvokeMethod(f"{classobj.name}<{template_types}>", classobj.functions)

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

def WriteTemplateInstantiations(template_instantiations, classes):
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
            template_pure_type = RemoveTemplateParams(template_type)
            if template_pure_type in classes:
                if includes.count(classes[template_pure_type].path) == 0:
                    includes.append(classes[template_pure_type].path)
            template_params = GetTemplateTypes(template_type)
            for template_param in template_params:
                if template_param in classes:
                    if includes.count(classes[template_param].path) == 0:
                        includes.append(classes[template_param].path)

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

def WriteClassFiles(classes, base_class_map, template_instantiations):
    source_dirs = set()
    for full_name, class_obj in classes.items():
        source_dirs.add(class_obj.project_src_dir)

    CreateClassInstantiationFiles(source_dirs, classes, template_instantiations)
    CreateSystemInstantiationFiles(source_dirs, classes)

    for full_name, classobj in classes.items():
        if classobj.is_reflected:
            if classobj.is_template:
                DefineTemplateClass(classobj, full_name, classes, base_class_map)
            else:
                DefineClass(classobj, full_name, classes, base_class_map)

    WriteTemplateInstantiations(template_instantiations, classes)
