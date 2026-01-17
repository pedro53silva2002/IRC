#!/bin/bash
echo running t2
nc -C localhost 6667 << 'EOF'
PASS PASS
USER SECOND 0 * SECOND
NICK Second
JOIN #FIRST
EOF