#pragma once

#include <vector>
#include <map>
#include <Eigen/Dense>

namespace PolygonalLibrary {

struct PolygonalMesh
{
    unsigned int NumCell0Ds = 0;
    unsigned int NumCell1Ds = 0;
    unsigned int NumCell2Ds = 0;

    std::vector<unsigned int> Cell0DsId;
    std::vector<unsigned int> Cell0DsMarker;
    Eigen::MatrixXd           Cell0DsCoordinates;

    std::vector<unsigned int> Cell1DsId;
    std::vector<unsigned int> Cell1DsMarker;
    std::vector<unsigned int> Cell1DsOrigin;
    std::vector<unsigned int> Cell1DsEnd;

    std::vector<unsigned int> Cell2DsId;
    std::vector<unsigned int> Cell2DsMarker;
    std::vector<std::vector<unsigned int>> Cell2DsVertices;
    std::vector<std::vector<unsigned int>> Cell2DsEdges;

    std::map<unsigned int, unsigned int> Cell0DIdToIndex;
    std::map<unsigned int, unsigned int> Cell1DIdToIndex;
    std::map<unsigned int, unsigned int> Cell2DIdToIndex;
};

} 
