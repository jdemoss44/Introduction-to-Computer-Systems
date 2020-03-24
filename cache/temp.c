S 68210c -> 1a08 8 c -- miss
L 682120 -> 1a08 9 0 -- miss
L 682104 -> 1a08 8 4 -- hit
L 682100 -> 1a08 8 0 -- hit

1st Round:
	//skip y x y

	L 602104 -> 1808 8 4 -- miss 	B
	S 642200 -> 1908 10 0 -- miss  

	L 602108 -> 1808 8 8 -- hit 	C
	S 642300 -> 1908 18 0 -- miss

	L 60210c -> 1808 8 c -- hit 	D
	S 642400 -> 1909 0 0 -- miss

	L 602100 -> 1808 8 0 -- hit 	A
	S 642100 -> 1908 8 0 -- miss

2nd Round:
	L 602200 -> 1808 10 0 -- miss 	I
	S 642104 -> 1908 8 4 -- hit

	//skip y x y

	L 602208 -> 1808 10 8 -- hit 	K
	S 642304 -> 1908 18 4 -- hit

	L 60220c -> 1808 10 c -- hit 	L
	S 642404 -> 1909 0 4 -- hit

	L 602204 -> 1808 10 4 -- hit 	J
	S 642204 -> 1908 10 4 -- miss

3rd Round:
	L 602300 -> 1808 18 0 -- miss 	Q
	S 642108 -> 1908 8 8 -- hit 

	L 602304 -> 1808 18 4 -- hit 	R
	S 642208 -> 1908 10 8 -- hit
	
	//skip y x y
	
	L 60230c -> 1808 18 c -- hit 	T
	S 642408 -> 1909 0 8 -- hit

	L 602308 -> 1808 18 8 -- hit 	S
	S 642308 -> 1908 18 8 -- miss

4th Round:
	L 602400 -> 1809 0 0 -- miss 
	S 64210c -> 1908 8 c -- hit

	L 602404 -> 1809 0 4 -- hit
	S 64220c -> 1908 10 c -- hit
	hits:20 misses:12 evictions:7
