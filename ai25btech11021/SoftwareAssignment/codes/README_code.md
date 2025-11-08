This explains all the instuctions and the format for my codes

Totally I have 3 codes 

1.colour.c      //This is for bonus//
2.grey.c                // The code is in modular mode in a single code//
3.visualization.py   //(This is bonus only It does'nt count as I'm using python)//

these are stored in the specifies respective folders in codes folder 

colour.c code works fine for all the coloured and B/W images both .png and .jpg but for clarification I also provided grey.c

The first 2 codes both take the input (image) from a subfolder named (subfolder) which you will be asked as an input and the no of K values also will be asked as input then you will be asked to input the K values you want to take.
 
like         (change grey with colour if using colour.c)

gcc grey.c -o grey -lm    (Step 1 both in vs code and ubuntu)

./grey                    (Step 2 )

(After the step 2)

Enter subfolder name inside figs:

Enter number of K values: 

 Enter 5 K values:  (example 5 here)    ( click enter after this or click ./grey)

then the output images for all the K values will be stored in the respective subfolders and the error.txt containing all the Frobenius error and ratios for the K value aslo will be saved in the subfolder.

Same for tthe python code also the input (Subfolder name) will be asked then the output plot will be saved in the subfolder (visualization) as (input sunfolder)_visu.png.

at last the .vscode is for the vs code settings I used.