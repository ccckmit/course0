#!/bin/bash
set -x
PAY4_DB_PATH=:memory: pytest tests/ -v "$@"
