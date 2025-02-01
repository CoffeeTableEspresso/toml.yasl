set -e

(
    cd tomlc99
    make
)

cmake .
make toml

mkdir -p /usr/local/lib/yasl/
cp libtoml.so /usr/local/lib/yasl/
cp toml.yasl /usr/local/lib/yasl/
