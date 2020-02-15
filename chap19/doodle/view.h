/////////////////////////////////////////////////////////////////////////////
// Name:        view.h
// Purpose:     View classes for Doodle sample
// Author:      Julian Smart
// Modified by:
// Created:     2004-12-07
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __DOODLE_VIEWH__
#define __DOODLE_VIEWH__

#include "wx/docview.h"

/*
 * DoodleCanvas is the window that displays the doodle document
 */

class DoodleView;
class DoodleCanvas: public wxScrolledWindow
{
    DECLARE_EVENT_TABLE()
public:
    DoodleCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size, const long style);

    /// Draws the document contents
    virtual void OnDraw(wxDC& dc);

    /// Processes mouse events
    void OnMouseEvent(wxMouseEvent& event);

    /// Set/get view
    void SetView(DoodleView* view) { m_view = view; }
    DoodleView* GetView() const { return m_view; }

protected:
    DoodleView *m_view;
};

/*
 * DoodleView mediates between documents and windows
 */

class DoodleFrame;
class DoodleView: public wxView
{
    DECLARE_DYNAMIC_CLASS(DoodleView)
    DECLARE_EVENT_TABLE()
public:
    DoodleView() { m_frame = NULL; }
    ~DoodleView() {};

    /// Called when the document is created
    virtual bool OnCreate(wxDocument *doc, long flags);

    /// Called to draw the document
    virtual void OnDraw(wxDC *dc);

    /// Called when the view needs to be updated
    virtual void OnUpdate(wxView *sender, wxObject *hint = NULL);

    /// Called when the view is closed
    virtual bool OnClose(bool deleteWindow = true);

    /// Processes the cut command
    void OnCut(wxCommandEvent& event);

private:
    DoodleFrame*    m_frame;
};

#endif
    // __DOODLE_VIEWH__

