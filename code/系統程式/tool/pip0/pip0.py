#!/usr/bin/env python3
"""
pip0.py - A pip-like package manager written in pure Python.
No dependency on pip itself.

Usage:
    python pip0.py install <package> [<package> ...]
    python pip0.py install -r requirements.txt
    python pip0.py uninstall <package> [<package> ...]
    python pip0.py list [--outdated]
    python pip0.py show <package>
    python pip0.py search <query>
    python pip0.py freeze
    python pip0.py download <package> [-d <dir>]
"""

import sys
import os
import re
import json
import shutil
import zipfile
import tarfile
import tempfile
import argparse
import hashlib
import email.parser
import compileall
import sysconfig
import urllib.request
import urllib.error
import urllib.parse
from pathlib import Path
from typing import Optional

# ─────────────────────────────────────────────
# Constants
# ─────────────────────────────────────────────
PYPI_JSON_URL   = "https://pypi.org/pypi/{package}/json"
PYPI_VER_URL    = "https://pypi.org/pypi/{package}/{version}/json"
PYPI_SIMPLE_URL = "https://pypi.org/simple/{package}/"
DEFAULT_TIMEOUT = 30

# Where packages are installed (same as current Python's site-packages)
SITE_PACKAGES   = Path(sysconfig.get_paths()["purelib"])
SCRIPTS_DIR     = Path(sysconfig.get_paths()["scripts"])


# ─────────────────────────────────────────────
# Helpers
# ─────────────────────────────────────────────

def log(msg: str, color: str = ""):
    codes = {"green": "\033[92m", "red": "\033[91m",
             "yellow": "\033[93m", "cyan": "\033[96m",
             "bold": "\033[1m", "": ""}
    reset = "\033[0m" if color else ""
    print(f"{codes[color]}{msg}{reset}")


def fetch_json(url: str) -> dict:
    try:
        req = urllib.request.Request(url, headers={"User-Agent": "pip0/1.0"})
        with urllib.request.urlopen(req, timeout=DEFAULT_TIMEOUT) as resp:
            return json.loads(resp.read().decode())
    except urllib.error.HTTPError as e:
        raise RuntimeError(f"HTTP {e.code}: {url}") from e
    except Exception as e:
        raise RuntimeError(f"Network error: {e}") from e


def download_file(url: str, dest: Path, label: str = "") -> Path:
    req = urllib.request.Request(url, headers={"User-Agent": "pip0/1.0"})
    with urllib.request.urlopen(req, timeout=DEFAULT_TIMEOUT) as resp:
        total = int(resp.headers.get("Content-Length", 0))
        done  = 0
        chunk = 65536
        with open(dest, "wb") as f:
            while True:
                block = resp.read(chunk)
                if not block:
                    break
                f.write(block)
                done += len(block)
                if total:
                    pct = done * 100 // total
                    bar = "#" * (pct // 5) + "-" * (20 - pct // 5)
                    print(f"\r  Downloading {label} [{bar}] {pct}%", end="", flush=True)
    print()
    return dest


def verify_hash(path: Path, algo: str, expected: str) -> bool:
    h = hashlib.new(algo)
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(65536), b""):
            h.update(chunk)
    return h.hexdigest() == expected


def normalize_name(name: str) -> str:
    return re.sub(r"[-_.]+", "-", name).lower()


# ─────────────────────────────────────────────
# Installed-package registry (dist-info / egg-info)
# ─────────────────────────────────────────────

def find_dist_info(package: str) -> Optional[Path]:
    norm = normalize_name(package)
    for d in SITE_PACKAGES.iterdir():
        if d.is_dir() and (d.suffix in (".dist-info", ".egg-info")):
            pkg_name = normalize_name(d.name.split("-")[0])
            if pkg_name == norm:
                return d
    return None


def installed_packages() -> list[dict]:
    pkgs = []
    for d in sorted(SITE_PACKAGES.iterdir()):
        if d.is_dir() and d.suffix in (".dist-info", ".egg-info"):
            meta = read_metadata(d)
            if meta:
                pkgs.append(meta)
    return pkgs


def read_metadata(dist_info: Path) -> Optional[dict]:
    for name in ("METADATA", "PKG-INFO"):
        meta_file = dist_info / name
        if meta_file.exists():
            return parse_metadata(meta_file)
    return None


def parse_metadata(path: Path) -> dict:
    text  = path.read_text(encoding="utf-8", errors="replace")
    parser = email.parser.HeaderParser()
    msg   = parser.parsestr(text)
    return {
        "name":    msg.get("Name", ""),
        "version": msg.get("Version", ""),
        "summary": msg.get("Summary", ""),
        "home":    msg.get("Home-page", msg.get("Project-URL", "")),
        "requires": msg.get_all("Requires-Dist") or [],
        "requires_python": msg.get("Requires-Python", ""),
    }


# ─────────────────────────────────────────────
# PyPI queries
# ─────────────────────────────────────────────

def pypi_info(package: str, version: str = "") -> dict:
    if version:
        url = PYPI_VER_URL.format(package=package, version=version)
    else:
        url = PYPI_JSON_URL.format(package=package)
    return fetch_json(url)


def choose_wheel_or_sdist(releases: list[dict]) -> Optional[dict]:
    """Prefer pure-Python wheel, then any wheel, then sdist."""
    wheels  = [r for r in releases if r["filename"].endswith(".whl")]
    sdists  = [r for r in releases if r["filename"].endswith((".tar.gz", ".zip"))]

    py_ver  = f"cp{sys.version_info.major}{sys.version_info.minor}"
    py_tag  = f"py{sys.version_info.major}"
    plat    = sysconfig.get_platform().replace("-", "_").replace(".", "_")

    def wheel_score(w: dict) -> int:
        fn = w["filename"]
        if "py3-none-any" in fn or f"{py_tag}-none-any" in fn:
            return 30
        if f"{py_ver}-none-any" in fn:
            return 25
        if "none-any" in fn:
            return 20
        if plat in fn and py_ver in fn:
            return 15
        if plat in fn:
            return 10
        return 1

    if wheels:
        return max(wheels, key=wheel_score)
    if sdists:
        return sdists[0]
    return None


# ─────────────────────────────────────────────
# Install
# ─────────────────────────────────────────────

def install_wheel(whl_path: Path):
    """Unpack a .whl file into site-packages."""
    with zipfile.ZipFile(whl_path, "r") as zf:
        for member in zf.infolist():
            target = SITE_PACKAGES / member.filename
            if member.filename.endswith("/"):
                target.mkdir(parents=True, exist_ok=True)
            else:
                target.parent.mkdir(parents=True, exist_ok=True)
                with zf.open(member) as src, open(target, "wb") as dst:
                    shutil.copyfileobj(src, dst)
    # Compile .py files
    compileall.compile_dir(str(SITE_PACKAGES), quiet=True, force=False)


def install_sdist(sdist_path: Path):
    """Unpack and install a source distribution (tar.gz / zip)."""
    with tempfile.TemporaryDirectory() as tmpdir:
        tmp = Path(tmpdir)
        if sdist_path.name.endswith(".tar.gz"):
            with tarfile.open(sdist_path, "r:gz") as tf:
                tf.extractall(tmp)
        else:
            with zipfile.ZipFile(sdist_path) as zf:
                zf.extractall(tmp)

        # Find extracted directory
        dirs = [d for d in tmp.iterdir() if d.is_dir()]
        if not dirs:
            raise RuntimeError("Empty sdist archive.")
        src_dir = dirs[0]

        # Try setup.py / pyproject.toml installs via subprocess
        setup_py = src_dir / "setup.py"
        if setup_py.exists():
            import subprocess
            result = subprocess.run(
                [sys.executable, "setup.py", "install",
                 f"--prefix={sysconfig.get_paths()['data']}"],
                cwd=src_dir, capture_output=True, text=True
            )
            if result.returncode != 0:
                raise RuntimeError(f"setup.py install failed:\n{result.stderr}")
        else:
            # Fallback: copy package dir manually
            for item in src_dir.iterdir():
                if item.is_dir() and (item / "__init__.py").exists():
                    dest = SITE_PACKAGES / item.name
                    if dest.exists():
                        shutil.rmtree(dest)
                    shutil.copytree(item, dest)


def install_package(package: str, version: str = "",
                    upgrade: bool = False, quiet: bool = False):
    name, ver_spec = package, version
    # Parse inline version like "requests==2.31.0" or "requests>=2"
    m = re.match(r"^([A-Za-z0-9_.\-]+)([>=<!~].+)?$", package)
    if m:
        name, ver_spec = m.group(1), (m.group(2) or version).strip("=")

    norm = normalize_name(name)

    # Check already installed
    dist = find_dist_info(name)
    if dist and not upgrade:
        meta = read_metadata(dist)
        log(f"Requirement already satisfied: {name} {meta['version']}", "yellow")
        return

    if not quiet:
        log(f"Collecting {name}{(' == ' + ver_spec) if ver_spec else ''}", "cyan")

    try:
        data = pypi_info(name, ver_spec)
    except RuntimeError as e:
        log(f"  ERROR: {e}", "red")
        return

    info     = data["info"]
    pkg_ver  = info["version"]
    releases = data["urls"] if not ver_spec else data.get("urls", [])

    release  = choose_wheel_or_sdist(releases)
    if not release:
        log(f"  ERROR: No suitable file found for {name} {pkg_ver}", "red")
        return

    url      = release["url"]
    filename = release["filename"]
    digests  = release.get("digests", {})

    with tempfile.TemporaryDirectory() as tmpdir:
        dest = Path(tmpdir) / filename
        if not quiet:
            log(f"  Downloading {filename}")
        try:
            download_file(url, dest, filename)
        except Exception as e:
            log(f"  ERROR downloading: {e}", "red")
            return

        # Verify hash
        if digests.get("sha256"):
            if not verify_hash(dest, "sha256", digests["sha256"]):
                log("  ERROR: SHA256 mismatch! Aborting.", "red")
                return

        if not quiet:
            log(f"  Installing {filename}")
        try:
            if filename.endswith(".whl"):
                install_wheel(dest)
            else:
                install_sdist(dest)
        except Exception as e:
            log(f"  ERROR installing: {e}", "red")
            return

    log(f"Successfully installed {info['name']}-{pkg_ver}", "green")


# ─────────────────────────────────────────────
# Uninstall
# ─────────────────────────────────────────────

def uninstall_package(package: str, yes: bool = False):
    dist = find_dist_info(package)
    if not dist:
        log(f"WARNING: Package '{package}' is not installed.", "yellow")
        return

    meta = read_metadata(dist)
    ver  = meta["version"] if meta else "?"

    # Collect files to remove via RECORD
    record = dist / "RECORD"
    files_to_remove = []
    if record.exists():
        for line in record.read_text().splitlines():
            rel = line.split(",")[0]
            p   = SITE_PACKAGES / rel
            if p.exists():
                files_to_remove.append(p)

    if not yes:
        log(f"  Found: {meta['name']}-{ver}", "cyan")
        log(f"  Files to remove: {len(files_to_remove) or 'dist-info dir'}")
        answer = input("Proceed? [y/N] ").strip().lower()
        if answer != "y":
            log("Aborted.", "yellow")
            return

    # Remove
    if files_to_remove:
        removed_dirs = set()
        for p in files_to_remove:
            try:
                if p.is_file() or p.is_symlink():
                    p.unlink()
                    removed_dirs.add(p.parent)
            except OSError:
                pass
        # Remove empty dirs
        for d in sorted(removed_dirs, reverse=True):
            try:
                if d.exists() and not any(d.iterdir()):
                    d.rmdir()
            except OSError:
                pass

    # Always remove dist-info
    if dist.exists():
        shutil.rmtree(dist)

    log(f"Successfully uninstalled {meta['name']}-{ver}", "green")


# ─────────────────────────────────────────────
# List
# ─────────────────────────────────────────────

def cmd_list(outdated: bool = False):
    pkgs = installed_packages()
    if not pkgs:
        log("No packages installed.", "yellow")
        return

    if outdated:
        log("Checking for updates...", "cyan")

    col_w = max((len(p["name"]) for p in pkgs), default=10) + 2
    header = f"{'Package':<{col_w}} {'Version':<15}"
    if outdated:
        header += f" {'Latest':<15} {'Status'}"
    log(header, "bold")
    log("-" * len(header))

    for p in pkgs:
        row = f"{p['name']:<{col_w}} {p['version']:<15}"
        if outdated:
            try:
                info   = pypi_info(p["name"])["info"]
                latest = info["version"]
                status = "OUTDATED" if latest != p["version"] else "up-to-date"
                color  = "yellow" if status == "OUTDATED" else ""
                row   += f" {latest:<15} {status}"
                log(row, color)
            except Exception:
                log(row + " (check failed)", "yellow")
        else:
            log(row)


# ─────────────────────────────────────────────
# Show
# ─────────────────────────────────────────────

def cmd_show(packages: list[str]):
    for pkg in packages:
        dist = find_dist_info(pkg)
        if not dist:
            log(f"WARNING: Package '{pkg}' not installed.", "yellow")
            continue
        meta = read_metadata(dist)
        if not meta:
            log(f"WARNING: Could not read metadata for '{pkg}'.", "yellow")
            continue
        print(f"Name:             {meta['name']}")
        print(f"Version:          {meta['version']}")
        print(f"Summary:          {meta['summary']}")
        print(f"Home-page:        {meta['home']}")
        print(f"Requires-Python:  {meta['requires_python']}")
        print(f"Requires:         {', '.join(meta['requires']) or 'None'}")
        print(f"Location:         {SITE_PACKAGES}")
        print()


# ─────────────────────────────────────────────
# Freeze
# ─────────────────────────────────────────────

def cmd_freeze():
    for p in installed_packages():
        print(f"{p['name']}=={p['version']}")


# ─────────────────────────────────────────────
# Search (PyPI simple search via JSON API)
# ─────────────────────────────────────────────

def cmd_search(query: str):
    log(f"Searching PyPI for '{query}'...", "cyan")
    # PyPI removed the XML-RPC search; use the JSON API with search endpoint
    url = f"https://pypi.org/search/?q={urllib.parse.quote(query)}&o=-zscore&c=&format=json"
    try:
        req = urllib.request.Request(url, headers={
            "User-Agent": "pip0/1.0",
            "Accept": "application/json"
        })
        with urllib.request.urlopen(req, timeout=DEFAULT_TIMEOUT) as resp:
            data = json.loads(resp.read().decode())
    except Exception:
        # Fallback: try pypi.org/search HTML scrape
        _search_html_fallback(query)
        return

    results = data.get("results", [])
    if not results:
        log("No results found.", "yellow")
        return

    col = max((len(r.get("name", "")) for r in results), default=10) + 2
    log(f"{'Name':<{col}} {'Version':<12} {'Description'}", "bold")
    log("-" * 72)
    for r in results[:20]:
        name  = r.get("name", "")
        ver   = r.get("version", "")
        desc  = r.get("summary") or r.get("description", "")
        desc  = (desc[:50] + "…") if len(desc) > 51 else desc
        print(f"{name:<{col}} {ver:<12} {desc}")


def _search_html_fallback(query: str):
    """Parse PyPI search HTML as a fallback."""
    url = f"https://pypi.org/search/?q={urllib.parse.quote(query)}"
    try:
        req = urllib.request.Request(url, headers={"User-Agent": "pip0/1.0"})
        with urllib.request.urlopen(req, timeout=DEFAULT_TIMEOUT) as resp:
            html = resp.read().decode("utf-8", errors="replace")
    except Exception as e:
        log(f"Search failed: {e}", "red")
        return

    names    = re.findall(r'class="package-snippet__name"[^>]*>([^<]+)<', html)
    versions = re.findall(r'class="package-snippet__version"[^>]*>([^<]+)<', html)
    descs    = re.findall(r'class="package-snippet__description"[^>]*>([^<]*)<', html)

    if not names:
        log("No results found.", "yellow")
        return

    col = max(len(n) for n in names) + 2
    log(f"{'Name':<{col}} {'Version':<12} {'Description'}", "bold")
    log("-" * 72)
    for n, v, d in zip(names, versions,
                       descs + [""] * len(names)):
        n = n.strip(); v = v.strip(); d = d.strip()
        d = (d[:50] + "…") if len(d) > 51 else d
        print(f"{n:<{col}} {v:<12} {d}")


# ─────────────────────────────────────────────
# Download (save .whl or sdist without installing)
# ─────────────────────────────────────────────

def cmd_download(packages: list[str], dest_dir: str = "."):
    dest = Path(dest_dir)
    dest.mkdir(parents=True, exist_ok=True)
    for pkg in packages:
        name, ver_spec = pkg, ""
        m = re.match(r"^([A-Za-z0-9_.\-]+)==(.+)$", pkg)
        if m:
            name, ver_spec = m.group(1), m.group(2)
        try:
            data    = pypi_info(name, ver_spec)
            info    = data["info"]
            release = choose_wheel_or_sdist(data["urls"])
            if not release:
                log(f"  No file found for {name}", "red")
                continue
            out = dest / release["filename"]
            log(f"Downloading {release['filename']} → {dest}/", "cyan")
            download_file(release["url"], out, release["filename"])
            log(f"  Saved: {out}", "green")
        except Exception as e:
            log(f"  ERROR: {e}", "red")


# ─────────────────────────────────────────────
# requirements.txt
# ─────────────────────────────────────────────

def parse_requirements(path: str) -> list[str]:
    reqs = []
    with open(path, encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if line and not line.startswith("#") and not line.startswith("-"):
                reqs.append(line)
    return reqs


# ─────────────────────────────────────────────
# CLI
# ─────────────────────────────────────────────

def build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        prog="pip0",
        description="pip0 – a pip-like package manager (no pip dependency)",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__
    )
    sub = p.add_subparsers(dest="command", metavar="<command>")

    # install
    pi = sub.add_parser("install", help="Install packages")
    pi.add_argument("packages", nargs="*", metavar="package")
    pi.add_argument("-r", "--requirement", metavar="FILE",
                    help="Install from requirements file")
    pi.add_argument("-U", "--upgrade", action="store_true",
                    help="Upgrade if already installed")
    pi.add_argument("-q", "--quiet", action="store_true")

    # uninstall
    pu = sub.add_parser("uninstall", help="Uninstall packages")
    pu.add_argument("packages", nargs="+", metavar="package")
    pu.add_argument("-y", "--yes", action="store_true",
                    help="Skip confirmation prompt")

    # list
    pl = sub.add_parser("list", help="List installed packages")
    pl.add_argument("--outdated", action="store_true",
                    help="Show outdated packages")

    # show
    ps = sub.add_parser("show", help="Show package info")
    ps.add_argument("packages", nargs="+", metavar="package")

    # freeze
    sub.add_parser("freeze", help="Output installed packages in requirements format")

    # search
    psr = sub.add_parser("search", help="Search PyPI")
    psr.add_argument("query", help="Search query")

    # download
    pd = sub.add_parser("download", help="Download packages without installing")
    pd.add_argument("packages", nargs="+", metavar="package")
    pd.add_argument("-d", "--dest", default=".", metavar="DIR",
                    help="Download directory (default: .)")

    return p


def main():
    parser = build_parser()
    args   = parser.parse_args()

    if not args.command:
        parser.print_help()
        sys.exit(0)

    if args.command == "install":
        pkgs = list(args.packages)
        if args.requirement:
            pkgs += parse_requirements(args.requirement)
        if not pkgs:
            log("ERROR: No packages specified.", "red")
            sys.exit(1)
        for pkg in pkgs:
            install_package(pkg, upgrade=args.upgrade, quiet=args.quiet)

    elif args.command == "uninstall":
        for pkg in args.packages:
            uninstall_package(pkg, yes=args.yes)

    elif args.command == "list":
        cmd_list(outdated=args.outdated)

    elif args.command == "show":
        cmd_show(args.packages)

    elif args.command == "freeze":
        cmd_freeze()

    elif args.command == "search":
        cmd_search(args.query)

    elif args.command == "download":
        cmd_download(args.packages, dest_dir=args.dest)


if __name__ == "__main__":
    main()