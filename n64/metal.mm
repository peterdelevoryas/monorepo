#include <cstdio>
#include <unistd.h>
#include <Cocoa/Cocoa.h>
#include <MetalKit/MetalKit.h>

#define let const auto
#define var auto
#define loop for (;;)

void osx_create_window() {
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    let bar = [[NSMenu alloc] init];
    [NSApp setMainMenu:bar];

    let item = [bar addItemWithTitle:@"" action:nullptr keyEquivalent:@""];
    let menu = [[NSMenu alloc] init];
    [item setSubmenu:menu];
    [menu addItemWithTitle:@"quit"
          action:@selector(terminate:)
          keyEquivalent:@"q"];

    let style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable;
    let rectangle = NSMakeRect(0, 0, 1000, 1000);
    let window = [[NSWindow alloc] initWithContentRect:rectangle
                                   styleMask:style
                                   backing:NSBackingStoreBuffered
                                   defer:NO];
    [window setTitle:@"n64"];
    [window setRestorable:NO];
    [window makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
}

void osx_drain_events() {
    loop {
        let event = [NSApp nextEventMatchingMask:NSEventMaskAny
                           untilDate:[NSDate distantPast]
                           inMode:NSDefaultRunLoopMode
                           dequeue:YES];
        if (!event) {
            break;
        }
        [NSApp sendEvent:event];
    }
}
