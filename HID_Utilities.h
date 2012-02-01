//
// File:       HID_Utilities.h
//
// Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Inc. ("Apple")
//             in consideration of your agreement to the following terms, and your use,
//             installation, modification or redistribution of this Apple software
//             constitutes acceptance of these terms.  If you do not agree with these
//             terms, please do not use, install, modify or redistribute this Apple
//             software.
//
//             In consideration of your agreement to abide by the following terms, and
//             subject to these terms, Apple grants you a personal, non - exclusive
//             license, under Apple's copyrights in this original Apple software ( the
//             "Apple Software" ), to use, reproduce, modify and redistribute the Apple
//             Software, with or without modifications, in source and / or binary forms;
//             provided that if you redistribute the Apple Software in its entirety and
//             without modifications, you must retain this notice and the following text
//             and disclaimers in all such redistributions of the Apple Software. Neither
//             the name, trademarks, service marks or logos of Apple Inc. may be used to
//             endorse or promote products derived from the Apple Software without specific
//             prior written permission from Apple.  Except as expressly stated in this
//             notice, no other rights or licenses, express or implied, are granted by
//             Apple herein, including but not limited to any patent rights that may be
//             infringed by your derivative works or by other works in which the Apple
//             Software may be incorporated.
//
//             The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
//             WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
//             WARRANTIES OF NON - INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
//             PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION
//             ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
//
//             IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
//             CONSEQUENTIAL DAMAGES ( INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//             SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//             INTERRUPTION ) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION
//             AND / OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER
//             UNDER THEORY OF CONTRACT, TORT ( INCLUDING NEGLIGENCE ), STRICT LIABILITY OR
//             OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright ( C ) 2001-2008 Apple Inc. All Rights Reserved.
//

#ifndef _HID_Utilities_h_
#define _HID_Utilities_h_

#include <Carbon/Carbon.h>

#include <IOKit/IOTypes.h>
// 10.0.x
//#include <IOKit/IOUSBHIDParser.h>
// 10.1.x
#include <Kernel/IOKit/hidsystem/IOHIDUsageTables.h>

#include <IOKit/HID/IOHIDLib.h>

// ==================================

#ifdef __cplusplus
extern "C" {
#endif

// ==================================

enum
{
    kDefaultUserMin = 0,					// default user min and max used for scaling
    kDefaultUserMax = 255,
};

enum HIDElementTypeMask
{
	kHIDElementTypeInput				= 1 << 1,
	kHIDElementTypeOutput            	= 1 << 2,
	kHIDElementTypeFeature           	= 1 << 3,
	kHIDElementTypeCollection        	= 1 << 4,
	kHIDElementTypeIO					= kHIDElementTypeInput | kHIDElementTypeOutput | kHIDElementTypeFeature,
	kHIDElementTypeAll					= kHIDElementTypeIO | kHIDElementTypeCollection
};
typedef enum HIDElementTypeMask HIDElementTypeMask;


struct recElement
{
    IOHIDElementType type;					// the type defined by IOHIDElementType in IOHIDKeys.h
    int usagePage;							// usage page from IOUSBHIDParser.h which defines general usage
    int usage;								// usage within above page from IOUSBHIDParser.h which defines specific usage
    IOHIDElementCookie cookie;				// unique value (within device of specific vendorID and productID) which identifies element, will NOT change
    int min;								// reported min value possible
    int max;								// reported max value possible
    int scaledMin;							// reported scaled min value possible
    int scaledMax;							// reported scaled max value possible
    int size;								// size in bits of data return from element
    Boolean relative;						// are reports relative to last report (deltas)
    Boolean wrapping;						// does element wrap around (one value higher than max is min)
    Boolean nonLinear;						// are the values reported non-linear relative to element movement
    Boolean preferredState;					// does element have a preferred state (such as a button)
    Boolean nullState;						// does element have null state
    int units;								// units value is reported in (not used very often)
    int unitExp;							// exponent for units (also not used very often)
    Str255 name;							// name of element (not used often)

// runtime variables
    int minReport; 						// min returned value
    int maxReport; 						// max returned value (calibrate call)
    int userMin; 							// user set value to scale to (scale call)
    int userMax;							
    
	struct recElement * pPrevious;			// previous element (NULL at list head)
    struct recElement * pChild;				// next child (only of collections)
    struct recElement * pSibling;			// next sibling (for elements and collections)
	
};
typedef struct recElement recElement;
typedef recElement* pRecElement;

struct recDevice
{
    IOHIDDeviceInterface ** interface;		// interface to device, NULL = no interface
    IOHIDQueueInterface ** queue;			// device queue, NULL = no queue
    Str255 transport;						// device transport
    int vendorID;							// id for device vendor, unique across all devices
    int productID;							// id for particular product, unique across all of a vendors devices
    int version;							// version of product
    Str255 manufacturer;					// name of manufacturer
    Str255 product;							// name of product
    Str255 serial;							// serial number of specific product, can be assumed unique across specific product or specific vendor (not used often)
    int locID;								// int representing location in USB (or other I/O) chain which device is pluged into, can identify specific device on machine
    int usage;								// usage page from IOUSBHID Parser.h which defines general usage
    int usagePage;							// usage within above page from IOUSBHID Parser.h which defines specific usage
    int totalElements;						// number of total elements (should be total of all elements on device including collections) (calculated, not reported by device)
	int features;							// number of elements of type kIOHIDElementTypeFeature
	int inputs;							// number of elements of type kIOHIDElementTypeInput_Misc or kIOHIDElementTypeInput_Button or kIOHIDElementTypeInput_Axis or kIOHIDElementTypeInput_ScanCodes
	int outputs;							// number of elements of type kIOHIDElementTypeOutput
	int collections;						// number of elements of type kIOHIDElementTypeCollection
    int axis;								// number of axis (calculated, not reported by device)
    int buttons;							// number of buttons (calculated, not reported by device)
    int hats;								// number of hat switches (calculated, not reported by device)
    int sliders;							// number of sliders (calculated, not reported by device)
    int dials;								// number of dials (calculated, not reported by device)
    int wheels;							// number of wheels (calculated, not reported by device)
    recElement* pListElements; 				// head of linked list of elements 
    struct recDevice* pNext; 				// next device
};
typedef struct recDevice recDevice;
typedef recDevice* pRecDevice;

// ==================================

// builds list of device with elements (allocates memory and captures devices)
// list is allcoated internally within HID Utilites and can be accessed via accessor functions
// structures within list are considered flat and user accessable, butnot user modifiable
// can be called again to rebuild list to account for new devices (will do the right thing in case of disposing existing list)
Boolean HIDBuildDeviceList (UInt32 usagePage, UInt32 usage);

// release list built by above function
// MUST be called prior to application exit to properly release devices
// if not called (or app crashes) devices can be recovered by pluging into different location in USB chain
void HIDReleaseDeviceList (void);

// does a device list exist
Boolean HIDHaveDeviceList (void);

// how many HID devices have been found
// returns 0 if no device list exist
UInt32 HIDCountDevices (void);

// how many elements does a specific device have
// returns 0 if device is invalid or NULL
// uses mask of HIDElementTypeMask to restrict element found
// use kHIDElementTypeIO to get non-collection elements
UInt32 HIDCountDeviceElements (pRecDevice pDevice, HIDElementTypeMask typeMask);

// get the first device in the device list
// returns NULL if no list exists
pRecDevice HIDGetFirstDevice (void);

// get next device in list given current device as parameter
// returns NULL if end of list
pRecDevice HIDGetNextDevice (pRecDevice pDevice);

// get the first element of device passed in as parameter
// returns NULL if no list exists or device does not exists or is NULL
// uses mask of HIDElementTypeMask to restrict element found
// use kHIDElementTypeIO to get previous HIDGetFirstDeviceElement functionality
pRecElement HIDGetFirstDeviceElement (pRecDevice pDevice, HIDElementTypeMask typeMask);

// get next element of given device in list given current element as parameter
// will walk down each collection then to next element or collection (depthwise traverse)
// returns NULL if end of list
// uses mask of HIDElementTypeMask to restrict element found
// use kHIDElementTypeIO to get previous HIDGetNextDeviceElement functionality
pRecElement HIDGetNextDeviceElement (pRecElement pElement, HIDElementTypeMask typeMask);

// get previous element of given device in list given current element as parameter
// this wlaks directly up the tree to the top element and does not search at each level
// returns NULL if beginning of list
// uses mask of HIDElementTypeMask to restrict element found
// use kHIDElementTypeIO to get non-collection elements
pRecElement HIDGetPreviousDeviceElement (pRecElement pElement, HIDElementTypeMask typeMask);

// returns C string type name given a type enumeration passed in as parameter (see IOHIDKeys.h)
// returns empty string for invlid types
void HIDGetTypeName (IOHIDElementType type, char * cstrName);

// returns C string usage given usage page and usage passed in as parameters (see IOUSBHIDParser.h)
// returns usage page and usage values in string form for unknown values
void HIDGetUsageName (int valueUsagePage, int valueUsage, char * cstrName);

// returns calibrated value given raw value passed in
// calibrated value is equal to min and max values returned by HIDGetElementValue since device list built scaled to element reported min and max values
SInt32 HIDCalibrateValue (SInt32 value, pRecElement pElement);

// returns scaled value given raw value passed in
// scaled value is equal to current value assumed to be in the range of element reported min and max values scaled to user min and max scaled values
SInt32 HIDScaleValue (SInt32 value, pRecElement pElement);

#ifdef __cplusplus
}
#endif

#endif // _HID_Utilities_h_
