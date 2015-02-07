#!/bin/sh

 # Create RRD to store counter and consumption:
# Counter is GAUGE
# Consumption is DERIVE
# 1 value per minute for 3 days
# 1 value per day for 30 days
# 1 value per week for 10 years
# Consolidation LAST for counter
# Consolidation AVERAGE for consumption
rrdtool create emeter.rrd \
--no-overwrite \
--step 60 \
DS:counter:GAUGE:86400:0:1000000 \
DS:consum:DERIVE:86400:0:1000000 \
RRA:LAST:0.5:1:4320 \
RRA:AVERAGE:0.5:1:4320 \
RRA:LAST:0.5:1440:30 \
RRA:AVERAGE:0.5:1440:30 \
RRA:LAST:0.5:10080:520 \
RRA:AVERAGE:0.5:10080:520
