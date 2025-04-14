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


bool ImportMesh(PolygonalMesh& mesh)
{  
    return false;
}


// Import dei Cell0D

bool ImportCell0Ds(const string& inputFilePath, PolygonalMesh& mesh)
{
    ifstream file(inputFilePath);
    if (!file) {
        cout << "Errore: Impossibile aprire il file " << inputFilePath << endl;
        return false;
    } 
    string line;
    if (!getline(file, line)) {
        cout << "Errore: file vuoto o formato errato." << endl;
        return false;
    }
    
    vector<unsigned int> id;
    vector<unsigned int> marker;
    vector<double> x;
    vector<double> y;

    while(getline(file, line)) {
        if(line.empty())
            continue;
        stringstream ss(line);
        string tmpp;
        
        // Legge l'ID
        getline(ss, tmpp, ';');
        unsigned int id_tmp = static_cast<unsigned int>(std::stoi(tmpp));

        // Legge il Marker
        getline(ss, tmpp, ';');
        unsigned int marker_tmp = static_cast<unsigned int>(std::stoi(tmpp));

        // Legge la coordinata X
        getline(ss, tmpp, ';');
        double x_tmp = stod(tmpp);

        // Legge la coordinata Y
        getline(ss, tmpp, ';');
        double y_tmp = stod(tmpp);

        id.push_back(id_tmp);
        marker.push_back(marker_tmp);
        x.push_back(x_tmp);
        y.push_back(y_tmp); 
    }

    mesh.NumCell0Ds = id.size();
    mesh.Cell0DsId = id;
    mesh.Cell0DsMarker = marker;

    mesh.Cell0DsCoordinates.resize(2, mesh.NumCell0Ds);
    for (size_t i = 0; i < mesh.NumCell0Ds; ++i) {
        mesh.Cell0DsCoordinates(0, i) = x[i];
        mesh.Cell0DsCoordinates(1, i) = y[i];
    }

    return true;
}


bool ImportCell1Ds(const string& inputFilePath, PolygonalMesh& mesh)
{
    ifstream file(inputFilePath);
    if (!file) {
        cout << "Errore: Impossibile aprire il file " << inputFilePath << endl;
        return false;
    } 
    string line;
    if (!getline(file, line)) {
        cout << "Errore: file vuoto o formato errato." << endl;
        return false;
    }
    
    vector<unsigned int> id;
    vector<unsigned int> marker;
    vector<unsigned int> origin;
    vector<unsigned int> end;

    while(getline(file, line)) {
        if(line.empty())
            continue;
        stringstream ss(line);
        string tmpp;
        
        // Legge l'ID
        getline(ss, tmpp, ';');
        unsigned int id_tmp = std::stoi(tmpp);

        // Legge il Marker
        getline(ss, tmpp, ';');
        unsigned int marker_tmp = std::stoi(tmpp);

        // Legge l'indice di origine
        getline(ss, tmpp, ';');
        unsigned int origin_tmp = std::stoi(tmpp);

        // Legge l'indice di fine
        getline(ss, tmpp, ';');
        unsigned int end_tmp = std::stoi(tmpp);

        // Controlla che gli indici di origine e fine siano validi
        if (origin_tmp >= mesh.NumCell0Ds || end_tmp >= mesh.NumCell0Ds) {
            cout << "Errore: riferimento a 0D inesistente per l'edge con ID = " << id_tmp << endl;
            return false;
        }
        
        // Calcola la distanza tra i due punti
        double x_origin = mesh.Cell0DsCoordinates(0, origin_tmp);
        double y_origin = mesh.Cell0DsCoordinates(1, origin_tmp);
        double x_end    = mesh.Cell0DsCoordinates(0, end_tmp);
        double y_end    = mesh.Cell0DsCoordinates(1, end_tmp);
        
        double dx = x_end - x_origin;
        double dy = y_end - y_origin;
        double distance_squared = dx * dx + dy * dy;
        if (distance_squared == 0.0) {
            cout << "Errore: lunghezza nulla per l'edge con ID = " << id_tmp << endl;
            return false;
        }

        id.push_back(id_tmp);
        marker.push_back(marker_tmp);
        origin.push_back(origin_tmp);
        end.push_back(end_tmp);
    }

    mesh.NumCell1Ds = id.size();
    mesh.Cell1DsId = id;
    mesh.Cell1DsMarker = marker;
    mesh.Cell1DsOrigin = origin;
    mesh.Cell1DsEnd = end;

    // La matrice Cell1DsCoordinates memorizza qui gli indici degli estremi
    mesh.Cell1DsCoordinates.resize(2, mesh.NumCell1Ds);
    for (size_t i = 0; i < mesh.NumCell1Ds; ++i) {
        mesh.Cell1DsCoordinates(0, i) = origin[i];
        mesh.Cell1DsCoordinates(1, i) = end[i];
    }

    return true;
}


bool ImportCell2Ds(const string& inputFilePath, PolygonalMesh& mesh)
{
    ifstream file(inputFilePath);
    if (!file) {
        cout << "Errore: Impossibile aprire il file " << inputFilePath << endl;
        return false;
    }

    string line;
    if (!getline(file, line)) {
        cout << "Errore: file vuoto o formato errato." << endl;
        return false;
    }

    vector<unsigned int> id;
    vector<unsigned int> marker;
    vector<vector<unsigned int>> verticesList;
    vector<vector<unsigned int>> edgesList;

    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string tmpp;

        // Legge l'ID
        getline(ss, tmpp, ';');
        unsigned int id_tmp = std::stoi(tmpp);

        // Legge il Marker
        getline(ss, tmpp, ';');
        unsigned int marker_tmp = std::stoi(tmpp);

        // Legge il numero dei vertici
        getline(ss, tmpp, ';');
        unsigned int numVertices = std::stoi(tmpp);

        vector<unsigned int> vertici;
        for (unsigned int i = 0; i < numVertices; ++i) {
            getline(ss, tmpp, ';');
            vertici.push_back(std::stoi(tmpp));
        }

        // Legge il numero dei lati
        getline(ss, tmpp, ';');
        unsigned int numEdges = std::stoi(tmpp);

        vector<unsigned int> lati;
        for (unsigned int i = 0; i < numEdges; ++i) {
            getline(ss, tmpp, ';');
            lati.push_back(std::stoi(tmpp));
        }

        // Calcola l'area del poligono
        double area = 0.0;
        for (size_t i = 0; i < numVertices; ++i) {
            unsigned int v1 = vertici[i];
            unsigned int v2 = vertici[(i + 1) % numVertices];

            double x1 = mesh.Cell0DsCoordinates(0, v1);
            double y1 = mesh.Cell0DsCoordinates(1, v1);
            double x2 = mesh.Cell0DsCoordinates(0, v2);
            double y2 = mesh.Cell0DsCoordinates(1, v2);

            area += (x1 * y2 - x2 * y1);
        }
        area = 0.5 * std::abs(area);

        if (area <= 0.0) {
            cout << "Errore: area nulla per il poligono con ID = " << id_tmp << endl;
            return false;
        }

        id.push_back(id_tmp);
        marker.push_back(marker_tmp);
        verticesList.push_back(vertici);
        edgesList.push_back(lati);
    }

    mesh.NumCell2Ds = id.size();
    mesh.Cell2DsId = id;
    mesh.Cell2DsMarker = marker;
    mesh.Cell2DsVertices = verticesList;
    mesh.Cell2DsEdges = edgesList;

    return true;
}

} 
