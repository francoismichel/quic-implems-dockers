FROM rust:1.44.1 as build

RUN apt-get update && apt-get install -y cmake && \
    rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y ca-certificates && \
    rm -rf /var/lib/apt/lists/*

ENV RUST_LOG=trace

RUN mkdir /quiche-generic
RUN mkdir /logs
RUN mkdir /certs

WORKDIR /quiche-generic

RUN mkdir src
COPY src/* src/
COPY Cargo.toml ./
COPY gen-certs.sh ./
COPY run_endpoint.sh ./
RUN chmod +x run_endpoint.sh

RUN sh gen-certs.sh
RUN cargo build
RUN cp target/debug/generic-http3-client ./
RUN cp target/debug/generic-http3-server ./
RUN cp cert.crt /certs/
RUN cp cert.key /certs/

ENV RUST_LOG=trace

WORKDIR /logs

ENTRYPOINT [ "/quiche-generic/run_endpoint.sh" ]
