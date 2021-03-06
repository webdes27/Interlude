#include "stdafx.h"

extern BYTE g_StaticKey[80][8];

LPBYTE CCoder::GenerateKey()
{
	return g_StaticKey[(rand() % 80)];
}

void CCoder::Constructor()
{
	memset(this, 0, sizeof(CCoder));
}

void CCoder::SetKey(LPBYTE key)
{
	memcpy(inKey, key, 8);
	memcpy(outKey, key, 8);
	cryptionByte1 = key[3];
	cryptionByte2 = key[6];
};

void CCoder::Decrypt(unsigned char* packet, int offset, int size)
{
	guard;
	
	if(isEnabled)
	{
		unsigned char* _packet = packet;

		int temp = 0;
#ifdef EXTEND_IOBUFFER
		if(size <= 16384)
#else
		if(size <= 8192)
#endif
		{
			for(int i = 0; i < size; i++)
			{
				int temp2 = _packet[offset+i] & 0xFF;
				_packet[offset+i] = (BYTE)(temp2 ^ inKey[i&7] ^ temp);
				temp = temp2;
			}
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Invalid size[%d] !", __FUNCTION__, size);
		}

		int old = inKey[0] & 0xFF;
		old |= inKey[1] << 8 & 0xFF00;
		old |= inKey[2] << 0x10 & 0xFF0000;
		old |= inKey[3] << 0x18 & 0xFF000000;

		old += size;

		inKey[0] = (BYTE)(old & 0xFF);
		inKey[1] = (BYTE)(old >> 0x08 & 0xFF);
		inKey[2] = (BYTE)(old >> 0x10 & 0xFF);
		inKey[3] = (BYTE)(old >> 0x18 & 0xFF);
	}

	unguard;
}

void CCoder::Encrypt(unsigned char* packet, int offset, int size)
{
	guard;

	if(!isEnabled)
	{
		isEnabled = true;
		unguard;
		return;
	}

	unsigned char* _packet = packet;

	int temp = 0;
#ifdef EXTEND_IOBUFFER
		if(size <= 16384)
#else
		if(size <= 8192)
#endif
	{
		for(int i = 0; i < size; i++)
		{
			int temp2 = _packet[offset+i] & 0xFF;
			temp = temp2 ^ outKey[i&7] ^ temp;
			_packet[offset+i] = (BYTE) temp;
		}
	}else
	{
		g_Log.Add(CLog::Error, "[%s] Invalid size[%d] !", __FUNCTION__, size);
	}

	int old = outKey[0] & 0xFF;
	old |= outKey[1] << 8 & 0xFF00;
	old |= outKey[2] << 0x10 & 0xFF0000;
	old |= outKey[3] << 0x18 & 0xFF000000;

	old += size;

	outKey[0] = (BYTE)(old & 0xFF);
	outKey[1] = (BYTE)(old >> 0x08 & 0xFF);
	outKey[2] = (BYTE)(old >> 0x10 & 0xFF);
	outKey[3] = (BYTE)(old >> 0x18 & 0xFF);

	unguard;
}