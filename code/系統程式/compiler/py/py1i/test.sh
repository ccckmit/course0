set -x

ROOT="$(cd "$(dirname "$0")" && pwd)"
TEST_ROOT="$(cd "$ROOT/../tests" && pwd)"

python3 "$TEST_ROOT/fact.py"
python3 "$ROOT/py1i.py" "$TEST_ROOT/fact.py"
python3 "$ROOT/py1i.py" "$ROOT/py1i.py" "$TEST_ROOT/fact.py"
python3 "$ROOT/py1i.py" "$ROOT/py1i.py" "$TEST_ROOT/hello.py"
python3 "$ROOT/py1i.py" "$TEST_ROOT/json.py"
