#include "FieldUI.h"

#include "EigenTypes.h"
#include "Reflection/ComponentRegistry.h"
#include "UI/Field.h"

#include <imgui.h>

#include <string>
#include <unordered_map>

namespace batap
{

namespace
{
using DrawFn = bool (*)(void*, const Field&);

std::unordered_map<std::string, DrawFn>& drawUIByName()
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
    static std::unordered_map<std::string, DrawFn> map;
#pragma clang diagnostic pop
    return map;
}

template <class M>
void set(DrawFn fn)
{
    auto& slot = fieldTypeSlot<M>();
    slot.drawUI = fn;
    drawUIByName()[slot.typeName] = fn;
}
}  // namespace

// Installs the editor half of each field type: how it is drawn. Serialization
// halves live in the engine (registerBuiltinFieldTypes); a game build without
// the editor simply never fills drawUI and never calls it.
void installFieldUI()
{
    set<float>(
        [](void* p, const Field& f)
        {
            ImGui::SetNextItemWidth(-1.0f);
            const bool changed = ImGui::DragFloat("##v", static_cast<float*>(p), f.meta.speed,
                                                  f.meta.min, f.meta.max);
            ui::WrapDragMouse();
            return changed;
        });

    set<bool>([](void* p, const Field&) { return ImGui::Checkbox("##v", static_cast<bool*>(p)); });

    set<int32_t>(
        [](void* p, const Field& f)
        {
            ImGui::SetNextItemWidth(-1.0f);
            const bool changed = ImGui::DragInt("##v", static_cast<int32_t*>(p), f.meta.speed,
                                                static_cast<int>(f.meta.min),
                                                static_cast<int>(f.meta.max));
            ui::WrapDragMouse();
            return changed;
        });

    set<uint32_t>(
        [](void* p, const Field& f)
        {
            ImGui::SetNextItemWidth(-1.0f);
            const bool changed = ImGui::DragScalar("##v", ImGuiDataType_U32, p, f.meta.speed);
            ui::WrapDragMouse();
            return changed;
        });

    set<v3f>(
        [](void* p, const Field& f)
        {
            auto* v = static_cast<v3f*>(p);
            ImGui::SetNextItemWidth(-1.0f);
            const bool changed = ImGui::DragFloat3("##v", v->data(), f.meta.speed);
            ui::WrapDragMouse();
            return changed;
        });

    set<col3>(
        [](void* p, const Field&)
        {
            ImGui::SetNextItemWidth(-1.0f);
            return ImGui::ColorEdit3("##v", static_cast<col3*>(p)->data());
        });
}

bool installFieldUIFor(FieldType& type)
{
    auto it = drawUIByName().find(type.typeName);
    if (it == drawUIByName().end())
        return false;
    type.drawUI = it->second;
    return true;
}

}  // namespace batap
