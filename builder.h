#ifndef _UPCTR_BUILDER_H_
#define _UPCTR_BUILDER_H_

#include "rose.h"

namespace UpctrBuilder {

    /*
     * Build AST to declare local array. Will look something like:
     *     double* __upctr_local_A[N];
     */
    SgExpression* buildLocalArray(
            SgPntrArrRefExp* shared_ref, SgExpression* subscript);

    /*
     * Build AST to reference the given local array. Will look
     * something like:
     *     __upctr_local_A[k]
     */
    SgExpression* buildLocalReference(
            SgExpression* local_array, SgExpression* subscript);

    /*
     * Build AST to execute fetch of shared_array into local_array. Will
     * look something like:
     *     bupc_memget_strided(local_array, ..., shared_array, ...);
     */
    SgStatement* buildFetch(
            SgPntrArrRefExp* local_array,
            SgPntrArrRefExp* shared_ref,
            SgExpression* subscript);

    /*
     * Build AST to execute store of local_array into shared ref. Will
     * look something like:
     *     bupc_memput_strided(shared_ref, ..., local_array, ...);
     */
    SgStatement* buildStore(
            SgPntrArrRefExp* shared_ref,
            SgPntrArrRefExp* local_array,
            SgExpression* subscript);

} /* end namespace UpcBuilder */

#endif /* _UPCTR_BUILDER_H_ */
