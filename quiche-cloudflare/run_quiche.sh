# server
docker run -ti -e ROLE=server --network host --mount type=bind,source="$(pwd)",target=/certs quiche-generic -p 4445 -k /certs/cert.key -c /certs/cert.crt

# client
docker run -ti -e ROLE=client --network host --mount type=bind,source="$(pwd)",target=/logs quiche-generic -G 50000000 -X keys.log 127.0.0.1 4445
