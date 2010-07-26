Some notes to the iPhone build:

- Some parts a bit experimental yet.
- Linked against SDL 1.3 trunk code. I.e. clone that code, open the included iPhone Xcode file and build it for the device and the simulator.
- In CG: The variable SDLDIR in the Xcode iPhone target must point to the SDL trunk.
- I don't have separates Xcode targets for simulator/device right now (was too lazy to sync them). So I switch frequently between them.
- To disable Code-signing for device build: Open /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS*.sdk/SDKSettings.plist and set CODE_SIGNING_REQUIRED = NO.
