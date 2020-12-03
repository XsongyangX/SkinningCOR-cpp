#include "serialize.h"

#include <fstream>
#include <algorithm>
#include <iterator>
#include <regex>

using namespace std;

regex whitespace("\\s+");

void SerializeVertices(const Eigen::MatrixXf &vertices, const std::string &path)
{
    ofstream file;
    file.open(path);
    file << vertices << endl;
    file.close();
}

void SerializeTriangles(const Eigen::MatrixXi &triangles, const std::string &path)
{
    ofstream file;
    file.open(path);
    file << triangles << endl;
    file.close();
}

// Matrix to triplets
std::vector<Eigen::Triplet<float>> to_triplets(const Eigen::SparseMatrix<float> &M)
{
    std::vector<Eigen::Triplet<float>> v;
    for (int i = 0; i < M.outerSize(); i++)
        for (typename Eigen::SparseMatrix<float>::InnerIterator it(M, i); it; ++it)
            v.emplace_back(it.row(), it.col(), it.value());
    return v;
}

void SerializeWeights(const Eigen::SparseMatrix<float> &weights, const std::string &path)
{
    auto triplets = to_triplets(weights);

    ofstream file;
    file.open(path);
    for (const auto &triplet : triplets)
    {
        file << triplet.row() << " " << triplet.col() << " " << triplet.value() << endl;
    }
    file.close();

    // serialize size of the sparse matrix
    ofstream metadata;
    metadata.open(path + std::string(".size"));
    metadata << weights.rows() << " " << weights.cols() << endl;
    metadata.close();
}

// Read one line of data separated by spaces
std::vector<std::string> tokenize(const std::string &line, int expectedTokenCount)
{
    // Tokenization (non-matched fragments)
    // Note that regex is matched only two times; when the third value is obtained
    // the iterator is a suffix iterator.
    
    sregex_token_iterator iter(line.begin(), line.end(), whitespace, -1);
    sregex_token_iterator end;

    vector<string> tokens;
    for (; iter != end; ++iter)
    {
        string token = *iter;
        if (token.length() != 0)
            tokens.push_back(token);
    }

    // some elements per line
    assert(tokens.size() == expectedTokenCount);

    return tokens;
}

// Read triplets
std::vector<Eigen::Triplet<float>> read_triplets(const std::string &path)
{
    vector<Eigen::Triplet<float>> triplets;

    ifstream file;
    file.open(path);

    assert(file.good());

    string line;
    while (getline(file, line))
    {
        auto tokens = tokenize(line, 3);
        triplets.push_back(Eigen::Triplet<float>(stoi(tokens[0]), stoi(tokens[1]), stof(tokens[2])));
    }

    file.close();

    return triplets;
}

// assume the path does not have an extension
Eigen::SparseMatrix<float> ReadWeights(const std::string &path, int rows, int cols)
{
    auto triplets = read_triplets(path + string(".weights"));

    Eigen::SparseMatrix<float> weights(rows, cols);

    weights.setFromTriplets(triplets.begin(), triplets.end());

    return weights;
}

Eigen::MatrixXf ReadVertices(const std::string &path)
{
    ifstream file;
    file.open(path + string(".vertices"));

    assert(file.good());

    vector<vector<string>> lines;

    string line;
    while (getline(file, line))
    {
        auto tokens = tokenize(line, 3);

        lines.push_back(tokens);
    }

    file.close();

    Eigen::MatrixXf vertices(lines.size(), 3);

    for (int i = 0; i < (int) lines.size(); i++)
    {
        const auto &line = lines[i];
        vertices.row(i) << stof(line[0]), stof(line[1]), stof(line[2]);
    }

    return vertices;
}

Eigen::MatrixXi ReadTriangles(const std::string &path)
{
    ifstream file;
    file.open(path + string(".triangles"));

    assert(file.good());

    vector<vector<string>> lines;

    string line;
    while (getline(file, line))
    {
        auto tokens = tokenize(line, 3);

        lines.push_back(tokens);
    }

    file.close();

    Eigen::MatrixXi triangles(lines.size(), 3);

    for (int i = 0; i < (int) lines.size(); i++)
    {
        const auto &line = lines[i];
        triangles.row(i) << stoi(line[0]), stoi(line[1]), stoi(line[2]);
    }

    return triangles;
}

Mesh ReadMesh(const string & path)
{
    auto vertices = ReadVertices(path);
    auto triangles = ReadTriangles(path);

    int rows = 0, cols = 0;

    string size = path + string(".weights.size");
    fstream sizeFile(size);
    string line;
    while (getline(sizeFile, line))
    {
        auto tokens = tokenize(line, 2);

        rows = stoi(tokens[0]);
        cols = stoi(tokens[1]);
    }

    auto weights = ReadWeights(path, rows, cols);

    return Mesh(vertices, triangles, weights);
}