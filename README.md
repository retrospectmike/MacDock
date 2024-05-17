MacDock 1.40 ©May 2024 by Mike
Dedicated to those of us still using these machines 40 years later...
<img width="1023" alt="image" src="https://github.com/retrospectmike/MacDock/assets/69996986/9fdddf25-5170-48fa-8211-4d7ac7cbd951">

What's new this release:
* Now recognizes Control Panels that run as applications
* Fixed an issue that caused File Sharing (and possibly other special extensions) to show up as blank slots in the dock

System Requirements:
* System 7.x - MacOS 8 (MacOS 9 incompatible)
* AppleScript with Finder Scripting Extension for full features

Description:
MacDock is like the Dock in modern macOS but for System 7 and 8.
To use it, simply launch the program.  MacDock will be visible at the bottom of your screen.  

You will see your running applications on the list, limited to 16 applications.  Beyond that, the program may become unstable but willl continue to run.  

* Clicking on any of them switches you to the app.
* Cmd + clicking reveals that application in the Finder
* Option + clicking hides the current application and switches to the clicked application (only when clicking from a non-MacDock application)

Options:
* Small Dock (Cmd+S) - Toggles the small dock
* Show Self in Dock (Cmd+D) - Shows MacDock app in the dock 

Updates:
Follow along at https://tinkerdifferent.com/threads/macdock-dev-progress-like-todays-macos-dock-but-for-system-7.2583/

This program is care-ware.  If you enjoy it, do something nice to someone today!

Release Notes
- v.1.40 -- 5/16/2024
  - Updated the formal limit to number of docked items to 16 from 8.
  - Now recognizes Control Panels that run as special application processes
  - Fixed an issue that caused special extension-based processes (e.g. File Sharing  on) to show up as blank slots in the dock
  - MacDock now resizes to fit the number of apps you have running so it never takes up more space than it needs!
  - Small Dock option replaces the old "Hide Dock" option.  When your desktop is cluttered the small dock will help you do more with less space and without the old tuck-away feature
  - Fixes compatibility with older Macintoshes with old ROM that doesn't support certain graphics calls in DeviceManager (e.g. SE and other true 68000 machines)
- v1.32 -- 8/29/2023
  - Added support for MacOS 8 (by fixing an OS 8-specific issue occuring when "Show In Dock" was selected, causing a blank Dock)
- v1.31 -- 8/21/2023
  - New icon!  It's not Susan Kare but it's nice :-D
  - Fixed an issue on color screens where applications that only have a black & white icon that caused those apps to show up as generic app icons
  - Fixed versioning in the About... box.
- v1.3 -- 8/18/2023
  - Added support for color
- v1.2 -- 7/6/2023
  - Added command + click to reveal app in Finder
- v1.1 -- 5/8/2023
  - No longer keeps After Dark screensavers from triggering when idle
  - Reduced CPU utilization to 1/10th - 1/100th by being selective about when to redraw the Dock and its icons
  - Added "About MacDock..." dialog
- v1.0
  - Initial releaseease
