#!/bin/sh

CLIENTS_COUNT=5

SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

cd $SCRIPT_DIR

#rm -rf prev
mkdir prev
mv cur prev/
mv EasyRSA-3.0.8 prev/

tar zxvf EasyRSA-3.0.8.tgz
cp vars EasyRSA-3.0.8/

cd EasyRSA-3.0.8

./easyrsa init-pki
./easyrsa --batch build-ca nopass
EASYRSA_CERT_EXPIRE=3650 ./easyrsa build-server-full server1 nopass

mkdir -p ../cur/keys_server
cp pki/ca.crt ../cur/keys_server
cp pki/issued/server1.crt ../cur/keys_server/entity.crt
cp pki/private/server1.key ../cur/keys_server/entity.key

for i in $(seq 1 $CLIENTS_COUNT);
do
    EASYRSA_CERT_EXPIRE=3650 ./easyrsa build-client-full "client$i" nopass
    CLIENT_DIR="../cur/keys_client${i}"

    mkdir -p $CLIENT_DIR
    cp pki/ca.crt $CLIENT_DIR
    cp pki/issued/client${i}.crt $CLIENT_DIR/entity.crt
    cp pki/private/client${i}.key $CLIENT_DIR/entity.key
done
