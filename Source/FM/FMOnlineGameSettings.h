#pragma once

/**
* General session settings for game
*/
class FFMOnlineSessionSettings : public FOnlineSessionSettings{
public:

	FFMOnlineSessionSettings(bool bIsLAN = false, bool bIsPresence = false, int32 MaxNumPlayers = 4);
	virtual ~FFMOnlineSessionSettings() {}
};

/**
* General search setting for game
*/
class FFMOnlineSearchSettings : public FOnlineSessionSearch{
public:
	FFMOnlineSearchSettings(bool bSearchingLAN = false, bool bSearchingPresence = false);

	virtual ~FFMOnlineSearchSettings() {}
};

/**
* Search settings for an empty dedicated server to host a match
*/
class FFMOnlineSearchSettingsEmptyDedicated : public FFMOnlineSearchSettings {
public:
	FFMOnlineSearchSettingsEmptyDedicated(bool bSearchingLAN = false, bool bSearchingPresence = false);

	virtual ~FFMOnlineSearchSettingsEmptyDedicated() {}
};
