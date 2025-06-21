#!/bin/bash

# 🔍 CEF Build Monitor Script
# Checks if CEF build is complete and provides status updates

echo "🔍 Checking CEF Build Status..."
echo "=================================="

CEF_DIR="./cef_build/cef"
BUILD_PROCESS=$(ps aux | grep python | grep automate | wc -l)

# Check if build process is running
if [ $BUILD_PROCESS -gt 0 ]; then
    echo "🔄 BUILD STATUS: RUNNING"
    echo "📊 Active build process detected"
else
    echo "⏸️  BUILD STATUS: NOT RUNNING"
fi

# Check directory sizes
if [ -d "$CEF_DIR" ]; then
    SIZE=$(du -sh "$CEF_DIR" 2>/dev/null | cut -f1)
    echo "📁 CEF Directory Size: $SIZE"
else
    echo "❌ CEF directory not found"
    exit 1
fi

# Check for built binaries
echo ""
echo "🔍 Checking for built components..."

# Look for Release build
if [ -d "$CEF_DIR/Release" ]; then
    echo "✅ Release build directory found"
    DYLIB_COUNT=$(find "$CEF_DIR/Release" -name "*.dylib" 2>/dev/null | wc -l)
    echo "   📚 Dynamic libraries: $DYLIB_COUNT"
else
    echo "⏳ Release build directory not yet created"
fi

# Look for Debug build
if [ -d "$CEF_DIR/Debug" ]; then
    echo "✅ Debug build directory found"
else
    echo "⏳ Debug build directory not yet created"
fi

# Look for cefclient
CEFCLIENT=$(find "$CEF_DIR" -name "cefclient" -type f 2>/dev/null | head -1)
if [ -n "$CEFCLIENT" ]; then
    echo "✅ cefclient executable found: $CEFCLIENT"
else
    echo "⏳ cefclient not yet built"
fi

# Look for key CEF files
if [ -f "$CEF_DIR/Release/Chromium Embedded Framework.framework/Chromium Embedded Framework" ]; then
    echo "✅ CEF Framework found"
elif [ -f "$CEF_DIR/Release/libcef.dylib" ]; then
    echo "✅ CEF Library found"
else
    echo "⏳ CEF binaries not yet ready"
fi

echo ""
echo "🎯 NEXT STEPS:"
if [ $BUILD_PROCESS -gt 0 ]; then
    echo "   ⏳ Wait for build to complete (check periodically with: ./check_cef_build.sh)"
    echo "   ☕ Estimated time remaining: 10-20 minutes"
else
    if [ -d "$CEF_DIR/Release" ] || [ -d "$CEF_DIR/Debug" ]; then
        echo "   🎉 BUILD APPEARS COMPLETE!"
        echo "   🚀 Ready to build yahooooooo-browser!"
        echo "   📝 Run: mkdir build && cd build && cmake .. && ninja"
    else
        echo "   ❌ Build may have failed or stopped"
        echo "   🔄 Consider restarting: cd cef_build && python3 automate-git.py ..."
    fi
fi

echo "==================================" 