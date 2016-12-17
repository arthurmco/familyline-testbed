/***
    Animation control class, for each mesh

    Copyright (C) 2016 Arthur M
*/

#include <glm/glm.hpp>
#include <map>
#include <list>
#include <vector>

#ifndef ANIMATIONDATA_HPP
#define ANIMATIONDATA_HPP

namespace Tribalia {
namespace Graphics {

struct AnimationFrame {
    int vindex;         /* The vertex index */

    /* The coordinate delta, calculating from frame named on 'frameBase' */
    glm::vec3 coords;
};

class AnimationData {
private:
    int _frameCount;     // Frame count
    int _frameActual;    // Frame we're in now.

    /*  The frame that we calculate the delta, that one stored on VertexData*,
        usually the frame 0 */
    int _frameBase;

    std::vector<glm::vec3>* _vlist;

    /*  A map that binds a frame number to a list of deltas;
        When the frame is modified, it's obligatory to have the last vertex here,
        so we know when to stop.*/
    std::map<int, std::vector<AnimationFrame>> _frames;

public:
    /*  Creates animation data. frameCount is the nº of frames, vlist is the
        vertex list for frameBase */
    AnimationData(int frameCount, int frameBase, std::vector<glm::vec3>* vlist);
    ~AnimationData();

    /* Inserts a frame to the animation */
    void InsertFrame(int num, glm::vec3* vertices);

    /* Bakes the vertex data for the actual frame */
    void BakeVertexData(glm::vec3* vlist);

    /* Get vertex raw data for the actual frame */
    glm::vec3* GetVertexRawData();

    void NextFrame();

};

}
}



#endif /* end of include guard: ANIMATIONDATA_HPP */
