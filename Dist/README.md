                                 ___    ________ __
                                /   |  / ____/ //_/
                               / /| | / /_  / ,<
                              / ___ |/ __/ / /| |
                             /_/  |_/_/   /_/ |_|
                                                             
                                version 1.5
                    Copyright (c) 2024 Elwynor Technologies

## WHAT IS AFK?

 If you are like most people, you probably spend a lot of time in
 Teleconference, and in the middle of chatting with your friends you have to do
 things like change music, get a Dew, answer the door or your phone, etc. When
 you come back to your keyboard, you often find that people have been talking
 to you and paging you with no clue you were gone. This can be frustrating for
 both you and your friends. AFK gives you a chance to end these frustrations.
 With a simple global command you tell people that you are leaving your
 keyboard, and then tell them when you get back. For example, suppose you are
 in Teleconference and you need to grab a candy bar from the kitchen. You just
 type afk and this will tell AFK you are away from your keyboard. Now if
 someone talks to you or pages you and you don't answer, they can type afk
 <your handle> and they will be told you are away from your keyboard.

## CONFIGURATION

 In LEVEL3, you can set what key is required to use the module.
 
 In LEVEL4, you can choose whether or not to use Teleconference action
 strings upon leaving and returning, or the module's text.  You can set the
 name of the teleconference module of your system (in case you've changed
 it), set the action words for going away and coming back, decide 
 whether or not to change the user's location (and what to set it to) when
 they go away, and set whether or not a user is BUSY when they're away.
 
 There are a few text blocks that can be configured in LEVEL6.
 
## INSTALLATION AND ACTIVATION
 
 Simply unzip the archive to your server directory, add it to your menu, 
 configure the MSG file to your liking, and start the BBS!  It's that easy! 

## MODULE HISTORY
 
 AFK was written in August 1995 by "Easy Ed" Erdman of EzSoft. The
 module was available up through Worldgroup 3.0. EzSoft was acquired by
 AdventureComm, which was acquired by Elwynor Technologies in 2004.

 Elwynor Technologies ported the module to Worldgroup 3.2 in May 2021,
 and to Major BBS V10 in June 2024.
  
## LICENSE

 This project is licensed under the AGPL v3. Additional terms apply to 
 contributions and derivative projects. Please see the LICENSE file for 
 more details.

## CONTRIBUTING

 We welcome contributions from the community. By contributing, you agree to the
 terms outlined in the CONTRIBUTING file.

## CREATING A FORK

 If you create an entirely new project based on this work, it must be licensed 
 under the AGPL v3, assign all right, title, and interest, including all 
 copyrights, in and to your fork to Rick Hadsall and Elwynor Technologies, and 
 you must include the additional terms from the LICENSE file in your project's 
 LICENSE file.

## COMPILATION

 This is a Worldgroup 3.2 / Major BBS v10 module. It's compiled using Borland 
 C/C++ 5.0 for Worldgroup 3.2. If you have a working Worldgroup 3.2 development 
 kit, a simple "make -f ELWAFK" should do it! For Major BBS v10, import this
 project folder in the isv/ subtree of Visual Studio 2022, right click the 
 project name and choose build! When ready to build for "release", ensure you
 are building for release.

## PACKING UP

 The DIST folder includes all of the items that should be packaged up in a 
 ELWAFK.ZIP. When unzipped in a Worldgroup 3.2 or Major BBS V10 installation 
 folder, it "installs" the module.


