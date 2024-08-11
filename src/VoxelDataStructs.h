#pragma once

#include <cstdint>
#include <vector>

namespace VoxelDataStructs
{
    struct Voxel
    {
        int32_t pos[3];
        uint32_t color;
    };

    struct SVO_node
    {
        bool leaf;
        SVO_node* children[8];
        Voxel voxel;
    };

    struct SVO
    {
        bool leaf;
        SVO_node* root;

        void construct(const std::vector<Voxel>& voxelList);
        void construct64_3(const std::vector<Voxel>& voxelList);
    };

    struct Chunk
    {
        int offsetPos[3];
        SVO SVO64_3;
    };

    static std::vector<Voxel> generateChunk_debug();
}

