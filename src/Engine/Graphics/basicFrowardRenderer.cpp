#include "Labyrinth/Engine/Graphics/basicFrowardRenderer.hpp"

#include "Labyrinth/engine.hpp"
#include "Labyrinth/Engine/Graphics/regularShader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Labyrinth/Engine/Resource/modelLoader.hpp"

#include <stb_image.h>

#include "Labyrinth/Engine/ECS/coreECS.hpp"
#include "Labyrinth/Engine/ComponentPosition.hpp"
#include "Labyrinth/Engine/ComponentPhysics.hpp"
#include "Labyrinth/Engine/ComponentSoundSource.hpp"
#include "Labyrinth/Engine/ComponentLight.hpp"

#include "Labyrinth/Engine/Graphics/debugPremadeMesh.hpp"

namespace
{
    void renderCube();
}

namespace lp::gl
{
    void ForwardRenderer::setup(const unsigned int cv_width, unsigned int cv_height)
    {
        this->mOutBuff.create(cv_width, cv_height);
        mFramebufferAttachColorID = this->mOutBuff.createTexture(Framebuffer::Image_Format::RGB32F);
        mFramebufferAttachDepthID = this->mOutBuff.createTexture(Framebuffer::Image_Format::DEPTH32F);

        g_engine.getEventManager().on(lp::EventTypes::WindowResize, [this](lp::Event& r_event){
            const auto& data = r_event.getData<lp::evt::WindowResize>();
            this->mOutBuff.resize(data.width, data.height);
        });

        {
            glCreateVertexArrays(1, &mVertexArrayModelTextured);
            glEnableVertexArrayAttrib(mVertexArrayModelTextured, 0);
            glEnableVertexArrayAttrib(mVertexArrayModelTextured, 1);
            glEnableVertexArrayAttrib(mVertexArrayModelTextured, 2);
            glEnableVertexArrayAttrib(mVertexArrayModelTextured, 3);
            glVertexArrayAttribFormat(mVertexArrayModelTextured, 0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribFormat(mVertexArrayModelTextured, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
            glVertexArrayAttribFormat(mVertexArrayModelTextured, 2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float));
            glVertexArrayAttribFormat(mVertexArrayModelTextured, 3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float));
            glVertexArrayAttribBinding(mVertexArrayModelTextured, 0, 0);
            glVertexArrayAttribBinding(mVertexArrayModelTextured, 1, 0);
            glVertexArrayAttribBinding(mVertexArrayModelTextured, 2, 0);
            glVertexArrayAttribBinding(mVertexArrayModelTextured, 3, 0);

            glCreateVertexArrays(1, &mVertexArrayBulletLineDebug);
            glEnableVertexArrayAttrib(mVertexArrayBulletLineDebug, 0);
            glEnableVertexArrayAttrib(mVertexArrayBulletLineDebug, 1);
            glVertexArrayAttribFormat(mVertexArrayBulletLineDebug, 0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribFormat(mVertexArrayBulletLineDebug, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
            glVertexArrayAttribBinding(mVertexArrayBulletLineDebug, 0, 0);
            glVertexArrayAttribBinding(mVertexArrayBulletLineDebug, 1, 0);

            

            glCreateVertexArrays(1, &mVertexArrayDummy);
        }


        {
            glCreateVertexArrays(1, &mVertexArrayISOSphere);
            glEnableVertexArrayAttrib(mVertexArrayISOSphere, 0);
            glVertexArrayAttribFormat(mVertexArrayISOSphere, 0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(mVertexArrayISOSphere, 0, 0);
            glCreateBuffers(1, &mVertexBufferISOSphere);
            glCreateBuffers(1, &mElementBufferISOSphere);
            glNamedBufferStorage(mVertexBufferISOSphere, sizeof(isoSphere::Positions), isoSphere::Positions, 0);
            glNamedBufferStorage(mElementBufferISOSphere, sizeof(isoSphere::Indicies), isoSphere::Indicies, 0);

            glVertexArrayVertexBuffer(mVertexArrayISOSphere, 0, mVertexBufferISOSphere, 0, sizeof(isoSphere::Positions[0]));
            glVertexArrayElementBuffer(mVertexArrayISOSphere, mElementBufferISOSphere);
        }

        glCreateBuffers(1, &mUBO_Player);
        glNamedBufferStorage(mUBO_Player, sizeof(RendererForwardPlus_PlayerData), nullptr, GL_DYNAMIC_STORAGE_BIT);


        {
            int x = 0; int y = 0; int channels = 0;
            stbi_uc* data = stbi_load("assets/images/icons/reshot-icon-light-max.png", &x, &y, &channels, 4); //
            if(data){
                this->mTexLightIcon.create(lp::gl::Format::RGBA8, glm::uvec2(x, y), data, 0, true);
                stbi_image_free(data);
            } else std::cout << "Couldn't load icon-light-max: " << stbi_failure_reason() << "\n";
            
            data = stbi_load("assets/images/icons/reshot-icon-high-audio.png ", &x, &y, &channels, 4);
            if(data){
                this->mTexSoundIcon.create(lp::gl::Format::RGBA8, glm::uvec2(x, y), data, 0, true);
                stbi_image_free(data);
            } else std::cout << "Couldn't load icon-high-audio: " << stbi_failure_reason() << "\n";
        }
    }

    void ForwardRenderer::render(const lp::gl::ProcessedScene& cv_pscene)
    {
        glViewport(0, 0, this->mOutBuff.getWidth(), this->mOutBuff.getHeight());
        
        glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE); //set clipping plane to [0,1], instead of the default [-1,1] 
        glDepthFunc(GL_GREATER); //these 3 reverse the depth buffer
        glClearDepth(0); //clear depth to 0

        glEnable(GL_MULTISAMPLE);
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK); //cull back faces

        glEnable(GL_DEPTH_TEST);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //sets the clear colour to black (red)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, mUBO_Player);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, cv_pscene.mSSB_Lights);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, cv_pscene.mSSB_AliveLights);

        RegularShader shader;
        shader.LoadShader(ShaderType::SimpleColor);
        shader.Use();
        
        const glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -10, 0));
        
        shader.SetUniform(3, model);
        shader.SetUniform(4, glm::vec3(0.1, 1.0f, 0.0f));
        renderCube();

        if(mTriggerDrawDebugBullet && cv_pscene.mBulletDebugDrawCount > 0 ){
            shader.LoadShader(ShaderType::DebugLine);
            shader.Use();
            constexpr glm::mat4 modelBulletDebugDraw = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
            shader.SetUniform(3, modelBulletDebugDraw);
            glBindVertexArray(mVertexArrayBulletLineDebug);
            glVertexArrayVertexBuffer(mVertexArrayBulletLineDebug, 0, cv_pscene.mBulletDebugBuffer, 0, 6 * sizeof(float));
            glDrawArrays(GL_LINES, 0, cv_pscene.mBulletDebugDrawCount);
            glBindVertexArray(0);
        }
        
        if(cv_pscene.mModelMap.size() >= 1){
            shader.LoadShader(ShaderType::ModelTextured);
            shader.Use();
            glBindVertexArray(mVertexArrayModelTextured);
            for(const auto& mdl: cv_pscene.mModelMap)
            {
                lp::res::LoadedModel::MaterialID_t lastMaterial = 4'000'000'000u; //hopefully we will never a model with this many materials
                for(const auto& mesh: mdl.second.mPtr->mMeshes)
                {
                    if(lastMaterial != mesh.mMaterialID)
                    {
                        lp::res::LoadedModel::Material& mat = mdl.second.mPtr->mMaterials[mesh.mMaterialID];
                        mat.mColor.Bind(0);
                        mat.mSpecular.Bind(1);
                        shader.SetUniform(4, mat.mShininess);
                        lastMaterial = mesh.mMaterialID;
                    }
                    glVertexArrayVertexBuffer(mVertexArrayModelTextured, 0, mesh.mVBO, 0, sizeof(lp::res::VertexFull));
                    glVertexArrayElementBuffer(mVertexArrayModelTextured, mesh.mEBO);

                    for(const auto& mat:mdl.second.mMatricies)
                    {
                        shader.SetUniform(3, mat);
                        glDrawElements(GL_TRIANGLES, mesh.mDrawCount, GL_UNSIGNED_INT, nullptr);
                    }
                }
            }
            glBindVertexArray(0);
            lp::gl::Texture::Unbind(0);
            lp::gl::Texture::Unbind(1);
        }

        if(cv_pscene.mLightCount != 0)
        {
            shader.LoadShader(ShaderType::IsoLightSphere);
            shader.Use();
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glBindVertexArray(mVertexArrayISOSphere);
            glDisable(GL_CULL_FACE);
            glDrawElementsInstanced(GL_TRIANGLES, sizeof(isoSphere::Indicies) / sizeof(isoSphere::Indicies[0]), GL_UNSIGNED_INT, nullptr, cv_pscene.mLightCount);
            glEnable(GL_CULL_FACE);
            glBindVertexArray(0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        glUseProgram(0);

        DrawDebugIcons();
    }


    void ForwardRenderer::debug001(std::vector<glm::vec3>& lightpositions, lp::gl::Texture& texture)
    {
        if(lightpositions.size() == 0) return;
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        RegularShader shader;
        shader.LoadShader(ShaderType::BillboardIcon);
        shader.Use();
        texture.Bind(0);
        glBindVertexArray(this->mVertexArrayDummy);
        for(auto& pos:lightpositions)
        {
            shader.SetUniform(3, pos);
            
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glBindVertexArray(0);
        lp::gl::Texture::Unbind(0);
        glEnable(GL_CULL_FACE);
    }

    void ForwardRenderer::updatePlayer(const lp::Player& cr_player, bool cv_wasChanged)
    {
        if(cv_wasChanged == false) return; //for now
        mPlayerData.mPosition = cr_player.getPosition();
        mPlayerData.mProjection = cr_player.getProjectionMatrix(mOutBuff.getWidthDivHeight());
        mPlayerData.mView = cr_player.getViewMatrix();
        glNamedBufferSubData(mUBO_Player, 0, sizeof(RendererForwardPlus_PlayerData), &mPlayerData);
    }

    void ForwardRenderer::destroy()
    {

    }

    void ForwardRenderer::DrawDebugIcons()
    {
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        RegularShader shader;
        shader.LoadShader(ShaderType::BillboardIcon);
        shader.Use();
        mTexSoundIcon.Bind(0);
        glBindVertexArray(this->mVertexArrayDummy);
        auto& Recs = lp::g_engine.getECS();
        if(this->mTriggerDrawDebugSoundIcons)
        {
            const lp::ecs::Signature CPSign =  Recs.getComponentSignature<lp::ComponentPosition>();
            const lp::ecs::Signature CSSSign =  Recs.getComponentSignature<lp::ComponentSoundSource>();
            const lp::ecs::Signature CPhySign =  Recs.getComponentSignature<lp::ComponentPhysics>();
            for(const auto& entit: Recs.getDebugEntityMap())
            {
                if(entit.second & CSSSign)
                {
                    glm::vec3 position = {};
                    if(entit.second & CPSign)
                    {
                        position = Recs.getComponent<lp::ComponentPosition>(entit.first).getPosition();
                    } else if(entit.second & CPhySign)
                    {
                        position = Recs.getComponent<lp::ComponentPhysics>(entit.first).getPosition();
                    }
                    shader.SetUniform(3, position);
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                }
            }
        }
        if(this->mTriggerDrawDebugLightIcons)
        {
            mTexLightIcon.Bind(0);
            const lp::ecs::Signature CLSign =  Recs.getComponentSignature<lp::ComponentLight>();
            for(const auto& entit: Recs.getDebugEntityMap())
            {
                if(entit.second & CLSign)
                {
                    shader.SetUniform(3, Recs.getComponent<lp::ComponentLight>(entit.first).getPosition());
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                }
            }
        }
         glBindVertexArray(0);
        lp::gl::Texture::Unbind(0);
        glEnable(GL_CULL_FACE);
    }
}

namespace
{

    void renderCube()
    {
        static unsigned int cubeVAO = 0;
        static unsigned int cubeVBO = 0;
        // initialize (if necessary)
        if (cubeVAO == 0)
        {
            float vertices[] = {
                // back face
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
                1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
                // front face
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                // left face
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                // right face
                1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
                1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
                // bottom face
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                // top face
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
                1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
            };

            glCreateBuffers(1, &cubeVBO);
            glCreateVertexArrays(1, &cubeVAO);

            glNamedBufferStorage(cubeVBO, sizeof(vertices), &vertices[0], 0);

            glVertexArrayVertexBuffer(cubeVAO, 0, cubeVBO, 0, 8 * sizeof(float));

            glEnableVertexArrayAttrib(cubeVAO, 0);
            glEnableVertexArrayAttrib(cubeVAO, 1);
            glEnableVertexArrayAttrib(cubeVAO, 2);
            glVertexArrayAttribFormat(cubeVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribFormat(cubeVAO, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
            glVertexArrayAttribFormat(cubeVAO, 2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float));
            glVertexArrayAttribBinding(cubeVAO, 0, 0);
            glVertexArrayAttribBinding(cubeVAO, 1, 0);
            glVertexArrayAttribBinding(cubeVAO, 2, 0);
        }
        // render Cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
}