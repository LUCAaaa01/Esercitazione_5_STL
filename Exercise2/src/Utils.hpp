#pragma once

#include <iostream>
#include "PolygonalMesh.hpp"

using namespace std;

namespace PolygonalLibrary
{

bool ImportMesh(PolygonalMesh& mesh);


bool ImportCell0Ds(const string& inputFilePath, PolygonalMesh& mesh);


bool ImportCell1Ds(const string& inputFilePath, PolygonalMesh& mesh);


bool ImportCell2Ds(const string& inputFilePath, PolygonalMesh& mesh);

}
