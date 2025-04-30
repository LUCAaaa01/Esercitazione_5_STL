#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>

#include "PolygonalMesh.hpp"
#include "Utils.hpp"
#include "UCDUtilities.hpp"
#include "Eigen/Dense"

using namespace std;
using namespace PolygonalLibrary;

const double EPS = 1e-12;

// Validate that all markers are non-negative
bool ValidateMarkers(const PolygonalMesh& mesh) {
    bool ok = true;
    for (size_t i = 0; i < mesh.Cell0DsMarker.size(); ++i) {
        if ((int)mesh.Cell0DsMarker[i] < 0) {
            cerr << "Errore: marker negativo in Cell0D all'indice " << i << "\n";
            ok = false;
        }
    }
    for (size_t i = 0; i < mesh.Cell1DsMarker.size(); ++i) {
        if ((int)mesh.Cell1DsMarker[i] < 0) {
            cerr << "Errore: marker negativo in Cell1D all'indice " << i << "\n";
            ok = false;
        }
    }
    for (size_t i = 0; i < mesh.Cell2DsMarker.size(); ++i) {
        if ((int)mesh.Cell2DsMarker[i] < 0) {
            cerr << "Errore: marker negativo in Cell2D all'indice " << i << "\n";
            ok = false;
        }
    }
    return ok;
}

// Validate that each edge has non-zero length
bool ValidateEdgeLengths(const PolygonalMesh& mesh) {
    bool ok = true;
    for (size_t k = 0; k < mesh.Cell1DsOrigin.size(); ++k) {
        unsigned int o = mesh.Cell1DsOrigin[k];
        unsigned int e = mesh.Cell1DsEnd[k];
        Eigen::Vector3d A = mesh.Cell0DsCoordinates.col(o);
        Eigen::Vector3d B = mesh.Cell0DsCoordinates.col(e);
        double length = (A - B).norm();
        if (length < EPS) {
            cerr << "Errore: segmento " << k << " ha lunghezza nulla o quasi nulla\n";
            ok = false;
        }
    }
    return ok;
}

// Compute polygon area using 2D shoelace formula
bool ValidatePolygonAreas(const PolygonalMesh& mesh) {
    bool ok = true;
    for (size_t c = 0; c < mesh.Cell2DsVertices.size(); ++c) {
        const auto& verts = mesh.Cell2DsVertices[c];
        size_t n = verts.size();
        if (n < 3) {
            cerr << "Errore: Cell2D " << c << " ha meno di 3 vertici\n";
            ok = false;
            continue;
        }
        double area2 = 0.0;
        for (size_t i = 0; i < n; ++i) {
            size_t j = (i + 1) % n;
            auto Pi = mesh.Cell0DsCoordinates.col(verts[i]);
            auto Pj = mesh.Cell0DsCoordinates.col(verts[j]);
            area2 += Pi.x() * Pj.y() - Pj.x() * Pi.y();
        }
        double area = 0.5 * fabs(area2);
        if (area < EPS) {
            cerr << "Errore: Cell2D " << c << " area nulla o quasi nulla: " << area << "\n";
            ok = false;
        }
    }
    return ok;
}

// MergeFiles: build UCD file reading counts from each .inp
void MergeFiles(const PolygonalMesh& mesh, size_t numPolygons) {
    ifstream in;
    ofstream out("mesh.inp");
    if (!out) {
        cerr << "Errore: non posso creare mesh.inp\n";
        return;
    }

    // Header: #points #polygons #segments
    out << "Merged mesh\n"
        << mesh.NumCell0Ds << " "
        << numPolygons        << " "
        << mesh.NumCell1Ds    << "\n";

    string line;
    int ptsCount, cellCount, dummy;

    // 1) Points from Cell0Ds.inp
    in.open("Cell0Ds.inp");
    getline(in, line); // title
    getline(in, line); // counts
    // grab counts if needed
    {
        stringstream ss(line);
        ss >> ptsCount >> cellCount >> dummy;
    }
    out << in.rdbuf();
    in.close();

    // 2) Polygons from Cell2Ds.inp
    in.open("Cell2Ds.inp");
    getline(in, line); // title
    getline(in, line); // counts: pts polygons
    {
        stringstream ss(line);
        ss >> ptsCount >> cellCount >> dummy;
    }
    // skip ptsCount lines of coords
    for (int i = 0; i < ptsCount; ++i) getline(in, line);
    // copy polygon definitions
    out << in.rdbuf();
    in.close();

    // 3) Segments from Cell1Ds.inp
    in.open("Cell1Ds.inp");
    getline(in, line); // title
    getline(in, line); // counts: pts segments
    {
        stringstream ss(line);
        ss >> ptsCount >> cellCount >> dummy;
    }
    // skip ptsCount lines of coords
    for (int i = 0; i < ptsCount; ++i) getline(in, line);
    // copy segment definitions
    out << in.rdbuf();
    in.close();

    cout << "Creato mesh.inp in formato UCD unificato\n";
}

int main() {
    PolygonalMesh mesh;
    // Import CSV
    if (!ImportCell0Ds("Cell0Ds.csv", mesh)) return 1;
    if (!ImportCell1Ds("Cell1Ds.csv", mesh)) return 1;
    if (!ImportCell2Ds("Cell2Ds.csv", mesh)) return 1;

    // Validazioni mesh
    if (!ValidateMarkers(mesh) || !ValidateEdgeLengths(mesh) || !ValidatePolygonAreas(mesh)) {
        cerr << "Validazione mesh fallita. Controlla gli errori sopra.\n";
        return 1;
    }
    cout << "Mesh validata con successo!\n";

    cout << "Numero di Cell0Ds: " << mesh.NumCell0Ds << "\n"
         << "Numero di Cell1Ds: " << mesh.NumCell1Ds << "\n"
         << "Numero di Cell2Ds: " << mesh.NumCell2Ds << "\n";

    Gedim::UCDUtilities utilities;
    vector<Gedim::UCDProperty<double>> emptyProps;

    // Export Points
    Eigen::VectorXi pointMats = Eigen::VectorXi::Zero(mesh.NumCell0Ds);
    utilities.ExportPoints(
        "Cell0Ds.inp",
        mesh.Cell0DsCoordinates,
        emptyProps,
        pointMats
    );
    cout << "Esportati i punti in Cell0Ds.inp\n";

    // Export Segments
    size_t numSeg = mesh.Cell1DsOrigin.size();
    Eigen::MatrixXi segments(2, numSeg);
    for (size_t i = 0; i < numSeg; ++i) {
        segments(0, i) = mesh.Cell1DsOrigin[i] + 1;
        segments(1, i) = mesh.Cell1DsEnd[i]    + 1;
    }
    Eigen::VectorXi segMats = Eigen::VectorXi::Zero(numSeg);
    utilities.ExportSegments(
        "Cell1Ds.inp",
        mesh.Cell0DsCoordinates,
        segments,
        emptyProps,
        emptyProps,
        segMats
    );
    cout << "Esportati i segmenti in Cell1Ds.inp\n";

    // Export Polygons (fan triangulation if N>3)
    vector<vector<unsigned int>> tris1based;
    for (auto& verts : mesh.Cell2DsVertices) {
        size_t N = verts.size();
        if (N < 3) continue;
        unsigned int v0 = verts[0] + 1;
        for (size_t i = 1; i + 1 < N; ++i) {
            tris1based.push_back({ v0,
                                   verts[i]   + 1,
                                   verts[i+1] + 1 });
        }
    }
    Eigen::VectorXi polyMats = Eigen::VectorXi::Zero(tris1based.size());
    utilities.ExportPolygons(
        "Cell2Ds.inp",
        mesh.Cell0DsCoordinates,
        tris1based,
        emptyProps,
        emptyProps,
        polyMats
    );
    cout << "Esportati i poligoni triangolati in Cell2Ds.inp\n";

    // Merge into single UCD
    MergeFiles(mesh, tris1based.size());

    return 0;
}
