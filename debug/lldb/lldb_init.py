def __lldb_init_module(debugger, internal_dict):
    debugger.HandleCommand("command script import ecs_id.py")
    debugger.HandleCommand("command script import string.py")