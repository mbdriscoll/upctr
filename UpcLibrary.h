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

    bool depFilter(DepType dt);

	void processOptions(
			std::vector<std::string> &argvList);

} /* end namespace UpcLibrary */

typedef std::map<SgForStatement*,SgExpression*> aff_map_t;
extern aff_map_t aff_exp_map;

#endif /* _UPC_LIBRARY_H_ */
