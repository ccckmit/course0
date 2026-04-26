#!/usr/bin/env python3
"""
make0 - A pure Python Makefile builder (no make required)
Parses Makefile syntax and executes recipes via shell.
"""

import sys
import os
import re
import subprocess
import argparse
from collections import defaultdict, OrderedDict
from typing import Optional


# ─────────────────────────────────────────────
# ANSI colors
# ─────────────────────────────────────────────
class C:
    RESET  = "\033[0m"
    BOLD   = "\033[1m"
    RED    = "\033[31m"
    GREEN  = "\033[32m"
    YELLOW = "\033[33m"
    CYAN   = "\033[36m"
    DIM    = "\033[2m"

def info(msg):   print(f"{C.CYAN}{C.BOLD}make0:{C.RESET} {msg}")
def ok(msg):     print(f"{C.GREEN}  ✔ {msg}{C.RESET}")
def warn(msg):   print(f"{C.YELLOW}  ⚠ {msg}{C.RESET}", file=sys.stderr)
def error(msg):  print(f"{C.RED}{C.BOLD}make0: *** {msg}{C.RESET}", file=sys.stderr)
def cmd_echo(c): print(f"{C.DIM}  $ {c}{C.RESET}")


# ─────────────────────────────────────────────
# Makefile parser
# ─────────────────────────────────────────────
class Makefile:
    def __init__(self, path: str = "Makefile"):
        self.path = path
        self.variables: dict[str, str] = {}
        self.rules: OrderedDict[str, dict] = OrderedDict()
        self.default_target: Optional[str] = None
        self._parse()

    # ------------------------------------------
    def _read_lines(self) -> list[str]:
        """Read file and handle line continuations."""
        with open(self.path, "r", encoding="utf-8") as f:
            raw = f.read()
        # Join backslash-continued lines (but not inside recipe lines)
        lines = []
        pending = ""
        for line in raw.splitlines():
            if line.endswith("\\") and not line.startswith("\t"):
                pending += line[:-1] + " "
            else:
                lines.append(pending + line)
                pending = ""
        if pending:
            lines.append(pending)
        return lines

    # ------------------------------------------
    def _expand(self, text: str) -> str:
        """Expand $(VAR) and ${VAR} references."""
        def replacer(m):
            name = m.group(1) or m.group(2)
            # Handle simple built-in functions (addprefix, addsuffix, wildcard, etc.)
            if name.startswith("wildcard "):
                import glob
                pattern = name[9:].strip()
                return " ".join(glob.glob(pattern))
            if name.startswith("shell "):
                cmd = name[6:].strip()
                result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
                return result.stdout.strip()
            if name.startswith("notdir "):
                words = name[7:].strip().split()
                return " ".join(os.path.basename(w) for w in words)
            if name.startswith("dir "):
                words = name[4:].strip().split()
                return " ".join(os.path.dirname(w) or "./" for w in words)
            if name.startswith("basename "):
                words = name[9:].strip().split()
                return " ".join(os.path.splitext(w)[0] for w in words)
            if name.startswith("suffix "):
                words = name[7:].strip().split()
                return " ".join(os.path.splitext(w)[1] for w in words)
            if name.startswith("subst "):
                parts = name[6:].split(",", 2)
                if len(parts) == 3:
                    return parts[2].replace(parts[0], parts[1])
            if name.startswith("patsubst "):
                parts = name[9:].split(",", 2)
                if len(parts) == 3:
                    pat, rep, text_ = parts
                    stem_pat = pat.replace("%", "(.*)")
                    def psub(w):
                        m2 = re.fullmatch(stem_pat, w)
                        if m2:
                            return rep.replace("%", m2.group(1))
                        return w
                    return " ".join(psub(w) for w in text_.split())
            if name.startswith("filter "):
                parts = name[7:].split(",", 1)
                if len(parts) == 2:
                    patterns, words_ = parts[0].split(), parts[1].split()
                    def matches(w):
                        for p in patterns:
                            rp = p.replace("%",".*")
                            if re.fullmatch(rp, w): return True
                        return False
                    return " ".join(w for w in words_ if matches(w))
            if name.startswith("strip "):
                return " ".join(name[6:].split())
            if name.startswith("word "):
                parts = name[5:].split(",", 1)
                if len(parts) == 2:
                    n, words_ = int(parts[0].strip()), parts[1].split()
                    return words_[n-1] if 0 < n <= len(words_) else ""
            if name.startswith("words "):
                return str(len(name[6:].split()))
            if name.startswith("firstword "):
                ws = name[10:].split()
                return ws[0] if ws else ""
            if name.startswith("lastword "):
                ws = name[9:].split()
                return ws[-1] if ws else ""
            # Plain variable
            return self.variables.get(name, os.environ.get(name, ""))
        pattern = re.compile(r'\$\(([^)]+)\)|\$\{([^}]+)\}')
        # Expand repeatedly until stable (handles nested expansions)
        for _ in range(10):
            new = pattern.sub(replacer, text)
            if new == text:
                break
            text = new
        return text

    # ------------------------------------------
    def _parse(self):
        if not os.path.exists(self.path):
            raise FileNotFoundError(f"No such file: '{self.path}'")

        lines = self._read_lines()
        current_target = None
        include_re = re.compile(r'^-?include\s+(.+)$')

        i = 0
        while i < len(lines):
            line = lines[i]
            i += 1

            # Skip blank & comments
            stripped = line.strip()
            if not stripped or stripped.startswith("#"):
                continue

            # include directive
            m = include_re.match(stripped)
            if m:
                for inc in self._expand(m.group(1)).split():
                    if os.path.exists(inc):
                        sub = Makefile(inc)
                        self.variables.update(sub.variables)
                        for t, r in sub.rules.items():
                            if t not in self.rules:
                                self.rules[t] = r
                            else:
                                self.rules[t]["recipe"].extend(r["recipe"])
                continue

            # Recipe line (starts with TAB)
            if line.startswith("\t") and current_target:
                self.rules[current_target]["recipe"].append(line[1:])
                continue

            # Variable assignment  VAR = / := / ?= / +=
            m = re.match(r'^([A-Za-z_][A-Za-z0-9_\.\-]*)\s*(\?=|:=|::=|!=|\+=|=)\s*(.*)$', stripped)
            if m:
                name, op, val = m.group(1), m.group(2), m.group(3).strip()
                if op == "?=":
                    if name not in self.variables:
                        self.variables[name] = self._expand(val)
                elif op in (":=", "::="):
                    self.variables[name] = self._expand(val)
                elif op == "!=":
                    result = subprocess.run(val, shell=True, capture_output=True, text=True)
                    self.variables[name] = result.stdout.strip()
                elif op == "+=":
                    existing = self.variables.get(name, "")
                    self.variables[name] = (existing + " " + self._expand(val)).strip()
                else:  # =  (recursive / lazy)
                    self.variables[name] = val  # expand lazily on use
                current_target = None
                continue

            # Rule line:  target(s) : prerequisites
            m = re.match(r'^([^:#=\t][^:=]*?)\s*::?\s*(.*)$', stripped)
            if m:
                targets_raw = m.group(1).strip()
                prereqs_raw = m.group(2).strip()
                # handle semicolon inline recipe
                inline_recipe = None
                if ";" in prereqs_raw:
                    prereqs_raw, inline_recipe = prereqs_raw.split(";", 1)

                targets = self._expand(targets_raw).split()
                prereqs = self._expand(prereqs_raw).split()

                for t in targets:
                    if t not in self.rules:
                        self.rules[t] = {"prereqs": prereqs, "recipe": []}
                    else:
                        self.rules[t]["prereqs"].extend(prereqs)
                    if inline_recipe:
                        self.rules[t]["recipe"].append(inline_recipe.strip())

                # track default (first non-special) target
                if self.default_target is None:
                    first = targets[0] if targets else None
                    if first and not first.startswith("."):
                        self.default_target = first

                current_target = targets[-1] if targets else None
                continue

            current_target = None  # unrecognised line resets context

    # ------------------------------------------
    def list_targets(self):
        return list(self.rules.keys())


# ─────────────────────────────────────────────
# Builder
# ─────────────────────────────────────────────
class Builder:
    PHONY_TARGETS = {".PHONY", ".SUFFIXES", ".DEFAULT", ".PRECIOUS",
                     ".INTERMEDIATE", ".SECONDARY", ".DELETE_ON_ERROR",
                     ".IGNORE", ".SILENT", ".EXPORT_ALL_VARIABLES"}

    def __init__(self, makefile: Makefile, dry_run=False, silent=False, jobs=1):
        self.mf = makefile
        self.dry_run = dry_run
        self.silent = silent
        self.jobs = jobs
        self.built: set[str] = set()
        self.failed: set[str] = set()
        # collect phony targets
        self.phony: set[str] = set()
        if ".PHONY" in makefile.rules:
            self.phony.update(makefile.rules[".PHONY"]["prereqs"])

    # ------------------------------------------
    def _needs_build(self, target: str, prereqs: list[str]) -> bool:
        if target in self.phony:
            return True
        if not os.path.exists(target):
            return True
        t_mtime = os.path.getmtime(target)
        for p in prereqs:
            if os.path.exists(p) and os.path.getmtime(p) > t_mtime:
                return True
        return False

    # ------------------------------------------
    def _expand_auto(self, recipe: str, target: str, prereqs: list[str]) -> str:
        """Expand automatic variables: $@, $<, $^, $?, $*, $+"""
        first_prereq = prereqs[0] if prereqs else ""
        newer = [p for p in prereqs if not os.path.exists(target) or
                 (os.path.exists(p) and os.path.getmtime(p) > os.path.getmtime(target))]
        stem = re.sub(r'\.[^.]+$', '', target)

        recipe = recipe.replace("$@", target)
        recipe = recipe.replace("$<", first_prereq)
        recipe = recipe.replace("$^", " ".join(dict.fromkeys(prereqs)))
        recipe = recipe.replace("$+", " ".join(prereqs))
        recipe = recipe.replace("$?", " ".join(newer))
        recipe = recipe.replace("$*", stem)
        recipe = recipe.replace("$$", "$")
        return self.mf._expand(recipe)

    # ------------------------------------------
    def build(self, target: str, depth: int = 0) -> bool:
        if target in self.built:
            return True
        if target in self.failed:
            return False
        if target in self.PHONY_TARGETS:
            return True

        rule = self.mf.rules.get(target)

        if rule is None:
            # Maybe it's a file that already exists
            if os.path.exists(target):
                return True
            error(f"No rule to make target '{target}'")
            self.failed.add(target)
            return False

        prereqs = rule["prereqs"]

        # Build prerequisites first
        for p in prereqs:
            if p in self.PHONY_TARGETS:
                continue
            if not self.build(p, depth + 1):
                self.failed.add(target)
                return False

        if not self._needs_build(target, prereqs):
            ok(f"'{target}' is up to date.")  # 移除 depth == 0 的限制
            self.built.add(target)
            return True

        # Execute recipe
        recipe = rule["recipe"]
        if not recipe:
            self.built.add(target)
            return True

        prefix = "  " * depth
        info(f"{prefix}Building target: {C.BOLD}{target}{C.RESET}")

        for raw_cmd in recipe:
            # Flags: @ = silent, - = ignore errors, + = always execute
            cmd = raw_cmd
            silent_cmd = self.silent
            ignore_error = False

            while cmd and cmd[0] in "@-+":
                flag = cmd[0]
                cmd = cmd[1:]
                if flag == "@": silent_cmd = True
                if flag == "-": ignore_error = True

            cmd = self._expand_auto(cmd.strip(), target, prereqs)
            if not cmd:
                continue

            if not silent_cmd:
                cmd_echo(cmd)

            if self.dry_run:
                continue

            ret = subprocess.run(cmd, shell=True, executable="/bin/sh")
            if ret.returncode != 0:
                if ignore_error:
                    warn(f"Ignored error (exit {ret.returncode}) in: {cmd}")
                else:
                    error(f"Recipe for target '{target}' failed (exit {ret.returncode})")
                    self.failed.add(target)
                    return False

        self.built.add(target)
        return True


# ─────────────────────────────────────────────
# CLI
# ─────────────────────────────────────────────
def main():
    parser = argparse.ArgumentParser(
        prog="make0",
        description="make0 — a pure-Python Makefile builder (no make required)",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  make0                        # build default target
  make0 all clean              # build multiple targets
  make0 -f build.mk install    # use a different Makefile
  make0 -n all                 # dry-run: print commands only
  make0 -p                     # print parsed variables & rules
  make0 -l                     # list all targets
""")
    parser.add_argument("targets", nargs="*", help="Target(s) to build")
    parser.add_argument("-f", "--file", default="Makefile", metavar="FILE",
                        help="Specify Makefile (default: Makefile)")
    parser.add_argument("-n", "--dry-run", action="store_true",
                        help="Print commands without executing")
    parser.add_argument("-s", "--silent", action="store_true",
                        help="Do not print recipe commands")
    parser.add_argument("-p", "--print-db", action="store_true",
                        help="Print the variable/rule database and exit")
    parser.add_argument("-l", "--list", action="store_true",
                        help="List all targets and exit")
    parser.add_argument("-C", "--directory", metavar="DIR",
                        help="Change to DIR before doing anything")
    parser.add_argument("-e", "--environment", action="store_true",
                        help="Environment variables override Makefile variables")
    parser.add_argument("--var", action="append", metavar="VAR=VAL",
                        help="Override a variable (can be used multiple times)")
    args = parser.parse_args()

    if args.directory:
        os.chdir(args.directory)

    try:
        mf = Makefile(args.file)
    except FileNotFoundError as e:
        error(str(e))
        sys.exit(2)

    # Apply --var overrides
    if args.var:
        for item in args.var:
            if "=" in item:
                k, v = item.split("=", 1)
                mf.variables[k.strip()] = v.strip()

    # Environment overrides
    if args.environment:
        for k, v in os.environ.items():
            mf.variables[k] = v

    # --list
    if args.list:
        targets = [t for t in mf.list_targets()
                   if not t.startswith(".")]
        print(f"{C.BOLD}Available targets:{C.RESET}")
        for t in targets:
            marker = " (default)" if t == mf.default_target else ""
            print(f"  {C.CYAN}{t}{C.RESET}{marker}")
        return

    # --print-db
    if args.print_db:
        print(f"\n{C.BOLD}=== Variables ==={C.RESET}")
        for k, v in sorted(mf.variables.items()):
            print(f"  {k} = {v}")
        print(f"\n{C.BOLD}=== Rules ==={C.RESET}")
        for t, r in mf.rules.items():
            prereqs = " ".join(r["prereqs"])
            print(f"\n  {C.CYAN}{t}{C.RESET}: {prereqs}")
            for line in r["recipe"]:
                print(f"    \t{line}")
        return

    # Determine targets to build
    targets = args.targets if args.targets else (
        [mf.default_target] if mf.default_target else []
    )
    if not targets:
        error("No targets specified and no default target found.")
        sys.exit(2)

    builder = Builder(mf, dry_run=args.dry_run, silent=args.silent)

    success = True
    for target in targets:
        if not builder.build(target):
            success = False
            break

    if not success:
        sys.exit(1)


if __name__ == "__main__":
    main()
