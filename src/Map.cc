// Copyright 2008 Isis Innovation Limited

#include "Map.h"
#include "KeyFrame.h"

Map::Map()
{
    Reset();
}

void Map::Reset()
{
    for (auto &vpPoint : vpPoints)
        delete vpPoint;
    vpPoints.clear();
    bGood = false;
    EmptyTrash();
}

void Map::MoveBadPointsToTrash()
{
    int nBad = 0;
    for(int i = (int)vpPoints.size()-1; i>=0; i--) {
        if (vpPoints[i]->bBad) {
            vpPointsTrash.push_back(vpPoints[i]);
            vpPoints.erase(vpPoints.begin() + i);
            nBad++;
        }
    }
}

void Map::EmptyTrash()
{
    for (auto &i : vpPointsTrash)
        delete i;
    vpPointsTrash.clear();
}


void MapPoint::RefreshPixelVectors()
{
    KeyFrame &k = *pPatchSourceKF;

    // Find patch pos in KF camera coords
    // Actually this might not exactly correspond to the patch pos!
    // Treat it as a general point on the plane.
    Vector<3> v3PlanePoint_C = k.se3CfromW * v3WorldPos;

    // Find the height of this above the plane.
    // Assumes the normal is  pointing toward the camera.
    double dCamHeight = fabs(v3PlanePoint_C * v3Normal_NC);

    double dPixelRate = fabs(v3Center_NC * v3Normal_NC);
    double dOneRightRate = fabs(v3OneRightFromCenter_NC * v3Normal_NC);
    double dOneDownRate = fabs(v3OneDownFromCenter_NC * v3Normal_NC);

    // Find projections onto plane
    Vector<3> v3CenterOnPlane_C = v3Center_NC * dCamHeight / dPixelRate;
    Vector<3> v3OneRightOnPlane_C = v3OneRightFromCenter_NC * dCamHeight / dOneRightRate;
    Vector<3> v3OneDownOnPlane_C = v3OneDownFromCenter_NC * dCamHeight / dOneDownRate;

    // Find differences of these projections in the world frame
    v3PixelRight_W = k.se3CfromW.get_rotation().inverse() * (v3OneRightOnPlane_C - v3CenterOnPlane_C);
    v3PixelDown_W = k.se3CfromW.get_rotation().inverse() * (v3OneDownOnPlane_C - v3CenterOnPlane_C);
}




