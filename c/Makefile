# MAKEFILE
#
# @author      Nicola Asuni <info@tecnick.com>
# @link        https://github.com/tecnickcom/binsearch
# ------------------------------------------------------------------------------

# Use bash as shell (Note: Ubuntu now uses dash which doesn't support PIPESTATUS).
SHELL=/bin/bash
.SHELLFLAGS=-o pipefail -c

# CVS path (path to the parent dir containing the project)
CVSPATH=github.com/tecnickcom

# Project owner
OWNER=Tecnick.com

# Project vendor
VENDOR=tecnickcom

# Project name
PROJECT=binsearch

# Project version
VERSION=$(shell cat ../VERSION)

# Project release number (packaging build number)
RELEASE=$(shell cat ../RELEASE)

# Current directory
CURRENTDIR=$(CURDIR)/

# Name of RPM or DEB package
PKGNAME=${VENDOR}-${PROJECT}-headers

# Path for binary files (where the executable files will be installed)
LIBPATH=usr/include/$(VENDOR)/$(PROJECT)/

# Path path for documentation
DOCPATH=usr/share/doc/$(PKGNAME)/

# Installation path for the binary files
PATHINSTLIB=$(DESTDIR)/$(LIBPATH)

# Installation path for documentation
PATHINSTDOC=$(DESTDIR)/$(DOCPATH)

# RPM Packaging path (where RPMs will be stored)
PATHRPMPKG=$(CURRENTDIR)/target/RPM

# DEB Packaging path (where DEBs will be stored)
PATHDEBPKG=$(CURRENTDIR)/target/DEB

# sed argument for in-place substitutions
SEDINPLACE=-i
ifeq ($(shell uname -s),Darwin)
	SEDINPLACE=-i ''
endif

# Options passed to lcov and genhtml
LCOVOPTS=--rc branch_coverage=1

# clang-tidy check list, shared by the linter and tidy targets
TIDYCHECKS=*,\
-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling,\
-altera-unroll-loops,\
-altera-id-dependent-backward-branch,-altera-struct-pack-align,\
-bugprone-easily-swappable-parameters,-cppcoreguidelines-avoid-magic-numbers,\
-readability-magic-numbers,-readability-identifier-length,\
-readability-isolate-declaration,-readability-function-cognitive-complexity,\
-llvmlibc-restrict-system-libc-headers,-llvm-header-guard,-llvm-include-order,\
-android-cloexec-open,-hicpp-no-assembler,-hicpp-signed-bitwise,\
-misc-include-cleaner,-concurrency-mt-unsafe,-cert-err33-c,\
-clang-analyzer-alpha.*

# cmake options: only pass the ones that are actually set, so that unset
# variables do not override the CMake defaults with an empty value.
CMAKEOPTS=\
$(if $(CMAKE_C_FLAGS),-DCMAKE_C_FLAGS="$(CMAKE_C_FLAGS)") \
$(if $(CMAKE_TOOLCHAIN_FILE),-DCMAKE_TOOLCHAIN_FILE="$(CMAKE_TOOLCHAIN_FILE)") \
$(if $(CMAKE_INSTALL_PATH),-DCMAKE_INSTALL_PREFIX="$(CMAKE_INSTALL_PATH)") \
$(if $(VH_BUILD_SHARED_LIB),-DBUILD_SHARED_LIB="$(VH_BUILD_SHARED_LIB)") \
$(if $(VH_BUILD_DOXYGEN),-DBUILD_DOXYGEN="$(VH_BUILD_DOXYGEN)")

# --- MAKE TARGETS ---

.PHONY: help
help:
	@echo ""
	@echo "$(PROJECT) Makefile."
	@echo "The following commands are available:"
	@echo ""
	@awk '/^## /{desc=substr($$0,4)} /^\.PHONY:/{if(NF>1) {target=$$2; if(desc) printf "  make %-15s: %s\n",target,desc; desc=""}}' Makefile
	@echo ""

all: clean version format linter test coverage sanitize build rpm deb doc

## Test C code compatibility with C++
.PHONY: testcpp
testcpp:
	# NOTE: xargs is required to propagate the compiler exit status: with
	# "find -exec ... \;" find always exits 0 and a compilation failure would be
	# silently ignored.
	find ./src/binsearch -type f -name '*.h' -print0 | xargs -0 -n 1 gcc -c -pedantic -Werror -Wall -Wextra -Wcast-align -Wundef -Wformat -Wformat-security -std=c++23 -x c++ -o /dev/null

## Lint the source code with cppcheck and clang-tidy
.PHONY: linter
linter:
	cppcheck --enable=warning,style,performance,portability --check-level=exhaustive --error-exitcode=1 --inline-suppr --std=c11 -I src/binsearch src/binsearch/binsearch.h
	# NOTE: the compile flags must be passed after "--". The previous "-p ."
	# pointed at a compilation database that does not exist, and without
	# --warnings-as-errors the target could never fail.
	clang-tidy src/binsearch/binsearch.h --warnings-as-errors='$(TIDYCHECKS)' -checks='$(TIDYCHECKS)' -- -x c -std=c2x -I src/binsearch
	cppcheck --enable=warning,style,performance,portability --check-level=exhaustive --error-exitcode=1 --inline-suppr --std=c11 -I src/binsearch test/*.c

## Build and run the unit tests
.PHONY: test
test: testcpp
	@mkdir -p target/test/test
	@echo -e "\n\n*** BUILD TEST ***\n"
	rm -rf target/test/*
	mkdir -p target/test/coverage
	cd target/test && \
	cmake $(CMAKEOPTS) -DCMAKE_BUILD_TYPE=Coverage \
	../.. | tee cmake.log ; test $${PIPESTATUS[0]} -eq 0 && \
	export LD_LIBRARY_PATH="$$LD_LIBRARY_PATH:./" && \
	make | tee make.log ; test $${PIPESTATUS[0]} -eq 0 && \
	lcov $(LCOVOPTS) --zerocounters --directory . && \
	lcov $(LCOVOPTS) --capture --initial --directory . --output-file coverage/binsearch.cov && \
	env CTEST_OUTPUT_ON_FAILURE=1 make test | tee test.log ; test $${PIPESTATUS[0]} -eq 0 && \
	lcov $(LCOVOPTS) --no-checksum --directory . --capture --output-file coverage/binsearch.info && \
	lcov $(LCOVOPTS) --remove coverage/binsearch.info '*/test/*' --output-file coverage/binsearch.info && \
	genhtml $(LCOVOPTS) -o coverage -t "binsearch Test Coverage" coverage/binsearch.info
ifeq ($(VH_BUILD_DOXYGEN),ON)
	cd target && \
	make doc | tee doc.log ; test $${PIPESTATUS[0]} -eq 0
endif

## Check that the line, function and branch coverage from the test target is 100%
.PHONY: coverage
coverage: test
	@summary=$$(lcov $(LCOVOPTS) --summary target/test/coverage/binsearch.info 2>&1); \
	for metric in lines functions branches; do \
		value=$$(echo "$$summary" | grep -oE "$$metric[.]*: [0-9]+\.[0-9]+%" | grep -oE '[0-9]+\.[0-9]+' | head -1); \
		echo "$$metric coverage: $$value%"; \
		if [ "$$value" != "100.0" ]; then echo "ERROR: $$metric coverage '$$value%' is below 100%"; exit 1; fi; \
	done

## Build and run the unit tests under the Address and Undefined Behaviour sanitizers
.PHONY: sanitize
sanitize:
	@mkdir -p target/sanitize
	# NOTE: the alignment check is enabled: the search functions read the
	# multi-byte values of a row with memcpy, and the column mode functions
	# require the caller to pass a properly aligned pointer.
	for f in test/*.c; do \
		$(CC) -std=c2x -pedantic -Wall -Wextra -Wcast-align -Wundef -Wformat -Wformat-security -Wshadow \
		-O1 -g -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer \
		-I src/binsearch -o "$(CURRENTDIR)/target/sanitize/$$(basename $$f .c)" "$$f" || exit 1; \
	done
	# The tests read their data files from the working directory.
	cd test/data && for t in "$(CURRENTDIR)/target/sanitize/"*; do "$$t" || exit 1; done

## Tidy the code via clang-tidy
.PHONY: tidy
tidy: linter

## Build the library
.PHONY: build
build:
	@mkdir -p target/build
	@echo -e "\n\n*** BUILD RELEASE ***\n"
	rm -rf target/build/*
	cd target/build && \
	cmake $(CMAKEOPTS) -DCMAKE_BUILD_TYPE=Release \
	../.. | tee cmake.log ; test $${PIPESTATUS[0]} -eq 0 && \
	export LD_LIBRARY_PATH="$$LD_LIBRARY_PATH:./" && \
	make | tee make.log ; test $${PIPESTATUS[0]} -eq 0
	cd target/build && \
	export LD_LIBRARY_PATH="$$LD_LIBRARY_PATH:./" && \
	env CTEST_OUTPUT_ON_FAILURE=1 make test | tee build.log ; test $${PIPESTATUS[0]} -eq 0

## Set the version from VERSION file
.PHONY: version
version:
	sed $(SEDINPLACE) "s/PROJECT_NUMBER.*=.*$$/PROJECT_NUMBER = $(VERSION)/" doc/Doxyfile

## Generate source code documentation
.PHONY: doc
doc:
	cd target/build && \
	make doc | tee doc.log ; test $${PIPESTATUS[0]} -eq 0

## Format the source code
.PHONY: format
format:
	astyle --style=allman --recursive --suffix=none 'src/binsearch/*.h'
	astyle --style=allman --recursive --suffix=none 'test/*.c'

## Remove any build artifact
.PHONY: clean
clean:
	rm -rf target

## Install this application
.PHONY: install
install: uninstall
	mkdir -p "$(PATHINSTLIB)"
	cp src/binsearch/*.h "$(PATHINSTLIB)"
	find "$(PATHINSTLIB)" -type f -exec chmod 644 {} \;
	mkdir -p "$(PATHINSTDOC)"
	cp -f ../LICENSE "$(PATHINSTDOC)"
	cp -f ../README.md "$(PATHINSTDOC)"
	cp -f ../VERSION "$(PATHINSTDOC)"
	cp -f ../RELEASE "$(PATHINSTDOC)"
	chmod -R 644 "$(PATHINSTDOC)"*

## Remove all installed files (excluding configuration files)
.PHONY: uninstall
uninstall:
	rm -rf "$(PATHINSTLIB)"
	rm -rf "$(PATHINSTDOC)"

## Build the RPM package for RedHat-like Linux distributions
.PHONY: rpm
rpm:
	@test $(words $(CURDIR)) -eq 1 || { echo "ERROR: rpmbuild does not support spaces in the project path: $(CURDIR)"; exit 1; }
	rm -rf "$(PATHRPMPKG)"
	rpmbuild \
	--define "_topdir $(PATHRPMPKG)" \
	--define "_vendor $(VENDOR)" \
	--define "_owner $(OWNER)" \
	--define "_cvspath $(CVSPATH)" \
	--define "_project $(PROJECT)" \
	--define "_package $(PKGNAME)" \
	--define "_version $(VERSION)" \
	--define "_release $(RELEASE)" \
	--define "_current_directory $(CURRENTDIR)" \
	--define "_libpath /$(LIBPATH)" \
	--define "_docpath /$(DOCPATH)" \
	-bb resources/rpm/rpm.spec

## Build the DEB package for Debian-like Linux distributions
.PHONY: deb
deb:
	rm -rf "$(PATHDEBPKG)"
	make install DESTDIR="$(PATHDEBPKG)/$(PKGNAME)-$(VERSION)"
	rm -f "$(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/$(DOCPATH)LICENSE"
	tar -zcvf "$(PATHDEBPKG)/$(PKGNAME)_$(VERSION).orig.tar.gz" -C "$(PATHDEBPKG)/" "$(PKGNAME)-$(VERSION)"
	cp -rf ./resources/debian "$(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/debian"
	find "$(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/debian/" -type f -exec sed $(SEDINPLACE) "s/~#DATE#~/`date -R`/" {} \;
	find "$(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/debian/" -type f -exec sed $(SEDINPLACE) "s/~#PKGNAME#~/$(PKGNAME)/" {} \;
	find "$(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/debian/" -type f -exec sed $(SEDINPLACE) "s/~#VERSION#~/$(VERSION)/" {} \;
	find "$(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/debian/" -type f -exec sed $(SEDINPLACE) "s/~#RELEASE#~/$(RELEASE)/" {} \;
	echo "$(LIBPATH)" > "$(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/debian/$(PKGNAME).dirs"
	echo "$(LIBPATH)* $(LIBPATH)" > "$(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/debian/install"
	echo "$(DOCPATH)" >> "$(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/debian/$(PKGNAME).dirs"
	echo "$(DOCPATH)* $(DOCPATH)" >> "$(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/debian/install"
	echo "new-package-should-close-itp-bug" >> "$(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/debian/$(PKGNAME).lintian-overrides"
	cd "$(PATHDEBPKG)/$(PKGNAME)-$(VERSION)" && debuild -us -uc
