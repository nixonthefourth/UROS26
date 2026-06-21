from pathlib import Path
import sys


REPO_ROOT = Path(__file__).resolve().parents[1]
BUILD_DIR = REPO_ROOT / "build"

if BUILD_DIR.exists():
    sys.path.insert(0, str(BUILD_DIR))

from uros26 import *

