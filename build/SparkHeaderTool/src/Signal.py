from dataclasses import dataclass
from src import Class
from src import Namespace

def ProcessSignal(line, template_instantiations, namespace_stack, filepath, class_list, using_namespace_stack, source_dir):
    start_index = 0
    end_index = len(line)
    num_open_template_params = 0
    has_started_template_types = False
    template_types = []
    current_template_type = ""
    for i in range(start_index, end_index):
        if not has_started_template_types:
            if line[i] == "<":
                has_started_template_types = True
                continue
        if not has_started_template_types:
            continue
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
        elif line[i] == '>' and num_open_template_params == 0:
            if len(current_template_type) > 0:
                template_types.append(current_template_type)
            break
        else:
            current_template_type += line[i]

    for i in range(0, len(template_types)):
        template_type = template_types[i]
        if not Class.IsBuiltinPrimitive(template_type):
            last_namespace_idx = Class.CountNamespaces(Namespace.MakeNamespace(namespace_stack)) - 1
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
                    inner_template_type = Class.GetFullClassName(inner_template_types[k], namespace_stack)
                    inner_template_type = Class.FindRealClassType(inner_template_type, class_list, last_namespace_idx, using_namespace_stack)
                    template_suffix += inner_template_type
                    if k < len(inner_template_types) - 1:
                        template_suffix += ","
                template_suffix += ">"
            template_type = Class.GetFullClassName(template_type, namespace_stack)
            template_type = Class.FindRealClassType(template_type, class_list, last_namespace_idx, using_namespace_stack)
            template_types[i] = template_type + template_suffix

    new_instantiation = Class.TemplateInstantiation("se::ecs", "Signal", template_types, filepath, source_dir)
    exists = any(inst.class_name == "Signal" and inst.template_types == template_types for inst in template_instantiations)
    if not exists:
        template_instantiations.append(new_instantiation)
