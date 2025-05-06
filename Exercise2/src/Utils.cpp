#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include "PolygonalMesh.hpp"

using namespace std;

namespace PolygonalLibrary
{
    bool ImportCell0Ds(const string& inputFilePath, PolygonalMesh& mesh)
    {
        ifstream file(inputFilePath);
        if (!file) {
            cerr << "Errore: impossibile aprire " << inputFilePath << endl;
            return false;
        }

        string line;
        if (!getline(file, line)) {
            cerr << "Errore: file vuoto o formato errato." << endl;
            return false;
        }

        vector<unsigned int> id, marker;
        vector<double> x, y;

        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string tmp;

            getline(ss, tmp, ';');
            unsigned int id_tmp = stoi(tmp);
            getline(ss, tmp, ';');
            unsigned int marker_tmp = stoi(tmp);
            getline(ss, tmp, ';');
            double x_tmp = stod(tmp);
            getline(ss, tmp, ';');
            double y_tmp = stod(tmp);

            
            mesh.Cell0DIdToIndex[id_tmp] = id.size();

            id.push_back(id_tmp);
            marker.push_back(marker_tmp);
            x.push_back(x_tmp);
            y.push_back(y_tmp);
        }

        mesh.NumCell0Ds      = id.size();
        mesh.Cell0DsId       = move(id);
        mesh.Cell0DsMarker   = move(marker);

        mesh.Cell0DsCoordinates.resize(3, mesh.NumCell0Ds);
        for (size_t i = 0; i < mesh.NumCell0Ds; ++i) {
            mesh.Cell0DsCoordinates(0, i) = x[i];
            mesh.Cell0DsCoordinates(1, i) = y[i];
            mesh.Cell0DsCoordinates(2, i) = 0.0;
        }
        return true;
    }

    bool ImportCell1Ds(const string& inputFilePath, PolygonalMesh& mesh)
    {
        ifstream file(inputFilePath);
        if (!file) {
            cerr << "Errore: impossibile aprire " << inputFilePath << endl;
            return false;
        }

        string line;
        if (!getline(file, line)) {
            cerr << "Errore: file vuoto o formato errato." << endl;
            return false;
        }

        vector<unsigned int> id, marker, originIdx, endIdx;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string tmp;

            getline(ss, tmp, ';'); unsigned int id_tmp     = stoi(tmp);
            getline(ss, tmp, ';'); unsigned int marker_tmp = stoi(tmp);
            getline(ss, tmp, ';'); unsigned int oid        = stoi(tmp);
            getline(ss, tmp, ';'); unsigned int eid        = stoi(tmp);

            id.push_back(id_tmp);
            marker.push_back(marker_tmp);
            originIdx.push_back(mesh.Cell0DIdToIndex[oid]);
            endIdx   .push_back(mesh.Cell0DIdToIndex[eid]);
        }

        mesh.NumCell1Ds     = id.size();
        mesh.Cell1DsId      = move(id);
        mesh.Cell1DsMarker  = move(marker);
        mesh.Cell1DsOrigin  = move(originIdx);
        mesh.Cell1DsEnd     = move(endIdx);
        return true;
    }

    bool ImportCell2Ds(const string& inputFilePath, PolygonalMesh& mesh)
    {
        ifstream file(inputFilePath);
        if (!file) {
            cerr << "Errore: impossibile aprire " << inputFilePath << endl;
            return false;
        }

        string line;
        if (!getline(file, line)) {
            cerr << "Errore: file vuoto o formato errato." << endl;
            return false;
        }

        vector<unsigned int> id, marker;
        vector<vector<unsigned int>> vertsList, edgesList;

        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string tmp;

            getline(ss, tmp, ';'); unsigned int id_tmp     = stoi(tmp);
            getline(ss, tmp, ';'); unsigned int marker_tmp = stoi(tmp);
            // vertici
            getline(ss, tmp, ';'); unsigned int nvert = stoi(tmp);
            vector<unsigned int> verts;
            verts.reserve(nvert);
            for (unsigned int i = 0; i < nvert; ++i) {
                getline(ss, tmp, ';');
                verts.push_back(mesh.Cell0DIdToIndex[stoi(tmp)]);
            }
            // edges
            getline(ss, tmp, ';'); unsigned int nedge = stoi(tmp);
            vector<unsigned int> eds;
            eds.reserve(nedge);
            for (unsigned int i = 0; i < nedge; ++i) {
                getline(ss, tmp, ';');
                eds.push_back(mesh.Cell0DIdToIndex[stoi(tmp)]);
            }

            id        .push_back(id_tmp);
            marker    .push_back(marker_tmp);
            vertsList .push_back(move(verts));
            edgesList .push_back(move(eds));
        }

        mesh.NumCell2Ds       = id.size();
        mesh.Cell2DsId        = move(id);
        mesh.Cell2DsMarker    = move(marker);
        mesh.Cell2DsVertices  = move(vertsList);
        mesh.Cell2DsEdges     = move(edgesList);
        return true;
    }
}
