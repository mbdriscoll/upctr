#include "builder.h"

SgExpression*
UpctrBuilder::buildLocalArray(
        SgPntrArrRefExp* shared_ref, SgExpression* subscript) {
    return NULL;
}

/*
 * Build AST to reference the given local array. Will look
 * something like:
 *     __upctr_local_A[k]
 */
SgExpression*
UpctrBuilder::buildLocalReference(
        SgPntrArrRefExp* local_array, SgExpression* subscript) {
    return NULL;
}

/*
 * Build AST to execute fetch of shared_array into local_array. Will
 * look something like:
 *     bupc_memget_strided(local_array, ..., shared_array, ...);
 */
SgStatement*
UpctrBuilder::buildFetch(
        SgPntrArrRefExp* local_array,
        SgPntrArrRefExp* shared_ref,
        SgExpression* subscript) {
    return NULL;
}

/*
 * Build AST to execute store of local_array into shared ref. Will
 * look something like:
 *     bupc_memput_strided(shared_ref, ..., local_array, ...);
 */
SgStatement*
UpctrBuilder::buildStore(
        SgPntrArrRefExp* shared_ref,
        SgPntrArrRefExp* local_array,
        SgExpression* subscript) {
    return NULL;
}
