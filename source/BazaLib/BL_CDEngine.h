#ifndef __BL_CDENGINE_H__
#define  __BL_CDENGINE_H__

class BL_CDEngine {
public:
  /** The possible states of the CD player. */
  enum tCdState {
    STATE_NO_DISC = 0,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_STOPPED
  };
  
  /** Constructor. */
  BL_CDEngine();  
  
  /** Destructor. */
  virtual ~BL_CDEngine();
  
  /** 
   * Opens a new CD player device.
   *
   * If another device was already opened it is closed first. You can get the
   * new CD player's device descriptor with the GetCurrentDevice() method.
   *
   * Although you're not supposed to, it won't hurt if you call methods such
   * as Play() and GetTrack() before OpenDevice(), because these methods do 
   * absolutely nothing as long as a valid device hasn't been opened yet.
   *
   * @param The index of the device in the list of devices.
   * @return TRUE if the device successfully opened, FALSE if an error
   *         occured.
   */
  bool OpenDevice(int8 aIndex);
  
  /* Closes the currently opened CD player device (if there is one).   */
  void CloseDevice();   
  
  /** Starts playing the CD at track 1 until the end of the disc. */
  void Play();
  /** 
   * Starts plays at a specific track until the end of the disc.
   * 
   * If the CD has a data track, Play() automatically skips it. If the CD
   * doesn't have any audio tracks, Play() does nothing and immediately
   * returns.
   *
   * @param aTrackNumber The number of the track to play. Although valid 
   *        track numbers must be between 1 and 99, the method can deal 
   *        with any values.
   */
  void Play(int32 aTrackNumber);

  /** Continues playing the CD after it has been paused. */
  void Resume();  

  /** Pauses the CD. */
  void Pause();

  /** Stops playing the CD. */
  void Stop();
  
  /** Ejects when a CD is loaded. */
  void Eject();
  
  /** Skips to the next track. */
  void Next();
  
  /** Skips to the previous track. */
  void Previous();
  
  /**
   * Returns the current CD player's device descriptor, or -1 if no device 
   * has been opened.
   */
  int GetCurrentDevice();

  /**
   * Returns the number of installed CD players.
   */
  int8 GetNumberOfDevices();
  
  /**
   * Returns the name of a device.
   */
  char* GetDeviceName(int8 aIndex);
  
  /** Returns the CD player's state. */
  tCdState GetState();
  
  /** 
   * Returns the number of the track that is currently playing, 0 if no track
   * is playing (i.e. the CD has stopped), and -1 if the engine couldn't read 
   * the track number (because there was no disc loaded or no device opened).
   *
   * Valid track numbers always lie between 1 and 99.
   */
  int8 GetCurrentTrack();

  /**
   * Reads the table of contents of the CD from the currently opened device.
   *
   * This function should always be called just before you call the
   * GetNumberOfTracks(), GetTrackNumber(), GetTrackFlags() or IsDataTrack()
   * methods, or their results cannot be guaranteed to be valid.
   *
   * Note that, although CD track numbers (usually) start at 1, the index of
   * the first entry from the table of contents always is 0.
   *
   * @return TRUE if the table of contents was read successfully, or FALSE
   *         if the table of contents could not be read.
   */
  bool GetContents();
  
  /**
   * Returns the number of tracks on the CD.
   */
  int8 GetNumberOfTracks();

  /**
   * Returns the track number of a certain entry in the CD's table of 
   * contents.
   *   
   * Note that this method is necessary because the number of a track does
   * not always correspond with its index in the table of contents.
   *
   * @param aIndex The index of the track in the table of contents.
   */
  int8 GetTrackNumber(int8 aIndex);

  /**
   * Returns the flags of a certain entry in the CD's table of contents.
   *
   * @param aIndex The index of the track in the table of contents.
   */
  int8 GetTrackFlags(int8 aIndex);
    
  // Inquiry -----------------------------------------------------------------
  
  /**
   * Determines whether the engine has an opened CD player device.
   */
  bool IsDeviceOpened();

  /** 
   * Determines whether a certain entry in the CD's table of contents is a 
   * data track instead of an audio track.
   *
   * @param aIndex The index of the track in the table of contents.
   */
  bool IsDataTrack(int8 aIndex);
  
protected:
  // Constant Definitions ====================================================

  // Type Definitions ========================================================

  // Attribute Definitions ===================================================

  // Member Function Definitions =============================================

private:
  // Constant Definitions ====================================================

  /** Flags for an entry in the table of contents. */
  enum {
    FLAG_AUDIO_PREEMPHASIS = 0x01,
    FLAG_DIGITAL_COPY_OK = 0x02,
    FLAG_DATA_TRACK = 0x04,
    FLAG_AUDIO_FOUR_CHAN = 0x08
  };

  // Type Definitions ========================================================

  /**
   * A CDDA address. 
   * 
   * The ioctl calls to the CD driver use a positioning type called MSF, 
   * which stands for minute, second, frame. A minute is 60 seconds, a second
   * is 75 frames, and a frame is one 2352-byte sector, which contains 588 
   * samples of 16-bit stereo audio. 
   * 
   * @see Be Newsletter Volume 1 Issue 66.
   * @see The ANSI SCSI standard or ATAPI specs.
   * @see Peter Urbanec's DumpTrack sample code.
   */
  typedef struct {
    uint8 mReserved;
    uint8 mMinute;
    uint8 mSecond;
    uint8 mFrame;
  } tAddress;

  /** An entry in the table of contents. */
  typedef struct {
    uint8   mReserved1;
    uint8   mFlags;
    uint8   mTrackNumber;
    uint8   mReserved2;
    tAddress mAddress;
  } tTrack;

  /** The table of contents for an entire CD. */
  typedef struct {
    uint16 mLength;      // Number of bytes in the table of contents.  
    uint8  mFirstTrack;  // The track number which will appear in track[0].
    uint8  mLastTrack;   // The last valid entry in the table of contents.
    tTrack  mTrack[100];
  } tToc;
  
  // Attribute Definitions ===================================================

  /** The currently opened CD player's device descriptor. */
  int mCurrentDevice;

  /** The names of the installed CD players. */
  BList* mpDevices;

  /** Pointer to the data structure that keeps the table of contents. */
  tToc* mpToc;
    
  // Member Function Definitions =============================================

  /** 
   * Recursively searches directories for CD player devices and adds their
   * names to the list of installed CD players.
   *
   * @param aDirName The name directory to start the search in.
   */
  void SearchForCdPlayer(const char* aDirName);

  /** Prints the table of contents to stdout if debugging is enabled. */
  void PrintContentsToStream();
};

// ***************************************************************************
// Global Variable Declarations
// ***************************************************************************

// ***************************************************************************
// Inline Function Definitions
// ***************************************************************************

// ***************************************************************************
// Helper Function Declarations
// ***************************************************************************

// ***************************************************************************
// Global Function Declarations
// ***************************************************************************

#endif // HUSTLER_CD_ENGINE_H
