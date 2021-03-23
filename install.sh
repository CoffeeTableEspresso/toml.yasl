set -e

cmake .
make toml

mv libtoml.so /usr/local/lib/yasl/
