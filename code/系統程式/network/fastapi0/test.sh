set -x
. venv/bin/activate
if ! command -v uvicorn &> /dev/null; then
    pip install uvicorn
fi
uvicorn example:app --reload
