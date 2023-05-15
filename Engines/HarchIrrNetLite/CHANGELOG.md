# Changelog
All notable changes to this project will be documented here. This project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## Unreleased
- Added attribute to SInPacket to track decryption/decompression failures with incoming packets.
- Added additional parametr for getPing(pID) (this allows the server to find out the player ping). Client the server ping is getted regardless of the value of this parameter.
- Added few ENet channels.
Attention: channelID with number 1 used for service information. If it value is 1, we have
only one channel with ID 0. If channelID == 2, we have two channels: first with channelID == 0
and second with channelID == 2 (not 1). If numberChannels == 3, we have three channels with
IDs 0, 2 and 3. Channel with number 1 is service channel! Maybe in next versions I change service
channel from 1 to 0, but not now.
- Fixed examples for few ENet channels.
- Fixed bug: after not hard kick client all outgoing packets not sending to client.
- Added ENet unsequenced packet send type.

## 2.1.1 - 2018-03-14
- Fixed getPeerCount() returning the maximum number of peers instead of the current peer count.
- Fixed clients not sending disconnect message on program exit. 
- Changed example clients from looping infintely.
- Renamed tutorial/examples.
- Restructed project directory.

## 2.1.0 - 2018-03-10
Initial commit of original code.