set -x

# 1. 初始化倉庫
./git0 init

# 2. 創建一個文件
echo "hello git0" > test.txt

# 3. 將文件添加到暫存區
./git0 add test.txt

# 4. 提交變更
./git0 commit "first commit"