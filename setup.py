from glob import glob
from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension

sources = glob("src/*.cc") + glob("pyds/*.cc")
ext_modules = [Pybind11Extension("pyds.ds", sources=sources, include_dirs=["include"])]

setup(ext_modules=ext_modules)
