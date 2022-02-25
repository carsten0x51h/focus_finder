#!/bin/bash

INDI_BIN_DIR=/usr/bin

indiserver -v ${INDI_BIN_DIR}/indi_simulator_wheel \
              ${INDI_BIN_DIR}/indi_simulator_telescope \
              ${INDI_BIN_DIR}/indi_simulator_ccd \
              ${INDI_BIN_DIR}/indi_simulator_focus
