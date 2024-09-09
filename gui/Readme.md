
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.3/install.sh | bash
source ~/.bashrc
nvm install 20
npm config set registry https://registry.npmmirror.com
npm install -g pnpm
pnpm install