import lldb

def __lldb_init_module(debugger, internal_dict):
    debugger.HandleCommand("type summary add --summary-string \"${var.m_Data.__begin_}\" se::String")

# class StringSynthetic:
#     def __init__(self, valobj, internal_dict):
#
#         self.valobj = valobj
#         self.childs = {"val": self.valobj.GetChildMemberWithName("m_Data").GetChildMemberWithName("__begin_")}
#
#         self.list_childs = list(self.childs.values())
#
#     def num_children(self) -> int:
#         return len(self.childs)
#
#     def get_child_index(self, name: str):
#         return self.childs.get(name, -1)
#
#     def get_child_at_index(self, index: str):
#         return self.list_childs[index]