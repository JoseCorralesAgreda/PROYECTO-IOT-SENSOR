import sys
from pathlib import Path

Import("env")

if sys.platform == "win32":
    env.Replace(BUILD_SCRIPT=str(Path(env.subst("$PROJECT_DIR")) / "scripts/native_msvc.py"))
else:
    env.Append(CXXFLAGS=["-std=c++17"])
