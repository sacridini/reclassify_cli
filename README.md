# Reclassify_cli
CLI reclassify implementation using GDAL C++ API

NAME
	reclassify
	- algorithm to reclassify images

SYNOPSIS
	reclassify [-type img_data_type] [-in inputfile] [-ot outputfile]
			   [-iv inputvalue1 inputvalue2 ... -iv_end]
			   [-ov outputvalue1 outputvalue2 ... -ov_end]

DESCRIPTION
	The reclassify utility is an image processing tool that changes the values from an input image to
	the output values specified by the user.

	-t / -type img_data_type:
	   choose the type of the output image. It can accept these types: uint8 (8 bit unsigned integer),
	   uint16 (16 bit unsigned integer), uint32 (32 bit unsigned integer), int (32 bit integer),
	   float (32 bit float), double (64 bit float).

	-i / -in inputfile:
	   set the path to the input image

	-o / -out outputfile:
	   set the path to the output image

	-iv inputvalue1 inputvalue2 ... -iv_end:
	   set the image values that you want to change. You can specify one or any number of values.
	   If the user specify more than one value, the values need to be set with a whitespace between
	   the other like: 1 2 3 9 10 70
	   At the end of the values specified, the user need to set the flag -iv_end even if you specify
	   just a single value.

	-ov outputvalue1 outputvalue2 ... -ov_end:
	   set the image values that you want to be present at the output image. You can specify one or any
	   number of values.
	   If the user specify more than one value, the values need to be set with a whitespace between
	   the other like: 1 2 3 9 10 70
	   At the end of the values specified, the user need to set the flag -ov_end even if you specify
	   just a single value.

EXAMPLES
	. For instance, the the original values of the input image 3 and 15 will be changed to 1 and saved
	as a new image.

	reclassify -type uint8 -in ./data/rj_1985.tif -out ./data/rj_1985_reclass.tif -iv 3 15 -iv_end -ov 1 1 -ov_end

AUTHOR
	Eduardo Ribeiro Lacerda eduardolacerdageo@gmail.com
