import os
import sys

from src import Widgets
from src import Components
from src import Namespace
from src import Class
from src import Enum
from src import Log

def ProcessHeaders():
    components = []
    widgets = []
    source_dirs = sys.argv[1].split(';')

    Log.Msg("Pass 1: Collect classes...")
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
                                Class.ProcessNativeClassFirstPass("class", line, namespace_stack, class_list)
                            if line.startswith("struct "):
                                Class.ProcessNativeClassFirstPass("struct", line, namespace_stack, class_list)
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
    Log.Msg("Pass 1: Done.")
    Log.Msg("Pass 2: Generate reflection code...")
    finalised_reflected_classes = {}
    class_heirachy_map = {}
    template_instantiations = []

    for dir in source_dirs:
        source_dir = dir.strip()
        if not source_dir.endswith("/") and not source_dir.endswith("\\"):
            source_dir += "/"
        source_dir += "generated/"
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
                                Class.ProcessClass("SPARK_WIDGET_COMPONENT", line, class_stack)
                                Widgets.ProcessWidget(widgets, path, class_stack, components)
                            elif line.startswith("SPARK_COMPONENT"):
                                Class.ProcessClass("SPARK_COMPONENT", line, class_stack)
                                Components.ProcessComponent(components, path, class_stack)
                            elif line.startswith("SPARK_SINGLETON_COMPONENT"):
                                Class.ProcessClass("SPARK_SINGLETON_COMPONENT", line, class_stack)
                                Components.ProcessSingletonComponent(components, path, class_stack)
                            elif line.startswith("namespace"):
                                Namespace.ProcessNamespace(line, namespace_stack, namespace_scope_depth_stack, current_scope_depth)
                            elif line.startswith("using namespace"):
                                Namespace.ProcessUsingNamespace(line, using_namespace_stack, using_namespace_scope_depth_stack, current_scope_depth)
                            elif line.startswith("SPARK_CLASS("):
                                Class.ProcessClass("SPARK_CLASS", line, class_stack)
                            elif line.startswith("SPARK_CLASS_TEMPLATED("):
                                if not Class.ProcessTemplateClass(line, lines, class_stack, root + "/" + file_path):
                                    return 1
                            elif line.startswith("SPARK_POD_CLASS("):
                                Class.ProcessClass("SPARK_POD_CLASS", line, class_stack)
                            elif line.startswith("SPARK_SYSTEM("):
                                Class.ProcessClass("SPARK_SYSTEM", line, class_stack)
                            elif line.startswith("SPARK_MEMBER") and i < lineCount - 1:
                                Class.ProcessMember(line, lines[i + 1], class_stack, namespace_stack, class_list, using_namespace_stack)
                            elif line.startswith("SPARK_FUNCTION") and i < lineCount - 1:
                                Class.ProcessFunction(line, lines[i + 1], class_stack, namespace_stack, class_list, using_namespace_stack)
                            elif line.startswith("class "):
                                Class.ProcessNativeClassSecondPass("class",
                                                                   line,
                                                                   lines[i - 1].strip(),
                                                                   class_list,
                                                                   class_heirachy_map,
                                                                   namespace_stack,
                                                                   using_namespace_stack,
                                                                   class_stack,
                                                                   class_depth_stack,
                                                                   current_scope_depth,
                                                                   root + "/" + file_path,
                                                                   source_dir)
                            elif line.startswith("struct "):
                                Class.ProcessNativeClassSecondPass("struct",
                                                                   line,
                                                                   lines[i - 1].strip(),
                                                                   class_list,
                                                                   class_heirachy_map,
                                                                   namespace_stack,
                                                                   using_namespace_stack,
                                                                   class_stack,
                                                                   class_depth_stack,
                                                                   current_scope_depth,
                                                                   root + "/" + file_path,
                                                                   source_dir)
                            elif line.startswith("SPARK_ENUM"):
                                Enum.ProcessEnum(lines[i + 1].strip(), enum_list, lines, i, namespace_stack, root + "/" + file_path, source_dir)
                            elif line.startswith("SPARK_INSTANTIATE_TEMPLATE"):
                                Class.ProcessInstantiateTemplate(line,
                                                                 template_instantiations,
                                                                 namespace_stack,
                                                                 root + "/" + file_path,
                                                                 class_list,
                                                                 using_namespace_stack,
                                                                 source_dir)

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
    Components.WriteComponentsFile(components)
    Class.WriteClassFiles(finalised_reflected_classes, class_heirachy_map, template_instantiations)
    Enum.WriteEnumFiles(enum_list)
    Log.Msg("Pass 2: Done.")
    return 0


if __name__ == '__main__':
    print("-- Running Spark Header Tool...")
    ret = ProcessHeaders()
    if ret == 0:
        print("-- Done.\n")
    exit(ret)
