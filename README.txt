Description:
  csv_merge.c is a program that merges two comma seperated value files (CSV). It takes in two files at a time
  and creates a series of output files demonstrating several different merging techniques and creating new files
  to store the results in, leaving the original CSV file unchainged. 

How to use csv_merge.c:
 1. Ensure input files are locaded in the same directory as the compiled program and named correctly by either
    - a) Opening csv_merge.c and near the top in the definitions section changing 
	 FILENAME1 and FILENAME2 to be defined as the names of the input files rather
         than input1.txt and input2.txt or	
    - b) Renaming the two files you currently wish the program to use to input1.txt
	 and input2.txt respectivly

    NOTE: Any changes to csv_merge.c will require recompilation of the program so
	  option b is recommended if the input files will have a variety of differing names
		  
 2. Compile csv_merge.c 
    	clang -Wall -DNDEBUG csv_merge.c -o csv_merge.out
		  
 3. Run csv_merge.out
    - Assuming the previous 2 steps were completed correctly this will preform the expected joins on the input files
      and create files named Natural_Join.txt, Left_Join.txt, Full_Outer_Join.txt containing there respective results.