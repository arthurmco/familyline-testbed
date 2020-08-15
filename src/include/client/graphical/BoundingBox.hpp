/***
    Bounding box representation

    Copyright (C) 2016 Arthur Mendes.

***/

#include <glm/glm.hpp>

#ifndef BOUNDINGBOX_HPP
#define BOUNDINGBOX_HPP

namespace familyline::graphics
{
enum BoundingBoxPoints {
    BOUNDING_BOX_LOWER_LEFT_FRONT,
    BOUNDING_BOX_LOWER_LEFT_BACK,
    BOUNDING_BOX_LOWER_RIGHT_BACK,
    BOUNDING_BOX_LOWER_RIGHT_FRONT,
    BOUNDING_BOX_UPPER_RIGHT_FRONT,
    BOUNDING_BOX_UPPER_RIGHT_BACK,
    BOUNDING_BOX_UPPER_LEFT_BACK,
    BOUNDING_BOX_UPPER_LEFT_FRONT,

    BOUNDING_BOX_COUNT,
};

enum BoundingBoxFaces {
    BOUNDING_BOX_FACE_TOP,
    BOUNDING_BOX_FACE_BOTTOM,
    BOUNDING_BOX_FACE_LEFT,
    BOUNDING_BOX_FACE_RIGHT,
    BOUNDING_BOX_FACE_FRONT,
    BOUNDING_BOX_FACE_BACK,

    BOUNDING_BOX_FACE_COUNT,
};

class BoundingBox
{
public:
    glm::vec3 points[BOUNDING_BOX_COUNT];
    float rotation;
    float minX, minY, minZ, maxX, maxY, maxZ;

    /* Check if normal collides with bouding box.
        Returns the face index, or -1 if no collision happens*/
    int CheckCollisionOnRay(glm::vec3 ray) const;

    /* Check if two bounding boxes collide.
   If true, return the faces that the first bounding box
        (the method owner) collides at 'firstFaces' and the faces that
        it collides on the another bb at 'secondFaces'.
        If false, return false. */
    bool CheckCollisionOnBoundingBox(BoundingBox, int* firstfaces, int* secondFaces) const;
};

}  // namespace familyline::graphics

#endif /* end of include guard: BOUNDINGBOX_HPP */
