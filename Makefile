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
VERSION=$(shell cat VERSION)

# Project release number (packaging build number)
# RELEASE=$(shell cat RELEASE)

# --- MAKE TARGETS ---

# Display general help about this command
.PHONY: help
help:
	@echo ""
	@echo "$(PROJECT) Makefile."
	@echo "The following commands are available:"
	@echo ""
	@echo "  make c         : Build and test the C version"
	@echo "  make go        : Build and test the GO version"
	@echo "  make python    : Build and test the Python version"
	@echo "  make clean     : Remove any build artifact"
	@echo "  make tag       : Tag the Git repository"
	@echo "  make versionup : Increase the patch number in the VERSION file"
	@echo ""

all: c go python

# Build and test the C version
.PHONY: c
c:
	cd c && make all

# Build and test the GO version
.PHONY: go
go:
	cd go && make all

# Build and test the Python version
.PHONY: python
python:
	cd python && make all

# Remove any build artifact
.PHONY: clean
clean:
	rm -rf target
	cd c && make clean
	cd go && make clean
	cd python && make clean

# Tag the Git repository
.PHONY: tag
tag:
	git tag -a "v$(VERSION)" -m "Version $(VERSION)" && \
	git push origin --tags

# Increase the patch number in the VERSION file
.PHONY: versionup
versionup:
	echo ${VERSION} | gawk -F. '{printf("%d.%d.%d",$$1,$$2,(($$3+1)));}' > VERSION
