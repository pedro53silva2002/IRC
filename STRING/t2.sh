#!/bin/bash
echo running t2
nc -C localhost 6667 << 'EOF'
CAP LS
PASS PASS
USER SECOND 0 * SECOND
NICK Second
JOIN #CHANNEL
EOF