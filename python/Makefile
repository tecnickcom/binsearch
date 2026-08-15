# MAKEFILE
#
# @author      Nicola Asuni <info@tecnick.com>
# @link        https://github.com/tecnickcom/binsearch
# ------------------------------------------------------------------------------

SHELL=/bin/bash
.SHELLFLAGS=-o pipefail -c

# Project name
PROJECT=binsearch

# Project version
VERSION=$(shell cat ../VERSION)

# Project release number (packaging build number)
RELEASE=$(shell cat ../RELEASE)

# Current directory
CURRENTDIR=$(CURDIR)/

# Path fot the python binary
PYTHON=$(shell which python3)

# sed argument for in-place substitutions
SEDINPLACE=-i
ifeq ($(shell uname -s),Darwin)
	SEDINPLACE=-i ''
endif

# --- MAKE TARGETS ---

.PHONY: help
help:
	@echo ""
	@echo "$(PROJECT) Makefile."
	@echo "The following commands are available:"
	@echo ""
	@awk '/^## /{desc=substr($$0,4)} /^\.PHONY:/{if(NF>1) {target=$$2; if(desc) printf "  make %-15s: %s\n",target,desc; desc=""}}' Makefile
	@echo ""

all: clean venv linter test coverage build

## Copy the files the package needs from the repository root
.PHONY: prepare
prepare:
	cp -f ../c/src/binsearch/binsearch.h binsearch/binsearch.h
	cp -f ../README.md README.md
	cp -f ../LICENSE LICENSE

## Build the package
.PHONY: build
build: version prepare
	source venv/bin/activate \
	&& python -m build --sdist --wheel

## Remove any build artifact
.PHONY: clean
clean:
	rm -rf venv target htmlcov build dist .pytest_cache .cache .benchmarks ./test/*.so ./test/__pycache__ ./binsearch/__pycache__ ./binsearch.egg-info
	rm -f binsearch/binsearch.h README.md LICENSE ./binsearch*.so
	# Leftovers of an older layout: a stale copy of the C sources and the
	# backup the version target used to write.
	rm -rf c setup.py.bak
	find . -type f -name '*.pyc' -exec rm -f {} \;
	find . -type f -name '*.gcda' -exec rm -f {} \;
	find . -type f -name '*.gcno' -exec rm -f {} \;

## Generate source code documentation
.PHONY: docs
docs:
	pydoc3 -p 1234 $(PROJECT)

## Format the source code
.PHONY: format
format:
	# NOTE: --recursive requires a wildcard in the path, and silently fails
	# with a plain file name.
	astyle --style=allman --suffix=none 'binsearch/pybinsearch.h' 'binsearch/pybinsearch.c'
	source venv/bin/activate \
	&& ruff format setup.py test

## Lint the source code with cppcheck (C extension) and ruff (Python)
.PHONY: linter
linter: prepare
	cppcheck --enable=warning,style,performance,portability --check-level=exhaustive --error-exitcode=1 --inline-suppr --suppress=unusedFunction -I binsearch binsearch/pybinsearch.c
	source venv/bin/activate \
	&& ruff check setup.py test

## Unit tests
.PHONY: test
test: prepare
	# NOTE: the editable install points at a prebuilt extension that is not
	# rebuilt on import, so the extension is rebuilt here: without this the
	# tests would silently run against the previous binary.
	source venv/bin/activate \
	&& python setup.py build_ext --inplace \
	&& pytest

## Check that the test suite and the C extension are fully covered
.PHONY: coverage
coverage:
	source venv/bin/activate \
	&& pytest --cov=test --cov-branch --cov-report=term-missing --cov-fail-under=100 test
	# coverage.py cannot see inside a C extension, so the extension is rebuilt
	# with gcov instrumentation and measured separately with lcov.
	rm -rf build target/coverage
	mkdir -p target/coverage
	# NOTE: -DNDEBUG matches the normal build: without it the assertions inside
	# the CPython macros add branches that no test can reach.
	source venv/bin/activate \
	&& CFLAGS="-O0 -g -DNDEBUG --coverage" LDFLAGS="--coverage" python setup.py build_ext --inplace --force \
	&& pytest -q --benchmark-disable test
	lcov --capture --directory build --output-file target/coverage/all.info --rc branch_coverage=1 --quiet
	lcov --extract target/coverage/all.info '*/binsearch/pybinsearch.c' --output-file target/coverage/ext.info --rc branch_coverage=1 --quiet
	@summary=$$(lcov --summary target/coverage/ext.info --rc branch_coverage=1 2>&1); \
	for metric in lines functions branches; do \
		value=$$(echo "$$summary" | grep -oE "$$metric[.]*: [0-9]+\.[0-9]+%" | grep -oE '[0-9]+\.[0-9]+' | head -1); \
		echo "extension $$metric coverage: $$value%"; \
		if [ "$$value" != "100.0" ]; then echo "ERROR: extension $$metric coverage '$$value%' is below 100%"; exit 1; fi; \
	done
	# Restore the optimised build.
	rm -rf build
	find . -type f -name '*.gcda' -exec rm -f {} \;
	find . -type f -name '*.gcno' -exec rm -f {} \;
	source venv/bin/activate && python setup.py build_ext --inplace --force

## Set the version from VERSION file
.PHONY: version
version:
	sed $(SEDINPLACE) "s/^    version=.*,$$/    version=\"$(VERSION).$(RELEASE)\",/" setup.py

## Create a virtual environment
.PHONY: venv
venv: clean version prepare
	rm -rf venv
	virtualenv -p $(PYTHON) venv
	source venv/bin/activate \
	&& python --version \
	&& pip install --upgrade pip \
	&& pip install --upgrade --pre wheel build pytest setuptools \
	&& pip install --use-pep517 --no-build-isolation -e .[test]
