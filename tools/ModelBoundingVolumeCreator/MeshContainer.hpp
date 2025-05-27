#ifndef CDF_TOOL_MESHCONTAINER_HPP
#define CDF_TOOL_MESHCONTAINER_HPP
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <string>
#include <string_view>
#include <cstdint>

namespace lpt
{
    using MeshID_t = std::uint32_t;
    constexpr MeshID_t const_invalid_MeshID = 0;

    class MeshContainer;

    struct MeshDragDropID
    {
        std::string mModel;
        MeshID_t mID = const_invalid_MeshID;
    };

    class MeshPhysics
    {
        public:
        void ui();
        friend class MeshContainer;

        const std::vector<glm::vec3>& getPositionsVector()const {return mPositions;}
        const std::vector<unsigned int>& getIndiciesVector()const {return mIndicies;}

        private:
        std::string mName = "Mesh";
        std::vector<glm::vec3> mPositions;
        std::vector<unsigned int> mIndicies;
    };

    class MeshContainer
    {
        public:
        void ui(bool* opened);
        
        /// @brief get mesh
        /// @param cv_id id to mesh
        /// @return nullptr on error
        MeshPhysics* getMesh(const MeshDragDropID& cv_id);
        private:

        void loadMeshes(const std::string_view cv_path);
        std::unordered_map<std::string, std::unordered_map<MeshID_t, MeshPhysics>> mMeshes;
        MeshID_t mNextID = const_invalid_MeshID + 1;
        std::uint32_t mNextModelID = 1;
        char mIMGUI_path[257] = {};
    };
}

#endif //CDF_TOOL_MESHCONTAINER_HPP