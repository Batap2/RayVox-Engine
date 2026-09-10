#include "ScenePanel.h"

#include "Components/Hierarchy_C.h"
#include "Components/Name_C.h"
#include "Instance/EntityFactory.h"
#include "Instance/InstanceManager.h"
#include "Reflection/ComponentRegistry.h"
#include "Scene.h"
#include "Systems/Hierarchy_S.h"
#include "UI/IconsMaterialDesign.h"
#include "World.h"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include <algorithm>
#include <cctype>
#include <vector>

namespace batap
{

static void sortByName(entt::registry& reg, std::vector<entt::entity>& entities)
{
    std::sort(entities.begin(), entities.end(),
              [&](entt::entity a, entt::entity b)
              {
                  const std::string& na = reg.get<Name_C>(a).name_;
                  const std::string& nb = reg.get<Name_C>(b).name_;
                  return std::lexicographical_compare(
                      na.begin(), na.end(), nb.begin(), nb.end(), [](char x, char y) {
                          return std::tolower(static_cast<unsigned char>(x)) <
                                 std::tolower(static_cast<unsigned char>(y));
                      });
              });
}

static EntityHandle duplicateEntity(World& world, EntityHandle src)
{
    auto& reg = *src.reg_;
    EntityHandle dst = world.entityFactory_->create(reg, Spawnables[0]);
    reg.get<Name_C>(dst.entity_).name_ = reg.get<Name_C>(src.entity_).name_;

    for (const ComponentType& t : ComponentRegistry::instance().all())
    {
        if (!t.tryGet || !t.tryGet(reg, src.entity_))
            continue;
        t.copy(reg, src.entity_, dst.entity_);
        if (t.meta.onDeserialized)
            t.meta.onDeserialized(dst, world);
        world.instanceManager_->markDirty(dst, t.mask);
    }

    std::vector<entt::entity> childList;
    for (entt::entity child : Hierarchy_S::children(src))
        childList.push_back(child);
    for (entt::entity child : childList)
        Hierarchy_S::attach(dst, duplicateEntity(world, {&reg, child}));

    if (auto* hc = reg.try_get<Hierarchy_C>(src.entity_); hc && hc->parent != entt::null)
        Hierarchy_S::attach({&reg, hc->parent}, dst);

    return dst;
}

void ScenePanel::drawEntityNode(World& world, entt::entity e,
                                std::optional<EntityHandle>& selectedEntity)
{
    auto& reg = world.scene_->registry_;
    EntityHandle h = {&reg, e};

    if (renaming_ && *renaming_ == h)
    {
        ImGui::SetNextItemWidth(-1.0f);
        if (renameFocusPending_)
        {
            ImGui::SetKeyboardFocusHere();
            renameFocusPending_ = false;
        }
        ImGui::InputText("##rename", &renameBuffer_);
        if (ImGui::IsItemDeactivated())
        {
            if (!renameBuffer_.empty())
                reg.get<Name_C>(e).name_ = renameBuffer_;
            renaming_.reset();
        }
        return;
    }

    const char* icon = spawnableFor(reg, e).icon;

    auto* hc = reg.try_get<Hierarchy_C>(e);
    bool hasChildren = hc && hc->firstChild != entt::null;
    bool selected = selectedEntity.has_value() && *selectedEntity == h;

    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (selected)
        flags |= ImGuiTreeNodeFlags_Selected;

    std::string label = std::string(icon) + " " + reg.get<Name_C>(e).name_;

    void* nodeId = reinterpret_cast<void*>(static_cast<uintptr_t>(entt::to_integral(e)));

    bool opened = false;
    if (hasChildren)
    {
        opened = ImGui::TreeNodeEx(nodeId, flags, "%s", label.c_str());
    }
    else
    {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        ImGui::TreeNodeEx(nodeId, flags, "%s", label.c_str());
    }

    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        selectedEntity = h;

    if (ImGui::BeginPopupContextItem())
    {
        selectedEntity = h;
        if (ImGui::MenuItem("Rename"))
        {
            renaming_ = h;
            renameBuffer_ = reg.get<Name_C>(e).name_;
            renameFocusPending_ = true;
        }
        if (ImGui::MenuItem("Duplicate"))
            pendingDuplicate_ = h;
        if (ImGui::MenuItem("Delete"))
            pendingDelete_ = h;
        ImGui::EndPopup();
    }

    // --- drag source ---
    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("ENTITY", &e, sizeof(entt::entity));
        ImGui::TextUnformatted(reg.get<Name_C>(e).name_.c_str());
        ImGui::EndDragDropSource();
    }

    // --- drop target : attache le dragged en enfant de ce noeud ---
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
        {
            entt::entity dragged = *static_cast<const entt::entity*>(payload->Data);
            if (dragged != e)
                Hierarchy_S::attach(h, {&reg, dragged});
        }
        ImGui::EndDragDropTarget();
    }

    if (opened)
    {
        // copie la liste des enfants avant de récurser (la liste peut changer via DnD)
        std::vector<entt::entity> childList;
        for (entt::entity child : Hierarchy_S::children(h))
            childList.push_back(child);
        sortByName(reg, childList);

        for (entt::entity child : childList)
            drawEntityNode(world, child, selectedEntity);

        ImGui::TreePop();
    }
}

void ScenePanel::draw(World& world, std::optional<EntityHandle>& selectedEntity)
{
    auto& reg = world.scene_->registry_;

    if (ImGui::Button(ICON_MD_ADD))
        ImGui::OpenPopup("AddEntityPopup");

    if (ImGui::BeginPopup("AddEntityPopup"))
    {
        for (const Spawnable& s : Spawnables)
        {
            const std::string label = std::string(s.icon) + " " + s.label;
            if (ImGui::MenuItem(label.c_str()))
                world.entityFactory_->create(world.scene_->registry_, s);
        }

        ImGui::EndPopup();
    }

    ImGui::Separator();

    ImGui::Text("Scene");

    // Arbre scrollable — laisse 32px en bas pour la drop zone fixe
    constexpr float kDropZoneH = 32.0f;
    ImGui::BeginChild("##scene_tree", ImVec2(0, -kDropZoneH), false,
                      ImGuiWindowFlags_HorizontalScrollbar);

    std::vector<entt::entity> roots;
    for (auto e : reg.storage<entt::entity>())
    {
        if (!reg.valid(e))
            continue;
        auto* hc = reg.try_get<Hierarchy_C>(e);
        if (hc && hc->parent != entt::null)
            continue;
        roots.push_back(e);
    }
    sortByName(reg, roots);

    for (entt::entity e : roots)
        drawEntityNode(world, e, selectedEntity);

    ImGui::EndChild();

    // Zone de drop fixe toujours visible en bas → détache l'entité draguée
    ImGui::InvisibleButton("##scenepanel_bg", ImVec2(-1, kDropZoneH));
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
        {
            entt::entity dragged = *static_cast<const entt::entity*>(payload->Data);
            Hierarchy_S::detach({&reg, dragged});
        }
        ImGui::EndDragDropTarget();
    }

    if (pendingDuplicate_)
    {
        selectedEntity = duplicateEntity(world, *pendingDuplicate_);
        pendingDuplicate_.reset();
    }
    if (pendingDelete_)
    {
        world.entityFactory_->destroy(*pendingDelete_);
        pendingDelete_.reset();
        if (selectedEntity && !reg.valid(selectedEntity->entity_))
            selectedEntity.reset();
    }
}

}  // namespace batap
