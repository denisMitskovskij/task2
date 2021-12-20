#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include <cassert>
#include <iostream>

uint64_t parseToInd(std::array<uint64_t, 3> const& cs, std::array<uint64_t, 3> const& dms){

    std::array<uint64_t, 3> dimList;
    dimList[2] = 1;
    uint64_t outVal = 0;
    for (std::size_t x = 2; x >= 1; --x){
        dimList[x - 1] = dimList[x] * dms[x];
    }

    for (std::size_t x = 0; x < 3; ++x){
        outVal += cs[x] * dimList[x];
    }
    return outVal;
}

uint64_t getAmountOfNodes(std::array<uint64_t, 3> const& dms){

    uint64_t outVal = 1;
    for (std::size_t x = 0; x < 3; ++x)    {
        outVal *= dms[x];
    }
    return outVal;
}

void calcGraph(std::vector<std::vector<uint64_t>>& sides,
    std::array<uint64_t, 3> const& dms, std::array<uint64_t, 3>& cs, std::size_t actualtCoordInd){

    if (actualtCoordInd == 3){
        uint64_t ind = parseToInd(cs, dms);
        uint64_t temp = 0;
        
        for (std::size_t x = 0; x < 3; ++x){
            temp = cs[x] + 1;
            if (temp < dms[x]){
                cs[x] = temp;
                uint64_t fInd = parseToInd(cs, dms);
                sides[ind].push_back(fInd);
                sides[fInd].push_back(ind);
                cs[x] -= 1;
            }
        }
        return;
    } else {
        for (uint64_t actualCoord = 0; actualCoord < dms[actualtCoordInd]; ++actualCoord){
            cs[actualtCoordInd] = actualCoord;
            calcGraph(sides, dms, cs, actualtCoordInd + 1);
        }
    }
}

std::vector<std::vector<uint64_t>> getGraph(std::array<uint64_t, 3> const& dms){

    uint64_t amountOfNodes = 1;
    for (std::size_t x = 0; x < 3; ++x){
        amountOfNodes *= dms[x];
    }
    std::vector<std::vector<uint64_t>> sides(amountOfNodes);
    std::array<uint64_t, 3> cs;
    calcGraph(sides, dms, cs, 0);
    return sides;
}