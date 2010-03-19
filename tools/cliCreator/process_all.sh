#!/bin/bash

#the zbe verion
zbe_ver=1

#see if there was a filename indicated
if [ "$1" == "" ]
  then
    out_filename="assets.zbe"
  else
    out_filename=$1
fi

#build the numPrefix programs
cc -o numPrefix/numPrefix16 numPrefix/numPrefix16.c
cc -o numPrefix/numPrefix32 numPrefix/numPrefix32.c

#run grit on all the files in the gfx folder, 
#should result with bin files in the current directory
$DEVKITARM/bin/grit ./gfx/*.png &> /dev/null

#remove those pesky header files
rm *.h

#process all of the bin files
for i in `ls | grep '.bin$'`
do
	echo -n "Now processing $i... "
	
	#use wc (word count) to count the bytes (-c) in each file.
	#pipe that to awk to only get the bytes number
	num_bytes=`wc -c $i | awk '{print $1}'`
	
	#prepend that number to the bin file and save it with a .zbe extension
	./numPrefix/numPrefix16 $i $num_bytes $i.zbe > /dev/null
	
	#remove this file
	rm $i
	
	echo "done ($num_bytes bytes)."
done

#count all the gfx and pal files
no_gfx=`ls | grep '.img.bin.zbe$' | wc -w`
no_pal=`ls | grep '.pal.bin.zbe$' | wc -w`
no_all=$(($no_gfx+$no_pal))

#happy echo
echo "Got $no_gfx graphics + $no_pal palettes = $no_all total assets."

#group all of the images and pals and save them into intermediate files
cat `ls | grep '.img.bin.zbe$'` > allgfx.zbe.tmp
cat `ls | grep '.pal.bin.zbe$'` > allpal.zbe.tmp

#clean up those files
rm `ls | grep '.bin.zbe$'`

#put the #assets on the front of those temp files
./numPrefix/numPrefix32 allgfx.zbe.tmp $no_gfx allgfx.zbe > /dev/null
./numPrefix/numPrefix32 allpal.zbe.tmp $no_pal allpal.zbe > /dev/null

#group those two files together
cat allgfx.zbe allpal.zbe > all.zbe.tmp

#clean up a bit
rm allgfx.zbe.tmp allpal.zbe.tmp allgfx.zbe allpal.zbe

#prefix that with the total number of assets
./numPrefix/numPrefix32 all.zbe.tmp $no_all numd.zbe.tmp > /dev/null

#prefix that with the version
./numPrefix/numPrefix16 numd.zbe.tmp $zbe_ver $out_filename > /dev/null

#final cleanup
rm all.zbe.tmp numd.zbe.tmp 

echo "All done. Assets saved into file $out_filename"
