#include "mCamera.h"

void mCamera::UpdateFromDirection(Vector3 pos, Vector3 dir, Vector3 u, const CameraMode &mode)
{
    position = pos;
    direction = Vector3Normalize(dir);
    up = Vector3Normalize(u);
    right = Vector3CrossProduct(direction, up);
    target = Vector3Add(position, direction);
    UpdateCamera(this, mode);
}
void mCamera::UpdateFromTarget(Vector3 pos, Vector3 tar, Vector3 u, const CameraMode &mode)
{
    position = pos;
    target = tar;
    UpdateCamera(this, mode);
    direction = Vector3Normalize(Vector3Subtract(target, position));
    up = Vector3Normalize(u);
    right = Vector3CrossProduct(direction, up);
}
void mCamera::Rotate(float lookHorizontal, float lookVertical) {
    
    direction = Vector3RotateByAxisAngle(direction, up, lookHorizontal);
    right = Vector3RotateByAxisAngle(right, up, lookHorizontal);
    direction = Vector3RotateByAxisAngle(direction, right, lookVertical);
    up = Vector3RotateByAxisAngle(up, right, lookVertical);
    UpdateFromDirection(position, direction, up);
}