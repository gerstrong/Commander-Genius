#!/bin/bash
# Build script for MacOs Bundle
# More setup hints here: https://www.steinzone.de/wordpress/cross-building-commander-genius-fuer-macos-using-a-linux-system/

OTOOL=x86_64-apple-darwin14-otool
INSTALL_NAME_TOOL=x86_64-apple-darwin14-install_name_tool
REPLACE=/opt/local/lib
BY=@loader_path/../libs
BUNDLEPATH=CGenius.app
LIBPATH_SRC=${OSXOPT}/local/lib
LIBPATH_DEST=CGenius.app/Contents/libs
echo "OTOOL = ${OTOOL}"
echo "INSTALL_NAME_TOOL = ${INSTALL_NAME_TOOL}"

SRC_FILE=$1
echo "File to make bundleable is ${SRC_FILE}"

if [ ! -f ${SRC_FILE} ]; then
	echo "File ${SRC_FILE} not found"
	exit 1
fi


${INSTALL_NAME_TOOL} -add_rpath @executable_path/../libs ${SRC_FILE}

declare -a SET_OF_DYLIBS

collect_list_of_dylibs()
{	
	echo "==== getting list of dylibs for $1 ==="
		
	LIST=`${OTOOL} -L $1`
	
	# Declare an array of string with type
	declare -a ARR=(${LIST})
 
	# Now reduce the array to smaller one containing only the string to be replaced
  	for index in "${!ARR[@]}" ; do [[ ! ${ARR[$index]} =~ "${REPLACE}" ]] && unset -v 'ARR[$index]' ; done
  	ARR=("${ARR[@]}")

	echo "content of SET_OF_DYLIBS: ${SET_OF_DYLIBS}"
	
		echo "${INSTALL_NAME_TOOL} -id ${BY}/${FILENAME} $1"
		${INSTALL_NAME_TOOL} -id ${BY}/${FILENAME} $1
 
	# Iterate the string array using for loop
	for val in ${ARR[@]}; do
		FILENAME=$(basename $val)
		
		SRC_FPATH=${LIBPATH_SRC}/${FILENAME}
		DST_FPATH=${LIBPATH_DEST}/${FILENAME}

		if [ ! -f ${DST_FPATH} ]; then
		    echo "cp -L ${SRC_FPATH} ${DST_FPATH}"
			cp -L ${SRC_FPATH} ${DST_FPATH}
		fi
		
		echo "${INSTALL_NAME_TOOL} -change ${REPLACE}/${FILENAME} ${BY}/${FILENAME} $1"
		${INSTALL_NAME_TOOL} -change ${REPLACE}/${FILENAME} ${BY}/${FILENAME} $1
		
	
   		if [[ "${SET_OF_DYLIBS[@]}" =~ "${FILENAME}" ]]; then
			echo "ignore ${FILENAME}"
			continue
		else
			echo recursion ${FILENAME}
			SET_OF_DYLIBS+="${FILENAME} "
			collect_list_of_dylibs ${DST_FPATH}
		fi	
		
   	done
   	
   	echo "==== leaving for $1 ==="
}

collect_list_of_dylibs ${SRC_FILE}

