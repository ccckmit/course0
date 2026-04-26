set -x
git clone http://localhost:8000/fastapi_test.git
cd fastapi_test
echo "Hello FastAPI Git Server" > readme.md
git add readme.md
git commit -m "FastAPI commit"
git push origin master
