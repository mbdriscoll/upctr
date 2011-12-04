#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include "optimize.h"
#include "UpcLibrary.h"
#include "AstDOTGeneration.h"

using namespace std;

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
 * Returns the outermost ForStatement that can be wrapped
 * in localization logic. If the subscript cannot be
 * localized, return NULL.
 *
 * upc_memget_strided(A_local, A, 100);
 * for (...) { ... A_local[..] ... } // (target)
 * upc_memput_strided(A, A_local, 100);
 */
SgForStatement*
find_localization_target(LoopTreeDepGraph* deps,
                  SgPntrArrRefExp*  reference,
                  SgExpression*     subscript) {

    /* TODO improve this function. I believe the target should be the
     * outermost loop that isn't a upc_forall loop and the common
     * 'ancestor' of the subscripts. So, for an A[i][k] reference in
     * an (i, j, k) nest, the target would be the j loop. */

    /* for now, just return the enclosing loop */
    return SageInterface::getEnclosingNode<SgForStatement>(reference);
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

        /* TODO: fail on complicated subscripts */

        /* attempt localization for each subscript */
        foreach (SgExpression* subscript_exp, *subscript_exps) {

            /* find the outermost for-stmt around which this subscript
             * can be localized. */
            SgForStatement* target =
                find_localization_target(depgraph, array_ref, subscript_exp);

            /* if a target exists, localize the subscript */
            if (target != NULL)
                localize(subscript_exp, target);
        }

        /* cleanup */
        delete subscript_exps;
    }
}
