// palera1n-compatible roothide.h shim
//
// On roothide, jbroot("/path") resolves to a UUID-randomized container:
//   /private/var/containers/Bundle/Application/.jbroot-XXXXXXXXXXXXXXXX/path
//
// On palera1n, the JB root is the fixed path /var/jb (symlink to /private/var/jb).
// Confirmed on iPadOS 18.7.9 / palera1n 2.3 / iPad 7th gen (A10 Fusion, T8010).
//
// This shim provides jbroot() and jbrand() compatible with both C (.c) and
// ObjC (.m) compilation units. Foundation is only imported in ObjC mode.

#ifndef PALERA1N_ROOTHIDE_COMPAT_H
#define PALERA1N_ROOTHIDE_COMPAT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

// The JB root on palera1n — confirmed by: cat /var/jb/etc/apt/sources.list.d/procursus.sources
#define PALERA1N_JB_ROOT_PATH "/var/jb"

// Fixed brand value for IPC session authentication.
// roothide generates a random UUID per boot; palera1n uses a fixed root path
// so we use a fixed constant. All processes share the same value.
static inline uint64_t jbrand(void) {
    return 0x70616C65726131ULL; // "palera1" in ASCII
}

// C string overload: jbroot("/basebin/foo") → "/var/jb/basebin/foo"
// Works in both C and ObjC translation units.
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

// NSString overload — only available in ObjC translation units (.m files)
#ifdef __OBJC__
#import <Foundation/Foundation.h>

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
#endif /* __OBJC__ */

#endif /* PALERA1N_ROOTHIDE_COMPAT_H */
