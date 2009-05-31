/*!
 * [NOTE] Ideas on improving the CD engine.
 *
 * Currently, you constantly have to ask the CD engine for whatever you want
 * to know. This keeps the code for the engine very simple, at the price of
 * a very limited engine. 
 *
 * In retrospect, the approach taken by Be's CDButton sample code is much 
 * better: clients (which would be the tHustler class in this case) can
 * simply subscribe to one or more notifiers, and are automatically notified
 * when something happens.
 *
 * One simple improvement to the current CD engine would be to put it in its
 * own thread. This would make the GUI a bit more responsive, because the 
 * Deskbar no longer has to wait for the ioctl's to complete.
 *
 * Something that this thread could monitor is whether a CD is present or 
 * not. Then, when a new CD is inserted, we can read its table of contents
 * once and cache it. From then on, calls to GetContents() will simply return
 * this cached data.
 */

// ***************************************************************************
// Implementation System Includes
// ***************************************************************************

#include <Directory.h>
#include <Drivers.h>
#include <Entry.h>
#include <List.h>
#include <Path.h>
#include <scsi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// ***************************************************************************
// Implementation Project Includes
// ***************************************************************************

#include "BL_CDEngine.h"

// ***************************************************************************
// Implementation Constant Definitions
// ***************************************************************************

// ***************************************************************************
// Implementation Type Definitions
// ***************************************************************************

// ***************************************************************************
// Implementation Variable Definitions
// ***************************************************************************

// ***************************************************************************
// Implementation Function Declarations
// ***************************************************************************

// ***************************************************************************
// Global Variable Definitions
// ***************************************************************************

// ***************************************************************************
// PUBLIC Member Function Definitions: Lifecycle
// ***************************************************************************

// ===========================================================================
// Constructor
// ===========================================================================

BL_CDEngine::BL_CDEngine() 
{

  // Initialize attributes.
  mCurrentDevice = -1;
  
  // Allocate memory for the table of contents.
  mpToc = (tToc*) malloc(sizeof(scsi_toc));
  if (mpToc == NULL) {
  }  
  // Build the list of devices.
  mpDevices = new BList(5);
  SearchForCdPlayer("/dev/disk");
}

// ===========================================================================
// Destructor
// ===========================================================================

BL_CDEngine::~BL_CDEngine()
{

  // Close the currently opened device (if any).
  CloseDevice();

  // Free the names of the CD player devices.
  void* apItem; 
  for (int8 t=0; (apItem = mpDevices->ItemAt(t)); ++t) {
    free(apItem);
  }
  delete mpDevices;

  // Free the table of contents.
  free(mpToc);  
}

// ***************************************************************************
// PUBLIC Member Function Definitions: Operators
// ***************************************************************************

// ***************************************************************************
// PUBLIC Member Function Definitions: Operations
// ***************************************************************************

// ===========================================================================
// OpenDevice
// ===========================================================================

bool 
BL_CDEngine::OpenDevice(int8 aIndex)
{
  // If a device is already opened, then close it first.
  CloseDevice();

  // Open the device for reading.
  mCurrentDevice = open(GetDeviceName(aIndex), O_RDONLY);

  // Error?
  if(mCurrentDevice < 0) {
    return FALSE;
  }
  return TRUE;
}

// ===========================================================================
// CloseDevice
// ===========================================================================

void 
BL_CDEngine::CloseDevice()
{
  if (mCurrentDevice >= 0) {
    close(mCurrentDevice);
  }
}

// =========================================================================== 
// Play
// ===========================================================================

void 
BL_CDEngine::Play()
{
  Play(1);
}

// ===========================================================================
// Play
// ===========================================================================

void 
BL_CDEngine::Play(int32 aTrackNumber)
{
  if (IsDeviceOpened()) {

    if (GetState() == STATE_NO_DISC) {
      return;
    }

    scsi_play_track lTrack;

    if (aTrackNumber < 1) {
      lTrack.start_track = 1;
    } else if (aTrackNumber > 99) {
      lTrack.start_track = 99;
    } else {
      lTrack.start_track = aTrackNumber;
    }

    // If first track is data track, we skip it.
    GetContents();
    if ((lTrack.start_track == 1) && (IsDataTrack(0))) {
    
      // If this CD doesn't contain any audio tracks, then we don't play.
      if (GetNumberOfTracks() <= 1) {
        return;
      }

      lTrack.start_track = 2;
    }

    lTrack.start_index = 1;
    lTrack.end_track   = 99;
    lTrack.end_index   = 1;

    status_t lResult = ioctl(mCurrentDevice, B_SCSI_PLAY_TRACK, &lTrack);
    if (lResult != B_OK) {

    }
  }
}

// ===========================================================================
// Resume
// ===========================================================================

void
BL_CDEngine::Resume()
{
  if (IsDeviceOpened()) {
    status_t lResult = ioctl(mCurrentDevice, B_SCSI_RESUME_AUDIO);
    if (lResult != B_OK) {

    }
  }
}

// ===========================================================================
// Pause
// ===========================================================================

void 
BL_CDEngine::Pause()
{
  if (IsDeviceOpened()) {
    status_t lResult = ioctl(mCurrentDevice, B_SCSI_PAUSE_AUDIO);
    if (lResult != B_OK) {
	}
  }
}

// ===========================================================================
// Stop
// ===========================================================================

void 
BL_CDEngine::Stop()
{
  if (IsDeviceOpened()) {
    status_t lResult = ioctl(mCurrentDevice, B_SCSI_STOP_AUDIO);
    if (lResult != B_OK) {
    }
  }
}

// ===========================================================================
// Eject
// ===========================================================================

void 
BL_CDEngine::Eject()
{
  if (IsDeviceOpened()) {

    if (GetState() == STATE_NO_DISC) {

      // Media loading is disabled: see the Gotcha comment from Hustler.cpp.
      
      //status_t lResult = ioctl(mCurrentDevice, B_LOAD_MEDIA, 0, 0);
      //if (lResult != B_OK) {
      //  BBG_WRITE(("[BL_CDEngine.Eject] Error loading."));
      //}

    } else {
      status_t lResult = ioctl(mCurrentDevice, B_SCSI_EJECT);
      if (lResult != B_OK) {
      }
    }
  }
}

// ===========================================================================
// Next
// ===========================================================================

void 
BL_CDEngine::Next()
{
  if (IsDeviceOpened()) {
    tCdState lState = GetState();

    if (lState == STATE_NO_DISC) {
      return;
    }

    Play(GetCurrentTrack() + 1);

    // Make sure we don't start playing if we were paused before.
	if (lState == STATE_PAUSED) {
      Pause();
    }
  }
}

// ===========================================================================
// Previous
// ===========================================================================

void 
BL_CDEngine::Previous()
{
  if (IsDeviceOpened()) {
    tCdState lState = GetState();	

    if (lState == STATE_NO_DISC) {
      return;
    }

    Play(GetCurrentTrack() - 1);

    // Make sure we don't start playing if we were paused before.
	if (lState == STATE_PAUSED) {
      Pause();
    }
  }
}

// ***************************************************************************
// PUBLIC Member Function Definitions: Access
// ***************************************************************************

// ===========================================================================
// GetCurrentDevice
// ===========================================================================

int 
BL_CDEngine::GetCurrentDevice()
{
  return mCurrentDevice;
}

// ===========================================================================
// GetNumberOfDevices
// ===========================================================================

int8 
BL_CDEngine::GetNumberOfDevices()
{
  return mpDevices->CountItems();
}

// ===========================================================================
// GetDeviceName
// ===========================================================================
  
char* 
BL_CDEngine::GetDeviceName(int8 aIndex)
{
  return (char*) mpDevices->ItemAt(aIndex);
}
  
// ===========================================================================
// GetState
// ===========================================================================

BL_CDEngine::tCdState
BL_CDEngine::GetState()
{
  if (IsDeviceOpened()) {

    status_t lMediaStatus = B_DEV_NO_MEDIA;
    ioctl(mCurrentDevice,
          B_GET_MEDIA_STATUS, 
          &lMediaStatus,
          sizeof(lMediaStatus));

    if (lMediaStatus != B_OK) {
      return STATE_NO_DISC;
    }

    scsi_position lPosition;
    status_t lResult = ioctl(mCurrentDevice, 
                             B_SCSI_GET_POSITION, 
                             &lPosition);

    if (lResult != B_OK) {
      return STATE_NO_DISC;

    } else if ((!lPosition.position[1]) 
           ||  (lPosition.position[1] >= 0x13) 
           ||  ((lPosition.position[1] == 0x12) && (!lPosition.position[6]))) {
      return STATE_STOPPED;
      
    } else if (lPosition.position[1] == 0x11) {
      return STATE_PLAYING;
     
    } else {
      return STATE_PAUSED;
    }
  }
  return STATE_NO_DISC;
}

// ===========================================================================
// GetCurrentTrack
// ===========================================================================

int8
BL_CDEngine::GetCurrentTrack()
{
  if (IsDeviceOpened()) {
  
    status_t lMediaStatus = B_DEV_NO_MEDIA;
    ioctl(mCurrentDevice, 
          B_GET_MEDIA_STATUS, 
          &lMediaStatus, 
          sizeof(lMediaStatus));
          
    if (lMediaStatus != B_OK) {
      return -1;
    }

    scsi_position lPosition;
	status_t lResult = ioctl(mCurrentDevice, 
	                         B_SCSI_GET_POSITION, 
	                         &lPosition);

    if (lResult != B_OK) {
      return -1;
    }
		
    if (!lPosition.position[1] 
    ||  lPosition.position[1] >= 0x13
    ||  (lPosition.position[1] == 0x12 && !lPosition.position[6])) {
      return 0;
    } else {
      return lPosition.position[6];
    }
  }
  return -1;
}

// ===========================================================================
// GetContents
// ===========================================================================

bool
BL_CDEngine::GetContents()
{
  if (IsDeviceOpened()) {
  
    int lResult = ioctl(mCurrentDevice, B_SCSI_GET_TOC, mpToc);
  
    if (lResult != 0)	{
      return FALSE;
    }

    PrintContentsToStream();
    return TRUE;
  }
  return FALSE;
}

// ===========================================================================
// GetNumberOfTracks
// ===========================================================================

int8 
BL_CDEngine::GetNumberOfTracks()
{
  return mpToc->mLastTrack;
}

// ===========================================================================
// GetTrackNumber
// ===========================================================================

int8
BL_CDEngine::GetTrackNumber(int8 aIndex)
{
  return mpToc->mTrack[aIndex].mTrackNumber;
}

// ===========================================================================
// GetTrackFlags
// ===========================================================================

int8
BL_CDEngine::GetTrackFlags(int8 aIndex)
{
  return mpToc->mTrack[aIndex].mFlags;
}

// ***************************************************************************
// PUBLIC Member Function Definitions: Inquiry
// ***************************************************************************

// ===========================================================================
// IsDeviceOpened
// ===========================================================================

bool
BL_CDEngine::IsDeviceOpened()
{
  if (GetCurrentDevice() < 0) {
    return FALSE;
  } else {
    return TRUE;
  }
}

// ===========================================================================
// IsDataTrack
// ===========================================================================

bool 
BL_CDEngine::IsDataTrack(int8 aIndex)
{
  if ((GetTrackFlags(aIndex) & FLAG_DATA_TRACK) != 0) {
    return TRUE;
  } 
  return FALSE;
}

// ***************************************************************************
// PROTECTED Member Function Definitions
// ***************************************************************************

// ***************************************************************************
// PRIVATE Member Function Definitions
// ***************************************************************************

// ===========================================================================
// SearchForCdPlayer
// ===========================================================================

void 
BL_CDEngine::SearchForCdPlayer(const char* aDirName) 
{
  BDirectory lDirectory;
  if (lDirectory.SetTo(aDirName) != B_OK) {
    return;
  }
  lDirectory.Rewind();

  BEntry lEntry;
  while(lDirectory.GetNextEntry(&lEntry) >= 0) {

    BPath lPath;
    if(lEntry.GetPath(&lPath) != B_OK) {
      continue;
    } 
    
    const char* lpName = lPath.Path();

    entry_ref lRef;
    if(lEntry.GetRef(&lRef) != B_OK) {
      continue;
    }

    if(lEntry.IsDirectory()) {

      // Ignore floppy. It's worth to explicitly check for the floppy 
      // device and ignore it, because opening it to get its geometry
      // would just make a lot of noise, and not any sense.
      if(strcmp(lRef.name, "floppy") == 0) {
        continue;
      }
      SearchForCdPlayer(lpName);
    
    } else {

      // Ignore partitions.
      if(strcmp(lRef.name, "raw") != 0) {
        continue;
      }

      // Try to open the device.
      int lDevice = open(lpName, O_RDONLY);
      if(lDevice < 0) {
        continue;
      }

      // Figure out is the device is a CD-ROM drive.
      device_geometry lGeometry;
      if(ioctl(lDevice, 
               B_GET_GEOMETRY, 
               &lGeometry, 
               sizeof(lGeometry)) < 0) {
        close(lDevice);
        continue;
      }

      // Hooray, we've found a CD-ROM drive.
      if(lGeometry.device_type == B_CD) {

        // Store the device's name in the list and close the device. 
        mpDevices->AddItem(strdup(lpName));
        close(lDevice);        
      }
    }
  }
}

// ===========================================================================
// PrintContentsToStream
// ===========================================================================

void
BL_CDEngine::PrintContentsToStream()
{
  #ifdef BUGSBGONE
  printf("\n");
  printf("Table of contents\n");
  
  printf("Length of the TOC in bytes: %d\n", mpToc->mLength);
  printf("Number of the first track:  %d\n", mpToc->mFirstTrack);
  
  for (int8 t=0; t<mpToc->mLastTrack; ++t) {
    printf("Track %d: flags = %d, address = %d:%d:%d\n",
           mpToc->mTrack[t].mTrackNumber,
           mpToc->mTrack[t].mFlags, 
           mpToc->mTrack[t].mAddress.mMinute,
           mpToc->mTrack[t].mAddress.mSecond,
           mpToc->mTrack[t].mAddress.mFrame);
  }
  printf("\n");
  #endif BUGSBGONE
}

// ***************************************************************************
// Helper Function Definitions
// ***************************************************************************

// ***************************************************************************
// Global Function Definitions
// ***************************************************************************

// ***************************************************************************
// Implementation Function Definitions
// ***************************************************************************
