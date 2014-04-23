// FindDevices.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "objbase.h"
#include "UPnP.h"
#include <iostream>
using namespace std;

void TraverseCollection( IUPnPDevices * pDevices )
{
    HRESULT         hr = S_OK;
    IUnknown        * pUnk = NULL;
    
    // Get an enumerator from the devices collection.

    hr = pDevices->get__NewEnum(&pUnk);

    if (SUCCEEDED(hr))
    {
        IEnumVARIANT    * pEnumVar = NULL;
        
        // Get the IEnumVARIANT interface.

        hr = pUnk->QueryInterface(IID_IEnumVARIANT, 
                                  (void **) &pEnumVar);
        
        if (SUCCEEDED(hr))
        {
            VARIANT varCurDevice;

            VariantInit(&varCurDevice);
            
            pEnumVar->Reset();
                
            // Traverse the collection.

            while (S_OK == pEnumVar->Next(1, &varCurDevice, NULL))
            {
                IDispatch   * pdispDevice = NULL;
                IUPnPDevice * pDevice = NULL;
    
                pdispDevice = V_DISPATCH(&varCurDevice);

                hr = pdispDevice->QueryInterface(IID_IUPnPDevice,
                                                 (void **) 
                                                   &pDevice);

                if (SUCCEEDED(hr))
                {
                    // Do something interesting with pDevice.

					BSTR strName=NULL;
					pDevice->get_FriendlyName(&strName);

					cout<<strName<<endl;

					SysFreeString(strName);


                    pDevice->Release();
                }

                VariantClear(&varCurDevice);                                
            };

            pEnumVar->Release();
        }
        else
        {
            wcerr << L"Could not get enumerator interface ¨C ";
//                  << L"HRESULT 0x" 
//                  << setbase(16)
//                  << hr << L"\n";
        }

        pUnk->Release();
    }
    else
    {
        wcerr << L"Could not get new enumerator - HRESULT 0x" ;
//              << setbase(16)
//              << hr << L"\n";
    }
}

IUPnPDevices *
FindMediaPlayerDevices(IUPnPDeviceFinder *pDeviceFinder)
{
    HRESULT         hr = S_OK;
    IUPnPDevices    * pFoundDevices = NULL;
    BSTR            bstrTypeURI = NULL;

    bstrTypeURI = 
        SysAllocString(L"urn:schemas-upnp-org:device:multidisk-dvd");

    if (bstrTypeURI)
    {
        hr = pDeviceFinder->FindByType(bstrTypeURI, 
                                       0,
                                       &pFoundDevices);

        if (SUCCEEDED(hr))
        {
            wprintf(L"FindMediaPlayerDevices(): "
                    L"Search returned successfully\n");
        }
        else
        {
            fwprintf(stderr, L"FindMediaPlayerDevices(): "
                     L"FindByType search failed - returned "
                     L"HRESULT 0x%x\n",
                     hr);
            pFoundDevices = NULL;
        }
        SysFreeString(bstrTypeURI);
    }
    else
    {
        fwprintf(stderr, L"FindMediaPlayerDevices(): "
                 L"Could not allocate BSTR for type URI\n");
    }

    return pFoundDevices;
}

int _tmain(int argc, _TCHAR* argv[])
{
	cout<<"--- Finding Devices Sample ---"<<endl;

	CoInitialize(NULL);

	HRESULT hr = S_OK;
	IUPnPDeviceFinder *pDeviceFinder = NULL;

	hr = CoCreateInstance(CLSID_UPnPDeviceFinder, 
						NULL,
						CLSCTX_INPROC_SERVER,
						IID_IUPnPDeviceFinder,
						(void **) &pDeviceFinder);

	if(FAILED(hr))
	{
		cout<<"Fail to create UPnPDeviceFinder!"<<endl;

		return 0;
	}
	
	cout<<"Searching...(This will take a few minutes)"<<endl;

	IUPnPDevices* pDevices=FindMediaPlayerDevices(pDeviceFinder);
	if(pDevices)
	{
		TraverseCollection(pDevices);
		pDevices->Release();
	}

	cout<<"Search completed"<<endl;

	cin.get();

	return 0;
}

