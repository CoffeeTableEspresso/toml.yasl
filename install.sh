set -e

(
    cd tomlc99
    make
)

cmake .
make toml

mkdir -p /usr/local/lib/yasl/
mv libtoml.so /usr/local/lib/yasl/
mv toml.yasl /usr/local/lib/yasl/
