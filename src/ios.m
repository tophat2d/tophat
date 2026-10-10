#if defined(__APPLE__)
#include <TargetConditionals.h>
#include <CoreFoundation/CoreFoundation.h>
#endif

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
bool
th_set_bundle_to_cwd(void)
{
	char path[PATH_MAX];

	CFURLRef url =
	    CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());

	if (url == NULL)
		return false;

	Boolean ok = CFURLGetFileSystemRepresentation(
	    url, true, (UInt8 *)path, sizeof path);

	CFRelease(url);

	return ok && chdir(path) == 0;
}
#endif
