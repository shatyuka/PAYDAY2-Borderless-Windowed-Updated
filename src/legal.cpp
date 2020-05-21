#include "superblt_flat.h"

// SuperBLT is licenced under the GNU GPL, and as such all modules linking to it must
// also be either under the GNU GPL, or a compatible licence (this includes Apache 2, MIT,
// and almost every other commonly used FOSS licence. TODO link to GNU page about this).
//
// To ensure the developer of the module understands their requirements under the licence,
// this string must be exported or the module will not load.
//
// The main reason why I didn't place the module-related code under the MIT licence or make an
// exception for it is because I do not want a repeat of HoxHUD, where someone tries to hide
// their mod's (Lua) source code in a DLL.
//
// Note that while SuperBLT itself is under the GPLv3, consider this an exception allowing
// the use of modules covered by the GPLv2, Sun/Oracle CDDL and Mozilla MPL.
//
// The reason I added this system in the first place is to run code that does stuff (such as
// a GUI framework or custom module stuff) that's impossible without the use of native code.
//
// If someone *does* start distributing modules that violate SuperBLT's licence, I may have to
// require all modules to be signed or something like that. It'd be a complete pain for everyone
// involved, but may be the only feasable way of preventing abuse of this system.
//
// If you have any questions or concerns about this, please contact me (ZNix). You can find my
// electronic mail address in the Git commit log for SuperBLT. If you prefer, you can also use
// Steam to contact me.
SBLT_API_EXPORT const char* MODULE_LICENCE_DECLARATION =
"This module is licenced under the GNU GPL version 2 or later, or another compatible licence";

// The location a user can go to find the source code for this module. This might be
// the web address of a source control hosting page (such as GitHub, BitBucket or GitLab),
// or if the source code is included in the original download it may be a path to it.
//
// If you don't use source control and distribute the source as a tarball or something, linking
// to it is fine too.
//
// NULL indicates a development version
SBLT_API_EXPORT const char* MODULE_SOURCE_CODE_LOCATION = "https://github.com/shatyuka/PAYDAY2-Borderless-Windowed-Updated";

// Normally a string to uniquely identify the source code this module was built from, such as
// a Mercurial tag name or or Git commit hash.
//
// If there is a significant problem setting this due to the nature of your build system or
// version control system, you can leave this at NULL. Filling it is greatly appreciated, though.
// Alternatively, linking to a page with a list of hashes and versions so the user can find the version
// of their binary works fine too.
SBLT_API_EXPORT const char* MODULE_SOURCE_CODE_REVISION = 0;
