find . -name "node_modules" -type d -prune
find . -name "node_modules" -type d -exec rm -rf {} +
find . -name "target" -type d -prune
find . -name "target" -type d -exec rm -rf {} +
find . -name ".DS_Store" -prune
find . -name ".DS_Store" -exec rm -rf {} +