#include "physicalDevice.h"
#include "helpers/log.h"
#include "helpers/searching.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#define SUITABLE 1
#define UNSUITABLE 0
#define TRUE 1
#define FALSE 0


static uint8_t validateDevice(VkPhysicalDevice device);


void selectPhysicalDevice(VkInstance instance, VkPhysicalDevice* physicalDevice) {
	if (!instance) { log_warn("Skipping selecting physical device due to lack of instance\n"); return; }

	// ----------------------
	// Get physical devices
	// ----------------------
	
	

	// ----------------------------------------------------------
	// Check each device and pick the best (first valid) option
	// ----------------------------------------------------------

	

	// ----------------------------------
	// Free up resources no longer used
	// ----------------------------------

	

	log_ok("Physical device selected\n");
}

static uint8_t validateDevice(VkPhysicalDevice device) {

	// -----------------------
	// Get device properties
	// -----------------------

	

	// ---------------
	// Version check
	// ---------------

	

	// -----------------------------
	// Get device queue properties
	// -----------------------------

	

	// --------------------------
	// Check family is sutiable
	// --------------------------

	

	// ---------------------------
	// Get the device extensions
	// ---------------------------

	


	// -------------------------------
	// Perform search for extensions
	// -------------------------------

	

	// -----------------------------
	// Setup all required features
	// -----------------------------

	

	// ----------------------------
	// Get and check the features
	// ----------------------------

	

	// --------------------------
	// Free up unused resources
	// --------------------------

	

	return SUITABLE;
}
