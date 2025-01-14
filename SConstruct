#!/usr/bin/env python
import os
import subprocess
from glob import glob
from pathlib import Path

# TODO: Do not copy environment after godot-cpp/test is updated <https://github.com/godotengine/godot-cpp/blob/master/test/SConstruct>.
env = SConscript("godot-cpp/SConstruct")
env.Replace(CC = 'gcc', 
            CXX = 'g++', 
            LINK = 'g++', 
            LEX = 'flex', 
            YACC = 'bison')

# Add source files.
env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp")

# Add chuck files
env.Append(CPPPATH=["chuck/src/core/"])

source_excludes = []
use_pregenerated_yacc_header = False

if env["platform"] == "windows":
    # Exclude server_thread.c to get around 'No threading implementation available' compile error on Windows. Unsure for other platforms.
    # For some reason server_thread.c isn't included in the Visual Studio solution, so the error doesn't happen when building Chuck directly.
    source_excludes.append("chuck/src/core/lo/server_thread.c")

    # See src\core\chuck.lex line 54 to determine which platforms use the yacc header. TODO handle others like Android/Emscripten
    use_pregenerated_yacc_header = True

if not use_pregenerated_yacc_header:
    source_excludes.append("chuck/src/core/chuck_yacc.c")

sources += Glob("chuck/src/core/*.cpp", exclude=source_excludes)
sources += Glob("chuck/src/core/*.c", exclude=source_excludes)
sources += Glob("chuck/src/core/lo/*.c", exclude=source_excludes)

if env["platform"] == "windows":
    # Maybe some of these can be trimmed down if relying on Godot audio APIs instead of Windows?
    env.Append(LIBS=["winmm","dsound","dinput8","dxguid","wsock32","ws2_32","iphlpapi","user32","advapi32"])
    
    # Chuck expects this custom define for Windows, TODO bring over all necessary defines.
    env.Append(CPPDEFINES=["WIN32"])

if env["platform"] == "macos":
    env.Append(CXXFLAGS=['-fexceptions'])


if not use_pregenerated_yacc_header:

    build_dir = 'build'
    # Ensure the build directory exists
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    # Function to run bison
    def run_bison(target, source, env):
        subprocess.run(['bison', '-dv', '-b', os.path.join(build_dir, 'chuck'), 'chuck/src/core/chuck.y'], check=True)

    # Function to run flex
    def run_flex(target, source, env):
        subprocess.run(['flex', '-o', os.path.join(build_dir, 'chuck.yy.c'), 'chuck/src/core/chuck.lex'], check=True)

    # Add custom builders for bison and flex
    bison_builder = Builder(action=run_bison)
    flex_builder = Builder(action=run_flex)

    env.Append(BUILDERS={'Bison': bison_builder, 'Flex': flex_builder})

    # Generate chuck.tab.h and chuck.tab.c using bison
    bison_output = env.Bison(target=[os.path.join(build_dir, 'chuck.tab.c'), os.path.join(build_dir, 'chuck.tab.h')], 
                             source='chuck/src/core/chuck.y')

    # Generate chuck.yy.c using flex
    flex_output = env.Flex(target=os.path.join(build_dir, 'chuck.yy.c'), source='chuck/src/core/chuck.lex')
    
    env.Depends(flex_output, bison_output)

    # Add the generated files to the sources list
    sources += [os.path.join(build_dir, 'chuck.tab.c'), os.path.join(build_dir, 'chuck.yy.c')]
    
    # Add the directory containing chuck.tab.h to the include path
    env.Append(CPPPATH=['.'])

# Find gdextension path even if the directory or extension is renamed (e.g. project/addons/example/example.gdextension).
(extension_path,) = glob("project/addons/*/*.gdextension")

# Get the addon path (e.g. project/addons/example).
addon_path = Path(extension_path).parent

# Get the project name from the gdextension file (e.g. example).
project_name = Path(extension_path).stem

scons_cache_path = os.environ.get("SCONS_CACHE")
if scons_cache_path != None:
    CacheDir(scons_cache_path)
    print("Scons cache enabled... (path: '" + scons_cache_path + "')")

# Create the library target (e.g. libexample.linux.debug.x86_64.so).
debug_or_release = "release" if env["target"] == "template_release" else "debug"
if env["platform"] == "macos":
    library = env.SharedLibrary(
        "{0}/bin/lib{1}.{2}.{3}.framework/{1}.{2}.{3}".format(
            addon_path,
            project_name,
            env["platform"],
            debug_or_release,
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "{}/bin/lib{}.{}.{}.{}{}".format(
            addon_path,
            project_name,
            env["platform"],
            debug_or_release,
            env["arch"],
            env["SHLIBSUFFIX"],
        ),
        source=sources,
    )

Default(library)
