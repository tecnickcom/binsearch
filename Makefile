# MAKEFILE
#
# @author      Nicola Asuni <info@tecnick.com>
# @link        https://github.com/tecnickcom/binsearch
# ------------------------------------------------------------------------------

# List special make targets that are not associated with files
.PHONY: help c go python clean

# --- MAKE TARGETS ---

# Display general help about this command
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
c:
	cd c && make all

# Build and test the GO version
go:
	cd go && make all

# Build and test the Python version
python:
	cd python && make all

# Remove any build artifact
clean:
	rm -rf target
	cd c && make clean
	cd go && make clean
	cd python && make clean
