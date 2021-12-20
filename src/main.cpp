#define NDEBUG

#include "sequential.h"
#include "parallel.h"
#include "graph_utils.h"
#include <chrono>
#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include <array>
#include "parray.hpp"

template <std::size_t DMS>
void getAllPoints(std::array<uint64_t, DMS> const& dims, std::array<uint64_t, DMS>& crds, std::size_t actInd, std::vector<std::array<uint64_t, DMS>>& result){
    if (actInd == DMS){
        result.push_back(crds);
        return;
    } else{
        for (uint64_t actCoord = 0; actCoord < dims[actInd]; ++actCoord){
            crds[actInd] = actCoord;
            getAllPoints(dims, crds, actInd + 1, result);
        }
    }
}

template <std::size_t DMS>
std::vector<std::array<uint64_t, DMS>> getAll(std::array<uint64_t, DMS> const& dims){
    std::vector<std::array<uint64_t, DMS>> result;
    std::array<uint64_t, DMS> crds;
    getAllPoints(dims, crds, 0, result);
    return result;
}

template <std::size_t DMS>
uint64_t calcDist(std::array<uint64_t, DMS> const& fst, std::array<uint64_t, DMS> const& snd){
    uint64_t result = 0;
    for (std::size_t y = 0; y < DMS; ++y){
        if (fst[y] > snd[y]){
            result += (fst[y] - snd[y]);
        } else {
            result += (snd[y] - fst[y]);
        }
    }
    return result;
}

template <template <typename, typename ...> typename C, std::size_t DMS>
uint64_t parLaunch(
    uint64_t amountNode, std::vector<std::vector<uint64_t>> const& graphSides, uint32_t rs, std::vector<std::array<uint64_t, DMS>> pts,
    std::function<C<int64_t>(uint64_t, uint64_t,  std::vector<std::vector<uint64_t>> const&)> const& calcBfs)
{
    uint64_t summ = 0;
    std::array<uint64_t, 3> start_point = {0, 0, 0};
    for (uint32_t x = 0; x < rs; ++x)
    {
        std::cout << "Sample " << x << std::endl;
        std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
        pasl::pctl::parray<int64_t> result = calcBfs(amountNode, 0, graphSides);
        std::chrono::steady_clock::time_point finishTime = std::chrono::steady_clock::now();
        summ += std::chrono::duration_cast<std::chrono::milliseconds>(finishTime - startTime).count();
        bool testPass = true;
        for (uint64_t y = 0; y < amountNode; ++y) {
            if (result[y] < 0 || calcDist(start_point, pts[y]) != result[y]){
                testPass = false;
            }
        }
        if (testPass){
            std::cout << "Test success" << std::endl;
        } else {
            std::cout << "Test fail" << std::endl;
        }
    }
    return summ / rs;
}



int main()
{
    std::array<uint64_t, 3> dims = {500, 500, 500};
    uint64_t amountNode = getAmountOfNodes(dims);
    auto graphSides = getGraph(dims);
    std::vector<std::array<uint64_t, 3>> pts = getAll(dims);
    uint32_t rs = 5;

    std::cout << "Processing sequential" << std::endl;
    uint64_t summ = 0;
        for (uint32_t x = 0; x < rs; ++x)
        {
            std::cout << "Sample " << x << std::endl;
            std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
            std::array<uint64_t, 3> start_point = {0, 0, 0};
            std::vector<int64_t> result = calsSeqBfs(amountNode, 0, graphSides);
            std::chrono::steady_clock::time_point finishTime = std::chrono::steady_clock::now();
            summ += std::chrono::duration_cast<std::chrono::milliseconds>(finishTime - startTime).count();
            bool testPass = true;
            for (uint64_t y = 0; y < amountNode; ++y) {
                if (result[y] < 0 || calcDist(start_point, pts[y]) != result[y]){
                    testPass = false;
                }
            }
            if (testPass){
                std::cout << "Test success" << std::endl;
            } else {
                std::cout << "Test fail" << std::endl;
            }
        }

    std::cout << "Average time is " << summ/rs << " ms" << std::endl;
    std::cout << "Processing parallel" << std::endl;

    std::vector<int64_t> result;
    uint64_t cas_res = parLaunch<pasl::pctl::parray, 3>(
            amountNode, graphSides, rs, pts, []
             (uint64_t amountNode, uint64_t start_node,
                std::vector<std::vector<uint64_t>> const& graphSides)
            {
                return calcBfsPar(
                    amountNode, start_node, graphSides
                );
            }
        );
    std::cout << "Average time is " << cas_res << " ms" << std::endl;

    return 0;
}
