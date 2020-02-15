/////////////////////////////////////////////////////////////////////////////
// Name:        view.cpp
// Purpose:     View class implementation for the Doodle sample
// Author:      Julian Smart
// Modified by:
// Created:     2004-11-07
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "doodle.h"
#include "doc.h"
#include "view.h"

IMPLEMENT_DYNAMIC_CLASS(DoodleView, wxView)

BEGIN_EVENT_TABLE(DoodleView, wxView)
    EVT_MENU(DOODLE_CUT, DoodleView::OnCut)
END_EVENT_TABLE()

// What to do when a view is created.
bool DoodleView::OnCreate(wxDocument *doc, long WXUNUSED(flags))
{
    // Associate the appropriate frame with this view.
    m_frame = GetMainFrame();
    SetFrame(m_frame);
    m_frame->GetCanvas()->SetView(this);        
        
    // Make sure the document manager knows that this is the
    // current view.
    Activate(true);
        
    // Initialize the edit menu Undo and Redo items
    doc->GetCommandProcessor()->SetEditMenu(m_frame->GetEditMenu());
    doc->GetCommandProcessor()->Initialize();
    
    return true;
}

// This function is used for default print/preview
// as well as drawing on the screen.
void DoodleView::OnDraw(wxDC *dc)
{
    dc->SetFont(*wxNORMAL_FONT);
    dc->SetPen(*wxBLACK_PEN);
    
    wxList::compatibility_iterator node = ((DoodleDocument *)GetDocument())->GetDoodleSegments().GetFirst();
    while (node)
    {
        DoodleSegment *seg = (DoodleSegment *)node->GetData();
        seg->Draw(dc);
        node = node->GetNext();
    }
}

void DoodleView::OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint))
{
    if (m_frame && m_frame->GetCanvas())
        m_frame->GetCanvas()->Refresh();
}

// Clean up windows used for displaying the view.
bool DoodleView::OnClose(bool WXUNUSED(deleteWindow))
{
    if (!GetDocument()->Close())
        return false;
    
    // Clear the canvas
    m_frame->GetCanvas()->ClearBackground();
    m_frame->GetCanvas()->SetView(NULL);
    
    if (m_frame)
        m_frame->SetTitle(wxTheApp->GetAppName());
    
    SetFrame(NULL);

    // Tell the document manager to stop routing events to the view
    Activate(false);

    return true;
}

void DoodleView::OnCut(wxCommandEvent& WXUNUSED(event))
{
    DoodleDocument *doc = (DoodleDocument *)GetDocument();
    doc->GetCommandProcessor()->Submit(
        new DoodleCommand(wxT("Cut Last Segment"), DOODLE_CUT, doc, NULL));
}

/*
 * Doodle canvas implementation
 */

BEGIN_EVENT_TABLE(DoodleCanvas, wxScrolledWindow)
    EVT_MOUSE_EVENTS(DoodleCanvas::OnMouseEvent)
END_EVENT_TABLE()

// Define a constructor
DoodleCanvas::DoodleCanvas(wxWindow *parent, const wxPoint& pos,
                           const wxSize& size, const long style):
    wxScrolledWindow(parent, wxID_ANY, pos, size, style)
{
    m_view = NULL;
}

// Define the repainting behaviour
void DoodleCanvas::OnDraw(wxDC& dc)
{
    if (m_view)
        m_view->OnDraw(& dc);
}

// This implements doodling behaviour. Drag the mouse using
// the left button.
void DoodleCanvas::OnMouseEvent(wxMouseEvent& event)
{
    // The last position
    static int xpos = -1;
    static int ypos = -1;
    static DoodleSegment *currentSegment = NULL;    

    if (!m_view)
        return;
    
    wxClientDC dc(this);
    DoPrepareDC(dc);
    
    dc.SetPen(*wxBLACK_PEN);

    // Take into account scrolling    
    wxPoint pt(event.GetLogicalPosition(dc));
    
    if (currentSegment && event.LeftUp())
    {
        if (currentSegment->GetLines().GetCount() == 0)
        {
            delete currentSegment;
            currentSegment = NULL;
        }
        else
        {
            // We've got a valid segment on mouse left up, so store it.
            DoodleDocument *doc = (DoodleDocument *) GetView()->GetDocument();
            
            doc->GetCommandProcessor()->Submit(
                new DoodleCommand(wxT("Add Segment"), DOODLE_ADD, doc, currentSegment));
            
            GetView()->GetDocument()->Modify(true);
            currentSegment = NULL;
        }
    }
    
    if (xpos > -1 && ypos > -1 && event.Dragging())
    {
        if (!currentSegment)
            currentSegment = new DoodleSegment;
        
        DoodleLine *newLine = new DoodleLine;
        newLine->m_x1 = xpos; 
        newLine->m_y1 = ypos;
        newLine->m_x2 = pt.x; 
        newLine->m_y2 = pt.y;
        currentSegment->GetLines().Append(newLine);
        
        dc.DrawLine(xpos, ypos, pt.x, pt.y);
    }
    xpos = pt.x;
    ypos = pt.y;
}

