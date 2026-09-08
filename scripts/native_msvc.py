from pathlib import Path

import SCons.Tool
from SCons.Script import AlwaysBuild, Default, DefaultEnvironment

env = DefaultEnvironment()
tool_directory = Path(env.subst("$PROJECT_DIR")) / ".pio/tools/scons/SCons/Tool"
if not (tool_directory / "MSCommon").is_dir():
    raise RuntimeError(
        "Instale SCons completo: python -m pip install --target .pio/tools/scons scons==4.11.1"
    )

# PlatformIO's trimmed SCons package omits the MSVC discovery modules.
SCons.Tool.__path__.append(str(tool_directory))
env.Replace(MSVC_VERSION="14.3", MSVC_TOOLSET_VERSION="14.44.35207", TARGET_ARCH="amd64")
for tool in ("msvc", "mslink", "mslib"):
    env.Tool(tool)
env.Append(CXXFLAGS=["/std:c++17", "/EHsc"])
target = env.BuildProgram()
AlwaysBuild(env.Alias("exec", target, env.VerboseAction("$SOURCE $PROGRAM_ARGS", "Executing $SOURCE")))
Default(target)
