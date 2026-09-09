#include "FieldUI.h"

#include "EigenTypes.h"
#include "Reflection/ComponentRegistry.h"
#include "UI/Field.h"

#include <imgui.h>

namespace batap
{

// Installs the editor half of each field type: how it is drawn. Serialization
// halves live in the engine (registerBuiltinFieldTypes); a game build without
// the editor simply never fills drawUI and never calls it.
void installFieldUI()
{
    fieldTypeSlot<float>().drawUI = [](void* p, const Field& f)
    {
        ImGui::SetNextItemWidth(-1.0f);
        const bool changed = ImGui::DragFloat("##v", static_cast<float*>(p), f.meta.speed,
                                              f.meta.min, f.meta.max);
        ui::WrapDragMouse();
        return changed;
    };

    fieldTypeSlot<bool>().drawUI = [](void* p, const Field&)
    { return ImGui::Checkbox("##v", static_cast<bool*>(p)); };

    fieldTypeSlot<int32_t>().drawUI = [](void* p, const Field& f)
    {
        ImGui::SetNextItemWidth(-1.0f);
        const bool changed = ImGui::DragInt("##v", static_cast<int32_t*>(p), f.meta.speed,
                                            static_cast<int>(f.meta.min),
                                            static_cast<int>(f.meta.max));
        ui::WrapDragMouse();
        return changed;
    };

    fieldTypeSlot<uint32_t>().drawUI = [](void* p, const Field& f)
    {
        ImGui::SetNextItemWidth(-1.0f);
        const bool changed = ImGui::DragScalar("##v", ImGuiDataType_U32, p, f.meta.speed);
        ui::WrapDragMouse();
        return changed;
    };

    fieldTypeSlot<v3f>().drawUI = [](void* p, const Field& f)
    {
        auto* v = static_cast<v3f*>(p);
        ImGui::SetNextItemWidth(-1.0f);
        const bool changed = ImGui::DragFloat3("##v", v->data(), f.meta.speed);
        ui::WrapDragMouse();
        return changed;
    };

    fieldTypeSlot<col3>().drawUI = [](void* p, const Field&)
    {
        ImGui::SetNextItemWidth(-1.0f);
        return ImGui::ColorEdit3("##v", static_cast<col3*>(p)->data());
    };
}

}  // namespace batap
