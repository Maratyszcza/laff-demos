#!/usr/bin/env python

import os
import sys
import glob
import argparse
import ninja


class Configuration:

    def __init__(self, options, ninja_build_file=os.path.join(os.path.dirname(os.path.abspath(__file__)), "build.ninja")):
        self.output = open(ninja_build_file, "w")
        self.writer = ninja.Writer(self.output)
        self.source_dir = None
        self.build_dir = None
        self.artifact_dir = None
        self.include_dirs = []
        self.object_ext = ".bc"

        emflags = [
            "EXPORTED_FUNCTIONS=\"['_runBenchmark','_main','_malloc','_free']\"",
            "DISABLE_EXCEPTION_CATCHING=1",
            "TOTAL_MEMORY=83886080",
            "ALLOW_MEMORY_GROWTH=0",
            "FORCE_ALIGNED_MEMORY=1",
            "PRECISE_F32=0",
            "GC_SUPPORT=0",
            "NO_EXIT_RUNTIME=1"
        ]

        # Variables
        self.writer.variable("nacl_sdk_dir", options.nacl_sdk)
        self._set_pnacl_vars()
        self.writer.variable("emscripten_blis_dir", options.emscripten_blis)
        self.writer.variable("cflags", "-std=c99")
        self.writer.variable("optflags", "-O3")
        self.writer.variable("emflags", " ".join(map(lambda emflag: "-s " + emflag, emflags)))

        # Rules
        self.writer.rule("EMCC", "emcc -o $out -c $in -MMD -MF $out.d $optflags $cflags $includes",
            deps="gcc", depfile="$out.d",
            description="CC[Emscripten] $descpath")
        self.writer.rule("EMCCLD", "emcc -o $out $in $optflags $libs $libdirs $ldflags $emflags",
            description="CCLD[Emscripten] $descpath")
        self.writer.rule("PNACLCC", "$pnacl_cc -o $out -c $in -MMD -MF $out.d $optflags $cflags $includes",
            deps="gcc", depfile="$out.d",
            description="CC[PNaCl] $descpath")
        self.writer.rule("PNACLCCLD", "$pnacl_cc -o $out $in $libs $libdirs $ldflags",
            description="CCLD[PNaCl] $descpath")
        self.writer.rule("PNACLFINALIZE", "$pnacl_finalize $finflags -o $out $in",
            description="FINALIZE[PNaCl] $descpath")
        self.writer.rule("CONCATENATE", "cat $in > $out",
            description="CONCATENATE $out")
        self.writer.rule("UGLIFYJS", "uglifyjs $in > $out",
            description="UGLIFYJS $out")


    def _set_pnacl_vars(self):
        if sys.platform == "win32":
            self.writer.variable("pnacl_toolchain_dir", "$nacl_sdk_dir/toolchain/win_pnacl")
            self.writer.variable("pnacl_cc", "$pnacl_toolchain_dir/bin/pnacl-clang.bat")
            self.writer.variable("pnacl_finalize", "$pnacl_toolchain_dir/bin/pnacl-finalize.bat")
        elif sys.platform == "linux2" or sys.platform == "darwin":
            if sys.platform == "linux2":
                self.writer.variable("pnacl_toolchain_dir", "$nacl_sdk_dir/toolchain/linux_pnacl")
            else:
                self.writer.variable("pnacl_toolchain_dir", "$nacl_sdk_dir/toolchain/mac_pnacl")
            self.writer.variable("pnacl_cc", "$pnacl_toolchain_dir/bin/pnacl-clang")
            self.writer.variable("pnacl_finalize", "$pnacl_toolchain_dir/bin/pnacl-finalize")
        else:
            raise OSError("Unsupported platform: " + sys.platform)


    def _cc(self, rule, source_file, object_file):
        if not os.path.isabs(source_file):
            source_file = os.path.join(self.source_dir, source_file)
        if object_file is None:
            object_file = os.path.join(self.build_dir, os.path.relpath(source_file, self.source_dir)) + self.object_ext
        variables = {
            "descpath": os.path.relpath(source_file, self.source_dir)
        }
        if self.include_dirs:
            variables["includes"] = " ".join(["-I" + i for i in self.include_dirs])
        if rule == "EMCC":
            variables["cflags"] = "$cflags -Wno-warn-absolute-paths"
        self.writer.build(object_file, rule, source_file, variables=variables)
        return object_file


    def pnacl_cc(self, source_file, object_file=None):
        return self._cc("PNACLCC", source_file, object_file)


    def emcc(self, source_file, object_file=None):
        return self._cc("EMCC", source_file, object_file)


    def _ccld(self, rule, object_files, binary_file, binary_dir, lib_dirs, libs):
        if not os.path.isabs(binary_file):
            binary_file = os.path.join(binary_dir, binary_file)
        variables = {
            "descpath": os.path.relpath(binary_file, binary_dir)
        }
        if lib_dirs:
            variables["libdirs"] = " ".join(["-L" + l for l in lib_dirs])
        if libs:
            variables["libs"] = " ".join(["-l" + l for l in libs])
        if rule == "EMCCLD":
            variables["ldflags"] = "$ldflags -Wno-warn-absolute-paths"
        self.writer.build(binary_file, rule, object_files, variables=variables)
        return binary_file


    def pnacl_ccld(self, object_files, binary_file, lib_dirs=[], libs=[]):
        return self._ccld("PNACLCCLD", object_files, binary_file, self.build_dir, lib_dirs, libs)


    def emccld(self, object_files, binary_file, lib_dirs=[], libs=[]):
        return self._ccld("EMCCLD", object_files, binary_file, self.artifact_dir, lib_dirs, libs)


    def pnacl_finalize(self, binary_file, executable_file):
        if not os.path.isabs(binary_file):
            binary_file = os.path.join(self.build_dir, binary_file)
        if not os.path.isabs(executable_file):
            executable_file = os.path.join(self.artifact_dir, executable_file)
        variables = {
            "descpath": os.path.relpath(executable_file, self.artifact_dir)
        }
        self.writer.build(executable_file, "PNACLFINALIZE", binary_file, variables=variables)
        return executable_file


parser = argparse.ArgumentParser(description="LAFF demo configuration script")
parser.add_argument("--with-nacl-sdk", dest="nacl_sdk", default=os.getenv("NACL_SDK_ROOT"),
    help="Native Client (Pepper) SDK to use")
parser.add_argument("--with-emscripten-blis", dest="emscripten_blis", default=os.getenv("EMBLIS"),
    help="Emscripten-compiled BLIS library")


def main():
    options = parser.parse_args()

    config = Configuration(options)

    root_dir = os.path.dirname(os.path.abspath(__file__))


    # Build targets
    sources = map(lambda f: os.path.relpath(f, os.path.join(root_dir, "src")),
        glob.glob(os.path.join(root_dir, "src", "*.c")))
    nacl_sources = map(lambda f: os.path.relpath(f, os.path.join(root_dir, "src")),
        glob.glob(os.path.join(root_dir, "src", "nacl", "*.c")))
    emscripten_sources = map(lambda f: os.path.relpath(f, os.path.join(root_dir, "src")),
        glob.glob(os.path.join(root_dir, "src", "emscripten", "*.c")))

    # Build PNaCl objects
    config.source_dir = os.path.join(root_dir, "src")
    config.artifact_dir = os.path.join(root_dir, "artifacts")

    config.build_dir = os.path.join(root_dir, "build", "pnacl")
    config.include_dirs = [os.path.join("$nacl_sdk_dir", "include"), os.path.join(root_dir, "src")]
    pnacl_objects = []
    for source in sources + nacl_sources:
        pnacl_objects.append(config.pnacl_cc(source))
    pnacl_binary = config.pnacl_ccld(pnacl_objects, "demo.bc", libs=["ppapi", "blis", "m"])
    pnacl_binary = config.pnacl_finalize(pnacl_binary, "demo.pexe")

    # Build Emscripten objects
    config.build_dir = os.path.join(root_dir, "build", "emscripten")
    config.include_dirs = [os.path.join(root_dir, "src"), os.path.join(options.emscripten_blis, "include")]
    emscripten_objects = []
    for source in sources + emscripten_sources:
        emscripten_objects.append(config.emcc(source))
    config.emccld(emscripten_objects, "demo.js", lib_dirs=[os.path.join(options.emscripten_blis, "lib")], libs=["blis"])


if __name__ == "__main__":
    sys.exit(main())
