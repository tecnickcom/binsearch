#!/usr/bin/env python

from codecs import open
from subprocess import call
from setuptools import setup, find_packages, Extension, Command


VERSION = (1, 0, 0)


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
        errno = call(['py.test'])
        raise SystemExit(errno)


setup(
    name='libpybinsearch',
    version=".".join([str(x) for x in VERSION]),
    keywords=('binsearch'),
    description="Binsearch Bindings for Python",
    long_description=open('../README.md', 'r').read(),
    author='Nicola Asuni',
    author_email='nicola.asuni@tecnick.com',
    url='https://github.com/tecnickcom/binsearch',
    packages=find_packages('src', exclude=['docs', 'tests*']),
    package_dir={'': 'src'},
    ext_modules=[
        Extension('libpybinsearch', [
            '../src/binsearch.c',
            'src/pybinsearch.c'
        ], extra_compile_args=["-O3"])
    ],
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'License :: OSI Approved :: MIT License',
        'Intended Audience :: Developers',
        'Programming Language :: C',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 3',
    ],
    extras_require = {
        'test': ['pytest', 'pytest-benchmark'],
    },
    cmdclass = {'test': RunTests},
)
