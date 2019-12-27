#!/bin/bash

# vcurve.dat without outlier detection
gnuplot -e "a=108.119" -e "b=4.58267" -e "c=30008.3" -e "d=-1.86116" -e "i='vcurve.dat'" -e "o='vcurve_curve_no_outlier_detection.png'" hyperbolic_fit.gplt

# vcurve.dat with outlier detection
#   Outliers
#    (x,y)=(30200,4.85), residual=2.61705
#    (x,y)=(30500,22.1), residual=2.62268
gnuplot -e "a=119.824" -e "b=5.01803" -e "c=30009.4" -e "d=-1.96536" -e "i='vcurve.dat'" -e "o='vcurve_curve_with_outlier_detection.png'" hyperbolic_fit.gplt






# vcurve2.dat without outlier detection
gnuplot -e "a=110.609" -e "b=4.71316" -e "c=30007.7" -e "d=-1.9773" -e "i='vcurve2.dat'" -e "o='vcurve2_curve_no_outlier_detection.png'" hyperbolic_fit.gplt

# vcurve2.dat with outlier detection
#   Outliers
#    (x,y)=(30500,22.1), residual=2.57808
#    (x,y)=(30200,4.85), residual=2.6246
gnuplot -e "a=120.96" -e "b=5.07517" -e "c=30009.1" -e "d=-2.01569" -e "i='vcurve2.dat'" -e "o='vcurve2_curve_with_outlier_detection.png'" hyperbolic_fit.gplt





# vcurve3.dat without outlier detection
gnuplot -e "a=218.591" -e "b=10.5494" -e "c=30005.9" -e "d=-7.35399" -e "i='vcurve3.dat'" -e "o='vcurve3_curve_no_outlier_detection.png'" hyperbolic_fit.gplt

# vcurve3.dat with outlier detection
#    (x,y)=(30500,22.1), residual=3.37812
gnuplot -e "a=262.142" -e "b=13.1955" -e "c=30011" -e "d=-9.81638" -e "i='vcurve3.dat'" -e "o='vcurve3_curve_with_outlier_detection.png'" hyperbolic_fit.gplt



# Trying averaging of parameters
gnuplot -e "a=262.142" -e "b=13.1955" -e "c=30011" -e "d=-9.81638" -e "i='vcurve3.dat'" -e "o='averaging1.png'" hyperbolic_fit.gplt
