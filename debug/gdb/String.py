class StringPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        return self.val['m_Data']['_M_impl']['_M_start']

    def children(self):
        yield 'm_Data', self.val['m_Data']

def lookup_type(val):
    if str(val.type) == 'se::String':
        return StringPrinter(val)
    elif str(val.type) == 'const se::String *':
        return StringPrinter(val)
    elif str(val.type) == 'se::String *':
        return StringPrinter(val)
    elif str(val.type) == 'const se::String &':
        return StringPrinter(val)
    elif str(val.type) == 'se::String &':
        return StringPrinter(val)
    return None


gdb.pretty_printers.append(lookup_type)