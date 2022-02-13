#!/bin/bash

rm -rf Build

find Assets -type f \( -name "*.vert" -o -name "*.frag" \) -exec \
    glslc "{}" -o "{}.spv" \
\;

find Assets -type f -not -path "Assets/Compiled/CompiledResources*" -exec \
    sh GenerateResource.sh "{}" \
\;
