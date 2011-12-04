#ifndef _UPC_LIBRARY_H_
#define _UPC_LIBRARY_H_

#include "rose.h"
#include "sageInterface.h"

//STLs
#include <vector>
#include <string>

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

	extern bool debug;
	extern bool phase2_only;

	SgExpression* buildThreadOfCall(
			SgExpression* exp,
			SgScopeStatement* scope);

	LoopTreeDepGraph* ComputeDependenceGraph(
			SgNode* loop);

    void printDepGraphAsDot(
            LoopTreeDepGraph* depgraph,
            char* filename);

	void processOptions(
			std::vector<std::string> &argvList);

} /* end namespace UpcLibrary */

#endif /* _UPC_LIBRARY_H_ */
