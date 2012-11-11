#/**********************************************************\ 
#
# Auto-Generated Plugin Configuration file
# for BitTorrent Launcher
#
#\**********************************************************/

set(PREPROCESSOR_DEFINE "CHROME")
set(PLUGIN_NAME "TorqueChrome")
set(PLUGIN_PREFIX "TQCH")
set(COMPANY_NAME "BitTorrent")

# ActiveX constants:
set(FBTYPELIB_NAME torqueChromeLib)
set(FBTYPELIB_DESC "TorqueChrome Type Library")
set(IFBControl_DESC "TorqueChrome Control Interface")
set(FBControl_DESC "TorqueChrome Control Class")
set(IFBComJavascriptObject_DESC "TorqueChrome IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "TorqueChrome ComJavascriptObject Class")
set(IFBComEventSource_DESC "TorqueChrome IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID BBEC98FE-325A-4d5f-A3EB-0574B9904CDB)
set(IFBControl_GUID B04D0582-C868-4fe1-9A28-ADE791C648B2)
set(FBControl_GUID D6FC9D5D-E596-4e5e-BFDD-ACAD768FB58A)
set(IFBComJavascriptObject_GUID 1EC1C228-F13D-496c-ACE1-01BB6C1B0F9A)
set(FBComJavascriptObject_GUID C2C120AB-AFA6-4794-87F9-28FB678DD128)
set(IFBComEventSource_GUID 9377D926-E34A-4f25-BE02-B3769408DCB0)

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "bittorrent.torquechrome")
set(MOZILLA_PLUGINID "bittorrent.com/torquechrome")

# strings
set(FBSTRING_CompanyName "BitTorrent, Inc")
set(FBSTRING_FileDescription "Launches BitTorrent Torque for Chrome")
set(FBSTRING_PLUGIN_VERSION "4.4.0")
set(FBSTRING_LegalCopyright "Copyright 2012 BitTorrent, Inc")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "TorqueChrome Plugin")
set(FBSTRING_FileExtents "")
set(FBSTRING_PluginName "TorqueChrome Plugin")
set(FBSTRING_MIMEType "application/x-bittorrent-torquechrome")

# Uncomment this next line if you're not planning on your plugin doing
# any drawing:

set (FB_GUI_DISABLED 1)

# Mac plugin settings. If your plugin does not draw, set these all to 0
set(FBMAC_USE_QUICKDRAW 0)
set(FBMAC_USE_CARBON 0)
set(FBMAC_USE_COCOA 0)
set(FBMAC_USE_COREGRAPHICS 0)
set(FBMAC_USE_COREANIMATION 0)
set(FBMAC_USE_INVALIDATINGCOREANIMATION 0)

# If you want to register per-machine on Windows, uncomment this line
#set (FB_ATLREG_MACHINEWIDE 1)
#add_firebreath_library(curl)