#!/bin/bash

# Prints everything from a FIFO
#   - intended to be used with 'go.sh'
#   - create FIFO via 'mkfifo <name>'

ERRORS_FIFO='./errors'

if ! stat -t "$ERRORS_FIFO" > /dev/null 2>&1; then
    echo "There is no pipe named \"$ERRORS_FIFO\""
    exit 2
fi

if ! [[ -p $ERRORS_FIFO ]]; then
    echo "$ERRORS_FIFO is not a pipe"
    exit 3
fi

echo "Attaching to named pipe $ERRORS_FIFO"

while true; do
    cat "$ERRORS_FIFO" || exit 4
done
