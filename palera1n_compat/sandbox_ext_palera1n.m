// palera1n-compatible generate_sandbox_extensions implementation.
//
// On roothide, generate_sandbox_extensions() issues sandbox extension tokens
// for the UUID-based JB root paths so sandboxed apps can access /var/jb content.
// It relies on jbrand() to build paths like:
//   /private/var/containers/Bundle/Application/.jbroot-XXXXXXXXXXXXXXXX/
//
// On palera1n:
// - The JB root is the fixed path /private/var/jb/ (no UUID component).
// - bootstrapd runs as root (uid=0), so most access restrictions don't apply.
// - Sandboxed apps on palera1n typically consume extensions that palera1n injects
//   separately; bootstrapd doesn't need to re-issue them here.
//
// For the initial port we return an empty extension string so:
//  - BSD_REQ_GET_SBTOKEN replies with an empty token (not NULL, which would crash @())
//  - No duplicate-symbol conflict with commlib.a's roothide version (guarded there)
//  - Can be upgraded later if per-app JIT or sandbox bypass via sbtoken is needed

#ifdef PALERA1N_BUILD

#import <Foundation/Foundation.h>
#include <string.h>

// Declared in commlib.h — we provide the palera1n implementation here
// while commlib.m's roothide version is excluded via #ifndef PALERA1N_BUILD.
const char* generate_sandbox_extensions(bool ext)
{
    // palera1n bootstrapd is unsandboxed (entitlement: no-sandbox=true) and runs
    // as root. Extension tokens for /var/jb are not required for daemon operation.
    // Return empty string (never NULL) so @(g_sandbox_extensions) in the IPC
    // reply handler produces @"" rather than undefined behaviour.
    return strdup("");
}

#endif /* PALERA1N_BUILD */
