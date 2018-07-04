#!/usr/bin/env python

from codecs import open
from os.path import dirname, join
from subprocess import call
from setuptools import setup, find_packages, Extension, Command

def read(fname):
    return open(join(dirname(__file__), fname)).read()

class RunTests(Command):
    """Run all tests."""
    description = 'run tests'
    user_options = []

    def initialize_options(self):
        pass

    def finalize_options(self):
        pass

    def run(self):
        """Run all tests!"""
        errno = call([
            'py.test',
            '--verbose',
        ])
        raise SystemExit(errno)


setup(
    name='binsearch',
    version='5.1.1',
    keywords=('binsearch'),
    description="Binsearch Bindings for Python",
    long_description=read('../README.md'),
    author='Nicola Asuni',
    author_email='info@tecnick.com',
    url='https://github.com/tecnickcom/binsearch',
    license='MIT',
    platforms='Linux',
    packages=find_packages(exclude=['docs', 'test*']),
    ext_modules=[
        Extension('binsearch', [
             '../c/src/binsearch.c',
            'binsearch/pybinsearch.c'
        ],
        include_dirs=[
            '../c/src',
            'binsearch',
        ],
        extra_compile_args=[
            "-O3",
            "-pedantic",
            "-std=c99",
            "-Wall",
            "-Wextra",
            "-Wno-strict-prototypes",
            "-Wunused-value",
            "-Wcast-align",
            "-Wundef",
            "-Wformat-security",
            "-Wshadow",
            "-I../src",
        ])
    ],
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'License :: OSI Approved :: MIT License',
        'Intended Audience :: Developers',
        'Programming Language :: C',
        'Programming Language :: Python',
    ],
    extras_require={
        'test': [
            'coverage',
            'pytest',
            'pytest-benchmark',
            'pytest-cov',
            'pycodestyle',
        ],
    },
    cmdclass={'test': RunTests},
)
