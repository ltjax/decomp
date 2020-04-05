#!/bin/bash

set -e
set -x

if [[ "$(uname -s)" == 'Darwin' ]]; then  
    # Activate our venv
    source venv/bin/activate
fi

python3 build.py
