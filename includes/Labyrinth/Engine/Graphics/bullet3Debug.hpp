#ifndef LABYRINTH_PENDING_ENGINE_GRAPHICS_BULLET3DEBUG_HPP
#define LABYRINTH_PENDING_ENGINE_GRAPHICS_BULLET3DEBUG_HPP

#include <bullet/LinearMath/btIDebugDraw.h>

#include <iostream>
#include <vector>
#include <glm/vec3.hpp>
#include <glad/gl.h>

namespace lp::gl
{
    /// @brief class that implements btIDebugDraw for the Bullet3 Physics Engine
    class Bullet3Debug: public btIDebugDraw
    {
        public:
        /// @brief internal struct representing a Vertex
        struct Vertex
        {
            /// @brief position
            glm::vec3 position = {};
            /// @brief color <0,1>
            glm::vec3 colour = {};
        };

        

        /// @brief draw a line
        /// @param from point from
        /// @param to point to
        /// @param color color of line
        virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& color) override final
        {
            const Vertex v = 
            {
                .position = {from.getX(), from.getY(), from.getZ()},
                .colour =  {color.getX(), color.getY(), color.getZ()},
            };
            mVerticies.push_back(v);
            const Vertex v2 = 
            {
                .position = {to.getX(), to.getY(), to.getZ()},
                .colour =  {color.getX(), color.getY(), color.getZ()},
            };
            mVerticies.push_back(v2);
            mDrawCount += 2;
            //std::cout << "BULLET::drawLine( from = (" << from.getX() << " ; " << from.getY() << " ; " << from.getZ() << 
            //"), to = (" << to.getX() << " ; " << to.getY() << " ; " << to.getZ() <<
            //"), color = (" << color.getX() << " ; " << color.getY() << " ; " << color.getZ() << "))\n";
        }

        /// @brief set this to specfy what functionality is implemented
        /// @return bitmap of btIDebugDraw::DebugDrawModes's that this class supports
        virtual int getDebugMode() const override final
        {
            return btIDebugDraw::DBG_DrawAabb | btIDebugDraw::DBG_DrawConstraintLimits | btIDebugDraw::DBG_DrawConstraints |
            btIDebugDraw::DBG_DrawFrames | btIDebugDraw::DBG_DrawNormals | btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_EnableCCD | btIDebugDraw::DBG_ProfileTimings;

            //return btIDebugDraw::DBG_DrawAabb | btIDebugDraw::DBG_DrawConstraintLimits | btIDebugDraw::DBG_DrawConstraints | btIDebugDraw::DBG_DrawContactPoints |
            //btIDebugDraw::DBG_DrawFeaturesText | btIDebugDraw::DBG_DrawFrames | btIDebugDraw::DBG_DrawNormals | btIDebugDraw::DBG_DrawText | btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_EnableCCD | btIDebugDraw::DBG_ProfileTimings;
        }
        /// @brief draw a line with interpolated colors
        /// @param from Position from
        /// @param to Position to
        /// @param fromColor Color from
        /// @param toColor Color to
        virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) override final
        {
            const Vertex v = 
            {
                .position = {from.getX(), from.getY(), from.getZ()},
                .colour =  {fromColor.getX(), fromColor.getY(), fromColor.getZ()},
            };
            mVerticies.push_back(v);
            const Vertex v2 = 
            {
                .position = {to.getX(), to.getY(), to.getZ()},
                .colour =  {toColor.getX(), toColor.getY(), toColor.getZ()},
            };
            mVerticies.push_back(v2);
            mDrawCount += 2;
            // std::cout << "BULLET::drawLine( from = (" << from.getX() << " ; " << from.getY() << " ; " << from.getZ() << 
            // "), to = (" << to.getX() << " ; " << to.getY() << " ; " << to.getZ() <<
            // "), fromColor = (" << fromColor.getX() << " ; " << fromColor.getY() << " ; " << fromColor.getZ() <<
            // "), toColor = (" << toColor.getX() << " ; " << toColor.getY() << " ; " << toColor.getZ() << "))\n";
        }

        //TODO: when logging is done, get logging in the stud emty function overrides

        /// @brief gets called every draw by bullet in the beggining
        virtual void clearLines()
        {
            //std::cout << "BULLET::clearLines()\n";
            this->mVerticies.clear();
            this->mDrawCount = 0;
        }

        /// @brief gets called every draw by bullet at the end - NOT
        ///
        /// UPDATE: this NEVER gets CALLED!!
        /// Call this yourself!!
        virtual void flushLines()
        {
            //std::cout << "POOP\n";
            //make buffer smaller if 2x too big
            //make buffer anew if too small
            if(mVertexBuffer != 0 && (mVertexBufferSize * 2 >  mVerticies.size() || mVertexBufferSize < mVerticies.size())) 
            {
                glDeleteBuffers(1, &mVertexBuffer); mVertexBuffer = 0;
            }

            if(mVertexBuffer == 0)
            {
                glCreateBuffers(1, &mVertexBuffer);
                glNamedBufferStorage(mVertexBuffer, mVerticies.size() * sizeof(lp::gl::Bullet3Debug::Vertex), mVerticies.data(), GL_DYNAMIC_STORAGE_BIT);
                mVertexBufferSize = mVerticies.size();
                return;
            }

            if(mVertexBuffer != 0 && mVertexBufferSize >= mVerticies.size())
            {
                glNamedBufferSubData(mVertexBuffer, 0, mVerticies.size() * sizeof(lp::gl::Bullet3Debug::Vertex), mVerticies.data());
            } else
            {
                glDeleteBuffers(1, &mVertexBuffer); mVertexBuffer = 0;
                glCreateBuffers(1, &mVertexBuffer);
                glNamedBufferStorage(mVertexBuffer, mVerticies.size() * sizeof(lp::gl::Bullet3Debug::Vertex), mVerticies.data(), GL_DYNAMIC_STORAGE_BIT);
                mVertexBufferSize = mVerticies.size();
            }
            //std::cout << "BULLET::flushLines()\n";
        }

        /// @brief sets the debug mode
        /// @param debugMode bitmap of btIDebugDraw's to use
        /// @note NOT IMPLEMPLEMENTED
        virtual void setDebugMode(int debugMode)
        {
            std::cout << "BULLET::setDebugMode(" << debugMode << ")\n";
        }

        /// @brief draws text in 3d as a billboard facing the player
        /// @param location position to display the text in
        /// @param textString text to display
        /// @note NOT IMPLEMPLEMENTED
        virtual void draw3dText(const btVector3& location, const char* textString)
        {
            std::cout << "BULLET::draw3dText( location = (" << location.getX() << " ; " << location.getY() << " ; " <<location.getZ() << "), textString = \"" << textString << "\")\n";
        }
        /// @brief report an error warning
        /// @param warningString error warning text
        /// @note NOT IMPLEMPLEMENTED
        virtual void reportErrorWarning(const char* warningString)
        {
            std::cout << "BULLET::reportErrorWarning(\"" << warningString << "\")\n";
        }

        /// @brief draws a contact point
        /// @note NOT IMPLEMPLEMENTED 
        virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
        {
            std::cout << "BULLET::drawContactPoint( PointOnB = (" << PointOnB.getX() << " ; " << PointOnB.getY() << " ; " <<PointOnB.getZ() << 
            "), normalOnB = (" << normalOnB.getX() << " ; " << normalOnB.getY() << " ; " <<normalOnB.getZ() <<
            "), distance = " << distance <<
            " lifeTime = " << lifeTime <<
            " color = (" << color.getX() << " ; " << color.getY() << " ; " <<color.getZ() << "))\n";
        }

        /// @brief sets the default colours
        /// @param colors colours to use for diffrent situations
        /// @note NOT IMPLEMPLEMENTED 
        void setDefaultColors([[maybe_unused]] const DefaultColors& colors)
        {
            std::cout << "BULLET::setDefaultColors( ... )\n";
        }

        /// @brief virtual destructor
        virtual ~Bullet3Debug()
        {
            if(mVertexBuffer)
            {
                glDeleteBuffers(1, &mVertexBuffer);
            }
        }

        /// @brief get the OpenGL buffer used for drawing.
        /// @return buffer ID or 0 if buffer uninitialized
        inline GLuint getBuffer() const
        {
            return mVertexBuffer;
        }

        /// @brief get the triangle draw count.
        /// @return draw count, or 0 if uninitialized
        inline GLuint getDrawCount() const
        {
            return mDrawCount;
        }   

        private:
        GLuint mVertexBuffer = 0;
        GLuint mVertexBufferSize = 0;

        /// @brief storage for Verticies
        std::vector<Vertex> mVerticies;

        /// @brief number of lines to draw
        GLuint mDrawCount = 0;

        //all other functions either have proper implementations, or have temp ones to avoid a crash
    };
}

#endif //LABYRINTH_PENDING_ENGINE_GRAPHICS_BULLET3DEBUG_HPP