#!/bin/bash
set -e

if [ "$ROLE" == "client" ]; then
    /quiche-generic/generic-http3-client $@
elif [ "$ROLE" == "server" ]; then
    echo "## Starting quiche server..."
    /quiche-generic/generic-http3-server $@
fi