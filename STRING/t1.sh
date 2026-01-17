#!/bin/bash
echo running t1
nc -C localhost 6667 << 'EOF'
PASS PASS
USER FIRST 0 * FIRST
NICK First
JOIN #FIRST
EOF