#!/bin/bash

# Exit immediately if an error occurs
set -e

echo "📦 Generating documentation with Doxygen..."

# Verify if Doxygen is installed
if ! command -v doxygen &> /dev/null; then
    echo "❌ Error: Doxygen is not installed. Install it with: sudo apt install doxygen"
    exit 1
fi

# Generate a base configuration file if it does not exist
if [ ! -f Doxyfile ]; then
    echo "📄 Creating base Doxyfile..."
    doxygen -g > /dev/null
fi

# Customize the configuration for the project
# Optimize for C, define input and output directories
echo "⚙️  Configuring project parameters..."
sed -i 's/^PROJECT_NAME           =.*/PROJECT_NAME           = "ReadPrompt"/' Doxyfile
sed -i 's/^INPUT                  =.*/INPUT                  = src README.md/' Doxyfile
sed -i 's/^OUTPUT_DIRECTORY       =.*/OUTPUT_DIRECTORY       = doc/' Doxyfile
sed -i 's/^OPTIMIZE_OUTPUT_FOR_C  =.*/OPTIMIZE_OUTPUT_FOR_C  = YES/' Doxyfile
sed -i 's/^RECURSIVE              =.*/RECURSIVE              = YES/' Doxyfile
sed -i 's/^USE_MDFILE_AS_MAINPAGE =.*/USE_MDFILE_AS_MAINPAGE = README.md/' Doxyfile

# Optional: Include private/internal documentation files as well
sed -i 's/^EXTRACT_ALL             =.*/EXTRACT_ALL             = YES/' Doxyfile

# Run Doxygen
doxygen Doxyfile

echo "✅ Documentation generated successfully in the 'doc/' folder!"
echo "🌐 Open 'doc/html/index.html' in your browser to view it."
