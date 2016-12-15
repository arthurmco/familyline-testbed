#include "AnimationData.hpp"
#include "AnimationManager.hpp"

using namespace Tribalia::Graphics;

AnimationData::AnimationData(int frameCount, int frameBase,
    std::vector<glm::vec3>* vlist)
{
    _frameCount = frameCount;
    _frameBase = frameBase;
    _frameActual = frameBase;
    _vlist = vlist;

    AnimationManager::GetInstance()->AddAnimation(this);
}

AnimationData::~AnimationData() {
    AnimationManager::GetInstance()->RemoveAnimation(this);
}

/* Inserts a frame to the animation */
void AnimationData::InsertFrame(int num, glm::vec3* vertices)
{
    std::vector<AnimationFrame> vecaf;
    bool hasDelta = false;

    /* Check if we had any difference from the base frame */
    for (int v = 0; v < _vlist->size(); v++) {
        if (vertices[v] == _vlist->at(v)) {
            hasDelta = true;

            AnimationFrame af;
            af.vindex = v;
            af.coords = glm::vec3(vertices[v].x - _vlist->at(v).x,
                vertices[v].y - _vlist->at(v).y,
                vertices[v].z - _vlist->at(v).z);

            vecaf.push_back(af);
        }


        if (v == _vlist->size()-1) {
            /*  if we had any delta, just add the last vertex, so the animation
                system know where to stop */
            if (hasDelta) {

                AnimationFrame af;
                af.vindex = v;
                af.coords = glm::vec3(vertices[v].x - _vlist->at(v).x,
                vertices[v].y - _vlist->at(v).y,
                vertices[v].z - _vlist->at(v).z);

                vecaf.push_back(af);
            }
        }
    }

    /* If we had any difference, then add the vertex data */
    if (hasDelta) {
        _frames.emplace(num, vecaf);
    } else {
        Log::GetLog()->Write("Frame %d ignored, same data than base frame", num);
    }

}

/* Bakes the vertex data for the actual frame */
void AnimationData::BakeVertexData(glm::vec3* vlist)
{
    if (_frames.find(_frameActual) == _frames.end()) {
        /* No changes, no difference */
        *vlist = *_vlist->data();
        return;
    }

    for (int i = 0; i < _vlist->size(); i++) {
        bool found = false;
        for (AnimationFrame& f : _frames[_frameActual]) {
            if (f.vindex == i) {
                vlist[i] = _vlist->at(i) + f.coords;
                found = true;
                break;
            }
        }

        if (!found) {
            vlist[i] = _frames[_frameActual][i].coords;
        }

    }
}

/* Get vertex raw data for the actual frame */
glm::vec3* AnimationData::GetVertexRawData()
{
    static int f;
    static glm::vec3* verts = nullptr;

    if (f != _frameActual) {
        if (verts) {
            delete[] verts;
            verts = nullptr;
        }
    }

    if (!verts) {
        verts = new glm::vec3[_vlist->size()];
        f = _frameActual;
        BakeVertexData(verts);
    }

    return verts;
}

void AnimationData::NextFrame()
{
    _frameActual++;
    if (_frameActual >= _frameCount)
        _frameActual = 0;
}
