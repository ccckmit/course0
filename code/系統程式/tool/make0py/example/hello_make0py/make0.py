# make0.py
app = target("ssl_app")
app.set_kind("binary")
app.add_files("main.c", "math_utils.c")

# 這裡完全平台無關，pmake 會自己想辦法找到 openssl
app.add_packages("openssl")