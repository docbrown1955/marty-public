#include <csl>

///////////////////////////////////////////////////
/*************************************************/
// Indicial expressions                          //
/*************************************************/
///////////////////////////////////////////////////

void printTensors(const csl::Space* space)
{
    csl::IParent delta = GetDelta(space);
    csl::IParent g     = GetMetric(space);
    csl::IParent eps   = GetEpsilon(space);

    csl::Index i = GenerateIndex(space, "i");
    csl::Index j = GenerateIndex(space, "j");
    csl::Index k = GenerateIndex(space, "k");

    std::cout << "Delta   : " << delta({+i, j}) << std::endl;
    std::cout << "Metric  : " << g({j, i}) << std::endl;
    std::cout << "Epsilon : " << eps({k, j, i}) << std::endl;
}

void printProperties(const csl::Space *space)
{
    csl::IParent delta = GetDelta(space);
    csl::IParent g     = GetMetric(space);
    csl::IParent eps   = GetEpsilon(space);

    csl::Index i = GenerateIndex(space, "i");
    csl::Index j = GenerateIndex(space, "j");
    csl::Index k = GenerateIndex(space, "k");
    csl::Index l = GenerateIndex(space, "l");
    csl::Index m = GenerateIndex(space, "m");
    csl::Index n = GenerateIndex(space, "n");

    csl::IParent X = csl::iparent_s("X", {space});
    csl::IParent Y = csl::iparent_s("Y", {space});

    std::cout << g({i, j}) * X(+i) << std::endl;
    std::cout << delta({+i, j}) * X(i) << std::endl;
    std::cout << eps({i, k, j}) * eps({l, n, m}) << std::endl;
    std::cout << g({+i, +m}) * eps({i, j, k}) * eps({l, m, n}) << std::endl;
    std::cout << g({+j, +m}) * g({+i, +l}) * g({+k, +n}) 
               * eps({i, j, k}) * eps({l, m, n}) << std::endl;
}

int main() {
    
    csl::Space space3D("R_3", 3);

    Expr metricTensor = csl::matrix_s(
            {{-1, 0, 0},
             {0, 1, 0},
             {0, 0, 1}}
            );
    csl::Space signedSpace3D("R_1_2", 3, "g", metricTensor);

    csl::Index::printIds = false;

    printTensors(&space3D);
    printTensors(&signedSpace3D);

    printProperties(&space3D);
    printProperties(&signedSpace3D);

    return 0;
}