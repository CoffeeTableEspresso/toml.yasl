set -e

(
    cd tomlc99
    make
)

cmake .
make toml

mv libtoml.so /usr/local/lib/yasl/
