/* Intel's UPnP MicroStack, Main Module */

#include <stdio.h>
#include "UPnPMicroStack.h"

void SwitchPower_GetStatus(void* upnptoken)
{
  printf("UPnP Invoke: SwitchPower_GetStatus();\r\n");

  /* TODO: Place Action Code Here... */

  /* UPnPResponse_Error(upnptoken,404,"Method Not Implemented"); */
  UPnPResponse_SwitchPower_GetStatus(upnptoken,1);
}

void SwitchPower_SetTarget(void* upnptoken,int newTargetValue)
{
  printf("UPnP Invoke: SwitchPower_SetTarget(%d);\r\n",newTargetValue);

  /* TODO: Place Action Code Here... */

  /* UPnPResponse_Error(upnptoken,404,"Method Not Implemented"); */
  UPnPResponse_SwitchPower_SetTarget(upnptoken);
}

void DimmingService_GetLoadLevelStatus(void* upnptoken)
{
  printf("UPnP Invoke: DimmingService_GetLoadLevelStatus();\r\n");

  /* TODO: Place Action Code Here... */

  /* UPnPResponse_Error(upnptoken,404,"Method Not Implemented"); */
  UPnPResponse_DimmingService_GetLoadLevelStatus(upnptoken,250);
}

void DimmingService_GetMinLevel(void* upnptoken)
{
  printf("UPnP Invoke: DimmingService_GetMinLevel();\r\n");

  /* TODO: Place Action Code Here... */

  /* UPnPResponse_Error(upnptoken,404,"Method Not Implemented"); */
  UPnPResponse_DimmingService_GetMinLevel(upnptoken,250);
}

void DimmingService_SetLoadLevelTarget(void* upnptoken,unsigned char NewLoadLevelTarget)
{
  printf("UPnP Invoke: DimmingService_SetLoadLevelTarget(%d);\r\n",NewLoadLevelTarget);

  /* TODO: Place Action Code Here... */

  /* UPnPResponse_Error(upnptoken,404,"Method Not Implemented"); */
  UPnPResponse_DimmingService_SetLoadLevelTarget(upnptoken);
}

int main(void)
{
  void* UPnPFM_SwitchPower[] = {&SwitchPower_GetStatus,&SwitchPower_SetTarget};
  void* UPnPFM_DimmingService[] = {&DimmingService_GetLoadLevelStatus,&DimmingService_GetMinLevel,&DimmingService_SetLoadLevelTarget};
  UPnPSFP_SwitchPower(UPnPFM_SwitchPower);
  UPnPSFP_DimmingService(UPnPFM_DimmingService);

printf("Intel's UPnP MicroStack 1.0\r\nConnected & Extended PC Lab (CEL)\r\n\r\n");
  UPnPStart("fb852742-474d-431d-b1ad-628c92c533b2","0000001", 120);

  return 0;
}

