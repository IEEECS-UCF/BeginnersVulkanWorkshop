#include <string.h>
#include "log.h"
#include "searching.h"


/** Will search through a given array of extensions, returning positively when all the targets are found (or not)
 *
 * @param arrSize: The size (length) of the searched array
 * @param arr: The array of strings that will be searched through
 * @param targetSize: The size (length) of the array of targets to look for
 * @param targets: An array of strings that will be searched for in the prior given array
 *
 * @return Positive or negative to whether or not all strings are present as defined by 'StringSearchResult'
 */ 
 ExtensionSearchResult searchExtensions(uint32_t extensionsSize, VkExtensionProperties* extensions, uint32_t targetSize, const char* const* targets) {

	uint32_t found = 0;
	for (uint32_t i = 0; i < targetSize; i++) {
		uint32_t initFound = found;
		for (uint32_t j = 0; j < extensionsSize - found; j++) {
			if (strcmp(extensions[j].extensionName, targets[i]) == 0) {
				found++;

				// To save time on searches, basically throwing away the found result via swap
				uint32_t swapIdx = extensionsSize - (found + 1);
				VkExtensionProperties tmp = extensions[swapIdx];
				extensions[swapIdx] = extensions[j];
				extensions[j] = tmp;

				break; // Exit this inner loop to save time
			}
		}

		if (initFound == found) {
			log_warn("Extension '%s' not found\n", targets[i]);
		}
	}

	if (found != targetSize)
		return 1; // Failure

	return 0;
}

