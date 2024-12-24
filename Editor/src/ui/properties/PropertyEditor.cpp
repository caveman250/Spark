#include "PropertyEditor.h"

namespace se::editor::ui::properties
{
    std::unordered_map<reflect::Type*, reflect::Type*>& GetPropertyEditorTypes()
    {
        static  std::unordered_map<reflect::Type*, reflect::Type*> s_Instance = {};
        return s_Instance;
    }
}
