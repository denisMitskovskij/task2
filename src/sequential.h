#pragma once

#include "calc_utils.h"
#include <vector>
#include <functional>
#include <cstdint>
#include <queue>
#include <atomic>
#include "parray.hpp"
#include "datapar.hpp"

inline std::vector<int64_t> calsSeqBfs(uint64_t amountNode, uint64_t beginNode,std::vector<std::vector<uint64_t>> const& graphSides){

        std::queue<uint64_t> nodesQueue;
        nodesQueue.push(beginNode);

        std::vector<int64_t> vals(amountNode, -1);
        vals[beginNode] = 0;

        while (!nodesQueue.empty()){
            uint64_t startingFrom = nodesQueue.front();
            nodesQueue.pop();
            for (uint64_t i = 0; i < graphSides[startingFrom].size(); ++i){
                if (vals[graphSides[startingFrom][i]] == -1){
                    nodesQueue.push(graphSides[startingFrom][i]);
                    vals[graphSides[startingFrom][i]] = vals[startingFrom] + 1;

                }
            }
        }

        return vals;
}