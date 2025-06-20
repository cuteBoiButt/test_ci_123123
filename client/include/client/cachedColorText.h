#pragma once

#include <wx/stattext.h>
#include <wx/bitmap.h>

class CachedColorText : public wxStaticText {
public:
    CachedColorText(wxWindow* parent,
        wxWindowID id,
        const wxString& label,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxStaticTextNameStr);

    // --- State-Changing Methods ---
    virtual void SetLabel(const wxString& label) override;
    virtual bool SetFont(const wxFont& font) override;

    /**
     * @brief Public method to be called by the parent to invalidate our cache.
     */
    void InvalidateCache();

    /**
     * @brief Public method to be called by the parent to invalidate our cache and force immediate Refresh.
     */
    void InvalidateCacheAndRefresh();

private:
    
    void RenderToCache();

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnSysColourChanged(wxSysColourChangedEvent& event);

    wxBitmap m_cache;

    wxDECLARE_EVENT_TABLE();
};
