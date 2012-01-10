#/**********************************************************\ 
#
# Auto-Generated Plugin Configuration file
# for BitTorrent Launcher
#
#\**********************************************************/

set(PLUGIN_NAME "btlauncher")
set(PLUGIN_PREFIX "BTL")
set(COMPANY_NAME "bittorrent")

# ActiveX constants:
set(FBTYPELIB_NAME btlauncherLib)
set(FBTYPELIB_DESC "btlauncher 1.0 Type Library")
set(IFBControl_DESC "btlauncher Control Interface")
set(FBControl_DESC "btlauncher Control Class")
set(IFBComJavascriptObject_DESC "btlauncher IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "btlauncher ComJavascriptObject Class")
set(IFBComEventSource_DESC "btlauncher IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID bb0dcefc-111a-54ea-856b-cbcb610282c7)
set(IFBControl_GUID e891eefb-5a8b-55d2-80fb-1c965a95069a)
set(FBControl_GUID 21930925-f5c4-5bfb-b38c-a1b3fd9962aa)
set(IFBComJavascriptObject_GUID ed803098-f91d-5a53-b869-f62a4cb25144)
set(FBComJavascriptObject_GUID 5ebd0eec-2267-58e8-ba4b-fa2943462c6f)
set(IFBComEventSource_GUID edcf337b-5209-51e1-97d7-7c65319c77e6)

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "bittorrent.btlauncher")
set(MOZILLA_PLUGINID "bittorrent.com/btlauncher")

# strings
set(FBSTRING_CompanyName "BitTorrent, Inc")
set(FBSTRING_FileDescription "Launches BitTorrent standalone application")
set(FBSTRING_PLUGIN_VERSION "1.0.0.0")
set(FBSTRING_LegalCopyright "Copyright 2012 BitTorrent, Inc")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "BitTorrent Launcher")
set(FBSTRING_FileExtents "")
set(FBSTRING_PluginName "BitTorrent Launcher")
set(FBSTRING_MIMEType "application/x-btlauncher")

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
