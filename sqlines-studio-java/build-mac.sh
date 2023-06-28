#!/bin/bash

# Check arguments
if [ -z "$1" ]
  then
    echo "Error: No version supplied"
    exit 1
fi

if [ -z "$2" ]
  then
    echo "Error: No sqlines file supplied"
    exit 1
fi

# Info
echo "--------------------------------------------------------------------------------"
echo "Building app bundle: SQLines Studio"
echo "JAR: target/sqlines-studio-$1.jar"
echo "SQLines command-line: $2"
echo "Version: $1"
echo "--------------------------------------------------------------------------------"

#Clean
rm -r SQLines\ Studio.app/

# Create dirs
echo "1. Creating app directory"
mkdir "SQLines Studio.app"
cd  SQLines\ Studio.app/
mkdir Contents
cd Contents
mkdir MacOS
mkdir Java
mkdir Resources

# Copy universalJavaApplicationStub 
echo "2. Copying universalJavaApplicationStub"
cp ../../build-files/universalJavaApplicationStub MacOS

# Grant execution permit to universalJavaApplicationStub
echo "3. Granting execution permit to universalJavaApplicationStub"
chmod +x MacOS/universalJavaApplicationStub

# Copy JAR
echo "4. Copying JAR"
cp ../../target/sqlines-studio-$1.jar Java

# Copy sqlines command-line
echo "5. Copying SQLines command-line"
cp $2 Java

# Grant execution permit to sqlines command-line
echo "6. Granting execution permit to sqlines command-line"
chmod +x Java/sqlines

# Copy license file 
echo "7. Copying license file"
cp ../../build-files/license.txt Java

# Copy logo 
echo "8. Copying logo"
cp ../../build-files/logo.icns Resources

#Create Info.plist
echo "9. Creating Info.plist"
touch Info.plist
echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">
<plist version=\"1.0\">
    <dict>
        <key>CFBundleDevelopmentRegion</key>
        <string>English</string>
        <key>CFBundleExecutable</key>
        <string>universalJavaApplicationStub</string>
        <key>CFBundleName</key>
        <string>SQLines Studio</string>
        <key>CFBundleIdentifier</key>
        <string>com.sqlines.sqlines-studio</string>
        <key>CFBundleInfoDictionaryVersion</key>
        <string>6.0</string>
        <key>CFBundlePackageType</key>
        <string>APPL</string>
        <key>CFBundleShortVersionString</key>
        <string>$1</string> 
        <key>CFBundleVersion</key>
        <string>$1</string>
        <key>CFBundleIconFile</key>
        <string>logo</string>
        <key>NSHumanReadableCopyright</key>
        <string>Copyright © 2021 SQLines. All rights reserved.</string>
        <key>JVMMainClassName</key>
        <string>com.sqlines.studio.Main</string>
    </dict>
</plist>" >> Info.plist

# Info
echo "--------------------------------------------------------------------------------"
echo "Done"
