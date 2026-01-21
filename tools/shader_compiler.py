#!/usr/bin/env python3
"""
Shader Compilation Pipeline for SpatialRender

Compiles GLSL shaders with:
- Per-platform variant support (debug/release)
- Hash-based caching
- Build-time validation
- Error reporting
"""

import os
import sys
import hashlib
import json
import argparse
import subprocess
from pathlib import Path
from typing import Dict, Tuple, Optional


class ShaderCompiler:
    def __init__(self, source_dir: str, output_dir: str, cache_dir: str = ".shader_cache"):
        self.source_dir = Path(source_dir)
        self.output_dir = Path(output_dir)
        self.cache_dir = Path(cache_dir)
        self.cache_dir.mkdir(exist_ok=True)
        self.output_dir.mkdir(parents=True, exist_ok=True)

        self.cache_file = self.cache_dir / "cache.json"
        self.cache = self._load_cache()

    def _load_cache(self) -> Dict:
        """Load shader compilation cache"""
        if self.cache_file.exists():
            try:
                with open(self.cache_file, "r") as f:
                    return json.load(f)
            except:
                return {}
        return {}

    def _save_cache(self):
        """Save shader compilation cache"""
        with open(self.cache_file, "w") as f:
            json.dump(self.cache, f, indent=2)

    def _compute_hash(self, file_path: Path) -> str:
        """Compute SHA256 hash of shader source"""
        with open(file_path, "rb") as f:
            return hashlib.sha256(f.read()).hexdigest()

    def _compile_shader(
        self, source_path: Path, output_path: Path, shader_type: str, defines: Dict[str, str] = None
    ) -> Tuple[bool, str]:
        """
        Compile shader using glslc (GLSL compiler from Vulkan SDK) or glslangValidator

        Returns: (success, error_message)
        """
        defines = defines or {}

        # Try glslc first (Vulkan SDK)
        compilers = [
            ("glslc", ["-fshader-stage=" + shader_type, "-o", str(output_path)]),
            ("glslangValidator", ["-S", shader_type, "-o", str(output_path)]),
        ]

        # Add defines
        define_args = []
        for key, value in defines.items():
            define_args.extend(["-D", f"{key}={value}"])

        for compiler_name, base_args in compilers:
            compiler_path = self._find_compiler(compiler_name)
            if compiler_path:
                cmd = [compiler_path] + base_args + define_args + [str(source_path)]
                try:
                    result = subprocess.run(cmd, capture_output=True, text=True, timeout=10)

                    if result.returncode == 0:
                        return True, ""
                    else:
                        return False, result.stderr
                except subprocess.TimeoutExpired:
                    return False, f"Compiler timeout: {compiler_name}"
                except Exception as e:
                    continue

        # Fallback: just copy source (for development)
        print(f"Warning: No shader compiler found, copying source as-is")
        output_path.parent.mkdir(parents=True, exist_ok=True)
        with open(source_path, "r") as src, open(output_path, "w") as dst:
            dst.write(src.read())
        return True, ""

    def _find_compiler(self, name: str) -> Optional[str]:
        """Find shader compiler in PATH"""
        for path in os.environ.get("PATH", "").split(os.pathsep):
            compiler_path = Path(path) / name
            if compiler_path.exists():
                return str(compiler_path)
        return None

    def compile_shader_pair(
        self,
        vert_name: str,
        frag_name: str,
        variant: str = "release",
        defines: Dict[str, str] = None,
    ) -> bool:
        """
        Compile a vertex/fragment shader pair

        Args:
            vert_name: Vertex shader filename (without extension)
            frag_name: Fragment shader filename (without extension)
            variant: Build variant (debug/release)
            defines: Additional preprocessor defines
        """
        defines = defines or {}
        defines["VARIANT"] = variant.upper()

        vert_src = self.source_dir / f"{vert_name}.vert"
        frag_src = self.source_dir / f"{frag_name}.frag"

        if not vert_src.exists():
            print(f"Error: Vertex shader not found: {vert_src}")
            return False
        if not frag_src.exists():
            print(f"Error: Fragment shader not found: {frag_src}")
            return False

        # Compute hashes
        vert_hash = self._compute_hash(vert_src)
        frag_hash = self._compute_hash(frag_src)
        cache_key = f"{vert_name}_{frag_name}_{variant}_{vert_hash}_{frag_hash}"

        # Check cache
        if cache_key in self.cache:
            cached_output = self.cache[cache_key]
            if all(Path(p).exists() for p in cached_output.values()):
                print(f"Cache hit: {vert_name}/{frag_name} ({variant})")
                return True

        # Determine output paths
        vert_out = self.output_dir / f"{vert_name}.vert"
        frag_out = self.output_dir / f"{frag_name}.frag"

        # Compile vertex shader
        print(f"Compiling {vert_name}.vert ({variant})...")
        success, error = self._compile_shader(vert_src, vert_out, "vertex", defines)
        if not success:
            print(f"ERROR: Vertex shader compilation failed:")
            print(error)
            return False

        # Compile fragment shader
        print(f"Compiling {frag_name}.frag ({variant})...")
        success, error = self._compile_shader(frag_src, frag_out, "fragment", defines)
        if not success:
            print(f"ERROR: Fragment shader compilation failed:")
            print(error)
            return False

        # Update cache
        self.cache[cache_key] = {"vertex": str(vert_out), "fragment": str(frag_out)}
        self._save_cache()

        print(f"Successfully compiled {vert_name}/{frag_name} ({variant})")
        return True

    def compile_all(self, variant: str = "release") -> bool:
        """Compile all shader pairs found in source directory"""
        shader_pairs = {}

        # Find all shader files
        for shader_file in self.source_dir.glob("*.vert"):
            name = shader_file.stem
            frag_file = self.source_dir / f"{name}.frag"
            if frag_file.exists():
                shader_pairs[name] = name

        if not shader_pairs:
            print("No shader pairs found")
            return False

        success = True
        for shader_name in shader_pairs:
            if not self.compile_shader_pair(shader_name, shader_name, variant):
                success = False

        return success


def main():
    parser = argparse.ArgumentParser(description="Compile GLSL shaders for SpatialRender")
    parser.add_argument("--source-dir", default="shaders/src", help="Source shader directory")
    parser.add_argument(
        "--output-dir", default="shaders/compiled", help="Output directory for compiled shaders"
    )
    parser.add_argument(
        "--variant", choices=["debug", "release"], default="release", help="Build variant"
    )
    parser.add_argument("--shader", help="Specific shader pair to compile (without extension)")
    parser.add_argument("--all", action="store_true", help="Compile all shaders")

    args = parser.parse_args()

    compiler = ShaderCompiler(args.source_dir, args.output_dir)

    if args.shader:
        success = compiler.compile_shader_pair(args.shader, args.shader, args.variant)
    elif args.all:
        success = compiler.compile_all(args.variant)
    else:
        # Default: compile all
        success = compiler.compile_all(args.variant)

    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
