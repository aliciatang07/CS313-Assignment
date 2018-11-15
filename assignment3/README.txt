
Put your answers to non-coding questions in this file as described in
the assignment description.

All answers are to be in plain text ASCII.


Task2: 
Predicting Cache Miss Rates 
SumTest1 = 131328
Miss rate =   0.2500
//since we only have one set, whenever we reach a new row we will have a cache miss and need to update the value and there are 4 rows in total, so we will have 1/4 miss rate in total 
SumTest2 = 131328
Miss rate =   1.0000
SumTest3 = 131328
Miss rate =   0.5000
SumTest4 = 131328
Miss rate =   1.0000
SumTest5 = 131328
Miss rate =   0.2500
SumTest6 = 131328
Miss rate =   0.3457
SumTest7 = 115440
Miss rate =   0.2500

Explanation 

a) The array is stored in a row-major order.
     There are cache misses every 4 longs.
     beacuse each cache block(32)bytes can store 4 longs. so every four time we will have a cold 
     miss since the cache line is empty. and the following 3 longs can assess data from the cache
     and have cache hit.As a result, the miss rate is 0.25.

b) The miss rate is 1.
     SumB read in a column-major order. But the arrays are stored in row-major order.
     Thus elements a[1][0](cache a[1][1-3]) a[3][0] (cache a[3][1-3]) all cached in same set
     a[0][0](cache a[0][1-3]) a[2][0](cache a[0][1-3]) all cached in another same set
     by the time we reach a[0][1] the cache in set 0 is already be overwritten, so there will 
     be a cache miss. 
     therefore, there are always a cache miss (cold miss+conflict miss) and have miss rate = 100%
     After that ,it all read the wrong cache because it do not follow the order of the array stored.
     So they will all miss. 
   
c)The miss rate is 0.5
     SumC read a 2*2 square. In this case, when it read i and j, it will first meet a cold miss, then hit when it read i and j+1.
     After that, when it read i+1 and j, it will first meet a cold miss too, and then when it read i+1 and j+1, it hit.
     After that, it go to iterate part, j and i both add 2. So when it read i and j next iteration, it will meet the same situation as before. 
     As a result, half of them miss. 

d)The miss rate is 1.  
     using two-way set-associative cache will not help as sumB read the array in a incorrect order. 
     Thus elements a[1][0](cache a[1][1-3]) a[3][0] (cache a[3][1-3]) all cached in same set  
     a[0][0](cache a[0][1-3]) a[2][0](cacohe a[0][1-3]) all cached in another same set
     by the time we reach a[0][1] the cache in set 0 is already be overwritten, so there will 
     be a cache miss. 
     therefore, there are always a cache miss (cold miss+conflict miss) and have miss rate = 100%
     As a result, we miss every time.

e)The miss rate is 0.25.  
    Using four-way set associative cache means that E=4. When we read the first coloumn, we meet 4 cold misses and put (i= 0-3, j=0) into the Line0 -3 successively(LRU replacement)with set 0. 
    Because we can load 32 byte a block(4 long), 
    Thus elements a[1][0](cache a[1][1-3]) a[3][0] (cache a[3][1-3]) all cached in same set
    a[0][0](cache a[0][1-3]) a[2][0](cache a[0][1-3]) all cached in another same set.
    since there are four lines per set. we don't need overwrite line to cache new values, 
    so when we reach a[0][1], we can find it's value in set0 with cache hit. and there are only
    1/4 cold miss due to empty cache line
    So in this case, we will miss 0.25.

f) The miss rate is 0.3457.(average)
    Using four-way set associative cache means that E=4. 
    since it involves random policy, there will be a best, average worst case. the best case
    should be 0.25, which is the same as LRU policy, which means it happens to choose the line that 
    are least frequently every time.
    on average the miss rate is about 0.35 since each line has the same chance to be overwritten(1/4),if choosing the line same as LRU policy, it will decrease miss rate. otherwise it will increase miss rate
    worst case is about 0.75, which means everytime the random choice is different from LRU policy and cause a cache miss. 

g)  The miss rate is 0.25
    since there is only one line per cache set, the replacement policy doesn't affect result.
    and it's a column-major sum. 
    because the column size change to 120, this will cause a[0][0] (cache a[0][1-3]), a[1][0](cache a[1][1-3]) a[2][0] (cache a[2][1-3]), a[3][0](cache a[1][1-3]) all be cached in different set 
    so by the time we reach a[0][1] we can find the value in cache with cache hit. the left loop will be the same pattern. 
    therefore there are only 1/4 cold miss due to the empty cache line. 


Task3：improve lifetime
version1: change the col-sum to row-sum, improive the spatial locality

The best time for the optimized run of the game of life took: 3263900 usec

The improved ratio: 5 times
 Optimized run[1] = 3332568
 Optimized run[2] = 3316727
 Optimized run[3] = 3265506
 Optimized run[4] = 3267397
 Optimized run[5] = 3266221
 Optimized run[6] = 3264472
 Optimized run[7] = 3263900
 Optimized run[8] = 3265045
 Optimized run[9] = 3267610
 Optimized run[10] = 3320088




version2:combine all the loop together to reduce the time iterate whole 
loop multiple times, improve the spatial locality and temporal locality

The best time for the optimized run of the game of life took: 1632690 usec

The improved ratio: 10 times
 Optimized run[1] = 1690942
 Optimized run[2] = 1632690
 Optimized run[3] = 1633085
 Optimized run[4] = 1634067
 Optimized run[5] = 1634148
 Optimized run[6] = 1636149
 Optimized run[7] = 1635834
 Optimized run[8] = 1636075
 Optimized run[9] = 1636116
 Optimized run[10] = 1632756


version3:read every 3 cols together, reduce the reuse of accessing same item 
in each loop, calculate 3 items in same row together 

The best time for the optimized run of the game of life took: 1448770 usec

The improved ratio: 11 times
 Optimized run[1] = 1510814
 Optimized run[2] = 1457131
 Optimized run[3] = 1455593
 Optimized run[4] = 1448770
 Optimized run[5] = 1453952
 Optimized run[6] = 1457051
 Optimized run[7] = 1455712
 Optimized run[8] = 1454184
 Optimized run[9] = 1452059
 Optimized run[10] = 1455804


version4: read every 3 cols and 3 rows together, in each loop we calculate 9 blocks nearby and 
define local variables for elements in oldworld that will be reused multiple times. increase temporal spatiality 

The best time for the optimized run of the game of life took: 1311516 usec

The improved ratio: 13 times
 Optimized run[1] = 1368737
 Optimized run[2] = 1312789
 Optimized run[3] = 1312982
 Optimized run[4] = 1312330
 Optimized run[5] = 1312419
 Optimized run[6] = 1312780
 Optimized run[7] = 1312993
 Optimized run[8] = 1312625
 Optimized run[9] = 1311706
 Optimized run[10] = 1311516




