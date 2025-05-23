#include "Labyrinth/Engine/ComponentPosition.hpp"

LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH()
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
LP_PRAGMA_DISABLE_ALL_WARNINGS_POP()

namespace lp
{

    glm::mat4 ComponentPosition::getModelMatrix() const
    {
        glm::mat4 mm = glm::scale(glm::mat4(1.0) ,this->mScale); 
        mm = glm::translate(mm, this->mPosisiton);
        return mm * glm::mat4_cast(this->mOrientation);
    }

}