#pragma once

#include <string>
#include "PolygonalMesh.hpp"

namespace PolygonalLibrary
{
    bool ImportCell0Ds(const std::string& path, PolygonalMesh& mesh);
    bool ImportCell1Ds(const std::string& path, PolygonalMesh& mesh);
    bool ImportCell2Ds(const std::string& path, PolygonalMesh& mesh);
    bool ImportMesh(const std::string& basePath, PolygonalMesh& mesh);

    bool ValidateEdges(const PolygonalMesh& mesh);
    bool ValidatePolygons(const PolygonalMesh& mesh);
}
