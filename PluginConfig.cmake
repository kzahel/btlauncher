#/**********************************************************\ 
#
# Auto-Generated Plugin Configuration file
# for BitTorrent Launcher
#
#\**********************************************************/

set(PREPROCESSOR_DEFINE "SHARE")
set(PLUGIN_NAME "SoShare")
set(PLUGIN_PREFIX "BTTQ")
set(COMPANY_NAME "Gyre")

# ActiveX constants:
set(FBTYPELIB_NAME soshareLib)
set(FBTYPELIB_DESC "SoShare Type Library")
set(IFBControl_DESC "SoShare Control Interface")
set(FBControl_DESC "SoShare Control Class")
set(IFBComJavascriptObject_DESC "SoShare IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "SoShare ComJavascriptObject Class")
set(IFBComEventSource_DESC "SoShare IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID bb0dcefc-111a-54ea-856b-cbcb610282c7)
set(IFBControl_GUID e891eefb-5a8b-55d2-80fb-1c965a95069a)
set(FBControl_GUID 21930925-f5c4-5bfb-b38c-a1b3fd9962aa)
set(IFBComJavascriptObject_GUID ed803098-f91d-5a53-b869-f62a4cb25144)
set(FBComJavascriptObject_GUID 5ebd0eec-2267-58e8-ba4b-fa2943462c6f)
set(IFBComEventSource_GUID edcf337b-5209-51e1-97d7-7c65319c77e6)

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "gyre.soshare")
set(MOZILLA_PLUGINID "gyre.com/soshare")

# strings
set(FBSTRING_CompanyName "Gyre, Inc")
set(FBSTRING_FileDescription "Launches Gyre SoShare")
set(FBSTRING_PLUGIN_VERSION "4.3.5")
set(FBSTRING_LegalCopyright "Copyright 2012 Gyre, Inc")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "SoShare Plugin")
set(FBSTRING_FileExtents "")
set(FBSTRING_PluginName "SoShare Plugin")
set(FBSTRING_MIMEType "application/x-gyre-soshare")

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