#include "optimize.h"
#include "UpcLibrary.h"

using namespace std;

/**
 * Optimize this loop nest.
 */
void optimize(SgForStatement* nest) {
    LoopTreeDepGraph* depgraph = UpcLibrary::ComputeDependenceGraph(nest);

    // TODO fancy things with the dep graph
}
