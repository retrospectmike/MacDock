MacDock 2.00a4 ©August 2024 by Mike
Dedicated to those of us still using these machines 40 years later...

What's new this release:
- v2.00a4 -- 8/11/2024
  - Dot (•) to indicate apps that are presently running (just like modern macOS Dock)
     - Very useful to distinguish Favorite apps (see below) that aren't running
  - Favorites!
     - Ctrl+Click on a docked item and it will be saved on the dock.  Even if you quit the app, its icon will remain
     - (TODO: ability to REMOVE item as a favorite)
     - Favorites persist betweeen launches of MacDock!
  - Preferences file
     - Used to save the Favorites
     - Stored in current running System Folder, so if you restart on a different System Folder, you won't pull up your favorites.  If I get feedback that people really want this, then I'll make the Preferences files live in the app's present directory..
  - Unlimited dock items (limited by memory allocated)
     - Switches to a linked-list internal structure that makes favorites possible

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
* Show Self in Dock (Cmd+D) - Toggles showing MacDock in the dock 

Updates:
Follow along at https://tinkerdifferent.com/threads/macdock-dev-progress-like-todays-macos-dock-but-for-system-7.2583/

This program is care-ware (Kare-ware?).  If you enjoy it, do something nice to someone today!

Release Notes
- v2.00a4 -- 8/11/2024
  - Dot (•) to indicate apps that are presently running (just like modern macOS Dock)
     - Very useful to distinguish Favorite apps (see below) that aren't running
  - Favorites!
     - Ctrl+Click on a docked item and it will be saved on the dock.  Even if you quit the app, its icon will remain
     - (TODO: ability to REMOVE item as a favorite)
     - Favorites persist betweeen launches of MacDock!
  - Preferences file
     - Used to save the Favorites
     - Stored in current running System Folder, so if you restart on a different System Folder, you won't pull up your favorites.  If I get feedback that people really want this, then I'll make the Preferences files live in the app's present directory..
  - Unlimited dock items (limited by memory allocated)
     - Switches to a linked-list internal structure that makes favorites possible
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
