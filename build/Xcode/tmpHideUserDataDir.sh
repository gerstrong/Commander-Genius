#!/bin/zsh

cgdir=~"/Library/Application Support/Commander Genius"
otherdirs=(${cgdir}.*) 2>/dev/null

if [ ${#otherdirs} = 0 ]; then
	if [ \! -d ${cgdir} ]; then
		echo "${cgdir} not found, you didn't run CG yet."
		echo "This script doesn't make sense to be used then."
		exit 1
	fi

	tmpdir=${cgdir}.$(date "+%Y-%m-%d")

	mv ${cgdir} ${tmpdir} && \
	echo "Moved CG user dir to ${tmpdir}"

elif [ ${#otherdirs} -gt 1 ]; then
	echo "More than one temporary CG user dir found:"
	echo ${otherdirs}
	echo "We cannot automatically see which one is the one you want to recover. Thus please do so manually and delete the others."
	exit 1

else # exactly one otherdir
	otherdir=${otherdirs}

	if [ -d ${cgdir} ]; then
		echo "WARNING: The current CG user dir will be deleted and ${otherdir} will be recovered. Are you sure? (y,n)"
		until read c && [ "$c" = "y" -o "$c" = "n" ]; do ; done
		[ \! "$c" = "y" ] && echo "Cancelled" && exit 1
		
		rm -r ${cgdir} && echo "Deleted current CG user dir" || exit 1 
	fi

	mv ${otherdir} ${cgdir}
	echo "Recovered old CG user dir ${otherdir}"
fi
