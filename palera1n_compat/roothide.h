// palera1n-compatible roothide.h shim
//
// On roothide, jbroot("/path") resolves to a UUID-randomized container:
//   /private/var/containers/Bundle/Application/.jbroot-XXXXXXXXXXXXXXXX/path
//
// On palera1n, the JB root is the fixed path /var/jb (symlink to /private/var/jb).
// Confirmed on iPadOS 18.7.9 / palera1n 2.3 / iPad 7th gen (A10 Fusion, T8010).
//
// This shim provides jbroot() and jbrand() so bootstrapd (a purely userland daemon)
// can be compiled against palera1n's fixed-path layout instead of roothide's kernel
// UUID mechanism. jailbreakd (kernel-dependent) is excluded from this port entirely.

#ifndef PALERA1N_ROOTHIDE_COMPAT_H
#define PALERA1N_ROOTHIDE_COMPAT_H

#import <Foundation/Foundation.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

// The JB root on palera1n — confirmed by: cat /var/jb/etc/apt/sources.list.d/procursus.sources
#define PALERA1N_JB_ROOT_PATH "/var/jb"

// A fixed brand value used for IPC session authentication.
// roothide generates a random UUID per boot; palera1n has a fixed root so we use
// a fixed magic constant instead. Value is "palera1" in ASCII — consistent across
// all processes sharing this build.
static inline uint64_t jbrand(void) {
    return 0x70616C65726131ULL;
}

// C string overload: jbroot("/basebin/foo") → "/var/jb/basebin/foo"
// Allocates with malloc — suitable for short-lived use (daemon startup paths).
__attribute__((overloadable))
static inline const char* jbroot(const char* path) {
    if (!path || path[0] == '\0') {
        return PALERA1N_JB_ROOT_PATH;
    }
    size_t rootLen = strlen(PALERA1N_JB_ROOT_PATH);
    size_t pathLen = strlen(path);
    char* result = (char*)malloc(rootLen + pathLen + 2);
    if (!result) return PALERA1N_JB_ROOT_PATH;
    memcpy(result, PALERA1N_JB_ROOT_PATH, rootLen);
    if (path[0] == '/') {
        memcpy(result + rootLen, path, pathLen + 1);
    } else {
        result[rootLen] = '/';
        memcpy(result + rootLen + 1, path, pathLen + 1);
    }
    return result;
}

// NSString overload: jbroot(@"/basebin/foo") → @"/var/jb/basebin/foo"
__attribute__((overloadable))
static inline NSString* jbroot(NSString* path) {
    if (!path || path.length == 0) {
        return @PALERA1N_JB_ROOT_PATH;
    }
    NSString* root = @PALERA1N_JB_ROOT_PATH;
    if ([path hasPrefix:@"/"]) {
        return [root stringByAppendingString:path];
    }
    return [root stringByAppendingPathComponent:path];
}

#endif /* PALERA1N_ROOTHIDE_COMPAT_H */
