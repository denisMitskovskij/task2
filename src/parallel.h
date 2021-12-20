#pragma once

#include <functional>
#include <queue>
#include <atomic>
#include <vector>
#include <cstdint>
#include "parray.hpp"
#include "datapar.hpp"


inline pasl::pctl::parray<int64_t> calcBfsPar(uint64_t amountNode, uint64_t beginNode, std::vector<std::vector<uint64_t>> const& graphSides){

        pasl::pctl::parray<int64_t> vals(amountNode, static_cast<int64_t>(-1));

        __atomic_store_n(&vals[beginNode], 0, __ATOMIC_SEQ_CST);

        pasl::pctl::parray<int64_t> actualBorder = {static_cast<int64_t>(beginNode)};

        while (true) {
            pasl::pctl::parray<uint64_t> sizes(
                actualBorder.size(),
                [&actualBorder, &graphSides, &vals](uint64_t idxN){
                    return static_cast<uint64_t>(graphSides[static_cast<uint64_t>(actualBorder[idxN])].size());
                }
            );

            pasl::pctl::parray<uint64_t> szPr = pasl::pctl::scan(
                sizes.begin(), sizes.end(), static_cast<uint64_t>(0),
                [](uint64_t x, uint64_t y){
                    uint64_t sum = x + y;
                    return sum;
                },
                pasl::pctl::scan_type::forward_exclusive_scan
            );

            uint64_t szNewBorder = szPr[szPr.size() - 1] + sizes[sizes.size() - 1];
            pasl::pctl::parray<int64_t> nBorder(szNewBorder, static_cast<int64_t>(-1));
            pasl::pctl::range::parallel_for(
                static_cast<uint64_t>(0), static_cast<uint64_t>(actualBorder.size()),
                [&szPr, szNewBorder](uint64_t lf, uint64_t rgh){
                    if (rgh < szPr.size()){
                        return szPr[rgh] - szPr[lf];
                    } else {
                        return szNewBorder - szPr[lf];
                    }
                },
                [&graphSides, &actualBorder, &vals, &nBorder, &szPr](uint64_t idxN){
                    calcNodePar(graphSides, actualBorder, vals, nBorder, szPr, idxN);
                },
                [&graphSides, &actualBorder, &vals, &nBorder, &szPr](uint64_t lf, uint64_t rgh){
                    for (uint64_t i = lf; i < rgh; ++i){
                        calcNodePar(graphSides, actualBorder, vals, nBorder, szPr, i);
                    }
                }
            );

            actualBorder = pasl::pctl::filter(
                nBorder.begin(), nBorder.end(),
                [](int64_t actual){
                    return actual != -1;
                }
            );
            if (actualBorder.size() == 0){
                return vals;
            }
        }
}
