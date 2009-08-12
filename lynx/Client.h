#pragma once

#include <enet/enet.h>
#include "WorldClient.h"

/*
    CClient k�mmert sich um die Netzwerk-Verwaltung auf Client-Seite.
    CClient stellt die Daten f�r CWorldClient bereit.

    Aktuell k�mmert sich CClient auch im den User-Input. Das soll
    allerdings noch in eine eigene Klasse ausgelagert werden.
 */

class CClient
{
public:
	CClient(CWorldClient* world);
	~CClient(void);

	bool Connect(char* server, int port);
	void Shutdown();

	bool IsConnecting() { return m_isconnecting && m_server; }
	bool IsConnected() { return m_server && !m_isconnecting; }
	bool IsRunning() { return (m_server != NULL); }

	void Update(const float dt, const DWORD ticks);

protected:
	void OnReceive(CStream* stream);

	void InputMouseMove(int dx, int dy);
	void InputCalcDir();
	void SendClientState();
	CObj* GetLocalController();

private:
	ENetHost* m_client;
	ENetPeer* m_server;
	bool m_isconnecting;

	// Input
	int m_forward;
	int m_backward;
	int m_strafe_left;
	int m_strafe_right;
    int m_jump;
    float m_lat; // mouse dx
    float m_lon; // mouse dy

	CWorldClient* m_world;

    DWORD m_lastupdate;
};
