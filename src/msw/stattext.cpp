/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/stattext.cpp
// Purpose:     wxStaticText
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "stattext.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_STATTEXT

#ifndef WX_PRECOMP
#include "wx/event.h"
#include "wx/app.h"
#include "wx/brush.h"
#endif

#include "wx/stattext.h"
#include "wx/msw/private.h"
#include <stdio.h>

#if wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxStaticTextStyle )

WX_BEGIN_FLAGS( wxStaticTextStyle )
    // new style border flags, we put them first to
    // use them for streaming out
    WX_FLAGS_MEMBER(wxBORDER_SIMPLE)
    WX_FLAGS_MEMBER(wxBORDER_SUNKEN)
    WX_FLAGS_MEMBER(wxBORDER_DOUBLE)
    WX_FLAGS_MEMBER(wxBORDER_RAISED)
    WX_FLAGS_MEMBER(wxBORDER_STATIC)
    WX_FLAGS_MEMBER(wxBORDER_NONE)
    
    // old style border flags
    WX_FLAGS_MEMBER(wxSIMPLE_BORDER)
    WX_FLAGS_MEMBER(wxSUNKEN_BORDER)
    WX_FLAGS_MEMBER(wxDOUBLE_BORDER)
    WX_FLAGS_MEMBER(wxRAISED_BORDER)
    WX_FLAGS_MEMBER(wxSTATIC_BORDER)
    WX_FLAGS_MEMBER(wxNO_BORDER)

    // standard window styles
    WX_FLAGS_MEMBER(wxTAB_TRAVERSAL)
    WX_FLAGS_MEMBER(wxCLIP_CHILDREN)
    WX_FLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    WX_FLAGS_MEMBER(wxWANTS_CHARS)
    WX_FLAGS_MEMBER(wxNO_FULL_REPAINT_ON_RESIZE)
    WX_FLAGS_MEMBER(wxALWAYS_SHOW_SB )
    WX_FLAGS_MEMBER(wxVSCROLL)
    WX_FLAGS_MEMBER(wxHSCROLL)

    WX_FLAGS_MEMBER(wxST_NO_AUTORESIZE)
    WX_FLAGS_MEMBER(wxALIGN_LEFT)
    WX_FLAGS_MEMBER(wxALIGN_RIGHT)
    WX_FLAGS_MEMBER(wxALIGN_CENTRE)

WX_END_FLAGS( wxStaticTextStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxStaticText, wxControl,"wx/stattext.h")

WX_BEGIN_PROPERTIES_TABLE(wxStaticText)
	WX_PROPERTY( Label,wxString, SetLabel, GetLabel, wxEmptyString , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    WX_PROPERTY_FLAGS( WindowStyle , wxStaticTextStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
WX_END_PROPERTIES_TABLE()

WX_BEGIN_HANDLERS_TABLE(wxStaticText)
WX_END_HANDLERS_TABLE()

WX_CONSTRUCTOR_6( wxStaticText , wxWindow* , Parent , wxWindowID , Id , wxString , Label , wxPoint , Position , wxSize , Size , long , WindowStyle ) 
#else
IMPLEMENT_DYNAMIC_CLASS(wxStaticText, wxControl)
#endif

bool wxStaticText::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxString& label,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name)
{
    if ( !CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    if ( !MSWCreateControl(wxT("STATIC"), label, pos, size) )
        return FALSE;

    return TRUE;
}

wxBorder wxStaticText::GetDefaultBorder() const
{
    return wxBORDER_NONE;
}

WXDWORD wxStaticText::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD msStyle = wxControl::MSWGetStyle(style, exstyle);

    // translate the alignment flags to the Windows ones
    //
    // note that both wxALIGN_LEFT and SS_LEFT are equal to 0 so we shouldn't
    // test for them using & operator
    if ( style & wxALIGN_CENTRE )
        msStyle |= SS_CENTER;
    else if ( style & wxALIGN_RIGHT )
        msStyle |= SS_RIGHT;
    else
        msStyle |= SS_LEFT;

    return msStyle;
}

wxSize wxStaticText::DoGetBestSize() const
{
    wxString text(wxGetWindowText(GetHWND()));

    int widthTextMax = 0, widthLine,
        heightTextTotal = 0, heightLineDefault = 0, heightLine = 0;

    bool lastWasAmpersand = FALSE;

    wxString curLine;
    for ( const wxChar *pc = text; ; pc++ )
    {
        if ( *pc == wxT('\n') || *pc == wxT('\0') )
        {
            if ( !curLine )
            {
                // we can't use GetTextExtent - it will return 0 for both width
                // and height and an empty line should count in height
                // calculation
                if ( !heightLineDefault )
                    heightLineDefault = heightLine;
                if ( !heightLineDefault )
                    GetTextExtent(_T("W"), NULL, &heightLineDefault);

                heightTextTotal += heightLineDefault;
            }
            else
            {
                GetTextExtent(curLine, &widthLine, &heightLine);
                if ( widthLine > widthTextMax )
                    widthTextMax = widthLine;
                heightTextTotal += heightLine;
            }

            if ( *pc == wxT('\n') )
            {
               curLine.Empty();
            }
            else
            {
               // the end of string
               break;
            }
        }
        else
        {
            // we shouldn't take into account the '&' which just introduces the
            // mnemonic characters and so are not shown on the screen -- except
            // when it is preceded by another '&' in which case it stands for a
            // literal ampersand
            if ( *pc == _T('&') )
            {
                if ( !lastWasAmpersand )
                {
                    lastWasAmpersand = TRUE;

                    // skip the statement adding pc to curLine below
                    continue;
                }

                // it is a literal ampersand
                lastWasAmpersand = FALSE;
            }

            curLine += *pc;
        }
    }

    return wxSize(widthTextMax, heightTextTotal);
}

void wxStaticText::DoSetSize(int x, int y, int w, int h, int sizeFlags)
{
    // we need to refresh the window after changing its size as the standard
    // control doesn't always update itself properly
    wxStaticTextBase::DoSetSize(x, y, w, h, sizeFlags);

    Refresh();
}

void wxStaticText::SetLabel(const wxString& label)
{
    wxStaticTextBase::SetLabel(label);

    // adjust the size of the window to fit to the label unless autoresizing is
    // disabled
    if ( !(GetWindowStyle() & wxST_NO_AUTORESIZE) )
    {
        DoSetSize(-1, -1, -1, -1, wxSIZE_AUTO_WIDTH | wxSIZE_AUTO_HEIGHT);
    }
}


bool wxStaticText::SetFont(const wxFont& font)
{
    bool ret = wxControl::SetFont(font);

    // adjust the size of the window to fit to the label unless autoresizing is
    // disabled
    if ( !(GetWindowStyle() & wxST_NO_AUTORESIZE) )
    {
        DoSetSize(-1, -1, -1, -1, wxSIZE_AUTO_WIDTH | wxSIZE_AUTO_HEIGHT);
    }

    return ret;
}

#endif // wxUSE_STATTEXT
