import os
import sys
import glob
import platform
import subprocess
import argparse
import re
from concurrent.futures import ThreadPoolExecutor, as_completed

# --- 專案配置類別 ---
_targets = []

class Target:
    def __init__(self, name):
        self.name = name
        self.kind = "binary"
        self.srcs = []
        self.includes = []
        self.cflags = []
        self.ldflags = []
        self.packages = []

    def set_kind(self, kind): self.kind = kind
    def add_files(self, *patterns):
        for p in patterns:
            self.srcs.extend(glob.glob(p))
    def add_includedirs(self, *dirs): self.includes.extend(dirs)
    def add_packages(self, *pkgs): self.packages.extend(pkgs)
    def add_cflags(self, *flags): self.cflags.append(flags)
    def add_ldflags(self, *flags): self.ldflags.append(flags)

def target(name):
    t = Target(name)
    _targets.append(t)
    return t

# --- 核心引擎 ---
class Toolchain:
    def __init__(self):
        self.os = platform.system()
        self.is_win = (self.os == "Windows")
        self.cc = "cl" if self.is_win else "clang"
        self.obj_ext = ".obj" if self.is_win else ".o"
        self.exe_ext = ".exe" if self.is_win else ""

    def get_compile_cmd(self, src, obj, includes):
        # 轉換 include 路徑為編譯器參數
        inc_args = [(f"/I{d}" if self.is_win else f"-I{d}") for d in includes]
        if self.is_win:
            return [self.cc, "/nologo", "/c", src, f"/Fo{obj}", "/EHsc"] + inc_args
        else:
            return [self.cc, "-c", src, "-o", obj, "-O2", "-Wall"] + inc_args

    def get_link_cmd(self, objs, output, ldflags):
        if self.is_win:
            # Windows 連結通常需要處理 .lib 路徑
            return [self.cc, "/nologo"] + objs + [f"/Fe{output}", "/link"] + ldflags
        else:
            return [self.cc] + objs + ["-o", output] + ldflags

class Builder:
    def __init__(self):
        self.tc = Toolchain()
        self.build_dir = ".pmake_cache"
        os.makedirs(self.build_dir, exist_ok=True)
        self.inc_regex = re.compile(r'^\s*#\s*include\s*"([^"]+)"')

    def _find_package(self, name):
        """自動偵測套件路徑：封裝平台差異"""
        res = {"inc": [], "link": []}
        if name.lower() == "openssl":
            if self.tc.os == "Darwin": # macOS (Homebrew)
                paths = ["/opt/homebrew/opt/openssl", "/usr/local/opt/openssl"]
                for p in paths:
                    if os.path.exists(p):
                        res["inc"].append(f"{p}/include")
                        res["link"].extend([f"-L{p}/lib", "-lssl", "-lcrypto"])
                        break
            elif self.tc.os == "Linux":
                res["link"].extend(["-lssl", "-lcrypto"])
            elif self.tc.os == "Windows":
                # 搜尋常見安裝路徑
                roots = [os.environ.get("OPENSSL_ROOT"), "C:/Program Files/OpenSSL-Win64"]
                for r in roots:
                    if r and os.path.exists(r):
                        res["inc"].append(f"{r}/include")
                        res["link"].extend([f"{r}/lib/libssl.lib", f"{r}/lib/libcrypto.lib"])
                        break
        return res

    def _get_deps(self, src, include_dirs):
        """遞迴追蹤標頭檔依賴"""
        deps, stack = set(), [src]
        while stack:
            curr = stack.pop()
            if curr in deps: continue
            if curr != src: deps.add(curr)
            if not os.path.exists(curr): continue
            
            with open(curr, 'r', encoding='utf-8', errors='ignore') as f:
                for line in f:
                    m = self.inc_regex.match(line)
                    if m:
                        h_name = m.group(1)
                        # 搜尋路徑：目前檔案目錄 -> 使用者指定目錄
                        search_dirs = [os.path.dirname(curr)] + include_dirs
                        for d in search_dirs:
                            h_path = os.path.normpath(os.path.join(d, h_name))
                            if os.path.exists(h_path):
                                stack.append(h_path)
                                break
        return deps

    def _needs_build(self, src, obj, includes):
        if not os.path.exists(obj): return True
        mtime = os.path.getmtime(obj)
        if os.path.getmtime(src) > mtime: return True
        return any(os.path.getmtime(h) > mtime for h in self._get_deps(src, includes))

    def _print_install_suggestion(self, pkg_name):
        """當出錯時，給予針對性的安裝建議"""
        sys_platform = platform.system()
        suggestions = {
            "openssl": {
                "Darwin": "brew install openssl",
                "Linux": "sudo apt-get install libssl-dev (Ubuntu/Debian) 或 sudo dnf install openssl-devel (Fedora)",
                "Windows": "請至 https://slproweb.com/products/Win32OpenSSL.html 下載安裝，或使用 vcpkg install openssl"
            }
        }
        
        msg = suggestions.get(pkg_name.lower(), {}).get(sys_platform, "請檢查該套件是否已安裝並配置於系統路徑中。")
        print(f"\n💡 [pmake 建議] 偵測到 {pkg_name} 可能未正確安裝或連結失敗。")
        print(f"👉 嘗試執行: {msg}\n")

    def build_target(self, t):
        print(f"🛠️  正在建置目標: {t.name}")
        
        # 1. 偵測套件 (如果找不到路徑就發出警告)
        for pkg in t.packages:
            info = self._find_package(pkg)
            if not info["inc"] and not info["link"]:
                print(f"⚠️  警告: 無法在系統中自動找到套件 '{pkg}'")
                self._print_install_suggestion(pkg)
            t.includes.extend(info["inc"])
            t.ldflags.extend(info["link"])

        # 2. 編譯階段 (將 .c 檔編譯為 .o 檔)
        objs =[]
        for src in t.srcs:
            # 決定對應的目的檔 (.o 或 .obj) 路徑
            obj = os.path.join(self.build_dir, os.path.basename(src) + self.tc.obj_ext)
            objs.append(obj)
            
            # 透過 _needs_build 判斷是否需要重新編譯
            if self._needs_build(src, obj, t.includes):
                print(f"  [CC] {src}")
                compile_cmd = self.tc.get_compile_cmd(src, obj, t.includes)
                try:
                    subprocess.run(compile_cmd, check=True)
                except subprocess.CalledProcessError as e:
                    print(f"\n❌ 編譯失敗！")
                    print(f"指令: {' '.join(compile_cmd)}")
                    sys.exit(1)
        
        # 3. 連結階段 (將所有 .o 檔連結成執行檔)
        out_exe = t.name + self.tc.exe_ext
        link_cmd = self.tc.get_link_cmd(objs, out_exe, t.ldflags)
        
        try:
            print(f"  [LINK] {out_exe}")
            subprocess.run(link_cmd, check=True, capture_output=True, text=True)
            print(f"✅ 建置成功: {out_exe}\n")
        except subprocess.CalledProcessError as e:
            print(f"\n❌ 連結失敗！")
            print(f"指令: {' '.join(link_cmd)}")
            print(f"錯誤訊息:\n{e.stderr}")
            
            # 檢查錯誤訊息中是否包含 ssl 關鍵字，以給出建議
            if "ssl" in e.stderr.lower() or "crypto" in e.stderr.lower():
                self._print_install_suggestion("openssl")
            
            sys.exit(1)

# --- 執行入口 ---
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("action", choices=["build", "clean"], default="build", nargs="?")
    args = parser.parse_args()

    if not os.path.exists("make0.py"):
        print("❌ 錯誤: 找不到 make0.py")
        return

    # 執行 make0.py 配置
    with open("make0.py", "r") as f:
        exec(f.read(), globals())

    builder = Builder()
    if args.action == "clean":
        import shutil
        if os.path.exists(builder.build_dir): shutil.rmtree(builder.build_dir)
        print("🧹 已清理快取")
    else:
        for t in _targets:
            builder.build_target(t)

if __name__ == "__main__":
    main()