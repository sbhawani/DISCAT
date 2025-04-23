# scripts/build_macparticles.sh
#!/bin/bash
echo "[MACP] Building ROOT dictionaries and shared objects..."
root -l -q $MACP/macros/Load.C