set terminal pngcairo enhanced font "Arial,12"
set output 'streamlines.png'

set palette defined (  0 '#000004',  1 '#100f56',  2 '#3d0961',  3 '#9e2a84', \
                       4 '#f18c3d',  5 '#feeb2a',  6 '#f0f921' )
set pm3d map

set size ratio -1
set autoscale fix

# Set the title and labels
set title "Streamlines"
set xlabel "x (lattice units)"
set ylabel "y (lattice units)"

set xrange [0:399]
set yrange [0:199]

set key off

# Plot the function
set grid
set arrow from 0,0 to 1,1 nohead lw 0.5


#plot "exports/data_1600.csv" skip 1 u 1:2:(sqrt($5**2 + $6**2)) w image

# Loop to generate frames
do for [i=1:30] {
    unset key
    set output sprintf('frames/frame_%03d.png', i)
    plot sprintf("exports/data_%d00.csv", i) skip 1 u 1:2:(sqrt($5**2 + $6**2)) w image
}
