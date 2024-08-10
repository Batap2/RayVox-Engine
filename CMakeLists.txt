cmake_minimum_required(VERSION 3.27)
project(RayVox_Engine)

set(CMAKE_CXX_STANDARD 17)

# Définir les drapeaux de compilation
add_compile_options(/WX)


# Définir le répertoire des sources et include
set(source_dir "${PROJECT_SOURCE_DIR}/src")
set(include_dir "${PROJECT_SOURCE_DIR}/include")

# Définir le répertoire de sortie des exécutables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(${include_dir}/DirectX-Headers)

include_directories(${include_dir})

# Ajouter les fichiers source
file(GLOB source_files "${source_dir}/*.cpp")

# Ajouter l'exécutable avec les fichiers source
add_executable(RayVox_Engine WIN32 ${source_files}
        src/DX12Context.cpp
        src/DX12ComputeContext.cpp
        src/App.cpp
        src/CommandQueue.cpp
        src/InputManager.cpp
        resources.rc
)

target_link_libraries(RayVox_Engine d3d12.lib dxgi.lib dxguid.lib D3DCompiler.lib)


# Définir _DEBUG pour les builds Debug
target_compile_definitions(RayVox_Engine
        PRIVATE
        $<$<CONFIG:Debug>:_DEBUG>
)

#target_link_directories(RayVox_Engine PUBLIC ${PROJECT_SOURCE_DIR}/include)
#target_include_directories(RayVox_Engine PUBLIC ${PROJECT_SOURCE_DIR}/include)