# Objectifs

**État (2026-09-11)** : Vulkan only. Pipeline composant réflexif
(`BATAP_COMPONENT`), éditeur-lib, Play/Stop, hot reload du jeu (DLL + snapshot
JSON), façade gameplay (`EntityHandle`/`World`, `batap.h`), timestep fixe —
tout ça est fait ; l'historique détaillé vit dans git. Fil conducteur
inchangé : réduire ce qu'un dev doit toucher, une seule source de vérité par
concept.

---

## 1. Physique — Jolt (chantier courant)

Décisions actées :
- **Jolt** (broadphase lock-free, requêtes complètes, adopté par Godot 4.4,
  shippé par Horizon FW) — pas de physique maison, pas de BVH gameplay maison.
- Les requêtes gameplay (raycast, overlap, sweep) passent par Jolt — toute
  entité à collider est requêtable, comme `Physics.Raycast`/PhysX chez Unity.
- Le `PhysicsSystem` Jolt vit dans **`World`** (pas `Engine` : deux Worlds ne
  partagent pas leurs corps), côté hôte (jamais dans la DLL jeu).
- **v1 = formes primitives** (box/sphère/capsule) : `Mesh` ne garde aucune
  donnée CPU, un `MeshShape` exigerait de relire le `.bmesh` — plus tard.

Étapes, chacune validable seule :

- [x] **1. Vendoring + build** — fait : submodule `include/JoltPhysics` épinglé
      **v5.6.0**, `add_subdirectory(Build)`, linké dans `Batap_Engine`. Les
      defines `JPH_*` et les flags ISA sont PUBLIC sur la cible → l'ODR est
      garanti par le link (conséquence : `/arch:AVX2` s'applique à tout le
      moteur). Jolt part en **DLL** (`JPH_BUILD_SHARED_LIBS ON`, explicite) →
      une seule copie partagée éditeur/DLL jeu, `Jolt.dll` copiée dans `bin/`.
      Validé par un smoke test temporaire (supprimé depuis) : sphère lâchée
      de y=4, au repos à y≈0.48 après 120 steps (0.5 − penetration slop 2 cm,
      normal).
- [x] **2. Monde physique dans `World`** — fait : `Physics/PhysicsWorld`
      (temp allocator, job pool, layers, `JPH::PhysicsSystem`) possédé par
      `World`, `clear()` appelé dans `resetScene()` — le registry meurt au
      Play/Stop et au hot reload, les corps meurent avec. `JoltRuntime`
      (Factory + `RegisterTypes`, process-wide) est refcompté et déclaré
      premier membre : `TempAllocatorImpl` alloue déjà via l'allocateur Jolt.
      Les includes de Jolt sont passés en SYSTEM côté racine, sinon
      `-Weverything -Werror` refuse ses headers. Validé par instrumentation
      temporaire du ctor de `World` : sphère au repos à y=0.48 après 120 steps,
      2 corps → 0 après `resetScene()`.
- [ ] **3. Composants** — `RigidBody_C` (motion type, masse, friction...) et
      `Collider_C` (forme primitive + dimensions), plats, `BATAP_COMPONENT`.
      Le `BodyID` (uint32) est de l'état runtime : membre **non réfléchi**,
      jamais dans un `.btpl`.
- [ ] **4. Sync ECS ↔ Jolt** — un système hôte : corps créés à l'apparition
      du composant (hooks entt, comme les pools GPU), transforms kinematic
      poussés vers Jolt, step à `fixedDt_`, read-back des dynamic via
      `setLocalPosition/Rotation` (le markDirty suit tout seul).
- [ ] **5. Interpolation du rendu** entre les deux derniers états fixes —
      sans elle, saccade dès que `fixedDt_` est plus lent que le framerate.
- [ ] **6. Requêtes** — `world.raycast(...)`, `world.overlapSphere(...)` →
      Jolt, filtrage par layers.
- [ ] `fixedLateUpdate` seulement si un cas concret le réclame (Unity n'en a
      pas ; les contacts passent par les listeners Jolt).

## 2. Structures d'accélération (rendu)

Le partage est réglé par le §1 : Jolt possède la seule structure CPU et ne
voit que les colliders. À nous le côté rendu — trois structures, dans l'ordre.
Rappel : **le frustum culling ne demande aucune structure** — en GPU-driven,
un compute teste linéairement les AABB de toutes les instances contre les 6
plans ; l'octree/BVH de culling est une optimisation CPU d'une autre époque.
Pas d'étape intermédiaire frustum CPU : elle serait jetée au GPU-driven.

- [ ] **1. Plomberie AABB** — le socle. AABB locale par mesh calculée à
      l'import (stockée dans le `.bmesh`), AABB monde par instance recalculée
      quand le transform change (le dirty-marking sait déjà quand).
      `Bbox.hpp` sort enfin du placard.
- [ ] **2. GPU-driven culling two-phase Hi-Z** :
      1. **arena géométrique** : un draw indirect ne rebinde pas de buffers,
         or chaque mesh a le sien (`createStaticBuffer` par mesh) — tous les
         meshes dans un buffer partagé, offsets par mesh ; le `submeshIndex_`
         en push constant migre dans la donnée par-draw (`firstInstance`) ;
      2. frustum culling en compute + `vkCmdDrawIndexedIndirectCount` — le
         CPU passe de ~8000 commandes/frame à 2 ;
      3. pyramide de profondeur (HZB) min-depth depuis le depth buffer ;
      4. two-phase : dessiner les visibles de N-1 → construire la HZB →
         tester le reste → dessiner les faux-culls.
      Prérequis (cf. notes) : slots GPU stables — free-list au lieu de
      swap-remove.
- [ ] **3. Grille de clusters de lumières** (froxels) — chaque cellule de vue
      liste ses lumières. Prérequis de tout éclairage à N lumières ; ressert
      pour le brouillard volumétrique.
- [ ] **Picking éditeur par id-buffer GPU** — les ids d'entité rendus dans une
      petite target, lecture du pixel sous la souris. Pixel-perfect sur le
      mesh de rendu (les colliders Jolt sont simplifiés).
- [ ] Au besoin : **grille de hash spatiale** pour du kNN sur des entités sans
      collider. ~100 lignes, le jour venu.

### Décision différée : RT hardware

Le TLAS/BLAS driver (`VK_KHR_acceleration_structure` + `ray_query`)
débloquerait ombres/AO/réflexions puis DDGI/ReSTIR, chaque étape « un shader
de plus ». Mais ~1/3 du parc Steam n'a pas de RT (RTX ≈ 60 %, GTX ≈ 12,5 % +
vieux AMD/iGPU) : les shadow maps devront exister de toute façon, donc le RT
n'économise rien — il s'ajoute. Décision au chantier éclairage ; les trois
structures ci-dessus n'engagent rien. Pari actuel : shadow maps universelles,
RT en tier optionnel si `ray_query` présent. Alternative sans RT : SDF façon
Lumen software, beaucoup plus de code.

## 3. Restes

- [ ] **Hot reload : snapshot binaire + hash de layout** (remplace le JSON) —
      snapshot memcpy par pool (composants trivially copyable), hash de layout
      par type (nom+type+offset, tout est dans le registry) ; type inchangé →
      restore memcpy, type modifié → migration champ-par-champ payée par ce
      type seul. Coût dominé par le link, indépendant de la taille de scène.
- [ ] **`findByName`** — la seule requête qui reste côté moteur.
- [ ] **Budget de staging par frame** — un débordement lève au lieu de
      corrompre, mais une frame lourde (gros import) tue le process.
      Allocateur de staging par blocs recyclés derrière une fence.

---

## Notes / vigilance (pas des tâches)

- **Règle DLL jeu : zéro état statique** — tout état durable vit dans le World
  (composants plats). Un static dans la DLL meurt au reload.
- **IDs GPU instables** (swap-remove dans les pools) : correct aujourd'hui,
  mais le culling GPU-driven, un historique TAA ou un picking différé
  persistent des index entre frames → slots stables + free-list (§2.2).
- Le triple-buffering des instance buffers est assumé (`FramesInFlight = 3`,
  simplicité/sécurité).
- Composants avec `std::string`/`std::vector` : interdits par design (handles
  + valeurs plates) — le `static_assert` de l'enregistrement les refuse.
- **Descriptor layouts câblés à la main** (`FrameSetBindingCount`, set
  bindless) : la réflexion SPIR-V les rendrait dérivables des shaders, comme
  une UI matériaux auto-générée. Même philosophie que `BATAP_COMPONENT`. À
  faire quand le nombre de bindings fera mal.
- Le chemin **macOS** (`Platform/MacOS/*.mm`, MoltenVK) est écrit mais n'a
  jamais été compilé : prévoir une passe de fix, pas une réécriture. MoltenVK
  ne traduit pas `ray_query`.
