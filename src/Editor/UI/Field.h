#pragma once

#include <imgui.h>
#include <imgui_internal.h>

// Usage:
//   if (auto _ = ui::BeginFields("id")) {
//       ui::Field("Position", [&]{ ... });
//   }

namespace batap::ui
{
struct BeginFields
{
    explicit BeginFields(const char* id)
    {
        active_ = ImGui::BeginTable(id, 2, ImGuiTableFlags_SizingFixedFit);
        if (active_)
        {
            ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("widget", ImGuiTableColumnFlags_WidthStretch);
        }
    } 
    ~BeginFields() { if (active_) ImGui::EndTable(); }

    explicit operator bool() const { return active_; }

    BeginFields(const BeginFields&)            = delete;
    BeginFields& operator=(const BeginFields&) = delete;

private:
    bool active_;
};

template <typename Fn>
auto Field(const char* label, Fn&& drawWidget) -> decltype(drawWidget())
{
    IM_ASSERT_USER_ERROR(ImGui::GetCurrentTable() != nullptr, "ui::Field must be called inside ui::BeginFields");
    ImGui::PushID(label);
    struct Guard { ~Guard() { ImGui::PopID(); } } guard;
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(label);
    ImGui::TableSetColumnIndex(1);
    return drawWidget();
}

inline void WrapDragMouse()
{
    if (!ImGui::IsItemActive() || !ImGui::IsMouseDown(ImGuiMouseButton_Left))
        return;

    const ImGuiViewport* vp = ImGui::GetMainViewport();
    const ImVec2 pos = ImGui::GetIO().MousePos;
    const float minX = vp->Pos.x, maxX = vp->Pos.x + vp->Size.x - 1;
    const float minY = vp->Pos.y, maxY = vp->Pos.y + vp->Size.y - 1;

    ImVec2 wrapped = pos;
    bool wrap = false;
    if (pos.x <= minX)      { wrapped.x = maxX - 1; wrap = true; }
    else if (pos.x >= maxX) { wrapped.x = minX + 1; wrap = true; }
    if (pos.y <= minY)      { wrapped.y = maxY - 1; wrap = true; }
    else if (pos.y >= maxY) { wrapped.y = minY + 1; wrap = true; }

    if (wrap)
        ImGui::TeleportMousePos(wrapped);
}

inline bool FieldDragFloat(const char* label, float* v, float speed = 1.0f, float min = 0.0f,
                           float max = 0.0f)
{
    return Field(label, [=] {
        ImGui::SetNextItemWidth(-1.0f);
        const bool changed = ImGui::DragFloat("##v", v, speed, min, max);
        WrapDragMouse();
        return changed;
    });
}

inline bool FieldDragFloat3(const char* label, float* v, float speed = 1.0f)
{
    return Field(label, [=] {
        ImGui::SetNextItemWidth(-1.0f);
        const bool changed = ImGui::DragFloat3("##v", v, speed);
        WrapDragMouse();
        return changed;
    });
}

}  // namespace batap::ui
