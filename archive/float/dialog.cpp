//----------------------------------------------------------------------------
//  Project asm_ide
//  Leo Inc.
//  Copyright © 1998. Alle Rechte vorbehalten.
//
//  SUBSYSTEM:    asm_ide.apx Application
//  FILE:         asm_idesetdebug.cpp
//  AUTHOR:       Leo
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TAsm_ideSetdebug (TDialog).
//
//----------------------------------------------------------------------------
#include <owl/pch.h>
#include <owl/static.h>

#include "dialog.h"


//{{TAsm_ideSetdebug Implementation}}

MainDialog::MainDialog(TWindow* parent,TResId resId, TModule* module, SetdebugTB* dtb)
:
    TDialog(parent, resId, module)
{
  // INSERT>> Your constructor code here.

 if(dtb!=NULL)
 {
  Edit1 = new TEdit( this, IDC_EDIT1, sizeof(dtb->edit1));
  Edit2 = new TEdit(this, IDC_EDIT2, sizeof(dtb->edit2));

  SetTransferBuffer(dtb);
 }

}


TAsm_ideSetdebug::~TAsm_ideSetdebug()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.
}
