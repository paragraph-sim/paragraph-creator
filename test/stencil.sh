#!/bin/bash

set -e

X_NODES=8
Y_NODES=11
Z_NODES=9
ITERATIONS=10
LOOP_COND_SECS=1e-6
COMP_SECS=1e-3
FACE_MSG_SIZE=800
EDGE_MSG_SIZE=80
CORNER_MSG_SIZE=8
ALLREDUCE_SIZE=16
REDUCTION_SECS=1e-7
    
./stencil \
    ${X_NODES} \
    ${Y_NODES} \
    ${Z_NODES} \
    ${ITERATIONS} \
    ${LOOP_COND_SECS} \
    ${COMP_SECS} \
    ${FACE_MSG_SIZE} \
    ${EDGE_MSG_SIZE} \
    ${CORNER_MSG_SIZE} \
    ${ALLREDUCE_SIZE} \
    ${REDUCTION_SECS} \
    some_cool_stencil.pb \
    -v 2
    
