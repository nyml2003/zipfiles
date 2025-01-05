import os
# 获取/app/version内容
def get_version():
    with open("/app/version", "r") as f:
        return f.read().strip()

app_name = "zipfiles"
version = get_version()

def new_version():
    v1,v2,v3 = version.split(".")
    v3 = str(int(v3)+1)
    return f"{v1}.{v2}.{v3}"

# 修改/app/version文件
with open("/app/version", "w") as f:
    f.write(new_version())

tar_name = f"{app_name}.tar.gz"
target_dir = "/app/release"
output_dir = f"/app/output/{version}"
os.system(f"mkdir -p {output_dir}")

# 执行打包命令
os.system(f"tar -czvf {output_dir}/{tar_name} {target_dir}")

# 修改download.sh文件, 替换版本号
download_sh = "/app/demo/download.sh"
with open(download_sh, "r") as f:
    lines = f.readlines()
with open(download_sh, "w") as f:
    for line in range(len(lines)):
        if "VERSION=" in lines[line]:
            lines[line] = f"VERSION={version}\n"
        f.write(lines[line])
    
