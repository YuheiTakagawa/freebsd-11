# $FreeBSD: releng/11.0/lib/clang/liblldbTarget/Makefile 296417 2016-03-05 20:24:31Z dim $

.include <bsd.own.mk>

LIB=	lldbTarget

CFLAGS+=-I${.CURDIR}/../../../contrib/llvm/tools/lldb/source/Plugins/Process/Utility

SRCDIR=	tools/lldb/source/Target
SRCS=	ABI.cpp \
	CPPLanguageRuntime.cpp \
	ExecutionContext.cpp \
	FileAction.cpp \
	InstrumentationRuntime.cpp \
	InstrumentationRuntimeStopInfo.cpp \
	JITLoader.cpp \
	JITLoaderList.cpp \
	Language.cpp \
	LanguageRuntime.cpp \
	Memory.cpp \
	MemoryHistory.cpp \
	ObjCLanguageRuntime.cpp \
	OperatingSystem.cpp \
	PathMappingList.cpp \
	Platform.cpp \
	Process.cpp \
	ProcessInfo.cpp \
	ProcessLaunchInfo.cpp \
	Queue.cpp \
	QueueItem.cpp \
	QueueList.cpp \
	RegisterContext.cpp \
	SectionLoadHistory.cpp \
	SectionLoadList.cpp \
	StackFrame.cpp \
	StackFrameList.cpp \
	StackID.cpp \
	StopInfo.cpp \
	SystemRuntime.cpp \
	Target.cpp \
	TargetList.cpp \
	Thread.cpp \
	ThreadCollection.cpp \
	ThreadList.cpp \
	ThreadPlan.cpp \
	ThreadPlanBase.cpp \
	ThreadPlanCallFunction.cpp \
	ThreadPlanCallFunctionUsingABI.cpp \
	ThreadPlanCallUserExpression.cpp \
	ThreadPlanPython.cpp \
	ThreadPlanRunToAddress.cpp \
	ThreadPlanShouldStopHere.cpp \
	ThreadPlanStepInRange.cpp \
	ThreadPlanStepInstruction.cpp \
	ThreadPlanStepOut.cpp \
	ThreadPlanStepOverBreakpoint.cpp \
	ThreadPlanStepOverRange.cpp \
	ThreadPlanStepRange.cpp \
	ThreadPlanStepThrough.cpp \
	ThreadPlanStepUntil.cpp \
	ThreadPlanTracer.cpp \
	ThreadSpec.cpp \
	UnixSignals.cpp \
	UnwindAssembly.cpp

TGHDRS=	AttrList \
	Attrs \
	DiagnosticCommonKinds \
	DeclNodes \
	StmtNodes \
	CommentCommandList

.include "../lldb.lib.mk"
