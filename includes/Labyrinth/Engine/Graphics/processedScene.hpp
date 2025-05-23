#ifndef LABYRINTH_PENDING_ENGINE_GRAPHICS_PROCESSEDSCENE_HPP
#define LABYRINTH_PENDING_ENGINE_GRAPHICS_PROCESSEDSCENE_HPP

#include <Labyrinth/Helpers/compilerErrors.hpp>

#include <glad/gl.h>
#include <Labyrinth/Engine/componentModel.hpp>

LP_PRAGMA_DISABLE_ALL_WARNINGS_POP();

#include <glm/mat4x4.hpp>

LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH();

#include <unordered_map>

namespace lp::gl
{
    struct ProcessedScene
    {
        /// @brief an SSB of ComponentLight::StructuredGLSL's 
        GLuint mSSB_Lights = 0;
        /// @brief an SSB of uint indices to the mSSB_Lights buffer
        GLuint mSSB_AliveLights = 0;

        /// @brief size of the mSSB_AliveLights buffer
        GLuint mLightCount = 0;
    
        struct ModelsHolder
        {
            std::shared_ptr<lp::res::LoadedModel> mPtr;
            std::vector<glm::mat4> mMatricies;
        };
        std::unordered_map<lp::res::ModelID_t, ModelsHolder> mModelMap;
    };


} // namespace lp::gl


#endif //LABYRINTH_PENDING_ENGINE_GRAPHICS_PROCESSEDSCENE_HPP