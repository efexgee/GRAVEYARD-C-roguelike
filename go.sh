#!/bin/bash

# makes and runs the executable
#   - redirects STDERR to a FIFO
#   - stops on warnings
#   - assumes 'make strict' which errors on warnings
#   - intended to be used with 'error_console.sh'
#   - create FIFO via 'mkfifo <name>'

EXEC_PATH='.'
EXEC_FILE='game'

ERRORS_FIFO='./errors'

EXECUTABLE="${EXEC_PATH}/${EXEC_FILE}"

if ! stat -t "$ERRORS_FIFO" > /dev/null 2>&1; then
    echo "There is no pipe named \"$ERRORS_FIFO\""
    exit 2
fi

if ! [[ -p $ERRORS_FIFO ]]; then
    echo "$ERRORS_FIFO is not a pipe"
    exit 3
fi

make strict || exit 1

if ! [[ -f $EXECUTABLE ]]; then
    echo "File does not exist: $EXECUTABLE"
    exit 4
fi

if ! [[ -x $EXECUTABLE ]]; then
    echo "File is not executable: $EXECUTABLE"
    exit 5
fi

echo "=== Launching $(basename $EXECUTABLE) ===" > "$ERRORS_FIFO"

if ! "$EXECUTABLE" 2> "$ERRORS_FIFO"; then
    # If the exectutable exits with an error, run 'reset'
    # because usually the terminal is broken at that point
    # Redirect STDOUT to FIFO since our terminal will be broken
    exec > "$ERRORS_FIFO"
    echo "$(basename $EXECUTABLE) exited with error"
    echo -n "Resetting terminal..."
    reset
    echo "Done."
fi
