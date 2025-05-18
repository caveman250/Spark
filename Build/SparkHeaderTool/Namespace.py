def MakeNamespace(namespace_stack):
    ret = ""
    for i in range(len(namespace_stack)):
        ret += namespace_stack[i]
        if i < len(namespace_stack) - 1:
            ret += "::"
    return ret

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

def ProcessUsingNamespace(line, using_namespace_stack, using_namespace_scope_depth_stack, current_scope_depth):
    start_index = len("using namespace") + 1
    end_index = len(line)
    using_namespace_scope_depth_stack.append(current_scope_depth + 1)
    namespace = ""
    for i in range(start_index, end_index):
        if line[i] == " " or line[i] == "\n" or line[i] == "{" or line[i] == ";":
            break
        namespace += line[i]
    using_namespace_stack[-1].append(namespace)