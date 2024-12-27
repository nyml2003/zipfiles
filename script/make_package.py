# 获取/app/version内容
def get_version():
    with open("/app/version", "r") as f:
        return f.read().strip()

app_name = "zipfiles"
version = get_version()
tar_name = f"{app_name}-{version}.tar.gz"
target_dir = "/app/release"
output_dir = "/app/output"
import os
# 执行打包命令
os.system(f"tar -czvf {output_dir}/{tar_name} {target_dir}")
