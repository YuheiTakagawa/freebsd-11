# $FreeBSD: releng/11.0/lib/clang/liblldbBreakpoint/Makefile 276479 2014-12-31 20:31:32Z dim $

.include <bsd.own.mk>

LIB=	lldbBreakpoint

SRCDIR=	tools/lldb/source/Breakpoint
SRCS=	Breakpoint.cpp \
	BreakpointID.cpp \
	BreakpointIDList.cpp \
	BreakpointList.cpp \
	BreakpointLocation.cpp \
	BreakpointLocationCollection.cpp \
	BreakpointLocationList.cpp \
	BreakpointOptions.cpp \
	BreakpointResolver.cpp \
	BreakpointResolverAddress.cpp \
	BreakpointResolverFileLine.cpp \
	BreakpointResolverFileRegex.cpp \
	BreakpointResolverName.cpp \
	BreakpointSite.cpp \
	BreakpointSiteList.cpp \
	Stoppoint.cpp \
	StoppointCallbackContext.cpp \
	StoppointLocation.cpp \
	Watchpoint.cpp \
	WatchpointList.cpp \
	WatchpointOptions.cpp

TGHDRS=	AttrList \
	Attrs \
	DiagnosticCommonKinds \
	DeclNodes \
	StmtNodes \
	CommentCommandList

.include "../lldb.lib.mk"
