# $FreeBSD: releng/11.0/lib/clang/libclangstaticanalyzercore/Makefile 296417 2016-03-05 20:24:31Z dim $

.include <bsd.own.mk>

LIB=	clangstaticanalyzercore

SRCDIR=	tools/clang/lib/StaticAnalyzer/Core
SRCS=	APSIntType.cpp \
	AnalysisManager.cpp \
	AnalyzerOptions.cpp \
	BasicValueFactory.cpp \
	BlockCounter.cpp \
	BugReporter.cpp \
	BugReporterVisitors.cpp \
	CallEvent.cpp \
	Checker.cpp \
	CheckerContext.cpp \
	CheckerHelpers.cpp \
	CheckerManager.cpp \
	CheckerRegistry.cpp \
	CommonBugCategories.cpp \
	ConstraintManager.cpp \
	CoreEngine.cpp \
	DynamicTypeMap.cpp \
	Environment.cpp \
	ExplodedGraph.cpp \
	ExprEngine.cpp \
	ExprEngineC.cpp \
	ExprEngineCXX.cpp \
	ExprEngineCallAndReturn.cpp \
	ExprEngineObjC.cpp \
	FunctionSummary.cpp \
	HTMLDiagnostics.cpp \
	IssueHash.cpp \
	LoopWidening.cpp \
	MemRegion.cpp \
	PathDiagnostic.cpp \
	PlistDiagnostics.cpp \
	ProgramState.cpp \
	RangeConstraintManager.cpp \
	RegionStore.cpp \
	SValBuilder.cpp \
	SVals.cpp \
	SimpleConstraintManager.cpp \
	SimpleSValBuilder.cpp \
	Store.cpp \
	SubEngine.cpp \
	SymbolManager.cpp

TGHDRS=	AttrList \
	Attrs \
	CommentCommandList \
	CommentNodes \
	DeclNodes \
	DiagnosticCommonKinds \
	DiagnosticFrontendKinds \
	StmtNodes

.include "../clang.lib.mk"
