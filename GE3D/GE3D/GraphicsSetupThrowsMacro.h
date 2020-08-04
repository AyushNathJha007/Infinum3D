#pragma once
//Additional Macros for handling GraphicsSetup exception throw stuff (Now also includes DXGI Info)
#define GFX_EXCEPT_NOINFO(HR) GraphicsSetup::HRException( __LINE__,__FILE__,(HR) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( HR = (hrcall) ) ) throw GraphicsSetup::HRException( __LINE__,__FILE__,HR )

#ifndef NDEBUG	//Additional info messages will be shown, when in debug mode. Else, additional info won't be shown.
#define GFX_EXCEPT(HR) GraphicsSetup::HRException( __LINE__,__FILE__,(HR),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( HR = (hrcall) ) ) throw GFX_EXCEPT(HR)
#define GFX_DEVICE_REMOVED_EXCEPT(HR) GraphicsSetup::DeviceRemovedException( __LINE__,__FILE__,(HR),infoManager.GetMessages() )
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v=infoManager.GetMessages(); if(!v.empty()){throw GraphicsSetup::InfoOnlyException(__LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(HR) GraphicsSetup::HRException( __LINE__,__FILE__,(HR) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(HR) GraphicsSetup::DeviceRemovedException(__LINE__,__FILE__,(HR))
#define GFX_THROW_INFO_ONLY(call) (call)
#endif