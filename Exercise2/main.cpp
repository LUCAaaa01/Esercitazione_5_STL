#include <iostream>
#include <vector>
#include "PolygonalMesh.hpp"
#include "Utils.hpp"
#include "ExportParaview/UCDUtilities.hpp"

using namespace PolygonalLibrary;

bool ValidateMarkers(const PolygonalMesh& mesh)
{
    for (auto mk : mesh.Cell0DsMarker)
        if (int(mk) < 0) return false;
    for (auto mk : mesh.Cell1DsMarker)
        if (int(mk) < 0) return false;
    return true;
}

bool ValidateAll(const PolygonalMesh& mesh)
{
    return ValidateMarkers(mesh)
        && ValidateEdges(mesh)
        && ValidatePolygons(mesh);
}

int main()
{
    PolygonalMesh mesh;
    std::string folder = ".";

    if (!ImportMesh(folder, mesh)) {
        std::cerr << "Import mesh fallito\n";
        return 1;
    }
    std::cout << "Mesh importata con successo!\n";

    if (!ValidateAll(mesh)) {
        std::cerr << "Mesh non valida: marker, spigoli o poligoni errati.\n";
        return 1;
    }
    std::cout << "La mesh è validata con successo!\n";

    Gedim::UCDUtilities util;

    size_t num_points = mesh.NumCell0Ds;
    Eigen::MatrixXd coords(3, num_points);
    Eigen::VectorXi dummy_marker_0D(num_points); //export points richiede un array di interi marker tanto lungo quanti sono i punti
    for (size_t i = 0; i < num_points; ++i) {
        coords(0, i) = mesh.Cell0DsCoordinates(0, i);
        coords(1, i) = mesh.Cell0DsCoordinates(1, i);
        coords(2, i) = mesh.Cell0DsCoordinates(2, i);
        dummy_marker_0D(i) = mesh.Cell0DsMarker[i]; 
    }

    size_t num_edges = mesh.NumCell1Ds;
    Eigen::MatrixXi conn(2, num_edges); //matrice di connettività dei lati in cui ogni colonna è un lato con indice vertice di arrivo e di fine
    Eigen::VectorXi mat1D(num_edges);
    for (size_t i = 0; i < num_edges; ++i) {
        conn(0, i) = mesh.Cell1DsOrigin[i];
        conn(1, i) = mesh.Cell1DsEnd[i];
        mat1D(i)    = mesh.Cell1DsMarker[i];
    }

    std::vector<Gedim::UCDProperty<double>> emptyProps; //vettore vuoto per soddisfare gli export
    util.ExportPoints  ("Cell0Ds.inp", coords, emptyProps, dummy_marker_0D);
    util.ExportSegments("Cell1Ds.inp", coords, conn, emptyProps, emptyProps, mat1D);

    std::cout << "Esportazione completata!\n";
    return 0;
}
