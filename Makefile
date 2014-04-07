# Borrowed from the BuDDy OCaml bindings:
#   https://github.com/abate/ocaml-buddy

NAME = mathsat
VERSION = 0.1

LIBS = _build/$(NAME).cma
LIBS_OPT = _build/$(NAME).cmxa _build/$(NAME).cmxs
RESULTS = $(LIBS)
RESULTS_OPT = $(LIBS_OPT)
SOURCES = $(wildcard *.ml *.mli) *.c

OCAMLBUILD = ocamlbuild
OBFLAGS = -classic-display -use-ocamlfind
OCAMLFIND = ocamlfind

DESTDIR =
LIBDIR = $(DESTDIR)/$(shell ocamlc -where)
BINDIR = $(DESTDIR)/usr/bin
ifeq ($(DESTDIR),)
INSTALL = $(OCAMLFIND) install
UNINSTALL = $(OCAMLFIND) remove
else
INSTALL = $(OCAMLFIND) install -destdir $(LIBDIR)
UNINSTALL = $(OCAMLFIND) remove -destdir $(LIBDIR)
endif

DIST_DIR = $(NAME)-$(VERSION)
DIST_TARBALL = $(DIST_DIR).tar.gz
DEB_TARBALL = $(subst -,_,$(DIST_DIR).orig.tar.gz)

all: $(RESULTS) opt test
opt: $(RESULTS_OPT)

$(RESULTS): $(SOURCES)
$(RESULTS_OPT): $(SOURCES)

clean:
	$(OCAMLBUILD) $(OBFLAGS) -clean

_build/%:
	$(OCAMLBUILD) $(OBFLAGS) $*
	@touch $@

docs:
	if [ ! -d doc ]; then mkdir doc; fi
	ocamlfind ocamldoc $(OCFLAGS) -package gmp -html -d doc $(NAME).mli

INSTALL_STUFF = META
INSTALL_STUFF += $(wildcard _build/*$(NAME)*.cma _build/$(NAME).cmxa _build/*$(NAME)*.a)
INSTALL_STUFF += $(wildcard _build/*$(NAME)*.cmi) $(wildcard *.mli)
INSTALL_STUFF += $(wildcard _build/*$(NAME)*.cmx _build/dll$(NAME)_stubs.so)

install:
	test -d $(LIBDIR) || mkdir -p $(LIBDIR)
	$(INSTALL) -ldconf ignore -patch-version $(VERSION) $(NAME) $(INSTALL_STUFF)

uninstall:
	$(UNINSTALL) $(NAME)

.PHONY: test
test:
	$(OCAMLBUILD) $(OBFLAGS) test.native
	./test.native

