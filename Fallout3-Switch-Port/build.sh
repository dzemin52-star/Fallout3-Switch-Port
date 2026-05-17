#!/bin/bash
set -e
echo "==> Cleaning..."
make clean
echo "==> Building..."
make -j$(nproc)
echo "==> Done. NRO: $(TARGET).nro"