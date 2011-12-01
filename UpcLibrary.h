#ifndef _UPC_LIBRARY_H_
#define _UPC_LIBRARY_H_

#include "rose.h"
#include "sageInterface.h"

//Array Annotation headers
#include <CPPAstInterface.h>
#include <ArrayAnnot.h>
#include <ArrayRewrite.h>

//Dependence graph headers
#include <AstInterface_ROSE.h>
#include <LoopTransformInterface.h>
#include <AnnotCollect.h>
#include <OperatorAnnotation.h>
#include <LoopTreeDepComp.h>

namespace UpcLibrary {

	SgExpression* buildThreadOfCall(
			SgExpression* exp,
			SgScopeStatement* scope);

	SgInitializedName* getLoopInvariant(
			SgNode* loop);

	LoopTreeDepGraph* ComputeDependenceGraph(
			SgNode* loop,
			ArrayInterface* array_interface,
			ArrayAnnotation* annot);

//	LoopTreeDepGraph* ComputeDependenceGraph(
//			SgNode* loop);

} /* end namespace UpcLibrary */

#endif /* _UPC_LIBRARY_H_ */
