"""Build script for the binsearch Python extension."""

from os import environ
from os.path import dirname, exists, join
from shutil import copyfile
from subprocess import call
from typing import ClassVar

from setuptools import Command, Extension, find_packages, setup


def read(fname):
    """Read a file from the package directory, returning "" when it is absent."""
    path = join(dirname(__file__), fname)
    try:
        with open(path, encoding="utf-8") as handle:
            return handle.read()
    except OSError:
        return ""


def prepare():
    """Copy the C library header next to the extension source.

    The header belongs to the C implementation at the root of the repository:
    it is copied here so that a build from a source checkout works without the
    Makefile, and it is part of the source distribution.
    """
    header = join(dirname(__file__), "binsearch", "binsearch.h")
    source = join(dirname(__file__), "..", "c", "src", "binsearch", "binsearch.h")
    if exists(source):
        copyfile(source, header)
    if not exists(header):
        raise SystemExit("missing binsearch/binsearch.h: run 'make prepare' first")


# setuptools appends extra_compile_args after the CFLAGS of the environment, so
# the optimisation level set here would override the one of a coverage or debug
# build. It is only added when the environment does not set its own flags.
# _DEFAULT_SOURCE makes the POSIX memory advice macros visible. Without it a
# strict ISO C build sees neither MADV_WILLNEED nor POSIX_MADV_WILLNEED, and the
# page requests of the find_many functions are not compiled at all.
COMPILE_ARGS = [
    "-pedantic",
    "-std=c2x",
    "-D_DEFAULT_SOURCE",
    "-Wall",
    "-Wextra",
    "-Wno-strict-prototypes",
    "-Wunused-value",
    "-Wcast-align",
    "-Wundef",
    "-Wformat",
    "-Wformat-security",
    "-Wshadow",
]

if not environ.get("CFLAGS"):
    COMPILE_ARGS.insert(0, "-O3")


class RunTests(Command):
    """Run all tests."""

    description = "run tests"
    user_options: ClassVar[list] = []

    def initialize_options(self):
        """Set the default values of the command options."""

    def finalize_options(self):
        """Validate the command options."""

    def run(self):
        """Run the test suite and exit with its status code."""
        errno = call(["py.test", "--verbose"])
        raise SystemExit(errno)


prepare()

setup(
    name="binsearch",
    version="10.0.2.0",
    keywords=("binsearch"),
    description="Binsearch Bindings for Python",
    long_description=read("README.md"),
    long_description_content_type="text/markdown",
    author="Nicola Asuni",
    author_email="info@tecnick.com",
    url="https://github.com/tecnickcom/binsearch",
    license="MIT",
    platforms="Linux",
    python_requires=">=3.9",
    packages=find_packages(exclude=["docs", "test*"]),
    ext_modules=[
        Extension(
            "binsearch",
            ["binsearch/pybinsearch.c"],
            include_dirs=["binsearch"],
            extra_compile_args=COMPILE_ARGS,
        )
    ],
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Developers",
        "Programming Language :: C",
        "Programming Language :: Python",
        "Programming Language :: Python :: 3",
    ],
    extras_require={
        "test": [
            "coverage",
            "pytest",
            "pytest-benchmark",
            "pytest-cov",
            "ruff",
        ]
    },
    cmdclass={"test": RunTests},
)
