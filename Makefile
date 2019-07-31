# MAKEFILE
#
# @author      Nicola Asuni <info@tecnick.com>
# @link        https://github.com/tecnickcom/binsearch
# ------------------------------------------------------------------------------

# --- MAKE TARGETS ---

# Display general help about this command
.PHONY: help
help:
	@echo ""
	@echo "BinSearch Makefile."
	@echo "The following commands are available:"
	@echo ""
	@echo "    make c          : Build and test the C version"
	@echo "    make go         : Build and test the GO version"
	@echo "    make python     : Build and test the Python version"
	@echo "    make clean      : Remove any build artifact"
	@echo ""

all: clean c go python

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
