
copy thru % circle "" "" sprintf("\s+8               (%g, %g)\s-8", $1/2, 3-($2/2)) at ($1, $2) rad 0.1 shaded "red" thickness 2 % until "END"
0 2
0 4
0 6
2 2
2 4
2 6
4 2
4 4
4 6
END

line from (3.8, 2) to (2.2, 2) thickness 1.5 
line from (1.8, 2) to (0.2, 2) thickness 1.5

line from (3.8, 4) to (2.2, 4) thickness 1.5
line from (1.8, 4) to (0.2, 4) thickness 1.5

line from (3.8, 6) to (2.2, 6) thickness 1.5
line from (1.8, 6) to (0.2, 6) thickness 1.5

line from (0, 2.2) to (0, 3.8) thickness 1.5
line from (0, 4.2) to (0, 5.8) thickness 1.5

line from (2, 2.2) to (2, 3.8) thickness 1.5
line from (2, 4.2) to (2, 5.8) thickness 1.5

line from (4, 2.2) to (4, 3.8) thickness 1.5
line from (4, 4.2) to (4, 5.8) thickness 1.5
