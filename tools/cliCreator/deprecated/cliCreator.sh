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

# process all of the graphic files
for i in `ls gfx | egrep '.png$|.bmp$|.gif$|.pcx$'`
do
	echo -n "Now processing $i... "

	# run grit on it and try to get its size output
	out=`$DEVKITARM/bin/grit ./gfx/$i 2>&1 | awk '/STATUS: Work-DIB creation complete:/ {print $5}'`
	outMod=$(echo $out | tr "x" " " | tr "@" " ")
	width=`echo $outMod | awk '{print $1}'`
	height=`echo $outMod | awk '{print $2}'`

	# figure out what the filename is for the processed file by stripping off its extension
	# and adding img.bin and pal.bin
	img_bin="${i%.*}.img.bin"
	pal_bin="${i%.*}.pal.bin"

	# use wc (word count) to count the bytes (-c) in each file.
	# pipe that to awk to only get the bytes number
	num_bytes_img=`wc -c $img_bin | awk '{print $1}'`
	num_bytes_pal=`wc -c $pal_bin | awk '{print $1}'`

	# prepend that number to the bin file and save it with a .zbe extension
	./bin/numPrefix16 $img_bin $num_bytes_img $img_bin.zbe.tmp > /dev/null
	./bin/numPrefix16 $pal_bin $num_bytes_pal $pal_bin.zbe > /dev/null

	# prepend the height then width to the gfx bin
	./bin/numPrefix8 $img_bin.zbe.tmp $height $img_bin.zbe.tmp1 > /dev/null
	./bin/numPrefix8 $img_bin.zbe.tmp1 $width $img_bin.zbe > /dev/null

	# clean up as much as possible
	rm $img_bin.zbe.tmp* $img_bin $pal_bin

	echo "done (img: $num_bytes_img bytes, $width x $height; palette: $num_bytes_pal bytes)."
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
./bin/numPrefix32 allgfx.zbe.tmp $no_gfx allgfx.zbe > /dev/null
./bin/numPrefix32 allpal.zbe.tmp $no_pal allpal.zbe > /dev/null

# group those two files together
cat allgfx.zbe allpal.zbe > allgfxpal.zbe.tmp

# clean up a bit
rm allgfx.zbe.tmp allpal.zbe.tmp allgfx.zbe allpal.zbe

# prefix that with the total number of assets
./bin/numPrefix32 allgfxpal.zbe.tmp $no_all allgfxpal.zbe > /dev/null

# prefix that with the version
./bin/numPrefix16 allgfxpal.zbe $zbe_ver vertopal.zbe > /dev/null

# cleanup
rm allgfxpal.zbe.tmp allgfxpal.zbe

# parse the objects xml FILE
./bin/objParser ./xml/objects.xml objects.zbe > /dev/null

# cat the objects bin on to the end of the assets file
cat vertopal.zbe objects.zbe > $out_filename

# cleanup
rm vertopal.zbe objects.zbe

echo "All done. Assets saved into file $out_filename"
