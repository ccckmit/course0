import builtins
import runpy
import sys


def run_path(path):
    old_argv = sys.argv
    sys.argv = sys.argv[1:]
    try:
        runpy.run_path(path, run_name="__main__")
    finally:
        sys.argv = old_argv


def main():
    if len(sys.argv) < 2:
        print("Usage: python host.py <script.py> [args...]")
        raise SystemExit(1)

    builtins.run_path = run_path
    target = sys.argv[1]
    sys.argv = sys.argv[1:]
    runpy.run_path(target, run_name="__main__")


if __name__ == "__main__":
    main()
