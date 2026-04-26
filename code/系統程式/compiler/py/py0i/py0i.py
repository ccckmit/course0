sys = __import__("sys")


def main():
    if len(sys.argv) < 2:
        print("Usage: py0i.py <script.py> [args...]")
        return
    run_path(sys.argv[1])


if __name__ == "__main__":
    main()
