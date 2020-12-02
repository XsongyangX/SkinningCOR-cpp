#include "serialize.h"

#include <fstream>

using namespace std;

void SerializeVertices(const Eigen::MatrixXf & vertices, const std::string & path)
{
    ofstream file;
    file.open(path);
    file << vertices << endl;
    file.close();
}

void SerializeTriangles(const Eigen::MatrixXi & triangles, const std::string & path)
{
    ofstream file;
    file.open(path);
    file << triangles << endl;
    file.close();
}

// Matrix to triplets
std::vector<Eigen::Triplet<float>> to_triplets(const Eigen::SparseMatrix<float> & M){
    std::vector<Eigen::Triplet<float>> v;
    for(int i = 0; i < M.outerSize(); i++)
        for(typename Eigen::SparseMatrix<float>::InnerIterator it(M,i); it; ++it)
            v.emplace_back(it.row(),it.col(),it.value());
    return v;
}

void SerializeWeights(const Eigen::SparseMatrix<float> & weights, const std::string & path)
{
    auto triplets = to_triplets(weights);

    ofstream file;
    file.open(path);
    for (const auto & triplet: triplets)
    {
        file << triplet.row() << " " << triplet.col() << " " << triplet.value() << endl;
    }
    file.close();
}