echo running t1
nc -C localhost 6667 << 'EOF'
CAP LS
PASS PASS
NICK First
USER FIRST 0 * FIRST
JOIN channel
JOIN #channel
EOF