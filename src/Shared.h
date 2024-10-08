#ifndef SHARED_H
#define SHARED_H

#include "nexus/Nexus.h"
#include "mumble/Mumble.h"

extern AddonAPI* APIDefs;
extern HMODULE hSelf;
extern HWND hClient;

extern Mumble::Data* MumbleLink;
extern Mumble::Identity* MumbleIdentity;
extern NexusLinkData* NexusLink;

#endif /* SHARED_H */
