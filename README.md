# QtJsonSocketLib_v3

## Description 
This is Sending Qt FrameWork Json Object BY TCP Socket

## Requirement
   This is RequireMent installed QtFramework in any PlatFrom.
   Please Check This WebSite and Install The Qt Framework.
   https://www.qt.io/download.
   
## UseAge 
  This Socket have 2 Side Recv Process.
  1. Sync Recv ==> QtJsonSocketLib_v3 Socket(true, false);
  2. Async Recv Signal ==> QtJsonSocketLib_v3 Socket(false, true);
  
  Sync Recv have Timeout Return So if you Recv Timeout Fail, Sync Recv Return Empty QJsonObject.
  
  ASync Recv Signal have must binding connection Slots that want to handle QtJsonSocketLib_v3 RecvSignal.
  This doesn't have Timeout Return So If Json buffer by Json Buffer Size is not Recv It will Return Empty JsonObject.
  
  
