# !/bin/bash

# the zbe verion
zbe_ver=1

# see if there was a filename indicated
if [ "$1" == "" ]
  then
    out_filename="assets.zbe"
  else
    out_filename=$1
fi

# build the numPrefix programs
cc -o numPrefix/numPrefix16 numPrefix/numPrefix16.c
cc -o numPrefix/numPrefix32 numPrefix/numPrefix32.c
cc -o numPrefix/numPrefix8 numPrefix/numPrefix8.c

# process all of the graphic files
for i in `ls gfx | egrep '.png$|.bmp$|.gif$|.pcx$'`
do
	echo -n "Now processing $i... "
	
	# run grit on it and try to get its size output
	out=`$DEVKITARM/bin/grit ./gfx/$i 2>&1 | awk '/STATUS: Work-DIB creation complete:/ {print $5}'`
	outMod=$(echo $out | tr "x" " " | tr "@" " ")
	width=`echo $outMod | awk '{print $1}'`
	# height=`echo $outMod | awk '{print $2}'`
	
	# figure out what the filename is for the processed file by stripping off its extension
	# and adding img.bin and pal.bin
	img_bin="${i%.*}.img.bin"
	pal_bin="${i%.*}.pal.bin"

	# use wc (word count) to count the bytes (-c) in each file.
	# pipe that to awk to only get the bytes number
	num_bytes_img=`wc -c $img_bin | awk '{print $1}'`
	num_bytes_pal=`wc -c $pal_bin | awk '{print $1}'`
	
	# prepend that number to the bin file and save it with a .zbe extension
	./numPrefix/numPrefix16 $img_bin $num_bytes_img $img_bin.zbe.tmp > /dev/null
	./numPrefix/numPrefix16 $pal_bin $num_bytes_pal $pal_bin.zbe > /dev/null
	
	# prepend the size to the gfx bin 
	./numPrefix/numPrefix8 $img_bin.zbe.tmp $width $img_bin.zbe > /dev/null
	
	# clean up as much as possible
	rm $img_bin.zbe.tmp $img_bin $pal_bin *.img.h *.pal.h
		
	echo "done (img: $num_bytes_img bytes, $width pixels; palette: $num_bytes_pal bytes)."
done

# count all the gfx and pal files
no_gfx=`ls | grep '.img.bin.zbe$' | wc -w`
no_pal=`ls | grep '.pal.bin.zbe$' | wc -w`
no_all=$(($no_gfx+$no_pal))

# happy echo
echo "Got $no_gfx graphics + $no_pal palettes = $no_all total assets."

# group all of the images and pals and save them into intermediate files
cat `ls | grep '.img.bin.zbe$'` > allgfx.zbe.tmp
cat `ls | grep '.pal.bin.zbe$'` > allpal.zbe.tmp

# clean up those files
rm `ls | grep '.bin.zbe$'`

# put the # assets on the front of those temp files
./numPrefix/numPrefix32 allgfx.zbe.tmp $no_gfx allgfx.zbe > /dev/null
./numPrefix/numPrefix32 allpal.zbe.tmp $no_pal allpal.zbe > /dev/null

# group those two files together
cat allgfx.zbe allpal.zbe > all.zbe.tmp

# clean up a bit
rm allgfx.zbe.tmp allpal.zbe.tmp allgfx.zbe allpal.zbe

# prefix that with the total number of assets
./numPrefix/numPrefix32 all.zbe.tmp $no_all numd.zbe.tmp > /dev/null

# prefix that with the version
./numPrefix/numPrefix16 numd.zbe.tmp $zbe_ver $out_filename > /dev/null

# final cleanup
rm all.zbe.tmp numd.zbe.tmp 

echo "All done. Assets saved into file $out_filename"
