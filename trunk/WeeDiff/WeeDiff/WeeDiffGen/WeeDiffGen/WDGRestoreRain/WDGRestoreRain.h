// -----------------------------------------------------------------
// WDGRestoreRain
// (c) 2012-2013 Ai4rei/AN
//
// This work is licensed under a
// Creative Commons BY-NC-SA 3.0 Unported License
// http://creativecommons.org/licenses/by-nc-sa/3.0/
//
// -----------------------------------------------------------------

#ifndef _WDGRESTORERAIN_H_
#define _WDGRESTORERAIN_H_

#include <tchar.h>

#include <WeeDiffGenPlugin.h>

using namespace WeeDiffGenPlugin;

class WDGPlugin : public IWDGPlugin
{
    DiffData m_DiffData;

public:
    WDGPlugin(LPVOID lpData) : IWDGPlugin(lpData)
    {
    }
    //
    virtual void Release(void);
    virtual void Free(LPVOID lpBuf);
    virtual LPWDGPLUGININFO GetPluginInfo(void);
    virtual INT32 Enabled(void);
    virtual INT32 Disabled(void);
    virtual LPCTSTR GetInputValue(void);
    virtual DiffData* GeneratePatch(void);
    virtual DiffData* GetDiffData(void);

private:
    bool IsVC9Image(void);
    bool IsSane(void);
    void SetByte(UINT32 uOffset, UCHAR uValue);
    void SetLong(UINT32 uOffset, UINT32 uValue);
};

#endif  /* _WDGRESTORERAIN_H_ */
