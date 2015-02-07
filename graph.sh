#!/bin/sh
rrdtool graph counter.gif \
  -s 'now -1 day' -e 'now' \
  -X 0 -Y -A \
  DEF:counter=emeter.rrd:counter:LAST \
  LINE2:counter#0000FF:kWh \
  VDEF:countlast=counter,LAST \
  'GPRINT:countlast:Zählerstand %.1lf kWh'
display counter.gif&
rrdtool graph consum.gif \
  -s 'now -1 day' -e 'now' \
  -Y -A \
  DEF:consum=emeter.rrd:consum:AVERAGE \
  LINE2:consum#00FF00:W \
  CDEF:conskwh=consum,24,*,1000,/ \
  VDEF:conspd=conskwh,AVERAGE \
  'GPRINT:conspd:%.1lf kWh am Tag'
display consum.gif&
