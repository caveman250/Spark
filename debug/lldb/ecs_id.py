import lldb
def __lldb_init_module(debugger, internal_dict):
    debugger.HandleCommand("type summary add --summary-string \"id: ${var.id} name: ${var.name}\" se::ecs::Id")