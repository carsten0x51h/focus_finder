#!/bin/bash

INDI_BIN_DIR=/usr/bin

indiserver -v ${INDI_BIN_DIR}/indi_atik_ccd \
              ${INDI_BIN_DIR}/indi_atik_wheel \
              ${INDI_BIN_DIR}/indi_moonlite_focus \
              ${INDI_BIN_DIR}/indi_eqmod_telescope
