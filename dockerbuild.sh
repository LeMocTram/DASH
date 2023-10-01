docker build -t server-img -f Dockerfile .
pushd database/
docker build -t database-img -f dockerfile .
popd
