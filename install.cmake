########################################################################
############### Prepare the installation of the compiled stuff here!

# This will install the application itself
INSTALL(TARGETS commandergenius 
	DESTINATION games)

# This will copy the resources files to the proper directory
INSTALL(DIRECTORY vfsroot/ 
	DESTINATION share/CommanderGenius)
