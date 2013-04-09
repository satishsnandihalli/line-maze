Overview of the algorithm:
  Create a distance field such that the following invariance property holds good. From the problem definition, the distance to 
any of the 8 neighbouring cells will be no more than the distance of the current cell from the start point plus one. The final 
distance field will be such that no cell is greater than one plus the minimum of the all its neighbouring cells.
  The computation of the field can be done using an incremental iterative approach. We initially set all the cells to infinity.
In this state above invariance property does hold true. Now we set the value at start location to 0. Since we changed the value
at the start location, values of all the neighbouring cells could potentially change. So, we will maintain a set of locations which
have just changed, so that, we can update its neighbours. We will add the neighbouring cells which change to the set if they actually
change when updated so that we can further update its neighbours. This process would end when the set of changed locations is empty.
   Once this field is computed, we walk along the boundaries of the maze looking for a cell which has the smallest value possible but 
is not same as the start specified by the problem. This location would form the exit point of our optimal path. If the minimum is 
equal to "infinity" we set before, it means that the problem does not have a solution and we report the same. If the value is smaller
than "infinity" then, we traceback looking at the neighbouring cells to find a cell smaller than the current cell. we continue to 
do this until we hit the start point remembering the path traversed. When we hit the start point, we would have obtained the complete
path. 


To compile the code : 

 g++ maze.cpp -o maze;
 
I used version 4.6.3. An compiler which has adequate support for standard template library should be fine.

To run the code 

./maze new_maze.txt 

I have other example mazes

1. 10x10.txt 
2. test.txt 
3. impossible.txt 	 
