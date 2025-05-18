import os
import sys

import Widgets
import Components
import Namespace
import Class
import Enum

def ProcessHeaders():
    components = []
    widgets = []
    source_dirs = sys.argv[1].split(':')

    print("-- -- First pass, collect classes.")
    class_list = []
    enum_list = []
    # first pass, collect all classes in their actual namespace so we cna look them up later when resolving inheritance.
    for dir in source_dirs:
        for root, dirs, files in os.walk(dir):
            for file_path in files:
                if file_path == "Reflect_fwd.h":
                    continue
                if file_path.endswith(".h"):
                    current_scope_depth = 0
                    namespace_stack = []
                    using_namespace_stack = [[]]
                    namespace_scope_depth_stack = []
                    using_namespace_scope_depth_stack = []
                    path = root + "/" + file_path
                    with open(path, 'r') as file:
                        lines = file.readlines()
                        lineCount = len(lines)
                        for i in range(0, lineCount):
                            line = lines[i].strip()
                            if line.startswith("class "):
                                Class.ProcessNativeClass("class", line, namespace_stack, class_list)
                            if line.startswith("struct "):
                                Class.ProcessNativeClass("struct", line, namespace_stack, class_list)
                            elif line.startswith("namespace"):
                                Namespace.ProcessNamespace(line, namespace_stack, namespace_scope_depth_stack, current_scope_depth)
                            elif line.startswith("using namespace"):
                                Namespace.ProcessUsingNamespace(line, using_namespace_stack, using_namespace_scope_depth_stack, current_scope_depth)
                            for char in line:
                                if char == "{":
                                    current_scope_depth += 1
                                    using_namespace_stack.append([])
                                elif char == "}":
                                    if len(namespace_scope_depth_stack) > 0 and current_scope_depth == namespace_scope_depth_stack[-1]:
                                        namespace_scope_depth_stack.pop()
                                        namespace_stack.pop()
                                    if len(using_namespace_scope_depth_stack) > 0 and current_scope_depth == using_namespace_scope_depth_stack[-1]:
                                        using_namespace_scope_depth_stack.pop()
                                        using_namespace_stack.pop()
                                    current_scope_depth -= 1


    print("-- -- Second pass, generate reflection code.")
    finalised_reflected_classes = {}
    class_heirachy_map = {}

    for dir in source_dirs:
        for root, dirs, files in os.walk(dir):
            for file_path in files:
                if file_path == "Reflect_fwd.h":
                    continue
                if file_path.endswith(".h"):
                    current_scope_depth = 0
                    namespace_stack = []
                    using_namespace_stack = [[]]
                    class_stack = []
                    namespace_scope_depth_stack = []
                    using_namespace_scope_depth_stack = []
                    class_depth_stack = []
                    path = root + "/" + file_path
                    with open(path, 'r') as file:
                        lines = file.readlines()
                        lineCount = len(lines)
                        for i in range(0, lineCount):
                            line = lines[i].strip()
                            if line.startswith("SPARK_WIDGET_COMPONENT"):
                                Widgets.ProcessWidget(widgets, path, class_stack, components)
                                Class.ProcessClass("SPARK_WIDGET_COMPONENT", line, class_stack)
                            elif line.startswith("SPARK_COMPONENT"):
                                Components.ProcessComponent(components, path, class_stack)
                                Class.ProcessClass("SPARK_COMPONENT", line, class_stack)
                            elif line.startswith("SPARK_SINGLETON_COMPONENT"):
                                Components.ProcessSingletonComponent(components, path, class_stack)
                                Class.ProcessClass("SPARK_SINGLETON_COMPONENT", line, class_stack)
                            elif line.startswith("namespace"):
                                Namespace.ProcessNamespace(line, namespace_stack, namespace_scope_depth_stack, current_scope_depth)
                            elif line.startswith("using namespace"):
                                Namespace.ProcessUsingNamespace(line, using_namespace_stack, using_namespace_scope_depth_stack, current_scope_depth)
                            elif line.startswith("SPARK_CLASS("):
                                Class.ProcessClass("SPARK_CLASS", line, class_stack)
                            elif line.startswith("SPARK_POD_CLASS("):
                                Class.ProcessClass("SPARK_POD_CLASS", line, class_stack)
                            elif line.startswith("SPARK_SYSTEM("):
                                Class.ProcessClass("SPARK_SYSTEM", line, class_stack)
                            elif line.startswith("SPARK_MEMBER") and i < lineCount - 1:
                                Class.ProcessMember(line, lines[i + 1], class_stack, namespace_stack, class_list, using_namespace_stack)
                            elif line.startswith("class "):
                                Class.ProcessNativeClassInheritance("class", line, class_list, class_heirachy_map, namespace_stack, using_namespace_stack, class_stack, class_depth_stack, current_scope_depth, root + "/" + file_path)
                            elif line.startswith("struct "):
                                Class.ProcessNativeClassInheritance("struct", line, class_list, class_heirachy_map, namespace_stack, using_namespace_stack, class_stack, class_depth_stack, current_scope_depth, root + "/" + file_path)
                            elif line.startswith("SPARK_ENUM_BEGIN"):
                                Enum.ProcessEnum(line, enum_list, lines, i, namespace_stack, root + "/" + file_path)

                            for char in line:
                                if char == "{":
                                    current_scope_depth += 1
                                elif char == "}":
                                    if len(class_depth_stack) > 0 and current_scope_depth == class_depth_stack[-1]:
                                        class_depth_stack.pop()
                                        finalised_reflected_classes[Class.GetFullnameFromClassObject(class_stack[-1])] = class_stack[-1]
                                        class_stack.pop()
                                    elif len(namespace_scope_depth_stack) > 0 and current_scope_depth == namespace_scope_depth_stack[-1]:
                                        namespace_scope_depth_stack.pop()
                                        namespace_stack.pop()
                                    current_scope_depth -= 1

    Widgets.WriteWidgetHeader(widgets)
    Components.WriteComponentRegistrationFiles(components)
    Class.WriteClassFiles(finalised_reflected_classes, class_heirachy_map)
    Enum.WriteEnumFiles(enum_list)
if __name__ == '__main__':
    print("-- Running Spark Header Tool...")
    ProcessHeaders()
    print("-- done.\n")



