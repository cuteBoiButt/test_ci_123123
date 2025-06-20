#include <client/cachedColorText.h>
#include <wx/graphics.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>

wxBEGIN_EVENT_TABLE(CachedColorText, wxStaticText)
    EVT_PAINT(CachedColorText::OnPaint)
    EVT_SIZE(CachedColorText::OnSize)
    EVT_SYS_COLOUR_CHANGED(CachedColorText::OnSysColourChanged)
wxEND_EVENT_TABLE()

CachedColorText::CachedColorText(wxWindow* parent, wxWindowID id, const wxString& label,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxStaticText(parent, id, label, pos, size, style, name) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void CachedColorText::SetLabel(const wxString& label) {
    wxStaticText::SetLabel(label);
    InvalidateCache();
}

bool CachedColorText::SetFont(const wxFont& font) {
    bool ret = wxStaticText::SetFont(font);
    InvalidateCache();
    return ret;
}

void CachedColorText::InvalidateCache() {
    m_cache = wxBitmap();
}

void CachedColorText::InvalidateCacheAndRefresh() {
    m_cache = wxBitmap();
    Refresh();
}

void CachedColorText::RenderToCache() {
    wxSize size = GetClientSize();
    if (size.x <= 0 || size.y <= 0) {
        m_cache = wxBitmap();
        return;
    }

    m_cache.Create(size);
    wxMemoryDC memDC(m_cache);

    wxWindow* parent = GetParent();
    if (parent) {
        memDC.SetBackground(parent->GetBackgroundColour());
    }
    memDC.Clear();

    wxGraphicsContext* gc = nullptr;
#ifdef __WXMSW__
    wxGraphicsRenderer* d2dRenderer = wxGraphicsRenderer::GetDirect2DRenderer();
    if (d2dRenderer) {
        gc = d2dRenderer->CreateContext(memDC);
    }
#else
    gc = wxGraphicsContext::Create(memDC);
#endif

    if (gc) {
        gc->SetFont(GetFont(), GetForegroundColour());
        gc->DrawText(GetLabel(), 0, 0);
        delete gc;
    }
}

void CachedColorText::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc{this};
    if(!m_cache.IsOk()) {
        RenderToCache();
    }
    if(m_cache.IsOk()) {
        dc.DrawBitmap(m_cache, 0, 0);
    }
}

void CachedColorText::OnSize(wxSizeEvent& event) {
    // Only invalidate if the size has actually changed.
    if(m_cache.IsOk() && GetClientSize() != m_cache.GetSize()) {
        InvalidateCache();
    }
    event.Skip();
}

void CachedColorText::OnSysColourChanged(wxSysColourChangedEvent& event) {
    // The system theme changed, so our colors are stale.
    InvalidateCache();
    event.Skip();
}
