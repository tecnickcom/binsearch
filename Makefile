# MAKEFILE
#
# @author      Nicola Asuni <info@tecnick.com>
# @link        https://github.com/tecnickcom/binsearch
# ------------------------------------------------------------------------------

# List special make targets that are not associated with files
.PHONY: help qa test tidy build python pytest version cgo go doc format clean

# Use bash as shell (Note: Ubuntu now uses dash which doesn't support PIPESTATUS).
SHELL=/bin/bash

# CVS path (path to the parent dir containing the project)
CVSPATH=github.com/tecnickcom

# Project owner
OWNER=Tecnick.com

# Project vendor
VENDOR=tecnickcom

# Project name
PROJECT=binsearch

# Project version
VERSION=$(shell cat VERSION)

# Project release number (packaging build number)
RELEASE=$(shell cat RELEASE)

# Name of RPM or DEB package
PKGNAME=${VENDOR}-${PROJECT}

# Current directory
CURRENTDIR=$(shell pwd)

# Conda environment
CONDA_ENV=${CURRENTDIR}/../env-${PROJECT}

# Include default build configuration
include $(CURRENTDIR)/config.mk

# --- MAKE TARGETS ---

# Display general help about this command
help:
	@echo ""
	@echo "$(PROJECT) Makefile."
	@echo "GOPATH=$(GOPATH)"
	@echo "The following commands are available:"
	@echo ""
	@echo "    make qa      : Run all the tests and static analysis reports"
	@echo "    make test    : Run the unit tests"
	@echo "    make tidy    : Check the code using clang-tidy"
	@echo "    make build   : Build the library"
	@echo "    make python  : Build the python module"
	@echo "    make pytest  : Test the python module"
	@echo "    make version : Set version from VERSION file"
	@echo "    make cgo     : Test the golang cgo module"
	@echo "    make go      : Test the native golang module"
	@echo "    make doc     : Generate source code documentation"
	@echo "    make format  : Format the source code"
	@echo "    make clean   : Remove any build artifact"
	@echo ""

# Alias for help target
all: clean format qa build cgo go python pytest

# Alias for test
qa: test tidy

# Build and run the unit tests
test:
	@mkdir -p target/test/test
	@echo -e "\n\n*** BUILD TEST - see config.mk ***\n"
	rm -rf target/test/*
	mkdir -p target/test/coverage
	cd target/test && \
	cmake -DCMAKE_C_FLAGS=$(CMAKE_C_FLAGS) \
	-DCMAKE_TOOLCHAIN_FILE=$(CMAKE_TOOLCHAIN_FILE) \
	-DCMAKE_BUILD_TYPE=Coverage \
	-DCMAKE_INSTALL_PREFIX=$(CMAKE_INSTALL_PATH) \
	-DBUILD_SHARED_LIB=$(BS_BUILD_SHARED_LIB) \
	-DBUILD_DOXYGEN=$(BS_BUILD_DOXYGEN) \
	../.. | tee cmake.log ; test $${PIPESTATUS[0]} -eq 0 && \
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ && \
	make | tee make.log ; test $${PIPESTATUS[0]} -eq 0 && \
	lcov --zerocounters --directory . && \
	lcov --capture --initial --directory . --output-file coverage/binsearch && \
	env CTEST_OUTPUT_ON_FAILURE=1 make test | tee test.log ; test $${PIPESTATUS[0]} -eq 0 && \
	lcov --no-checksum --directory . --capture --output-file coverage/binsearch.info && \
	lcov --remove coverage/binsearch.info "/test_*" --output-file coverage/binsearch.info && \
	genhtml -o coverage -t "Binsearch Test Coverage" coverage/binsearch.info
ifeq ($(BS_BUILD_DOXYGEN),ON)
	cd target && \
	make doc | tee doc.log ; test $${PIPESTATUS[0]} -eq 0
endif

# use clang-tidy
tidy:
	clang-tidy -checks='*,-llvm-header-guard' -header-filter=.* -p . src/*.c

# Build the library
build:
	@mkdir -p target/build
	@echo -e "\n\n*** BUILD RELEASE - see config.mk ***\n"
	rm -rf target/build/*
	cd target/build && \
	cmake -DCMAKE_C_FLAGS=$(CMAKE_C_FLAGS) \
	-DCMAKE_TOOLCHAIN_FILE=$(CMAKE_TOOLCHAIN_FILE) \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX=$(CMAKE_INSTALL_PATH) \
	-DBUILD_SHARED_LIB=$(BS_BUILD_SHARED_LIB) \
	-DBUILD_DOXYGEN=$(BS_BUILD_DOXYGEN) \
	../.. | tee cmake.log ; test $${PIPESTATUS[0]} -eq 0 && \
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ && \
	make | tee make.log ; test $${PIPESTATUS[0]} -eq 0
	cd target/build && \
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ && \
	env CTEST_OUTPUT_ON_FAILURE=1 make test | tee test.log ; test $${PIPESTATUS[0]} -eq 0

# Set the version from VERSION file
version:
	#sed -i "s/version:.*$$/version: $(VERSION).$(RELEASE)/" conda/meta.yaml
	sed -i "s/__version__.*$$/__version__ = '$(VERSION)'/" python/binsearch/__init__.py

# Build the python module
python: version
	cd python && \
	rm -rf ./build && \
	python3 setup.py build_ext --include-dirs=../src && \
	rm -f tests/*.so && \
	find build/ -iname '*.so' -exec cp {} tests/ \;

# Test python module
pytest:
	cd python && \
	python3 setup.py test

# Build a conda package
conda: version
	./conda/setup-conda.sh && \
	${CONDA_ENV}/bin/conda build --prefix-length 160 --no-anaconda-upload --no-remove-work-dir --override-channels $(ARTIFACTORY_CONDA_CHANNELS) conda

# Test golang cgo module
cgo:
	cd cgo && \
	make deps qa

# Test golang module
go:
	cd go && \
	make deps qa

# Generate source code documentation
doc:
	cd target/build && \
	make doc | tee doc.log ; test $${PIPESTATUS[0]} -eq 0

# Format the source code
format:
	astyle --style=allman --recursive --suffix=none 'src/*.h'
	astyle --style=allman --recursive --suffix=none 'src/*.c'
	astyle --style=allman --recursive --suffix=none 'test/*.c'
	astyle --style=allman --recursive --suffix=none 'python/binsearch/*.h'
	astyle --style=allman --recursive --suffix=none 'python/binsearch/*.c'
	autopep8 --in-place ./python/tests/*.py
	cd cgo && make format
	cd go && make format

# Remove any build artifact
clean:
	rm -rf target htmlcov build dist .cache .benchmarks ./tests/*.so ./tests/__pycache__ ./pytemp/__pycache__ ./pytemp.egg-info
	find . -type f -name '*.pyc' -exec rm -f {} \;
