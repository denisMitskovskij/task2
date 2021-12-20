#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include <cassert>
#include <iostream>

template <std::size_t DMS>
uint64_t parseToInd(std::array<uint64_t, DMS> const& cs, std::array<uint64_t, DMS> const& dms){

    std::array<uint64_t, DMS> dimList;
    dimList[DMS - 1] = 1;
    uint64_t outVal = 0;
    for (std::size_t x = DMS - 1; x >= 1; --x){
        dimList[x - 1] = dimList[x] * dms[x];
    }

    for (std::size_t x = 0; x < DMS; ++x){
        outVal += cs[x] * dimList[x];
    }
    return outVal;
}

template <std::size_t DMS>
uint64_t getAmountOfNodes(std::array<uint64_t, DMS> const& dms){

    uint64_t outVal = 1;
    for (std::size_t x = 0; x < DMS; ++x)    {
        outVal *= dms[x];
    }
    return outVal;
}

template <std::size_t DMS>
void calcGraph(std::vector<std::vector<uint64_t>>& sides,
    std::array<uint64_t, DMS> const& dms, std::array<uint64_t, DMS>& cs, std::size_t actualtCoordInd){

    if (actualtCoordInd == DMS){
        uint64_t ind = parseToInd(cs, dms);
        uint64_t temp = 0;
        
        for (std::size_t x = 0; x < DMS; ++x){
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

template <std::size_t DMS>
std::vector<std::vector<uint64_t>> getGraph(std::array<uint64_t, DMS> const& dms){

    uint64_t amountOfNodes = 1;
    for (std::size_t x = 0; x < DMS; ++x){
        amountOfNodes *= dms[x];
    }
    std::vector<std::vector<uint64_t>> sides(amountOfNodes);
    std::array<uint64_t, DMS> cs;
    calcGraph(sides, dms, cs, 0);
    return sides;
}