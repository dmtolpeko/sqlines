#include <stdio.h>
#include <oci.h>

int main()
{
/*   signed int major_version, minor_version, update_num, patch_num, port_update_num;   */
/*   /\*signed int res =*\/ OCIClientVersion (  &major_version, */
/* 					   &minor_version, */
/* 					   &update_num, */
/* 					   &patch_num, */
/* 					   &port_update_num ); */
  
/*   printf("%d.%d.%d.%d.%d\n", */
/* 	 major_version, minor_version, update_num, patch_num, port_update_num); */

/*  printf("%dR%d\n",  OCI_MAJOR_VERSION, OCI_MINOR_VERSION); */
  return OCI_MAJOR_VERSION * 10 + OCI_MINOR_VERSION;
}
