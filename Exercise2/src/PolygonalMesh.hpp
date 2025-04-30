#pragma once

#include <iostream>
#include <vector>
#include <map> //
#include "Eigen/Eigen"

using namespace std;
using namespace Eigen;

namespace PolygonalLibrary {

struct PolygonalMesh
{
    unsigned int NumCell0Ds;
    unsigned int NumCell1Ds;
    unsigned int NumCell2Ds;

    vector<unsigned int> Cell0DsId;
    vector<unsigned int> Cell0DsMarker;  

    vector<unsigned int> Cell1DsId;
    vector<unsigned int> Cell1DsMarker;    
    vector<unsigned int> Cell1DsOrigin;    
    vector<unsigned int> Cell1DsEnd;

    vector<unsigned int> Cell2DsId;
    vector<unsigned int> Cell2DsMarker;
    vector<vector<unsigned int>> Cell2DsVertices;
    vector<vector<unsigned int>> Cell2DsEdges;

    map<unsigned int, unsigned int> Cell0DIdToIndex; // <--- AGGIUNTO

    MatrixXd Cell0DsCoordinates;
    MatrixXd Cell1DsCoordinates;
    MatrixXd Cell2DsCoordinates;
};

}
