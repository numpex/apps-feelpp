#!/usr/bin/env python3
"""Exercise the real distance executable on the same coarse cube in serial and MPI."""
import argparse
import json
import math
import os
import pathlib
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--executable", type=pathlib.Path, required=True)
    parser.add_argument("--mesh", type=pathlib.Path, required=True)
    parser.add_argument("--mpiexec", required=True)
    parser.add_argument("--numproc-flag", default="-n")
    args = parser.parse_args()
    executable, mesh = args.executable.resolve(), args.mesh.resolve()
    environment = dict(os.environ, OMP_NUM_THREADS="1")
    with tempfile.TemporaryDirectory(prefix="distance-bvh-", dir=".") as folder:
        root = pathlib.Path(folder).resolve()

        def run(name, ranks, rays, valid=True):
            directory = root / name
            directory.mkdir()
            command = ([args.mpiexec, args.numproc_flag, str(ranks)] if ranks > 1 else [])
            command += [str(executable), "--gmsh.filename", str(mesh), "--M", str(rays),
                        "--h", "0.4", "--directory", str(directory),
                        "--repository.append.np", "0", "--exporter.export", "0"]
            result = subprocess.run(command, cwd=directory, env=environment,
                                    text=True, capture_output=True, timeout=90)
            output = result.stdout + result.stderr
            if valid and result.returncode:
                raise AssertionError(f"{name} failed ({result.returncode}):\n{output[-12000:]}")
            if not valid:
                if result.returncode == 0 or "M must be a positive number of rays" not in output:
                    raise AssertionError(
                        f"{name} did not reject invalid ray count (exit {result.returncode}):\n"
                        f"{output[-12000:]}")
                print(f"{name}: rejected M={rays}")
                return
            files = list(directory.rglob("measures.json"))
            if len(files) != 1:
                raise AssertionError(f"{name}: expected one measures.json, got {files}\n{output[-12000:]}")
            values = json.loads(files[0].read_text())
            for key in ("L2_error_FMM", "L2_error_BVH"):
                error = values["error"][key]
                if not math.isfinite(error) or not 0 <= error < 0.25:
                    raise AssertionError(f"{name}: invalid unit-cube error {key}={error}")
            print(f"{name}: {json.dumps(values['error'], sort_keys=True)}")
            return values["error"]

        serial = run("serial", 1, 32)
        parallel = run("mpi-2", 2, 32)
        for key in serial:
            if not math.isclose(serial[key], parallel[key], rel_tol=1e-5, abs_tol=1e-7):
                raise AssertionError(f"Serial/MPI mismatch for {key}: {serial[key]} vs {parallel[key]}")
        for ranks in (1, 2):
            for rays in (0, -1):
                run(f"invalid-{ranks}-{rays}", ranks, rays, valid=False)
    print("Distance/BVH serial, MPI, and invalid-ray regression checks passed")


if __name__ == "__main__":
    main()
