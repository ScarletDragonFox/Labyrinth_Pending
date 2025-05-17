#include "Labyrinth/Engine/Graphics/basicFrowardRenderer.hpp"

#include "Labyrinth/engine.hpp"
#include "Labyrinth/Engine/Graphics/regularShader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Labyrinth/Engine/Resource/modelLoader.hpp"

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
        }

        glCreateBuffers(1, &mUBO_Player);
        glNamedBufferStorage(mUBO_Player, sizeof(RendererForwardPlus_PlayerData), nullptr, GL_DYNAMIC_STORAGE_BIT);
    }

    void ForwardRenderer::render(const DebugRendererData& cv_data)
    {
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

        RegularShader shader;
        shader.LoadShader(ShaderType::SimpleColor);
        shader.Use();
        
        const glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -10, 0));;
        
        shader.SetUniform(3, model);
        shader.SetUniform(4, glm::vec3(0.1, 1.0f, 0.0f));
        renderCube();

        if(cv_data.drawCount > 0 ){
            shader.LoadShader(ShaderType::DebugLine);
            shader.Use();
            constexpr glm::mat4 modelBulletDebugDraw = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
            shader.SetUniform(3, modelBulletDebugDraw);
            glBindVertexArray(mVertexArrayBulletLineDebug);
            glVertexArrayVertexBuffer(mVertexArrayBulletLineDebug, 0, cv_data.VBO, 0, 6 * sizeof(float));
            glDrawArrays(GL_LINES, 0, cv_data.drawCount);
            glBindVertexArray(0);
        }
    
        if(cv_data.mdl != nullptr)
        {
            //std::cout << "mdl render()\n";
            shader.LoadShader(ShaderType::ModelTextured);
            shader.Use();
            shader.SetUniform(3, glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 0.0f, 0.0f)));

            lp::res::LoadedModel::MaterialID_t lastMaterial = 4'000'000'000u; //hopefully we will never a model with this many materials

            glBindVertexArray(mVertexArrayModelTextured);
            for(const auto &i : cv_data.mdl->mMeshes)
            {
                if(lastMaterial != i.mMaterialID)
                {
                    cv_data.mdl->mMaterials[i.mMaterialID].mColor.Bind(0);
                }
                //std::cout << "VAO: " << mVertexArrayModelTextured << ", VBO: " << i.mVBO << ", EBO: " << i.mEBO << ", DrawCount: " << i.mDrawCount << "\n";
                glVertexArrayVertexBuffer(mVertexArrayModelTextured, 0, i.mVBO, 0, sizeof(lp::res::ModelLoader::LoadingModel::Vertex));
                glVertexArrayElementBuffer(mVertexArrayModelTextured, i.mEBO);
                glDrawElements(GL_TRIANGLES, i.mDrawCount, GL_UNSIGNED_INT, nullptr);
            }
            glBindVertexArray(0);
            lp::gl::Texture::Unbind(0); //unbind color texture
            //std::cout << "mdl render() -- END ----\n";
        }

        glUseProgram(0);



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