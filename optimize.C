#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include "optimize.h"
#include "UpcLibrary.h"
#include "AstDOTGeneration.h"

using namespace std;
using namespace SageInterface;

/**
 * True if REF is a top-level array reference.
 */
bool isOuterReference(SgPntrArrRefExp* ref) {
    ROSE_ASSERT(ref);
    return !isSgPntrArrRefExp( ref->get_parent() );
}

/**
 * Localize REF. Must be localizable.
 */
void localize(SgExpression* subscript, SgForStatement* target) {
    /* TODO create a new local array __upctr_local_NAME */

    /* TODO replace subscript with a reference to the local array */

    /* TODO if subscript is a read, insert bupc_memget_strided call
     * before target via SageInterface::prependStatement */

    /* TODO if subscript is a write, insert bupc_memput_strided call
     * after target via SageInterface::appendStatement */
}

/**
 * For the given reference, test if it can be localized. If it can,
 * localize it.
 *
 * Localization is legal at given level if the reference has no
 * loop-carried dependences at or inside the level in question.
 *
 * Localization is most profitable at the level just inside the
 * innermost upc_forall loop.
 *
 * 1-dimensional localization must occur inside the loop iterating
 * over the second dimension.
 */
void
find_and_localize(LoopTreeDepGraph* deps,
                  SgPntrArrRefExp*  reference,
                  vector<SgExpression*>* subscript_exps) {

    /* convert subscripts to a name. fail on more complicated subscripts */
    vector<SgVariableSymbol*> subscripts;
    foreach (SgExpression* subscript_exp, *subscript_exps) {
        SgVarRefExp* varref = isSgVarRefExp(subscript_exp);
        if (varref == NULL) {
            cerr << "Warning: skipping complicated subscript: "
                << subscript_exp->unparseToString() << " class="
                << subscript_exp->class_name() << endl;
            continue;
        }
        subscripts.push_back( varref->get_symbol() );
    }

    /* Walk outward until we can't keep the local array 1d */
    SgForStatement* target = getEnclosingNode<SgForStatement>(reference);
}

/**
 * Optimize this loop nest.
 */
void optimize(SgForStatement* nest) {

    /* build the dependence graph */
    LoopTreeDepGraph* depgraph = UpcLibrary::ComputeDependenceGraph(nest);

    /* debugging: print dep graph and ast */
    if (UpcLibrary::debug) {
        UpcLibrary::printDepGraphAsDot(depgraph, (char*) "graph.dot");
        AstDOTGeneration dot; dot.generate(nest, "tree");
    }

    /* collect all array references, including non-top-level ones */
    vector<SgPntrArrRefExp*> all_array_refs =
        SageInterface::querySubTree<SgPntrArrRefExp>(nest);

    /* filter out non-top-level array references */
    vector<SgPntrArrRefExp*> array_refs;
    foreach (SgPntrArrRefExp* array_ref, all_array_refs)
        if (isOuterReference(array_ref))
            array_refs.push_back(array_ref);

    /* test and localize each subscript */
    foreach (SgPntrArrRefExp* array_ref, array_refs) {

        /* extract all subscript expressions */
        SgExpression* name_exp;
        vector<SgExpression*>* subscript_exps = new vector<SgExpression*>();;
        assert(SageInterface::isArrayReference(array_ref, &name_exp, &subscript_exps));

        /* find the outermost for-stmt around which this subscript
         * can be localized, and localize it */
        find_and_localize(depgraph, array_ref, subscript_exps);

        /* cleanup */
        delete subscript_exps;
    }
}
