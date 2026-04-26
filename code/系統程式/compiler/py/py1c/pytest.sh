set -x

python py0c.py py/hello.py -o qd/hello.qd
python py0c.py py/fact.py -o qd/fact.qd
python py0c.py py/json.py -o qd/json.qd
python py0c.py qd0vm.py -o qd/qd0vm.qd
python py0c.py py/class.py -o qd/class.qd
python py0c.py py/oop.py -o qd/oop.qd

python qd0vm.py qd/hello.qd
python qd0vm.py qd/fact.qd
python qd0vm.py qd/json.qd
python qd0vm.py qd/class.qd # success, 這版是 claude 做的。
python qd0vm.py qd/oop.qd # success, 這版是 claude 做的。
# python qd0vm.py qd/qd0vm.qd qd/hello.qd # fail, 這版是 claude 做的。
PYTHONNODEBUGRANGES=1 python qd0vm.py qd/qd0vm.qd qd/hello.qd