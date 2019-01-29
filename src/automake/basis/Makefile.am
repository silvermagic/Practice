NULL =

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src

INTLTOOL_FILES =            \
	$(NULL)

DISTCLEANFILES =            \
	$(NULL)

# 不会被编译，但是执行 make dist 又需要被打包的文件
EXTRA_DIST =                \
	autogen.sh				\
	$(NULL)

# 构建过程中生成的文件，执行 make maintainer-clean 时要被删除的文件
MAINTAINERCLEANFILES =      \
	aclocal.m4				\
	config.h.in				\
	build-aux/depcomp       \
	build-aux/install-sh    \
	build-aux/ltmain.sh     \
	build-aux/missing       \
	Makefile.in             \
    src/Makefile.in         \
    configure               \
    ChangeLog               \
    AUTHORS                 \
    README                  \
    NEWS                    \
	$(NULL)

# 执行 make distclean 要被删除的文件
DISTCLEAN_FILES =           \
	$(NULL)

dist-hook: gen-ChangeLog gen-AUTHORS

# Generate the ChangeLog file (with all entries since the switch to git)
# and insert it into the directory we're about to use to create a tarball.

.PHONY: gen-ChangeLog gen-AUTHORS

gen-ChangeLog:
	if test -d .git || test -d ../.git; then		\
	  $(top_srcdir)/build-aux/gitlog-to-changelog   \
	    > $(distdir)/cl-t;					        \
	  rm -f $(distdir)/ChangeLog;				    \
	  mv $(distdir)/cl-t $(distdir)/ChangeLog;		\
	fi

gen-AUTHORS:
	$(AM_V_GEN)if test -d $(srcdir)/.git; then                                                           \
	   out="`cd $(srcdir) && git log --pretty=format:'%aN <%aE>' | sort -u`" > $(distdir)/AUTHORS-tmp && \
	   mv -f $(distdir)/AUTHORS-tmp $(distdir)/AUTHORS ;                                                 \
	fi
