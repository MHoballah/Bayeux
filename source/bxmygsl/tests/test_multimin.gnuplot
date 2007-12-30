# -*- mode: gnuplot; -*- 
# mygsl - test_multimin.gnuplot

#! rm -f test_multimin.his test_multimin.sol
#! time ${MYGSL_ROOT}/Linux-i686/bin/test_multimin -l -d

set title "mygsl - test_multimin"

set grid

set xrange [0:1024] 
set xrange [0:600] 
set yrange [0:100] 
#set yrange [0:50000] 
plot 'test_multimin.hist' using 1:3 with steps, \
     'test_multimin.sol' using 1:2 with lines
     
#plot 'test_multimin.hist' using 1:3:(sqrt($3)) with errorbars, \
#     'test_multimin.sol' using 1:2 with lines

# end of test_multimin.gnuplot
