#include <iostream>
#include "PolygonalMesh.hpp"
#include "Utils.hpp"
#include "UCDUtilities.hpp"

using namespace std;
using namespace PolygonalLibrary;

int main()
{
    PolygonalMesh mesh;

    
    if (!ImportCell0Ds("Cell0Ds.csv", mesh)) {
        cerr << "Errore: impossibile importare i Cell0Ds" << endl;
        return 1;
    }

    
    if (!ImportCell1Ds("Cell1Ds.csv", mesh)) {
        cerr << "Errore: impossibile importare i Cell1Ds" << endl;
        return 1;
    }

    
    if (!ImportCell2Ds("Cell2Ds.csv", mesh)) {
        cerr << "Errore: impossibile importare i Cell2Ds" << endl;
        return 1;
    }

    
    cout << "Mesh importata con successo!" << endl;
    cout << "Numero di Cell0Ds: " << mesh.NumCell0Ds << endl;
    cout << "Numero di Cell1Ds: " << mesh.NumCell1Ds << endl;
    cout << "Numero di Cell2Ds: " << mesh.NumCell2Ds << endl;

    
    
    return 0;
}
