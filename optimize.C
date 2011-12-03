#include "optimize.h"
#include "UpcLibrary.h"

using namespace std;

/**
 * Optimize this loop nest.
 */
void optimize(SgForStatement* nest) {
    // Replace operators with their equivalent counterparts defined
    // in "inline" annotations
    AstInterfaceImpl faImpl_1(nest);
    CPPAstInterface fa_body(&faImpl_1);
    OperatorInlineRewrite()(fa_body, AstNodePtrImpl(nest));

    // Pass annotations to arrayInterface and use them to collect
    // alias info. function info etc.
    SgFunctionDefinition* defn = SageInterface::getEnclosingProcedure(nest);
    ArrayAnnotation *annot = ArrayAnnotation::get_inst();
    ArrayInterface array_interface(*annot);
    array_interface.initialize(fa_body, AstNodePtrImpl(defn));
    array_interface.observe(fa_body);

    //FR(06/07/2011): aliasinfo was not set which caused segfault
    LoopTransformInterface::set_aliasInfo(&array_interface);

    // X. Loop normalization for all loops within body
    NormalizeForLoop(fa_body, AstNodePtrImpl(nest));

    LoopTreeDepGraph* depgraph = UpcLibrary::ComputeDependenceGraph(nest, &array_interface, annot);
}
