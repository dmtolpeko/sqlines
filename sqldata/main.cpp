
// SQLData - main function
// Copyright (c) 2012 SQLines. All rights reserved.

#include <stdio.h>
#include "sqldatacmd.h"

// Entry point
int main(int argc, char** argv)
{
	SqlDataCmd cmd;

	int rc = 0;

	try
	{
		cmd.SetLicense(argv[0]);

		rc = cmd.DefineOptions(argc, argv);

		// Run SQLData in command line
		rc = cmd.Run();		
	}
	catch(...)
	{
		return -2;
	}

#if !defined(WIN32) && !defined(_WIN64)
	if(!cmd.IsStdOut())
		printf("\n");
#endif

	return rc;
}

