#include <cstdio>
#include <Cocoa/Cocoa.h>
#include <QuartzCore/CAMetalLayer.h>

#define let const auto
#define var auto

@interface Window : NSWindow {}
@end

@implementation Window
- (BOOL)canBecomeKeyWindow {
    return YES;
}
- (BOOL)canBecomeMainWindow {
    return YES;
}
@end

@interface View : NSView {}
@end

@implementation View
- (BOOL)canBecomeKeyView {
    return YES;
}
- (BOOL)acceptsFirstResponder {
    return YES;
}
- (BOOL)wantsUpdateLayer {
    return YES;
}
@end

void metal_create_window() {
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    let rect = NSMakeRect(0, 0, 1000, 1000);
    let style_mask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable;
    let window = [[Window alloc] initWithContentRect: rect
                                 styleMask: style_mask
                                 backing: NSBackingStoreBuffered
                                 defer: NO ];
    [window center];

    let view = [[NSView alloc] initWithFrame: rect];
    [view setHidden:NO];
    [view setNeedsDisplay:YES];
    [view setWantsLayer:YES];
    [window setContentView:view];
    [window makeFirstResponder:view];
    [window orderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
    [window makeKeyAndOrderFront:nil];
    [NSApp run];

    printf("done\n");
}
