class EcsIdPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        return str.format("id: {}, name: {}", self.val['id'], self.val['name']['m_Data']['_M_impl']['_M_start'])

    def children(self):
        yield 'id', self.val['id']
        yield 'name', self.val['name']
        yield 'flags', self.val['flags']

def lookup_type(val):
    if str(val.type) == 'se::ecs::Id':
        return EcsIdPrinter(val)
    elif str(val.type) == 'const se::ecs::Id':
        return EcsIdPrinter(val)
    elif str(val.type) == 'const se::ecs::Id *':
        return EcsIdPrinter(val)
    elif str(val.type) == 'se::ecs::Id *':
        return EcsIdPrinter(val)
    elif str(val.type) == 'const se::ecs::Id &':
        return EcsIdPrinter(val)
    elif str(val.type) == 'se::ecs::Id &':
        return EcsIdPrinter(val)
    return None


gdb.pretty_printers.append(lookup_type)