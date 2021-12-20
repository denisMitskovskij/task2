#pragma once

#include <queue>
#include <atomic>
#include <functional>
#include <vector>
#include <cstdint>
#include "datapar.hpp"
#include "parray.hpp"

inline void calsSide(pasl::pctl::parray<int64_t>& vals, pasl::pctl::parray<int64_t>& nBorder, std::vector<uint64_t> const& sides,
    uint64_t indSt, uint64_t startingFrom, uint64_t indE){

        int64_t exp = -1;
        bool outcome = __atomic_compare_exchange_n(
            &vals[sides[indE]], &exp, (__atomic_load_n(&vals[startingFrom], __ATOMIC_SEQ_CST) + 1),
            false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST
        );
        if (outcome){
            nBorder[indSt + indE] = sides[indE];
        }
}

inline void calcNodePar(std::vector<std::vector<uint64_t>> const& graphSides, pasl::pctl::parray<int64_t> const& actualBorder, pasl::pctl::parray<int64_t>& vals,
    pasl::pctl::parray<int64_t>& nBorder, pasl::pctl::parray<uint64_t> const& szPr,
    uint64_t idxN){

        uint64_t startingFrom = actualBorder[idxN];
        uint64_t indSt = szPr[idxN];

        std::vector<uint64_t> const& sides = graphSides[startingFrom];
        pasl::pctl::parallel_for(
            static_cast<uint64_t>(0), static_cast<uint64_t>(sides.size()),
            [&vals, &nBorder, &sides, indSt, startingFrom](uint64_t indE)
            {
                calsSide(
                    vals, nBorder, sides,
                    indSt, startingFrom, indE
                );
            }
        );
}