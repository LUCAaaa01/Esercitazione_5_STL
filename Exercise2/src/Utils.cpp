// Utils.cpp
#include "Utils.hpp"
#include <fstream>
#include <sstream>
#include <cmath>

namespace PolygonalLibrary
{

bool ImportCell0Ds(const std::string& path, PolygonalMesh& mesh)
{
    std::ifstream f(path + "/Cell0Ds.csv");
    if (!f) return false;
    std::string line;
    std::getline(f, line);

    mesh.Cell0DIdToIndex.clear();
    std::vector<unsigned int> ids, marks;
    std::vector<double> xs, ys;

    while (std::getline(f, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string tmp;
        std::getline(ss, tmp, ';'); unsigned id = std::stoul(tmp);
        std::getline(ss, tmp, ';'); unsigned mk = std::stoul(tmp);
        std::getline(ss, tmp, ';'); double x = std::stod(tmp);
        std::getline(ss, tmp, ';'); double y = std::stod(tmp);

        mesh.Cell0DIdToIndex[id] = ids.size();
        ids.push_back(id);
        marks.push_back(mk);
        xs.push_back(x);
        ys.push_back(y);
    }

    mesh.NumCell0Ds = ids.size();
    mesh.Cell0DsId = std::move(ids);
    mesh.Cell0DsMarker = std::move(marks);
    mesh.Cell0DsCoordinates.resize(3, mesh.NumCell0Ds);
    for (unsigned i = 0; i < mesh.NumCell0Ds; ++i) {
        mesh.Cell0DsCoordinates(0,i)=xs[i];
        mesh.Cell0DsCoordinates(1,i)=ys[i];
        mesh.Cell0DsCoordinates(2,i)=0.0;
    }
    return true;
}

bool ImportCell1Ds(const std::string& path, PolygonalMesh& mesh)
{
    std::ifstream f(path + "/Cell1Ds.csv");
    if (!f) return false;
    std::string line;
    std::getline(f, line);

    mesh.Cell1DIdToIndex.clear();
    std::vector<unsigned int> ids, marks, oidx, eidx;

    while (std::getline(f, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string tmp;
        std::getline(ss, tmp, ';'); unsigned id = std::stoul(tmp);
        std::getline(ss, tmp, ';'); unsigned mk = std::stoul(tmp);
        std::getline(ss, tmp, ';'); unsigned o  = std::stoul(tmp);
        std::getline(ss, tmp, ';'); unsigned e  = std::stoul(tmp);

        auto itO = mesh.Cell0DIdToIndex.find(o);
        auto itE = mesh.Cell0DIdToIndex.find(e);
        if (itO == mesh.Cell0DIdToIndex.end() || itE == mesh.Cell0DIdToIndex.end())
            return false;

        mesh.Cell1DIdToIndex[id] = ids.size();
        ids.push_back(id);
        marks.push_back(mk);
        oidx.push_back(itO->second);
        eidx.push_back(itE->second);
    }

    mesh.NumCell1Ds    = ids.size();
    mesh.Cell1DsId     = std::move(ids);
    mesh.Cell1DsMarker = std::move(marks);
    mesh.Cell1DsOrigin = std::move(oidx);
    mesh.Cell1DsEnd    = std::move(eidx);
    return true;
}

bool ImportCell2Ds(const std::string& path, PolygonalMesh& mesh)
{
    std::ifstream f(path + "/Cell2Ds.csv");
    if (!f) return false;
    std::string line;
    std::getline(f, line);

    mesh.Cell2DIdToIndex.clear();
    std::vector<unsigned int> ids, marks;
    std::vector<std::vector<unsigned int>> verts, eds;

    while (std::getline(f, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string tmp;
        std::getline(ss, tmp, ';'); unsigned id = std::stoul(tmp);
        std::getline(ss, tmp, ';'); unsigned mk = std::stoul(tmp);
        std::getline(ss, tmp, ';'); unsigned nv = std::stoul(tmp);
        std::vector<unsigned int> v(nv);
        for (unsigned i = 0; i < nv; ++i) { std::getline(ss, tmp, ';'); v[i] = mesh.Cell0DIdToIndex.at(std::stoul(tmp)); }
        std::getline(ss, tmp, ';'); unsigned ne = std::stoul(tmp);
        std::vector<unsigned int> e(ne);
        for (unsigned i = 0; i < ne; ++i) { std::getline(ss, tmp, ';'); e[i] = mesh.Cell1DIdToIndex.at(std::stoul(tmp)); }

        mesh.Cell2DIdToIndex[id] = ids.size();
        ids.push_back(id);
        marks.push_back(mk);
        verts.push_back(std::move(v));
        eds.push_back(std::move(e));
    }

    mesh.NumCell2Ds      = ids.size();
    mesh.Cell2DsId       = std::move(ids);
    mesh.Cell2DsMarker   = std::move(marks);
    mesh.Cell2DsVertices = std::move(verts);
    mesh.Cell2DsEdges    = std::move(eds);
    return true;
}

bool ImportMesh(const std::string& basePath, PolygonalMesh& mesh)
{
    return ImportCell0Ds(basePath, mesh)
        && ImportCell1Ds(basePath, mesh)
        && ImportCell2Ds(basePath, mesh);
}

bool ValidateEdges(const PolygonalMesh& mesh)
{
    for (size_t i = 0; i < mesh.NumCell1Ds; ++i) {
        double x0 = mesh.Cell0DsCoordinates(0, mesh.Cell1DsOrigin[i]);
        double y0 = mesh.Cell0DsCoordinates(1, mesh.Cell1DsOrigin[i]);
        double x1 = mesh.Cell0DsCoordinates(0, mesh.Cell1DsEnd[i]);
        double y1 = mesh.Cell0DsCoordinates(1, mesh.Cell1DsEnd[i]);
        if (std::hypot(x1 - x0, y1 - y0) == 0.0) return false;
    }
    return true;
}

bool ValidatePolygons(const PolygonalMesh& mesh)
{
    for (size_t c = 0; c < mesh.NumCell2Ds; ++c) {
        const auto& V = mesh.Cell2DsVertices[c];
        double area = 0.0;
        for (size_t i = 0, n = V.size(); i < n; ++i) {
            size_t j = (i + 1) % n;
            double xi = mesh.Cell0DsCoordinates(0, V[i]);
            double yi = mesh.Cell0DsCoordinates(1, V[i]);
            double xj = mesh.Cell0DsCoordinates(0, V[j]);
            double yj = mesh.Cell0DsCoordinates(1, V[j]);
            area += xi * yj - xj * yi;
        }
        if (std::abs(area) < 1e-16) return false;
    }
    return true;
}

} // namespace PolygonalLibrary
