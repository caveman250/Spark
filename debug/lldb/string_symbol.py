import lldb

def Symbol_SummaryProvider(valobj, internal_dict):
    id_obj = valobj.GetChildMemberWithName('m_ID')
    if not id_obj.IsValid():
        return '""'

    m_id = id_obj.GetValueAsUnsigned(0)
    if m_id == 0:
        return '"None"'

    process = valobj.GetProcess()
    frame = process.GetSelectedThread().GetSelectedFrame()

    expr = f"se::string::SymbolRegistry::Get().GetString({m_id}).data()"
    res = frame.EvaluateExpression(expr)

    if res.GetError().Success():
        return res.GetSummary() or res.GetValue() or '"<invalid_string>"'
    return f'"<ID: {m_id}>"'

def __lldb_init_module(debugger, dict):
    module_name = __name__
    debugger.HandleCommand(f'type summary add -F {module_name}.Symbol_SummaryProvider se::string::Symbol')