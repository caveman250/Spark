from src import Namespace
from src import Components
import os
from src import Log

def ProcessWidget(widget_list, path, class_stack, components):
    type = class_stack[-1].name
    namespace = class_stack[-1].namespace
    dev_only = class_stack[-1].dev_only
    editor_only = class_stack[-1].editor_only
    component = Components.ComponentFile(os.path.abspath(path), type, namespace, dev_only, editor_only)
    widget_list.append(component)
    components.append(component)

def IncludeWidgetFiles(widget_list):
    ret = ""
    for i in range(len(widget_list)):
        ret += "#include \"" + widget_list[i].path + "\"\n"
    return ret

def WriteWidgetTypesMacros(widget_list):
    ret = "\n#define SPARK_WIDGET_TYPES "
    for i in range(len(widget_list)):
        if len(widget_list[i].namespace) > 0:
            ret += widget_list[i].namespace + "::"
        ret += widget_list[i].name
        if i < len(widget_list) - 1:
            ret += ","

    ret += "\n#define SPARK_CONST_WIDGET_TYPES "
    for i in range(len(widget_list)):
        ret += "const "
        if len(widget_list[i].namespace) > 0:
            ret += widget_list[i].namespace + "::"
        ret += widget_list[i].name
        if i < len(widget_list) - 1:
            ret += ","

    ret += "\n#define SPARK_WIDGET_TYPES_WITH_NULLTYPE "
    for i in range(len(widget_list)):
        if len(widget_list[i].namespace) > 0:
            ret += widget_list[i].namespace + "::"
        ret += widget_list[i].name
        if i < len(widget_list) - 1:
            ret += ","
    ret += ",se::ecs::NullComponentType"

    ret += "\n#define SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE "
    for i in range(len(widget_list)):
        ret += "const "
        if len(widget_list[i].namespace) > 0:
            ret += widget_list[i].namespace + "::"
        ret += widget_list[i].name
        if i < len(widget_list) - 1:
            ret += ","
    ret += ",const se::ecs::NullComponentType"

    ret += "\n#define SPARK_WIDGET_POINTER_TYPES "
    for i in range(len(widget_list)):
        if len(widget_list[i].namespace) > 0:
            ret += widget_list[i].namespace + "::"
        ret += widget_list[i].name
        ret += "*"
        if i < len(widget_list) - 1:
            ret += ","

    ret += "\n#define SPARK_WIDGET_CONST_POINTER_TYPES "
    for i in range(len(widget_list)):
        ret += "const "
        if len(widget_list[i].namespace) > 0:
            ret += widget_list[i].namespace + "::"
        ret += widget_list[i].name
        ret += "*"
        if i < len(widget_list) - 1:
            ret += ","

    ret += "\n#define SPARK_WIDGET_POINTER_TYPES_WITH_NULLTYPE "
    for i in range(len(widget_list)):
        if len(widget_list[i].namespace) > 0:
            ret += widget_list[i].namespace + "::"
        ret += widget_list[i].name
        ret += "*"
        if i < len(widget_list) - 1:
            ret += ","
    ret += ",se::ecs::NullComponentType*"

    ret += "\n#define SPARK_WIDGET_CONST_POINTER_TYPES_WITH_NULLTYPE "
    for i in range(len(widget_list)):
        ret += "const "
        if len(widget_list[i].namespace) > 0:
            ret += widget_list[i].namespace + "::"
        ret += widget_list[i].name
        ret += "*"
        if i < len(widget_list) - 1:
            ret += ","
    ret += ",const se::ecs::NullComponentType*"

    return ret

def WriteWidgetVariant():
    ret = "\ntypedef std::variant<SPARK_WIDGET_POINTER_TYPES> WidgetVariant;"
    ret += "\ntypedef std::variant<SPARK_WIDGET_CONST_POINTER_TYPES> ConstWidgetVariant;"
    return ret

def WriteWidgetHeader(widget_list):
    contents = "#pragma once\n\n#include \"spark.h\"\n"

    contents += IncludeWidgetFiles(widget_list)
    contents += WriteWidgetTypesMacros(widget_list)
    contents += WriteWidgetVariant()

    output_dir = "../../engine/src/generated/"
    output_path = "../../engine/src/generated/Widgets.generated.h"
    if not os.path.exists(output_dir):
        os.mkdir(output_dir)

    existing_contents = ""
    if os.path.isfile(output_path):
        input_handle = open(output_path, "r")
        existing_contents = input_handle.read()
        input_handle.close()
    if existing_contents != contents:
        Log.Msg("Widgets.generated.h generating...")
        output_handle = open(output_path, "w+")
        output_handle.write(contents)
        output_handle.close()