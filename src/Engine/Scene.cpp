#include "Scene.h"

namespace batap
{
Scene::Scene(GPUInstanceManager& instanceManager) : instanceManager_(instanceManager)
{
    instanceManager.connectHooks(registry_);
}
}  // namespace batap
