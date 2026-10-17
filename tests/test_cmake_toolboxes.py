"""Configure the real project against minimal SDK fixtures, without compiling Feel++."""
import pathlib
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
SDK = r"""
set(Feel++_FOUND TRUE)
set(FEELPP_FOUND TRUE)
set(FEELPP_TOOLBOXES_FOUND ${TEST_HAS_TOOLBOXES})
if("Toolboxes" IN_LIST Feel++_FIND_COMPONENTS AND NOT TEST_HAS_TOOLBOXES)
  set(Feel++_FOUND FALSE)
  set(Feel++_NOT_FOUND_MESSAGE "Toolboxes are not installed in this core-only SDK")
  return()
endif()
if(NOT TARGET Feelpp::feelpp)
  add_library(Feelpp::feelpp INTERFACE IMPORTED GLOBAL)
endif()
function(feelpp_add_application name)
  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${name}-stub.cpp" "int main() { return 0; }\n")
  add_executable(feelpp_app_${name} "${CMAKE_CURRENT_BINARY_DIR}/${name}-stub.cpp")
endfunction()
"""


class ToolboxesConfigurationTests(unittest.TestCase):
    def configure(self, required, available, in_tree=False):
        with tempfile.TemporaryDirectory() as folder:
            root = pathlib.Path(folder)
            (root / "Feel++Config.cmake").write_text(SDK)
            (root / "pybind11Config.cmake").write_text("set(pybind11_FOUND TRUE)\n")
            (root / "FindPython.cmake").write_text(
                "set(Python_FOUND TRUE)\nset(Python_EXECUTABLE python3)\n")
            source = ROOT
            if in_tree:
                source = root / "parent"
                source.mkdir()
                (source / "CMakeLists.txt").write_text(
                    'cmake_minimum_required(VERSION 3.21)\nproject(parent)\n'
                    f'include("{root}/Feel++Config.cmake")\n'
                    f'add_subdirectory("{ROOT}" apps-feelpp)\n')
            command = ["cmake", "-S", str(source), "-B", str(root / "build"),
                       f"-DCMAKE_MODULE_PATH={root}", f"-Dpybind11_DIR={root}",
                       f"-DFeel++_DIR={root}", "-DFEELPP_PROJECT_ENABLE_TESTS=OFF",
                       "-DTEST_HAS_TOOLBOXES=" + ("ON" if available else "OFF")]
            if required is not None:
                command += ["-DFEELPP_PROJECT_REQUIRE_TOOLBOXES=" + ("ON" if required else "OFF")]
            return subprocess.run(command, capture_output=True, text=True, timeout=90)

    def test_core_only_sdk_with_opt_out(self):
        result = self.configure(False, False)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_default_requires_toolboxes(self):
        result = self.configure(None, False)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("Toolboxes", result.stdout + result.stderr)

    def test_default_with_toolboxes(self):
        result = self.configure(None, True)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_in_tree_core_only_with_opt_out(self):
        result = self.configure(False, False, in_tree=True)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_in_tree_default_requires_toolboxes(self):
        result = self.configure(None, False, in_tree=True)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("Toolboxes SDK is required", result.stdout + result.stderr)

    def test_in_tree_default_with_toolboxes(self):
        result = self.configure(None, True, in_tree=True)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


if __name__ == "__main__":
    unittest.main()
