set -x
rustc js0i.rs
./js0i factorial.js
RUST_BACKTRACE=1 ./js0i js0i.js factorial.js
