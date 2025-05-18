import os
import sys

import Widgets
import Components
import Namespace
import Class

def ProcessHeaders():
    components = []
    widgets = []
    source_dirs = sys.argv[1].split(':')

    print("-- -- First pass, collect classes.")
    class_list = []
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
                                Class.ProcessNativeClass(line, namespace_stack, class_list)
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
                            if line.startswith("DECLARE_SPARK_WIDGET_COMPONENT"):
                                Widgets.ProcessWidget(line, widgets, path, namespace_stack, components)
                                Class.ProcessClass("DECLARE_SPARK_WIDGET_COMPONENT", line, class_stack, class_depth_stack, namespace_stack, current_scope_depth, root + "/" + file_path)
                            elif line.startswith("DECLARE_SPARK_COMPONENT"):
                                Components.ProcessComponent(line, components, path, namespace_stack)
                                Class.ProcessClass("DECLARE_SPARK_COMPONENT", line, class_stack, class_depth_stack, namespace_stack, current_scope_depth, root + "/" + file_path)
                            elif line.startswith("DECLARE_SPARK_SINGLETON_COMPONENT"):
                                Components.ProcessSingletonComponent(line, components, path, namespace_stack)
                                Class.ProcessClass("DECLARE_SPARK_SINGLETON_COMPONENT", line, class_stack, class_depth_stack, namespace_stack, current_scope_depth, root + "/" + file_path)
                            elif line.startswith("namespace"):
                                Namespace.ProcessNamespace(line, namespace_stack, namespace_scope_depth_stack, current_scope_depth)
                            elif line.startswith("using namespace"):
                                Namespace.ProcessUsingNamespace(line, using_namespace_stack, using_namespace_scope_depth_stack, current_scope_depth)
                            elif line.startswith("DECLARE_SPARK_CLASS("):
                                Class.ProcessClass("DECLARE_SPARK_CLASS", line, class_stack, class_depth_stack, namespace_stack, current_scope_depth, root + "/" + file_path)
                            elif line.startswith("DECLARE_SPARK_POD_CLASS("):
                                Class.ProcessClass("DECLARE_SPARK_POD_CLASS", line, class_stack, class_depth_stack, namespace_stack, current_scope_depth, root + "/" + file_path)
                            elif line.startswith("DECLARE_SPARK_SYSTEM("):
                                Class.ProcessClass("DECLARE_SPARK_SYSTEM", line, class_stack, class_depth_stack, namespace_stack, current_scope_depth, root + "/" + file_path)
                            elif line.startswith("SPARK_MEMBER") and i < lineCount - 1:
                                Class.ProcessMember(line, lines[i + 1], class_stack, namespace_stack, class_list, using_namespace_stack)
                            elif line.startswith("class "):
                                Class.ProcessNativeClassInheritance(line, class_list, class_heirachy_map, namespace_stack, using_namespace_stack)

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
if __name__ == '__main__':
    print("-- Running Spark Header Tool...")
    ProcessHeaders()
    print("-- done.\n")



