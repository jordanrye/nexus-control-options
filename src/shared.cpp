#include "shared.h"

AddonAPI* APIDefs = nullptr;
HMODULE hSelf = nullptr;
HWND hClient = nullptr;

Mumble::Data* MumbleLink = nullptr;
Mumble::Identity* MumbleIdentity = nullptr;
NexusLinkData* NexusLink = nullptr;