BEGIN_SW_NS
int ActorFollowTrack(short SpriteNum,short locktics);
void ActorLeaveTrack(short SpriteNum);
void RefreshPoints(SECTOR_OBJECTp sop,int nx,int ny,SWBOOL dynamic);
void TrackSetup(void);
void PlaceSectorObject(SECTOR_OBJECTp sop,short newang,int newx,int newy);
void PlaceSectorObjectsOnTracks(void);
void PlaceActorsOnTracks(void);
void SetupSectorObject(short sectnum,short tag);
void PostSetupSectorObject(void);
void VehicleSetSmoke(SECTOR_OBJECTp sop,ANIMATORp animator);
void CollapseSectorObject(SECTOR_OBJECTp sop,int nx,int ny);
void KillSectorObjectSprites(SECTOR_OBJECTp sop);
void MoveSectorObjects(SECTOR_OBJECTp sop,short locktics);
END_SW_NS
